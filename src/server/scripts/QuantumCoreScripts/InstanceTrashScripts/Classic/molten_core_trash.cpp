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
#include "../../../scripts/EasternKingdoms/MoltenCore/molten_core.h"

enum MoltenCoreSpells
{
	SPELL_SMASH              = 18944,
	SPELL_KNOCK_AWAY         = 18945,
	SPELL_SUMMON_LAVA_SPAWN  = 19392,
	SPELL_SOUL_BURN          = 19393,
	SPELL_FLAME_BUFFET       = 16168,
	SPELL_LAVA_BREATH        = 19272,
	SPELL_VICIOUS_BITE       = 19319,
	SPELL_GROUND_STOMP       = 19364,
	SPELL_ANCIENT_DREAD      = 19365,
	SPELL_CAUTERIZING_FLAMES = 19366,
	SPELL_WITHERING_HEAT     = 19367,
	SPELL_ANCIENT_DESPAIR    = 19369,
	SPELL_ANCIENT_HYSTERIA   = 19372,
	SPELL_FIRE_NOVA          = 20602,
	SPELL_SURGE              = 19196,
	SPELL_FIREBALL           = 19391,
	SPELL_SPLIT_1            = 19569,
	SPELL_SPLIT_2            = 19570,
	SPELL_SERRATED_BITE      = 19771,
	SPELL_SHADOW_SHOCK       = 20603,
	SPELL_DOMINATE_MIND      = 20604,
	SPELL_CLEAVE             = 20605,
	SPELL_SHADOW_BOLT        = 21077,
	SPELL_DARK_MENDING       = 19775,
    SPELL_SHADOW_WORD_PAIN   = 19776,
	SPELL_DARK_STRIKE        = 19777,
    SPELL_IMMOLATE           = 20294,
	SPELL_FE_FIREBALL        = 20420,
	SPELL_FIREBLAST          = 20623,
	SPELL_BLAST_WAVE         = 20229,
	SPELL_SUNDER_ARMOR       = 15502,
	SPELL_STRIKE             = 19730,
	SPELL_FIST_OF_RAGNAROS   = 20277,
	SPELL_PYROCLAST_BARRAGE  = 19641,
	SPELL_LR_CLEAVE          = 19642,
	SPELL_LR_STRIKE          = 19644,
	SPELL_INCITE_FLAMES      = 19635,
	SPELL_FIRE_BLOSSOM       = 19637, // 19636 NOT WORK
	SPELL_MANGLE             = 19820,
	SPELL_GOLEMAGG_TRUST     = 20553,
	SPELL_CONE_OF_FIRE       = 19630,
	SPELL_MELT_ARMOR         = 19631,
};

enum Texts
{
    EMOTE_AEGIS = -1409002,
};

class npc_molten_giant : public CreatureScript
{
public:
    npc_molten_giant() : CreatureScript("npc_molten_giant") { }

    struct npc_molten_giantAI : public QuantumBasicAI
    {
        npc_molten_giantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SmashTimer;
		uint32 KnockAwayTimer;

        void Reset()
        {
			SmashTimer = 2*IN_MILLISECONDS;
			KnockAwayTimer = 4*IN_MILLISECONDS;

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

			if (SmashTimer <= diff)
			{
				DoCastAOE(SPELL_SMASH);
				SmashTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SmashTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCK_AWAY);
				KnockAwayTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else KnockAwayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_molten_giantAI(creature);
    }
};

class npc_firelord : public CreatureScript
{
public:
    npc_firelord() : CreatureScript("npc_firelord") { }

    struct npc_firelordAI : public QuantumBasicAI
    {
        npc_firelordAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SoulBurnTimer;

        void Reset()
        {
			SoulBurnTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_LAVA_SPAWN, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_SUMMON_LAVA_SPAWN, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SoulBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SOUL_BURN);
					SoulBurnTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SoulBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_firelordAI(creature);
    }
};

class npc_ancient_core_hound : public CreatureScript
{
public:
    npc_ancient_core_hound() : CreatureScript("npc_ancient_core_hound") { }

    struct npc_ancient_core_houndAI : public QuantumBasicAI
    {
        npc_ancient_core_houndAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 LavaBreathTimer;
		uint32 ViciousBiteTimer;
		uint32 RandomSpellTimer;

        void Reset()
        {
			LavaBreathTimer = 1*IN_MILLISECONDS;
			ViciousBiteTimer = 2*IN_MILLISECONDS;
			RandomSpellTimer = 4*IN_MILLISECONDS;

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

			if (LavaBreathTimer <= diff)
			{
				DoCastVictim(SPELL_LAVA_BREATH);
				LavaBreathTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else LavaBreathTimer -= diff;

			if (ViciousBiteTimer <= diff)
			{
				DoCastVictim(SPELL_VICIOUS_BITE);
				ViciousBiteTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ViciousBiteTimer -= diff;

			if (RandomSpellTimer <= diff)
			{
				DoCastVictim(RAND(SPELL_GROUND_STOMP, SPELL_ANCIENT_DREAD, SPELL_CAUTERIZING_FLAMES, SPELL_WITHERING_HEAT, SPELL_ANCIENT_DESPAIR, SPELL_ANCIENT_HYSTERIA));
				RandomSpellTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else RandomSpellTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancient_core_houndAI(creature);
    }
};

class npc_flame_imp : public CreatureScript
{
public:
    npc_flame_imp() : CreatureScript("npc_flame_imp") { }

    struct npc_flame_impAI : public QuantumBasicAI
    {
        npc_flame_impAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireNovaTimer;

        void Reset()
        {
			FireNovaTimer = 2*IN_MILLISECONDS;

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

			if (FireNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FIRE_NOVA);
				FireNovaTimer = 4*IN_MILLISECONDS;
			}
			else FireNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flame_impAI(creature);
    }
};

class npc_lava_annihilator : public CreatureScript
{
public:
    npc_lava_annihilator() : CreatureScript("npc_lava_annihilator") { }

    struct npc_lava_annihilatorAI : public QuantumBasicAI
    {
        npc_lava_annihilatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FlameBuffetTimer;

        void Reset()
        {
			FlameBuffetTimer = 0.5*IN_MILLISECONDS;

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

			if (FlameBuffetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_BUFFET);

					if (DoGetThreat(me->GetVictim()))
                        DoModifyThreatPercent(target, -100);

					FlameBuffetTimer = 4*IN_MILLISECONDS;
				}
			}
			else FlameBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_annihilatorAI(creature);
    }
};

class npc_lava_surger : public CreatureScript
{
public:
    npc_lava_surger() : CreatureScript("npc_lava_surger") { }

    struct npc_lava_surgerAI : public QuantumBasicAI
    {
        npc_lava_surgerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SurgeTimer;

        void Reset()
        {
			SurgeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SURGE, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SurgeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SURGE);
					SurgeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else SurgeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_surgerAI(creature);
    }
};

class npc_lava_spawn : public CreatureScript
{
public:
    npc_lava_spawn() : CreatureScript("npc_lava_spawn") { }

    struct npc_lava_spawnAI : public QuantumBasicAI
    {
        npc_lava_spawnAI(Creature* creature) : QuantumBasicAI(creature)
		{
			Reset();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireballTimer;
		uint32 SplitTimer;

        void Reset()
        {
			FireballTimer = 0.5*IN_MILLISECONDS;
			SplitTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			SelectValidTarget();
        }

		void SelectValidTarget()
		{
			if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 200.0f, true))
				me->SetInCombatWith(target);
		}

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
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = 3*IN_MILLISECONDS;
				}
			}
			else FireballTimer -= diff;

			if (SplitTimer <= diff)
			{
				DoCast(me, RAND(SPELL_SPLIT_1, SPELL_SPLIT_2));
				SplitTimer = 45*IN_MILLISECONDS;
			}
			else SplitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_spawnAI(creature);
    }
};

class npc_lava_elemental : public CreatureScript
{
public:
    npc_lava_elemental() : CreatureScript("npc_lava_elemental") { }

    struct npc_lava_elementalAI : public QuantumBasicAI
    {
        npc_lava_elementalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PyroclastBarrageTimer;

        void Reset()
        {
			PyroclastBarrageTimer = 1*IN_MILLISECONDS;

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

			if (PyroclastBarrageTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PYROCLAST_BARRAGE);
					PyroclastBarrageTimer = 8*IN_MILLISECONDS;
				}
			}
			else PyroclastBarrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_elementalAI(creature);
    }
};

class npc_lava_reaver : public CreatureScript
{
public:
    npc_lava_reaver() : CreatureScript("npc_lava_reaver") { }

    struct npc_lava_reaverAI : public QuantumBasicAI
    {
        npc_lava_reaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CleaveTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 4*IN_MILLISECONDS;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_LR_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_LR_STRIKE);
				StrikeTimer = 6*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_reaverAI(creature);
    }
};

class npc_core_hound : public CreatureScript
{
public:
    npc_core_hound() : CreatureScript("npc_core_hound") { }

    struct npc_core_houndAI : public QuantumBasicAI
    {
        npc_core_houndAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SerratedBiteTimer;

        void Reset()
        {
			SerratedBiteTimer = 2*IN_MILLISECONDS;

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

			if (SerratedBiteTimer <= diff)
			{
				DoCastVictim(SPELL_SERRATED_BITE);
				SerratedBiteTimer = 5*IN_MILLISECONDS;
			}
			else SerratedBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_core_houndAI(creature);
    }
};

class npc_flamewaker_protector : public CreatureScript
{
public:
    npc_flamewaker_protector() : CreatureScript("npc_flamewaker_protector") { }

    struct npc_flamewaker_protectorAI : public QuantumBasicAI
    {
        npc_flamewaker_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CleaveTimer;
		uint32 DominateMindTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			DominateMindTimer = 4*IN_MILLISECONDS;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (DominateMindTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DOMINATE_MIND);
					DominateMindTimer = 8*IN_MILLISECONDS;
				}
			}
			else DominateMindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flamewaker_protectorAI(creature);
    }
};

class npc_flamewaker_healer : public CreatureScript
{
public:
    npc_flamewaker_healer() : CreatureScript("npc_flamewaker_healer") { }

    struct npc_flamewaker_healerAI : public QuantumBasicAI
    {
        npc_flamewaker_healerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltTimer;
		uint32 ShadowShockTimer;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			ShadowShockTimer = 1500;

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
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ShadowShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_SHOCK);
					ShadowShockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flamewaker_healerAI(creature);
    }
};

class npc_flamewaker_priest : public CreatureScript
{
public:
    npc_flamewaker_priest() : CreatureScript("npc_flamewaker_priest") { }

    struct npc_flamewaker_priestAI : public QuantumBasicAI
    {
        npc_flamewaker_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ImmolateTimer;
		uint32 ShadowWordPainTimer;
		uint32 DarkStrikeTimer;
		uint32 DarkMendingTimer;

        void Reset()
        {
			ImmolateTimer = 0.5*IN_MILLISECONDS;
			ShadowWordPainTimer = 1500;
			DarkStrikeTimer = 3*IN_MILLISECONDS;
			DarkMendingTimer = 5*IN_MILLISECONDS;

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

			if (ImmolateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IMMOLATE);
					ImmolateTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ImmolateTimer -= diff;

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_WORD_PAIN);
					ShadowWordPainTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (DarkStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_DARK_STRIKE);
				DarkStrikeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DarkStrikeTimer -= diff;

			if (DarkMendingTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_DARK_MENDING);
					DarkMendingTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else DarkMendingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flamewaker_priestAI(creature);
    }
};

class npc_flamewaker_elite : public CreatureScript
{
public:
    npc_flamewaker_elite() : CreatureScript("npc_flamewaker_elite") { }

    struct npc_flamewaker_eliteAI : public QuantumBasicAI
    {
        npc_flamewaker_eliteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireballTimer;
		uint32 FireblastTimer;
		uint32 BlastWaveTimer;

        void Reset()
        {
			FireballTimer = 0.5*IN_MILLISECONDS;
			FireblastTimer = 1500;
			BlastWaveTimer = 3*IN_MILLISECONDS;

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

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FE_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (FireblastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBLAST);
					FireblastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FireblastTimer -= diff;

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(SPELL_BLAST_WAVE);
				BlastWaveTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BlastWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flamewaker_eliteAI(creature);
    }
};

class npc_flamewaker : public CreatureScript
{
public:
    npc_flamewaker() : CreatureScript("npc_flamewaker") { }

    struct npc_flamewakerAI : public QuantumBasicAI
    {
        npc_flamewakerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SunderArmorTimer;
		uint32 StrikeTimer;
		uint32 FistOfRagnarosTimer;

        void Reset()
        {
			SunderArmorTimer = 1*IN_MILLISECONDS;
			StrikeTimer = 2*IN_MILLISECONDS;
			FistOfRagnarosTimer = 4*IN_MILLISECONDS;

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

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = 4*IN_MILLISECONDS;
			}
			else SunderArmorTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = 6*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (FistOfRagnarosTimer <= diff)
			{
				DoCastVictim(SPELL_FIST_OF_RAGNAROS);
				FistOfRagnarosTimer = 8*IN_MILLISECONDS;
			}
			else FistOfRagnarosTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flamewakerAI(creature);
    }
};

class npc_firewalker : public CreatureScript
{
public:
    npc_firewalker() : CreatureScript("npc_firewalker") { }

    struct npc_firewalkerAI : public QuantumBasicAI
    {
        npc_firewalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 InciteFlamesTimer;
		uint32 FireBlossomTimer;

        void Reset()
        {
			InciteFlamesTimer = 1*IN_MILLISECONDS;
			FireBlossomTimer = 2*IN_MILLISECONDS;

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

			if (InciteFlamesTimer <= diff)
			{
				DoCastAOE(SPELL_INCITE_FLAMES);
				InciteFlamesTimer = 3*IN_MILLISECONDS;
			}
			else InciteFlamesTimer -= diff;

			if (FireBlossomTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIRE_BLOSSOM);
					FireBlossomTimer = 6*IN_MILLISECONDS;
				}
			}
			else FireBlossomTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_firewalkerAI(creature);
    }
};

class npc_core_rager : public CreatureScript
{
public:
	npc_core_rager() : CreatureScript("npc_core_rager") { }

	struct npc_core_ragerAI : public QuantumBasicAI
	{
		npc_core_ragerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		InstanceScript* instance;

		uint32 MangleTimer;

		void Reset()
		{
			MangleTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
		{
			if (HealthAbovePct(50) || !instance)
				return;

			if (Creature* Golemagg = instance->instance->GetCreature(instance->GetData64(BOSS_GOLEMAGG_THE_INCINERATOR)))
			{
				if (Golemagg->IsAlive())
				{
					DoSendQuantumText(EMOTE_AEGIS, me);
					me->AddAura(SPELL_GOLEMAGG_TRUST, me);
					me->SetFullHealth();
				}
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MangleTimer <= diff)
			{
				DoCastVictim(SPELL_MANGLE);
				MangleTimer = 5*IN_MILLISECONDS;
			}
			else MangleTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_core_ragerAI(creature);
	}
};

class npc_flameguard : public CreatureScript
{
public:
    npc_flameguard() : CreatureScript("npc_flameguard") { }

    struct npc_flameguardAI : public QuantumBasicAI
    {
        npc_flameguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ConeOfFireTimer;
		uint32 MeltArmorTimer;

        void Reset()
        {
			ConeOfFireTimer = 1*IN_MILLISECONDS;
			MeltArmorTimer = 2*IN_MILLISECONDS;

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

			if (ConeOfFireTimer <= diff)
			{
				DoCastAOE(SPELL_CONE_OF_FIRE);
				ConeOfFireTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ConeOfFireTimer -= diff;

			if (MeltArmorTimer <= diff)
			{
				DoCastAOE(SPELL_MELT_ARMOR);
				MeltArmorTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MeltArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flameguardAI(creature);
    }
};

class npc_son_of_flame : public CreatureScript
{
public:
	npc_son_of_flame() : CreatureScript("npc_son_of_flame") { }

	struct npc_son_of_flameAI: public QuantumBasicAI
	{
		npc_son_of_flameAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = me->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		InstanceScript* instance;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* /*victim*/)
		{
			if (instance)
				instance->SetData(DATA_RAGNAROS_ADDS, 1);
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
		return new npc_son_of_flameAI(creature);
	}
};

void AddSC_molten_core_trash()
{
	new npc_molten_giant();
	new npc_firelord();
	new npc_ancient_core_hound();
	new npc_flame_imp();
	new npc_lava_annihilator();
	new npc_lava_surger();
	new npc_lava_spawn();
	new npc_lava_elemental();
	new npc_lava_reaver();
	new npc_core_hound();
	new npc_flamewaker_protector();
	new npc_flamewaker_healer();
	new npc_flamewaker_priest();
	new npc_flamewaker_elite();
	new npc_flamewaker();
	new npc_firewalker();
	new npc_core_rager();
	new npc_flameguard();
	new npc_son_of_flame();
}