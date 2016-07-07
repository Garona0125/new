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

enum NorthrendSpells
{
	SPELL_ICECROWN_PURPLE_BEAM = 50036,
	SPELL_NECROTIC_PURPLE_BEAM = 45491,
	SPELL_ANIMAL_BLOOD         = 46221,
	SPELL_DUAL_WIELD           = 42459,
};

enum WyrmReanimator
{
	SPELL_SHADOW_BOLT = 9613,
	SPELL_CRIPPLE     = 11443,
	SPELL_CORRUPTION  = 32063,

	NPC_BEAM_STALKER  = 27047,
};

class npc_wyrm_reanimator : public CreatureScript
{
public:
    npc_wyrm_reanimator() : CreatureScript("npc_wyrm_reanimator") {}

    struct npc_wyrm_reanimatorAI : public QuantumBasicAI
    {
        npc_wyrm_reanimatorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PurpleBeamTimer;
		uint32 CorruptionTimer;
		uint32 CrippleTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			PurpleBeamTimer = 0.1*IN_MILLISECONDS;
			CorruptionTimer = 0.5*IN_MILLISECONDS;
			CrippleTimer = 1*IN_MILLISECONDS;
			ShadowBoltTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* stalker = me->FindCreatureWithDistance(NPC_BEAM_STALKER, 75.0f))
				{
					me->SetFacingToObject(stalker);
					DoCast(stalker, SPELL_ICECROWN_PURPLE_BEAM, true);
					PurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CORRUPTION);
					CorruptionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CorruptionTimer -= diff;

			if (CrippleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRIPPLE);
					CrippleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else CrippleTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wyrm_reanimatorAI(creature);
    }
};

enum CultistCorrupter
{
	SPELL_SHARPENED_BONE = 50323,
	SPELL_CC_BONE_ARMOR  = 50324,
};

class npc_cultist_corrupter : public CreatureScript
{
public:
    npc_cultist_corrupter() : CreatureScript("npc_cultist_corrupter") {}

    struct npc_cultist_corrupterAI : public QuantumBasicAI
    {
        npc_cultist_corrupterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoneArmorTimer;
		uint32 SharpenedBoneTimer;

        void Reset()
        {
			BoneArmorTimer = 0.1*IN_MILLISECONDS;
			SharpenedBoneTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (BoneArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_CC_BONE_ARMOR, true);
				BoneArmorTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else BoneArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Bone Armor Buff Check in Combat
			if (!me->HasAura(SPELL_CC_BONE_ARMOR))
				DoCast(me, SPELL_CC_BONE_ARMOR, true);

			if (SharpenedBoneTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHARPENED_BONE);
					SharpenedBoneTimer = 4*IN_MILLISECONDS;
				}
			}
			else SharpenedBoneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cultist_corrupterAI(creature);
    }
};

enum VrykulNecrolord
{
	SPELL_VR_SHADOW_BOLT = 9613,
	SPELL_TUG_SOUL       = 50027,
	SPELL_SHADOW_NOVA    = 32712,
};

class npc_vrykul_necrolord : public CreatureScript
{
public:
    npc_vrykul_necrolord() : CreatureScript("npc_vrykul_necrolord") {}

    struct npc_vrykul_necrolordAI : public QuantumBasicAI
    {
        npc_vrykul_necrolordAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TugSoulTimer;
		uint32 ShadowNovaTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			TugSoulTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 1.5*IN_MILLISECONDS;
			ShadowNovaTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TugSoulTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TUG_SOUL);
					TugSoulTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else TugSoulTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VR_SHADOW_BOLT);
					ShadowBoltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ShadowNovaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_NOVA);
					ShadowNovaTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vrykul_necrolordAI(creature);
    }
};

enum HulkingAbomination
{
	SPELL_SCOURGE_HOOK     = 50335,
	SPELL_EXPLODING_CORPSE = 58995,
	SPELL_CLEAVE           = 40504,
};

class npc_hulking_abomination : public CreatureScript
{
public:
    npc_hulking_abomination() : CreatureScript("npc_hulking_abomination") {}

    struct npc_hulking_abominationAI : public QuantumBasicAI
    {
        npc_hulking_abominationAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SCOURGE_HOOK);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_EXPLODING_CORPSE, true);
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
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hulking_abominationAI(creature);
    }
};

enum MaleficNecromancer
{
	SPELL_BONE_AMOR      = 50324,
	SPELL_ML_SHADOW_BOLT = 34344,
};

class npc_malefic_necromancer : public CreatureScript
{
public:
    npc_malefic_necromancer() : CreatureScript("npc_malefic_necromancer") {}

    struct npc_malefic_necromancerAI : public QuantumBasicAI
    {
        npc_malefic_necromancerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltTimer;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_BONE_AMOR);
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
					DoCast(target, SPELL_ML_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_malefic_necromancerAI(creature);
    }
};

enum UndyingMinion
{
	SPELL_NECROTIC_STRIKE = 60626,
};

class npc_undying_minion : public CreatureScript
{
public:
    npc_undying_minion() : CreatureScript("npc_undying_minion") {}

    struct npc_undying_minionAI : public QuantumBasicAI
    {
        npc_undying_minionAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NecroticStrikeTimer;

        void Reset()
        {
			NecroticStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NecroticStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_NECROTIC_STRIKE);
				NecroticStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else NecroticStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_undying_minionAI(creature);
    }
};

enum ScourgeConverter
{
	SPELL_GRIP_OF_THE_SCOURGE = 60212,
	SPELL_FROST_NOVA          = 11831,
	SPELL_CONE_OF_COLD        = 20828,
	SPELL_FROSTBOLT           = 20822,
};

class npc_scourge_converter : public CreatureScript
{
public:
    npc_scourge_converter() : CreatureScript("npc_scourge_converter") {}

    struct npc_scourge_converterAI : public QuantumBasicAI
    {
        npc_scourge_converterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBoltTimer;
		uint32 ConeOfColdTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FrostBoltTimer = 0.5*IN_MILLISECONDS;
			ConeOfColdTimer = 2*IN_MILLISECONDS;
			FrostNovaTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostBoltTimer -= diff;

			if (ConeOfColdTimer <= diff)
			{
				DoCast(SPELL_CONE_OF_COLD);
				ConeOfColdTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ConeOfColdTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_converterAI(creature);
    }
};

enum ScourgeBannerBearer
{
	SPELL_SCOURGE_BANNER_BEARER = 59942,
	SPELL_SCOURGE_BANNER_AURA   = 60023,
	SPELL_SCB_SHADOW_NOVA       = 32712,
	SPELL_SHADOW_SHOCK          = 16583,
};

class npc_scourge_banner_bearer : public CreatureScript
{
public:
    npc_scourge_banner_bearer() : CreatureScript("npc_scourge_banner_bearer") {}

    struct npc_scourge_banner_bearerAI : public QuantumBasicAI
    {
        npc_scourge_banner_bearerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowShockTimer;
		uint32 ShadowNovaTimer;

        void Reset()
        {
			ShadowShockTimer = 0.5*IN_MILLISECONDS;
			ShadowNovaTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_SCOURGE_BANNER_BEARER);
			DoCast(me, SPELL_SCOURGE_BANNER_AURA);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_SHOCK);
					ShadowShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowShockTimer -= diff;

			if (ShadowNovaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SCB_SHADOW_NOVA);
					ShadowNovaTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_banner_bearerAI(creature);
    }
};

enum VargulWanderer
{
	SPELL_DIMINISH_SOUL = 36789,
};

class npc_vargul_wanderer : public CreatureScript
{
public:
    npc_vargul_wanderer() : CreatureScript("npc_vargul_wanderer") {}

    struct npc_vargul_wandererAI : public QuantumBasicAI
    {
        npc_vargul_wandererAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DiminishSoulTimer;

        void Reset()
        {
			DiminishSoulTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DiminishSoulTimer <= diff)
			{
				DoCastVictim(SPELL_DIMINISH_SOUL);
				DiminishSoulTimer = urand(4.5*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DiminishSoulTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vargul_wandererAI(creature);
    }
};

enum ScourgeSoulbinder
{
	SPELL_FROST_BLAST   = 60814,
	SPELL_CHAINS_OF_ICE = 22744,
	SPELL_DRAIN_LIFE    = 17620,
};

class npc_scourge_soulbinder : public CreatureScript
{
public:
    npc_scourge_soulbinder() : CreatureScript("npc_scourge_soulbinder") {}

    struct npc_scourge_soulbinderAI : public QuantumBasicAI
    {
        npc_scourge_soulbinderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBlastTimer;
		uint32 ChainsOfIceTimer;
		uint32 DrainLifeTimer;

        void Reset()
        {
			FrostBlastTimer = 3*IN_MILLISECONDS;
			ChainsOfIceTimer = 5*IN_MILLISECONDS;
			DrainLifeTimer = 7*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBlastTimer <= diff)
			{
				DoCastVictim(SPELL_FROST_BLAST);
				FrostBlastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostBlastTimer -= diff;

			if (ChainsOfIceTimer <= diff)
			{
				DoCastVictim(SPELL_CHAINS_OF_ICE);
				ChainsOfIceTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else ChainsOfIceTimer -= diff;

			if (DrainLifeTimer <= diff)
			{
				DoCastVictim(SPELL_DRAIN_LIFE);
				DrainLifeTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else DrainLifeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_soulbinderAI(creature);
    }
};

enum PustulentColossus
{
	SPELL_KNOCK_BACK = 28405,
	SPELL_STOMP      = 63546,
};

class npc_pustulent_colossus : public CreatureScript
{
public:
    npc_pustulent_colossus() : CreatureScript("npc_pustulent_colossus") {}

    struct npc_pustulent_colossusAI : public QuantumBasicAI
    {
        npc_pustulent_colossusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockbackTimer;
		uint32 StompTimer;

        void Reset()
        {
			KnockbackTimer = 2*IN_MILLISECONDS;
			StompTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KnockbackTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCK_BACK);
				KnockbackTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else KnockbackTimer -= diff;

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
        return new npc_pustulent_colossusAI(creature);
    }
};

enum NecroticWebspinner
{
	SPELL_POISON = 744,
	SPELL_WEB    = 745,
};

class npc_necrotic_webspinner : public CreatureScript
{
public:
    npc_necrotic_webspinner() : CreatureScript("npc_necrotic_webspinner") {}

    struct npc_necrotic_webspinnerAI : public QuantumBasicAI
    {
        npc_necrotic_webspinnerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonTimer;

        void Reset()
        {
			PoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_WEB);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_POISON);
				PoisonTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_necrotic_webspinnerAI(creature);
    }
};

enum SkeletalArchmage
{
	SPELL_FIREBALL        = 15242,
	SPELL_SA_CONE_OF_COLD = 15244,
	SPELL_COUNTERSPELL    = 15122,
	SPELL_FROST_ARMOR     = 18100,
};

class npc_skeletal_archmage : public CreatureScript
{
public:
    npc_skeletal_archmage() : CreatureScript("npc_skeletal_archmage") {}

    struct npc_skeletal_archmageAI : public QuantumBasicAI
    {
        npc_skeletal_archmageAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FireballTimer;
		uint32 ConeOfColdTimer;
		uint32 CounterSpellTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;
			ConeOfColdTimer = 3*IN_MILLISECONDS;
			CounterSpellTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FrostArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_FROST_ARMOR))
				DoCast(me, SPELL_FROST_ARMOR, true);

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (ConeOfColdTimer <= diff)
			{
				DoCast(SPELL_SA_CONE_OF_COLD);
				ConeOfColdTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ConeOfColdTimer -= diff;

			if (CounterSpellTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_COUNTERSPELL);
					CounterSpellTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else CounterSpellTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_archmageAI(creature);
    }
};

enum Deathchill
{
	SPELL_TD_FROST_ARMOR          = 18100,
	SPELL_DEATHCHILL_EMPOWERMENT  = 59663,
	SPELL_FEAR                    = 12096,
	SPELL_TD_FROSTBOLT            = 61747,
	SPELL_CHILL_NOVA              = 18099,
	SPELL_LICH_SLAP               = 60924,
};

class npc_thexal_deathchill : public CreatureScript
{
public:
    npc_thexal_deathchill() : CreatureScript("npc_thexal_deathchill") {}

    struct npc_thexal_deathchillAI : public QuantumBasicAI
    {
        npc_thexal_deathchillAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FearTimer;
		uint32 FrostboltTimer;
		uint32 ChillNovaTimer;
		uint32 LichSlapTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FearTimer = 0.5*IN_MILLISECONDS;
			FrostboltTimer = 1.5*IN_MILLISECONDS;
			LichSlapTimer = 3*IN_MILLISECONDS;
			ChillNovaTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DEATHCHILL_EMPOWERMENT, true);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FrostArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_TD_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_TD_FROST_ARMOR))
				DoCast(me, SPELL_TD_FROST_ARMOR, true);

			if (FearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FearTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TD_FROSTBOLT);
					FrostboltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (LichSlapTimer <= diff)
			{
				DoCastVictim(SPELL_LICH_SLAP);
				LichSlapTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else LichSlapTimer -= diff;

			if (ChillNovaTimer <= diff)
			{
				DoCastAOE(SPELL_CHILL_NOVA);
				ChillNovaTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ChillNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thexal_deathchillAI(creature);
    }
};

class npc_harbinger_of_horror : public CreatureScript
{
public:
    npc_harbinger_of_horror() : CreatureScript("npc_harbinger_of_horror") {}

    struct npc_harbinger_of_horrorAI : public QuantumBasicAI
    {
        npc_harbinger_of_horrorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FearTimer;
		uint32 FrostboltTimer;
		uint32 ChillNovaTimer;
		uint32 LichSlapTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FearTimer = 0.5*IN_MILLISECONDS;
			FrostboltTimer = 1.5*IN_MILLISECONDS;
			LichSlapTimer = 3*IN_MILLISECONDS;
			ChillNovaTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DEATHCHILL_EMPOWERMENT, true);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FrostArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_TD_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_TD_FROST_ARMOR))
				DoCast(me, SPELL_TD_FROST_ARMOR, true);

			if (FearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FearTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TD_FROSTBOLT);
					FrostboltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (LichSlapTimer <= diff)
			{
				DoCastVictim(SPELL_LICH_SLAP);
				LichSlapTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else LichSlapTimer -= diff;

			if (ChillNovaTimer <= diff)
			{
				DoCastAOE(SPELL_CHILL_NOVA);
				ChillNovaTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ChillNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_harbinger_of_horrorAI(creature);
    }
};

enum OrbazBloodbane
{
	SPELL_EMPOWER_RUNEBLADE = 61015,
	SPELL_PLAGUE_STRIKE     = 60186,
};

class npc_orbaz_bloodbane : public CreatureScript
{
public:
    npc_orbaz_bloodbane() : CreatureScript("npc_orbaz_bloodbane") {}

    struct npc_orbaz_bloodbaneAI : public QuantumBasicAI
    {
        npc_orbaz_bloodbaneAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PlagueStrikeTimer;
		uint32 RunebladeTimer;

        void Reset()
        {
			PlagueStrikeTimer = 2*IN_MILLISECONDS;
			RunebladeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			if (RunebladeTimer <= diff)
			{
				DoCast(SPELL_EMPOWER_RUNEBLADE);
				RunebladeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RunebladeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orbaz_bloodbaneAI(creature);
    }
};

enum WarsongRaider
{
	SPELL_DEMORALIZING_SHOOT = 13730,
	SPELL_HEROIC_STRIKE      = 25710,
	SPELL_INTERCEPT          = 27577,
};

class npc_warsong_raider : public CreatureScript
{
public:
    npc_warsong_raider() : CreatureScript("npc_warsong_raider") {}

    struct npc_warsong_raiderAI : public QuantumBasicAI
    {
        npc_warsong_raiderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DemoralizingTimer;
		uint32 HeroicStrikeTimer;

        void Reset()
        {
			DemoralizingTimer = 1*IN_MILLISECONDS;
			HeroicStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_INTERCEPT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DemoralizingTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOOT);
				DemoralizingTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else DemoralizingTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HEROIC_STRIKE);
				HeroicStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HeroicStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warsong_raiderAI(creature);
    }
};

enum PlaguedFiend
{
	SPELL_DISEASE_CLOUD = 50106,
	SPELL_PLAGUE_BITE   = 60678,
};

class npc_plagued_fiend : public CreatureScript
{
public:
    npc_plagued_fiend() : CreatureScript("npc_plagued_fiend") {}

    struct npc_plagued_fiendAI : public QuantumBasicAI
    {
        npc_plagued_fiendAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PlagueBiteTimer;

        void Reset()
        {
			PlagueBiteTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_DISEASE_CLOUD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueBiteTimer <= diff)
			{
				DoCastAOE(SPELL_PLAGUE_BITE);
				PlagueBiteTimer = 4*IN_MILLISECONDS;
			}
			else PlagueBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plagued_fiendAI(creature);
    }
};

enum HulkingHorror
{
	SPELL_INFECTED_BITE = 49861,
	SPELL_UPPERCUT      = 10966,
};

class npc_hulking_horror : public CreatureScript
{
public:
    npc_hulking_horror() : CreatureScript("npc_hulking_horror") {}

    struct npc_hulking_horrorAI : public QuantumBasicAI
    {
        npc_hulking_horrorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedBiteTimer;
		uint32 UppercutTimer;

        void Reset()
        {
			InfectedBiteTimer = 2*IN_MILLISECONDS;
			UppercutTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedBiteTimer <= diff)
			{
				DoCastVictim(SPELL_INFECTED_BITE);
				InfectedBiteTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else InfectedBiteTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else UppercutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hulking_horrorAI(creature);
    }
};

enum ValianceCommando
{
	SPELL_PUNCTURE_ARMOR = 35918,
	SPELL_SHOOT          = 15547,
	SPELL_DETERENCE      = 31567,
};

class npc_valiance_commando : public CreatureScript
{
public:
    npc_valiance_commando() : CreatureScript("npc_valiance_commando") {}

    struct npc_valiance_commandoAI : public QuantumBasicAI
    {
        npc_valiance_commandoAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PunctureArmorTimer;
		uint32 DeterenceTimer;

        void Reset()
        {
			PunctureArmorTimer = 1*IN_MILLISECONDS;
			DeterenceTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHOOT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PunctureArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PUNCTURE_ARMOR);
				PunctureArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PunctureArmorTimer -= diff;

			if (DeterenceTimer <= diff)
			{
				DoCast(SPELL_DETERENCE);
				DeterenceTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else DeterenceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valiance_commandoAI(creature);
    }
};

enum IceskinSentry
{
	SPELL_ICESKIN_STONEFORM = 58269,
};

class npc_iceskin_sentry : public CreatureScript
{
public:
    npc_iceskin_sentry() : CreatureScript("npc_iceskin_sentry") {}

    struct npc_iceskin_sentryAI : public QuantumBasicAI
    {
        npc_iceskin_sentryAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustRespawned()
		{
			DoCast(me, SPELL_ICESKIN_STONEFORM);
		}

		void JustReachedHome()
		{
			DoCast(me, SPELL_ICESKIN_STONEFORM);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_ICESKIN_STONEFORM);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iceskin_sentryAI(creature);
    }
};

enum LumberingAtrocity
{
	SPELL_LA_CLEAVE = 40504,
};

class npc_lumbering_atrocity : public CreatureScript
{
public:
    npc_lumbering_atrocity() : CreatureScript("npc_lumbering_atrocity") {}

    struct npc_lumbering_atrocityAI : public QuantumBasicAI
    {
        npc_lumbering_atrocityAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_LA_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lumbering_atrocityAI(creature);
    }
};

enum SaroniteShaper
{
	SPELL_WAR_STOMP = 60960,
};

class npc_saronite_shaper : public CreatureScript
{
public:
    npc_saronite_shaper() : CreatureScript("npc_saronite_shaper") {}

    struct npc_saronite_shaperAI : public QuantumBasicAI
    {
        npc_saronite_shaperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WarStompTimer;

        void Reset()
        {
			WarStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = 8*IN_MILLISECONDS;
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_saronite_shaperAI(creature);
    }
};

enum SkeletalRunesmith
{
	SPELL_SR_CLEAVE    = 42724,
	SPELL_PIERCE_ARMOR = 46202,
};

class npc_skeletal_runesmith : public CreatureScript
{
public:
    npc_skeletal_runesmith() : CreatureScript("npc_skeletal_runesmith") {}

    struct npc_skeletal_runesmithAI : public QuantumBasicAI
    {
        npc_skeletal_runesmithAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 PierceArmorTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			PierceArmorTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_SR_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCE_ARMOR);
				PierceArmorTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_runesmithAI(creature);
    }
};

enum UmbralBrute
{
	SPELL_UB_UPPERCUT = 10966,
	SPELL_ENRAGE      = 50420,
	SPELL_STRIKE      = 13446,
};

class npc_umbral_brute : public CreatureScript
{
public:
    npc_umbral_brute() : CreatureScript("npc_umbral_brute") {}

    struct npc_umbral_bruteAI : public QuantumBasicAI
    {
        npc_umbral_bruteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 UppercutTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;
			UppercutTimer = 4*IN_MILLISECONDS;

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

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UB_UPPERCUT);
				UppercutTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else UppercutTimer -= diff;

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
        return new npc_umbral_bruteAI(creature);
    }
};

enum ShamblingZombie
{
	SPELL_PLAGUE_BLAST = 61095,
};

class npc_shambling_zombie : public CreatureScript
{
public:
    npc_shambling_zombie() : CreatureScript("npc_shambling_zombie") {}

    struct npc_shambling_zombieAI : public QuantumBasicAI
    {
        npc_shambling_zombieAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PlagueBlastTimer;

        void Reset()
        {
			PlagueBlastTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueBlastTimer <= diff)
			{
				DoCastAOE(SPELL_PLAGUE_BLAST);
				PlagueBlastTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PlagueBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shambling_zombieAI(creature);
    }
};

enum Rokir
{
	SPELL_DEATH_DECAY      = 61112,
	SPELL_RK_PLAGUE_STRIKE = 61109,
	SPELL_ZOMBIE_HORDE     = 61088,

	NPC_SUMMONED_ZOMBIE    = 32499,
};

class npc_rokir : public CreatureScript
{
public:
    npc_rokir() : CreatureScript("npc_rokir") {}

    struct npc_rokirAI : public QuantumBasicAI
    {
        npc_rokirAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 PlagueStrikeTimer;
		uint32 DeathDecayTimer;
		uint32 ZombieHordeTimer;

		SummonList Summons;

        void Reset()
        {
			PlagueStrikeTimer = 4*IN_MILLISECONDS;
			DeathDecayTimer = 5*IN_MILLISECONDS;
			ZombieHordeTimer = 10*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SUMMONED_ZOMBIE)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_RK_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			if (DeathDecayTimer <= diff)
			{
				DoCastAOE(SPELL_DEATH_DECAY);
				DeathDecayTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else DeathDecayTimer -= diff;

			if (ZombieHordeTimer <= diff)
			{
				DoCast(SPELL_ZOMBIE_HORDE);
				ZombieHordeTimer = urand(18*IN_MILLISECONDS, 20*IN_MILLISECONDS);
			}
			else ZombieHordeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rokirAI(creature);
    }
};

enum Sapph
{
	SPELL_SF_CHAINS_OF_ICE = 22744,
	SPELL_NUNGERING_COLD   = 61058,
};

class npc_sapph : public CreatureScript
{
public:
    npc_sapph() : CreatureScript("npc_sapph") {}

    struct npc_sapphAI : public QuantumBasicAI
    {
        npc_sapphAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainsOfIceTimer;
		uint32 HungeringColdTimer;

        void Reset()
        {
			ChainsOfIceTimer = 1*IN_MILLISECONDS;
			HungeringColdTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChainsOfIceTimer <= diff)
			{
				DoCastVictim(SPELL_SF_CHAINS_OF_ICE);
				ChainsOfIceTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ChainsOfIceTimer -= diff;

			if (HungeringColdTimer <= diff)
			{
				DoCastAOE(SPELL_NUNGERING_COLD);
				HungeringColdTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HungeringColdTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sapphAI(creature);
    }
};

enum SpikedGhoul
{
	SPELL_CLAW_SLASH   = 54185,
	SPELL_JAGGED_SPIKE = 60876,
};

class npc_spiked_ghoul : public CreatureScript
{
public:
    npc_spiked_ghoul() : CreatureScript("npc_spiked_ghoul") {}

    struct npc_spiked_ghoulAI : public QuantumBasicAI
    {
        npc_spiked_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ClawSlashTimer;
		uint32 JaggedSpikeTimer;

        void Reset()
        {
			ClawSlashTimer = 2*IN_MILLISECONDS;
			JaggedSpikeTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ClawSlashTimer <= diff)
			{
				DoCastVictim(SPELL_CLAW_SLASH);
				ClawSlashTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ClawSlashTimer -= diff;

			if (JaggedSpikeTimer <= diff)
			{
				DoCastVictim(SPELL_JAGGED_SPIKE);
				JaggedSpikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else JaggedSpikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spiked_ghoulAI(creature);
    }
};

enum IntrepidGhoul
{
	SPELL_FESTERING_BITE = 60873,
	SPELL_RAVENOUS_CLAW  = 60872,
	SPELL_GHOULEXPLOISON = 58137,
};

class npc_intrepid_ghoul : public CreatureScript
{
public:
    npc_intrepid_ghoul() : CreatureScript("npc_intrepid_ghoul") {}

    struct npc_intrepid_ghoulAI : public QuantumBasicAI
    {
        npc_intrepid_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FesteringBiteTimer;
		uint32 RavenousClawTimer;

        void Reset()
        {
			FesteringBiteTimer = 2*IN_MILLISECONDS;
			RavenousClawTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FesteringBiteTimer <= diff)
			{
				DoCastVictim(SPELL_FESTERING_BITE);
				FesteringBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FesteringBiteTimer -= diff;

			if (RavenousClawTimer <= diff)
			{
				DoCastVictim(SPELL_RAVENOUS_CLAW);
				RavenousClawTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RavenousClawTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
				DoCastAOE(SPELL_GHOULEXPLOISON);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_intrepid_ghoulAI(creature);
    }
};

enum ScourgebeakFleshripper
{
	SPELL_SWOOP = 55082,
};

class npc_scourgebeak_fleshripper : public CreatureScript
{
public:
    npc_scourgebeak_fleshripper() : CreatureScript("npc_scourgebeak_fleshripper") {}

    struct npc_scourgebeak_fleshripperAI : public QuantumBasicAI
    {
        npc_scourgebeak_fleshripperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SwoopTimer;

        void Reset()
        {
			SwoopTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SwoopTimer <= diff)
			{
				DoCastVictim(SPELL_SWOOP);
				SwoopTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SwoopTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourgebeak_fleshripperAI(creature);
    }
};

enum SkeletalConstructor
{
	SPELL_BONE_ARMOR     = 50324,
	SPELL_SC_SHADOW_BOLT = 9613,
};

class npc_skeletal_constructor : public CreatureScript
{
public:
    npc_skeletal_constructor() : CreatureScript("npc_skeletal_constructor") {}

    struct npc_skeletal_constructorAI : public QuantumBasicAI
    {
        npc_skeletal_constructorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoneArmorTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			BoneArmorTimer = 0.1*IN_MILLISECONDS;
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (BoneArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_BONE_ARMOR);
				BoneArmorTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else BoneArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Bone Armor Buff Check in Combat
			if (!me->HasAura(SPELL_BONE_ARMOR))
				DoCast(me, SPELL_BONE_ARMOR, true);

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SC_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_constructorAI(creature);
    }
};

enum MasterSummonerZarod
{
	SPELL_MSZ_FROSTBOLT  = 9672,
	SPELL_MSZ_FROST_NOVA = 11831,
	SPELL_MSZ_LICH_SLAP  = 28873,
};

class npc_master_summoner_zarod : public CreatureScript
{
public:
	npc_master_summoner_zarod() : CreatureScript("npc_master_summoner_zarod") {}

    struct npc_master_summoner_zarodAI : public QuantumBasicAI
    {
        npc_master_summoner_zarodAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;
		uint32 LichSlapTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 2*IN_MILLISECONDS;
			LichSlapTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_MSZ_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_MSZ_FROST_NOVA);
				FrostNovaTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			if (LichSlapTimer <= diff)
			{
				DoCastVictim(SPELL_MSZ_LICH_SLAP);
				LichSlapTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else LichSlapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_summoner_zarodAI(creature);
    }
};

enum CultistShardWatcher
{
	SPELL_SHADOW_BOLT_VOLLEY  = 39175,
	SPELL_SHADOW_WORD_PAIN    = 34441,
	SPELL_UNSTABLE_AFFLICTION = 34439,
};

class npc_cultist_shard_watcher : public CreatureScript
{
public:
    npc_cultist_shard_watcher() : CreatureScript("npc_cultist_shard_watcher") {}

    struct npc_cultist_shard_watcherAI : public QuantumBasicAI
    {
        npc_cultist_shard_watcherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PurpleBeamTimer;
		uint32 ShadowBoltVolleyTimer;
		uint32 ShadowWordPainTimer;
		uint32 UnstableAfflictionTimer;

        void Reset()
        {
			PurpleBeamTimer = 0.1*IN_MILLISECONDS;
			ShadowBoltVolleyTimer = 0.5*IN_MILLISECONDS;
			ShadowWordPainTimer = 1*IN_MILLISECONDS;
			UnstableAfflictionTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* stalker = me->FindCreatureWithDistance(NPC_BEAM_STALKER, 75.0f))
				{
					me->SetFacingToObject(stalker);
					DoCast(stalker, SPELL_ICECROWN_PURPLE_BEAM, true);
					PurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
					ShadowBoltVolleyTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltVolleyTimer -= diff;

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_WORD_PAIN);
					ShadowWordPainTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (UnstableAfflictionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UNSTABLE_AFFLICTION);
					UnstableAfflictionTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else UnstableAfflictionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cultist_shard_watcherAI(creature);
    }
};

enum YmirjarElementShaper
{ 
	SPELL_AVALANCHE  = 55216,
	SPELL_TORN_EARTH = 61897,
};

class npc_ymirjar_element_shaper : public CreatureScript
{
public:
    npc_ymirjar_element_shaper() : CreatureScript("npc_ymirjar_element_shaper") {}

    struct npc_ymirjar_element_shaperAI : public QuantumBasicAI
    {
        npc_ymirjar_element_shaperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 AvalancheTimer;
		uint32 TornEarthTimer;

        void Reset()
        {
			AvalancheTimer = 0.5*IN_MILLISECONDS;
			TornEarthTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AvalancheTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AVALANCHE);
					AvalancheTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else AvalancheTimer -= diff;

			if (TornEarthTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TORN_EARTH);
					TornEarthTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else TornEarthTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_element_shaperAI(creature);
    }
};

enum YmirheimChosenWarrior
{
	SPELL_JUMP_ATTACK = 61227,
	SPELL_YMCW_CLEAVE = 15496,
	SPELL_BLOODTHIRST = 35949,
};

class npc_ymirheim_chosen_warrior : public CreatureScript
{
public:
    npc_ymirheim_chosen_warrior() : CreatureScript("npc_ymirheim_chosen_warrior") {}

    struct npc_ymirheim_chosen_warriorAI : public QuantumBasicAI
    {
        npc_ymirheim_chosen_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 BloodthirstTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			BloodthirstTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_JUMP_ATTACK);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_YMCW_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(SPELL_BLOODTHIRST);
				BloodthirstTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else BloodthirstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirheim_chosen_warriorAI(creature);
    }
};

enum SavageProtoDrake
{
	SPELL_FLAME_BREATH = 51219,
	SPELL_WING_BUFFET  = 41572,
};

class npc_savage_proto_drake : public CreatureScript
{
public:
    npc_savage_proto_drake() : CreatureScript("npc_savage_proto_drake") {}

    struct npc_savage_proto_drakeAI : public QuantumBasicAI
    {
        npc_savage_proto_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlameBreathTimer;
		uint32 WingBuffetTimer;

        void Reset()
        {
			FlameBreathTimer = 2*IN_MILLISECONDS;
			WingBuffetTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameBreathTimer <= diff)
			{
				DoCastAOE(SPELL_FLAME_BREATH);
				FlameBreathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FlameBreathTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_WING_BUFFET);
				WingBuffetTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_savage_proto_drakeAI(creature);
    }
};

enum YmirheimSpearGun
{
	SPELL_LAUNCH_SPEAR = 59894,
};

class npc_ymirheim_spear_gun : public CreatureScript
{
public:
    npc_ymirheim_spear_gun() : CreatureScript("npc_ymirheim_spear_gun") {}

    struct npc_ymirheim_spear_gunAI : public QuantumBasicAI
    {
        npc_ymirheim_spear_gunAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 LaunchSpearTimer;

        void Reset()
        {
			LaunchSpearTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LaunchSpearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LAUNCH_SPEAR);
					LaunchSpearTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LaunchSpearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirheim_spear_gunAI(creature);
    }
};

enum YmirheimDefender
{
	SPELL_THROW_HARPOON  = 59633,
	SPELL_POISONED_SPEAR = 60988,
};

class npc_ymirheim_defender : public CreatureScript
{
public:
    npc_ymirheim_defender() : CreatureScript("npc_ymirheim_defender") {}

    struct npc_ymirheim_defenderAI : public QuantumBasicAI
    {
        npc_ymirheim_defenderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowHarpoonTimer;
		uint32 PoisonedSpearTimer;

        void Reset()
        {
			ThrowHarpoonTimer = 2*IN_MILLISECONDS;
			PoisonedSpearTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				DoCast(target, SPELL_THROW_HARPOON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowHarpoonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_THROW_HARPOON);
					ThrowHarpoonTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ThrowHarpoonTimer -= diff;

			if (PoisonedSpearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POISONED_SPEAR);
					PoisonedSpearTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PoisonedSpearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirheim_defenderAI(creature);
    }
};

enum ArgentChampion
{
	SPELL_HEROIC_LEAP = 53625,
};

class npc_argent_champion_ic : public CreatureScript
{
public:
    npc_argent_champion_ic() : CreatureScript("npc_argent_champion_ic") {}

    struct npc_argent_champion_icAI : public QuantumBasicAI
    {
        npc_argent_champion_icAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_HEROIC_LEAP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_champion_icAI(creature);
    }
};

enum FrostbroodDestroyer
{
	SPELL_FREEZING_BREATH = 57477,
	SPELL_FD_WING_BUFFET  = 53363,
};

class npc_frostbrood_destroyer : public CreatureScript
{
public:
    npc_frostbrood_destroyer() : CreatureScript("npc_frostbrood_destroyer") {}

    struct npc_frostbrood_destroyerAI : public QuantumBasicAI
    {
        npc_frostbrood_destroyerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FreezingBreathTimer;
		uint32 WingBuffetTimer;

        void Reset()
        {
			FreezingBreathTimer = 5*IN_MILLISECONDS;
			WingBuffetTimer = 7*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FreezingBreathTimer <= diff)
			{
				DoCastAOE(SPELL_FREEZING_BREATH);
				FreezingBreathTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FreezingBreathTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_FD_WING_BUFFET);
				WingBuffetTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostbrood_destroyerAI(creature);
    }
};

enum HiglorldTirionFordring
{
	SPELL_GREATER_TURN_EVIL = 57546,
	SPELL_ASHBRINGER        = 57545,
	SPELL_TIRION_AGGRO      = 57550,
};

class npc_highlord_tirion_fordring_av : public CreatureScript
{
public:
    npc_highlord_tirion_fordring_av() : CreatureScript("npc_highlord_tirion_fordring_av") {}

    struct npc_highlord_tirion_fordring_avAI : public QuantumBasicAI
    {
        npc_highlord_tirion_fordring_avAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TurnEvilTimer;

        void Reset()
        {
			TurnEvilTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_TIRION_AGGRO);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_ASHBRINGER);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TurnEvilTimer <= diff)
			{
				DoCastVictim(SPELL_GREATER_TURN_EVIL);
				TurnEvilTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else TurnEvilTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highlord_tirion_fordring_avAI(creature);
    }
};

enum ReanimatedCaptain
{
	SPELL_AVENGERS_SHIELD     = 32674,
	SPELL_HAMMER_OF_INJUSTICE = 58154,
	SPELL_UNHOLY_LIGHT        = 58153,
};

class npc_reanimated_captain : public CreatureScript
{
public:
    npc_reanimated_captain() : CreatureScript("npc_reanimated_captain") {}

    struct npc_reanimated_captainAI : public QuantumBasicAI
    {
        npc_reanimated_captainAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HammerOfInjusticeTimer;

        void Reset()
        {
			HammerOfInjusticeTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_AVENGERS_SHIELD);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfInjusticeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HAMMER_OF_INJUSTICE);
					HammerOfInjusticeTimer = 7*IN_MILLISECONDS;
				}
			}
			else HammerOfInjusticeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
				DoCast(me, SPELL_UNHOLY_LIGHT);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reanimated_captainAI(creature);
    }
};

enum HalofTheDeathbringer
{
	SPELL_BLOODBOIL_R4  = 49941,
	SPELL_DEATH_COIL_R5 = 49895,
	SPELL_ICY_TOUCH_R5  = 49909,
	SPELL_OBLITERATE    = 51425,
};

class npc_halof_the_deathbringer : public CreatureScript
{
public:
    npc_halof_the_deathbringer() : CreatureScript("npc_halof_the_deathbringer") {}

    struct npc_halof_the_deathbringerAI : public QuantumBasicAI
    {
        npc_halof_the_deathbringerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IcyTouchTimer;
		uint32 BloodBoilTimer;
		uint32 ObliterateTimer;
		uint32 DeathCoilTimer;

        void Reset()
        {
			IcyTouchTimer = 0.5*IN_MILLISECONDS;
			BloodBoilTimer = 2*IN_MILLISECONDS;
			ObliterateTimer = 3*IN_MILLISECONDS;
			DeathCoilTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH_R5);
				IcyTouchTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else IcyTouchTimer -= diff;

			if (BloodBoilTimer <= diff)
			{
				DoCastAOE(SPELL_BLOODBOIL_R4);
				BloodBoilTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else BloodBoilTimer -= diff;

			if (ObliterateTimer <= diff)
			{
				DoCastVictim(SPELL_OBLITERATE);
				ObliterateTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ObliterateTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				DoCastVictim(SPELL_DEATH_COIL_R5);
				DeathCoilTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else DeathCoilTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_halof_the_deathbringerAI(creature);
    }
};

enum ScourgeDrudge
{
	SPELL_SD_CLEAVE = 51917,
	SPELL_FLESH_ROT = 49678,
};

class npc_scourge_drudge : public CreatureScript
{
public:
    npc_scourge_drudge() : CreatureScript("npc_scourge_drudge") {}

    struct npc_scourge_drudgeAI : public QuantumBasicAI
    {
        npc_scourge_drudgeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 FleshRotTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			FleshRotTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_SD_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (FleshRotTimer <= diff)
			{
				DoCastVictim(SPELL_FLESH_ROT);
				FleshRotTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FleshRotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_drudgeAI(creature);
    }
};

enum FrosthowlScreecher
{
	SPELL_FS_SHADOWBOLT = 52257,
};

class npc_frosthowl_screecher : public CreatureScript
{
public:
    npc_frosthowl_screecher() : CreatureScript("npc_frosthowl_screecher") {}

    struct npc_frosthowl_screecherAI : public QuantumBasicAI
    {
        npc_frosthowl_screecherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowboltTimer;

        void Reset()
        {
			ShadowboltTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FS_SHADOWBOLT);
					ShadowboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frosthowl_screecherAI(creature);
    }
};

enum Hailscorn
{
	SPELL_ICE_CLAW      = 3130,
	SPELL_H_WING_BUFFET = 51144,
};

class npc_hailscorn : public CreatureScript
{
public:
    npc_hailscorn() : CreatureScript("npc_hailscorn") {}

    struct npc_hailscornAI : public QuantumBasicAI
    {
        npc_hailscornAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IceClawTimer;
		uint32 WingBuffetTimer;

        void Reset()
        {
			IceClawTimer = 3*IN_MILLISECONDS;
			WingBuffetTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IceClawTimer <= diff)
			{
				DoCastVictim(SPELL_ICE_CLAW);
				IceClawTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else IceClawTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_H_WING_BUFFET);
				WingBuffetTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hailscornAI(creature);
    }
};

enum UrgrethOfTheThousandTombs
{
	SPELL_MORTAL_STRIKE  = 16856,
	SPELL_SHADOW_BARRAGE = 51074,
};

class npc_urgreth_of_the_thousand_tombs : public CreatureScript
{
public:
    npc_urgreth_of_the_thousand_tombs() : CreatureScript("npc_urgreth_of_the_thousand_tombs") {}

    struct npc_urgreth_of_the_thousand_tombsAI : public QuantumBasicAI
    {
        npc_urgreth_of_the_thousand_tombsAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 ShadowBarrageTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;
			ShadowBarrageTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (ShadowBarrageTimer <= diff)
			{
				DoCastAOE(SPELL_SHADOW_BARRAGE);
				ShadowBarrageTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ShadowBarrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_urgreth_of_the_thousand_tombsAI(creature);
    }
};

enum BythiusTheFleshShaper
{
	SPELL_POISON_SPRAY = 30043,
};

class npc_bythius_the_flesh_shaper : public CreatureScript
{
public:
    npc_bythius_the_flesh_shaper() : CreatureScript("npc_bythius_the_flesh_shaper") {}

    struct npc_bythius_the_flesh_shaperAI : public QuantumBasicAI
    {
        npc_bythius_the_flesh_shaperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonSprayTimer;

        void Reset()
        {
			PoisonSprayTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonSprayTimer <= diff)
			{
				DoCastAOE(SPELL_POISON_SPRAY);
				PoisonSprayTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PoisonSprayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bythius_the_flesh_shaperAI(creature);
    }
};

enum ForgottenDepthsAmbusher
{
	SPELL_FDA_CLEAVE = 40505,
};

class npc_forgotten_depths_ambusher : public CreatureScript
{
public:
    npc_forgotten_depths_ambusher() : CreatureScript("npc_forgotten_depths_ambusher") {}

    struct npc_forgotten_depths_ambusherAI : public QuantumBasicAI
    {
        npc_forgotten_depths_ambusherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_FDA_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_depths_ambusherAI(creature);
    }
};

enum CarrionFleshstripper
{
	SPELL_PARALYTIC_POISON = 35201,
};

class npc_carrion_fleshstripper : public CreatureScript
{
public:
    npc_carrion_fleshstripper() : CreatureScript("npc_carrion_fleshstripper") {}

    struct npc_carrion_fleshstripperAI : public QuantumBasicAI
    {
        npc_carrion_fleshstripperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ParalyticPoisonTimer;

        void Reset()
        {
			ParalyticPoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ParalyticPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_PARALYTIC_POISON);
				ParalyticPoisonTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ParalyticPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_carrion_fleshstripperAI(creature);
    }
};

enum ForgottenDepthsSlayer
{
	SPELL_FDS_CLAW_SLASH = 54185,
	SPELL_WEB_ROPE       = 56223, //WTF????

	SPELL_THE_RECKONING  = 57415,
	SPELL_CANNON_ASSAULT = 57542,
};

class npc_forgotten_depths_slayer : public CreatureScript
{
public:
    npc_forgotten_depths_slayer() : CreatureScript("npc_forgotten_depths_slayer") {}

    struct npc_forgotten_depths_slayerAI : public QuantumBasicAI
    {
        npc_forgotten_depths_slayerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ClawSlashTimer;

        void Reset()
        {
			ClawSlashTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_THE_RECKONING || spell->Id == SPELL_CANNON_ASSAULT)
				me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ClawSlashTimer <= diff)
			{
				DoCastVictim(SPELL_FDS_CLAW_SLASH);
				ClawSlashTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ClawSlashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_depths_slayerAI(creature);
    }
};

enum PlagueDrenchedGhoul
{
	SPELL_PDG_PLAGUE_BITE   = 60678,
	SPELL_PDG_DISIASE_CLOUD = 50106,
};

class npc_plague_drenched_ghoul : public CreatureScript
{
public:
    npc_plague_drenched_ghoul() : CreatureScript("npc_plague_drenched_ghoul") {}

    struct npc_plague_drenched_ghoulAI : public QuantumBasicAI
    {
        npc_plague_drenched_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PlagueBiteTimer;

        void Reset()
        {
			PlagueBiteTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_PDG_DISIASE_CLOUD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueBiteTimer <= diff)
			{
				DoCastAOE(SPELL_PDG_PLAGUE_BITE);
				PlagueBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PlagueBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plague_drenched_ghoulAI(creature);
    }
};

enum RampagingGhoul
{
	SPELL_RG_PIERCE_ARMOR = 6016,
	SPELL_RAMPAGE         = 54475,
};

class npc_rampaging_ghoul : public CreatureScript
{
public:
    npc_rampaging_ghoul() : CreatureScript("npc_rampaging_ghoul") {}

    struct npc_rampaging_ghoulAI : public QuantumBasicAI
    {
        npc_rampaging_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PierceArmorTimer;
		uint32 RampageTimer;

        void Reset()
		{
			PierceArmorTimer = 4*IN_MILLISECONDS;
			RampageTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_RG_PIERCE_ARMOR);
				PierceArmorTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			if (RampageTimer <= diff)
			{
				DoCast(me, SPELL_RAMPAGE);
				RampageTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else RampageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rampaging_ghoulAI(creature);
    }
};

enum SkeletalCraftsman
{
	SPELL_SC_PIERCE_ARMOR = 46202,
	SPELL_SC_CLEAVE       = 42724,
};

class npc_skeletal_craftsman : public CreatureScript
{
public:
    npc_skeletal_craftsman() : CreatureScript("npc_skeletal_craftsman") {}

    struct npc_skeletal_craftsmanAI : public QuantumBasicAI
    {
        npc_skeletal_craftsmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PierceArmorTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			PierceArmorTimer = 2*IN_MILLISECONDS;
			CleaveTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SC_PIERCE_ARMOR);
				PierceArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_SC_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_craftsmanAI(creature);
    }
};

enum WrathstrikeGargoyle
{
	SPELL_GARGOYLE_STRIKE = 16564,
};

class npc_wrathstrike_gargoyle : public CreatureScript
{
public:
    npc_wrathstrike_gargoyle() : CreatureScript("npc_wrathstrike_gargoyle") {}

    struct npc_wrathstrike_gargoyleAI : public QuantumBasicAI
    {
        npc_wrathstrike_gargoyleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GargoyleStrikeTimer;

        void Reset()
		{
			GargoyleStrikeTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GargoyleStrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GARGOYLE_STRIKE);
					GargoyleStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else GargoyleStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrathstrike_gargoyleAI(creature);
    }
};

enum MangalCrocolisk
{
	SPELL_POWERFUL_BITE = 48287,
	SPELL_THICK_HIDE    = 50502,
};

class npc_mangal_crocolisk : public CreatureScript
{
public:
    npc_mangal_crocolisk() : CreatureScript("npc_mangal_crocolisk") {}

    struct npc_mangal_crocoliskAI : public QuantumBasicAI
    {
        npc_mangal_crocoliskAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PowerfulBiteTimer;
		uint32 ThickHideTimer;

        void Reset()
		{
			PowerfulBiteTimer = 2*IN_MILLISECONDS;
			ThickHideTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PowerfulBiteTimer <= diff)
			{
				DoCastVictim(SPELL_POWERFUL_BITE);
				PowerfulBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PowerfulBiteTimer -= diff;

			if (ThickHideTimer <= diff)
			{
				DoCast(me, SPELL_THICK_HIDE);
				ThickHideTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ThickHideTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mangal_crocoliskAI(creature);
    }
};

enum MonstrousWight
{
	SPELL_SMASH = 51334,
};

class npc_monstrous_wight : public CreatureScript
{
public:
    npc_monstrous_wight() : CreatureScript("npc_monstrous_wight") {}

    struct npc_monstrous_wightAI : public QuantumBasicAI
    {
        npc_monstrous_wightAI(Creature* creature) : QuantumBasicAI(creature){}

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
				SmashTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_monstrous_wightAI(creature);
    }
};

enum LongneckGrazer
{
	SPELL_HOOF_STRIKE = 29577,
};

class npc_longneck_grazer : public CreatureScript
{
public:
    npc_longneck_grazer() : CreatureScript("npc_longneck_grazer") {}

    struct npc_longneck_grazerAI : public QuantumBasicAI
    {
        npc_longneck_grazerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HoofStrikeTimer;

        void Reset()
		{
			HoofStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HoofStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HOOF_STRIKE);
				HoofStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HoofStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_longneck_grazerAI(creature);
    }
};

enum SparktouchedWarrior
{
	SPELL_SW_ENRAGE       = 18501,
	SPELL_VICIOUS_ROAR    = 52974,
	SPELL_LIGHTNING_WHIRL = 54429,
};

class npc_sparktouched_warrior : public CreatureScript
{
public:
    npc_sparktouched_warrior() : CreatureScript("npc_sparktouched_warrior") {}

    struct npc_sparktouched_warriorAI : public QuantumBasicAI
    {
        npc_sparktouched_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ViciousRoarTimer;
		uint32 LightningWhirlTimer;

        void Reset()
		{
			ViciousRoarTimer = 2*IN_MILLISECONDS;
			LightningWhirlTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ViciousRoarTimer <= diff)
			{
				DoCastAOE(SPELL_VICIOUS_ROAR);
				ViciousRoarTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ViciousRoarTimer -= diff;

			if (LightningWhirlTimer <= diff)
			{
				DoCastAOE(SPELL_LIGHTNING_WHIRL);
				LightningWhirlTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else LightningWhirlTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_SW_ENRAGE, 0))
				{
					DoCast(me, SPELL_SW_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
			{
				EnterEvadeMode();
				return;
			}

			if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
			{
				EnterEvadeMode();
				return;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sparktouched_warriorAI(creature);
    }
};

enum SparktouchedOracle
{
	SPELL_FORKED_LIGHTNING = 12549,
	SPELL_LIGHTNING_BURST  = 54916,
};

class npc_sparktouched_oracle : public CreatureScript
{
public:
    npc_sparktouched_oracle() : CreatureScript("npc_sparktouched_oracle") {}

    struct npc_sparktouched_oracleAI : public QuantumBasicAI
    {
        npc_sparktouched_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ForkedLightningTimer;
		uint32 LightningBurstTimer;

        void Reset()
		{
			ForkedLightningTimer = 2*IN_MILLISECONDS;
			LightningBurstTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ForkedLightningTimer <= diff)
			{
				DoCastVictim(SPELL_FORKED_LIGHTNING);
				ForkedLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ForkedLightningTimer -= diff;

			if (LightningBurstTimer <= diff)
			{
				DoCastAOE(SPELL_LIGHTNING_BURST);
				LightningBurstTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else LightningBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sparktouched_oracleAI(creature);
    }
};

enum ServantOfFreya
{
	SPELL_FLOWER_POWER  = 55067,
	SPELL_WILD_GROWTH_1 = 52948,
	SPELL_WILD_GROWTH_2 = 61750,
	SPELL_WILD_GROWTH_3 = 61751,
};

class npc_servant_of_freya : public CreatureScript
{
public:
    npc_servant_of_freya() : CreatureScript("npc_servant_of_freya") {}

    struct npc_servant_of_freyaAI : public QuantumBasicAI
    {
        npc_servant_of_freyaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlowerPowerTimer;

        void Reset()
		{
			FlowerPowerTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_WILD_GROWTH_1);
			DoCast(me, SPELL_WILD_GROWTH_2);
			DoCast(me, SPELL_WILD_GROWTH_3);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlowerPowerTimer <= diff)
			{
				DoCast(me, SPELL_FLOWER_POWER);
				FlowerPowerTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FlowerPowerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_servant_of_freyaAI(creature);
    }
};

enum EmprerorCobra
{
	SPELL_POISON_SPIT = 32093,
};

class npc_empreror_cobra : public CreatureScript
{
public:
    npc_empreror_cobra() : CreatureScript("npc_empreror_cobra") {}

    struct npc_empreror_cobraAI : public QuantumBasicAI
    {
        npc_empreror_cobraAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonSpitTimer;

        void Reset()
		{
			PoisonSpitTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonSpitTimer <= diff)
			{
				DoCastVictim(SPELL_POISON_SPIT);
				PoisonSpitTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PoisonSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_empreror_cobraAI(creature);
    }
};

enum OracleSooNee
{
	SPELL_RS_RAINSPEAKER_ORACLE_STATE = 50503,
	SPELL_EARTH_SHOCK                 = 54511,
};

class npc_oracle_soo_nee : public CreatureScript
{
public:
    npc_oracle_soo_nee() : CreatureScript("npc_oracle_soo_nee") {}

    struct npc_oracle_soo_neeAI : public QuantumBasicAI
    {
        npc_oracle_soo_neeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EarthShockTimer;

        void Reset()
		{
			EarthShockTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_RS_RAINSPEAKER_ORACLE_STATE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EarthShockTimer <= diff)
			{
				DoCastVictim(SPELL_EARTH_SHOCK);
				EarthShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else EarthShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oracle_soo_neeAI(creature);
    }
};

enum RainspeakerWarrior
{
	SPELL_RAINSPEAKER_ORACLE_STATE = 50503,
	SPELL_FLIP_ATTACK              = 50533,
};

class npc_rainspeaker_warrior : public CreatureScript
{
public:
    npc_rainspeaker_warrior() : CreatureScript("npc_rainspeaker_warrior") {}

    struct npc_rainspeaker_warriorAI : public QuantumBasicAI
    {
        npc_rainspeaker_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlipAttackTimer;

        void Reset()
		{
			FlipAttackTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_RAINSPEAKER_ORACLE_STATE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlipAttackTimer <= diff)
			{
				DoCastVictim(SPELL_FLIP_ATTACK);
				FlipAttackTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FlipAttackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rainspeaker_warriorAI(creature);
    }
};

enum RainspeakerOracle
{
	SPELL_RR_RAINSPEAKER_ORACLE_STATE = 50503,
	SPELL_CHAIN_LIGHTNING             = 15305,
	SPELL_WARPED_ARMOR                = 54919,
};

class npc_rainspeaker_oracle : public CreatureScript
{
public:
    npc_rainspeaker_oracle() : CreatureScript("npc_rainspeaker_oracle") {}

    struct npc_rainspeaker_oracleAI : public QuantumBasicAI
    {
        npc_rainspeaker_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainLightningTimer;
		uint32 WarpedArmorTimer;

        void Reset()
		{
			ChainLightningTimer = 2*IN_MILLISECONDS;
			WarpedArmorTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_RR_RAINSPEAKER_ORACLE_STATE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChainLightningTimer <= diff)
			{
				DoCastVictim(SPELL_CHAIN_LIGHTNING);
				ChainLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ChainLightningTimer -= diff;

			if (WarpedArmorTimer <= diff)
			{
				DoCastVictim(SPELL_WARPED_ARMOR);
				WarpedArmorTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WarpedArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rainspeaker_oracleAI(creature);
    }
};

enum FrenzyheartBerserker
{
	SPELL_FB_ENRAGE = 50420,
};

class npc_frenzyheart_berserker : public CreatureScript
{
public:
    npc_frenzyheart_berserker() : CreatureScript("npc_frenzyheart_berserker") {}

    struct npc_frenzyheart_berserkerAI : public QuantumBasicAI
    {
        npc_frenzyheart_berserkerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_FB_ENRAGE, 0))
				{
					DoCast(me, SPELL_FB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzyheart_berserkerAI(creature);
    }
};

enum BroodmotherSlivina
{
	SPELL_BS_CLAW         = 51772,
	SPELL_BS_FLAME_BREATH = 51768,
};

class npc_broodmother_slivina : public CreatureScript
{
public:
    npc_broodmother_slivina() : CreatureScript("npc_broodmother_slivina") {}

    struct npc_broodmother_slivinaAI : public QuantumBasicAI
    {
        npc_broodmother_slivinaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ClawTimer;
		uint32 FlameBreathTimer;

        void Reset()
		{
			ClawTimer = 2*IN_MILLISECONDS;
			FlameBreathTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ClawTimer <= diff)
			{
				DoCastVictim(SPELL_BS_CLAW);
				ClawTimer = 4*IN_MILLISECONDS;
			}
			else ClawTimer -= diff;

			if (FlameBreathTimer <= diff)
			{
				DoCastAOE(SPELL_BS_FLAME_BREATH);
				FlameBreathTimer = 6*IN_MILLISECONDS;
			}
			else FlameBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_broodmother_slivinaAI(creature);
    }
};

enum LightningTarget
{
	SPELL_LIGTNING_TO_CLOUD = 51172,
	NPC_LIGHTNING_TARGET    = 28254,
	EVENT_LIGHTNING_CLOUD   = 1,
};

class npc_mistwhisper_lightning_cloud : public CreatureScript
{
public:
    npc_mistwhisper_lightning_cloud() : CreatureScript("npc_mistwhisper_lightning_cloud") {}

    struct npc_mistwhisper_lightning_cloudAI : public QuantumBasicAI
    {
        npc_mistwhisper_lightning_cloudAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
		{
			events.ScheduleEvent(EVENT_LIGHTNING_CLOUD, 12*IN_MILLISECONDS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_LIGHTNING_CLOUD:
						DoCast(SPELL_LIGTNING_TO_CLOUD);
						events.ScheduleEvent(EVENT_LIGHTNING_CLOUD, 18*IN_MILLISECONDS, 20*IN_MILLISECONDS);
						break;
				}
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mistwhisper_lightning_cloudAI(creature);
    }
};

enum BonescytheRavager
{
	SPELL_BONE_SAW = 54770,
};

class npc_bonescythe_ravager : public CreatureScript
{
public:
    npc_bonescythe_ravager() : CreatureScript("npc_bonescythe_ravager") {}

    struct npc_bonescythe_ravagerAI : public QuantumBasicAI
    {
        npc_bonescythe_ravagerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoneSawTimer;

        void Reset()
		{
			BoneSawTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoneSawTimer <= diff)
			{
				DoCastVictim(SPELL_BONE_SAW);
				BoneSawTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BoneSawTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonescythe_ravagerAI(creature);
    }
};

enum ShattertuskBull
{
	SPELL_SB_STOMP = 55196,
	SPELL_TRAMPLE  = 51944,
};

class npc_shattertusk_bull : public CreatureScript
{
public:
    npc_shattertusk_bull() : CreatureScript("npc_shattertusk_bull") {}

    struct npc_shattertusk_bullAI : public QuantumBasicAI
    {
        npc_shattertusk_bullAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StompTimer;
		uint32 TrampleTimer;

        void Reset()
		{
			StompTimer = 2*IN_MILLISECONDS;
			TrampleTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StompTimer <= diff)
			{
				DoCastAOE(SPELL_SB_STOMP);
				StompTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else StompTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattertusk_bullAI(creature);
    }
};

enum PerchGuardian
{
	SPELL_RUNE_PUNCH = 52702,
};

class npc_perch_guardian : public CreatureScript
{
public:
    npc_perch_guardian() : CreatureScript("npc_perch_guardian") {}

    struct npc_perch_guardianAI : public QuantumBasicAI
    {
        npc_perch_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RunePunchTimer;

        void Reset()
		{
			RunePunchTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RunePunchTimer <= diff)
			{
				DoCastVictim(SPELL_RUNE_PUNCH);
				RunePunchTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RunePunchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_perch_guardianAI(creature);
    }
};

enum SholazarGuardian
{
	SPELL_SG_RUNE_PUNCH = 52702,
};

class npc_sholazar_guardian : public CreatureScript
{
public:
    npc_sholazar_guardian() : CreatureScript("npc_sholazar_guardian") {}

    struct npc_sholazar_guardianAI : public QuantumBasicAI
    {
        npc_sholazar_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RunePunchTimer;

        void Reset()
		{
			RunePunchTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RunePunchTimer <= diff)
			{
				DoCastVictim(SPELL_SG_RUNE_PUNCH);
				RunePunchTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RunePunchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sholazar_guardianAI(creature);
    }
};

enum OverlookSentry
{
	SPELL_RUNE_DETONATION = 55030,
};

class npc_overlook_sentry : public CreatureScript
{
public:
    npc_overlook_sentry() : CreatureScript("npc_overlook_sentry") {}

    struct npc_overlook_sentryAI : public QuantumBasicAI
    {
        npc_overlook_sentryAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RuneDetonationTimer;

        void Reset()
		{
			RuneDetonationTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RuneDetonationTimer <= diff)
			{
				DoCastAOE(SPELL_RUNE_DETONATION);
				RuneDetonationTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RuneDetonationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overlook_sentryAI(creature);
    }
};

enum NesingwaryGameWarden
{
	SPELL_CRAZED       = 48147,
	SPELL_MULTI_SHOT   = 31942,
	SPELL_SCATTER_SHOT = 23601,
	SPELL_NG_SHOOT     = 23337,
	SPELL_NET          = 6533,
};

class npc_nesingwary_game_warden : public CreatureScript
{
public:
    npc_nesingwary_game_warden() : CreatureScript("npc_nesingwary_game_warden") {}

    struct npc_nesingwary_game_wardenAI : public QuantumBasicAI
    {
        npc_nesingwary_game_wardenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrazedTimer;
		uint32 MultiShotTimer;
		uint32 ScatterShotTimer;
		uint32 NetTimer;

        void Reset()
		{
			CrazedTimer = 2*IN_MILLISECONDS;
			MultiShotTimer = 3*IN_MILLISECONDS;
			ScatterShotTimer = 4*IN_MILLISECONDS;
			NetTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_NG_SHOOT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrazedTimer <= diff)
			{
				DoCast(me, SPELL_CRAZED);
				CrazedTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CrazedTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				DoCastVictim(SPELL_MULTI_SHOT);
				MultiShotTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MultiShotTimer -= diff;

			if (ScatterShotTimer <= diff)
			{
				DoCastVictim(SPELL_SCATTER_SHOT);
				ScatterShotTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ScatterShotTimer -= diff;

			if (NetTimer <= diff)
			{
				DoCastVictim(SPELL_NET);
				NetTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nesingwary_game_wardenAI(creature);
    }
};

enum Venomtip
{
	SPELL_COBRA_STRIKE  = 61550,
	SPELL_V_POISON_SPIT = 32330,
};

class npc_venomtip : public CreatureScript
{
public:
    npc_venomtip() : CreatureScript("npc_venomtip") {}

    struct npc_venomtipAI : public QuantumBasicAI
    {
        npc_venomtipAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CobraStrikeTimer;
		uint32 PoisonSpitTimer;

        void Reset()
		{
			CobraStrikeTimer = 2*IN_MILLISECONDS;
			PoisonSpitTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CobraStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_COBRA_STRIKE);
				CobraStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CobraStrikeTimer -= diff;

			if (PoisonSpitTimer <= diff)
			{
				DoCastVictim(SPELL_V_POISON_SPIT);
				PoisonSpitTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else PoisonSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venomtipAI(creature);
    }
};

enum BttertideHydra
{
	SPELL_HYDRA_SPUTUM = 52307,
};

class npc_bittertide_hydra : public CreatureScript
{
public:
    npc_bittertide_hydra() : CreatureScript("npc_bittertide_hydra") {}

    struct npc_bittertide_hydraAI : public QuantumBasicAI
    {
        npc_bittertide_hydraAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HydraSputumTimer;

        void Reset()
		{
			HydraSputumTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HydraSputumTimer <= diff)
			{
				DoCastVictim(SPELL_HYDRA_SPUTUM);
				HydraSputumTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HydraSputumTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bittertide_hydraAI(creature);
    }
};

enum PrimordialDrake
{
	SPELL_BIRTHING_FLAME = 52791,
	SPELL_FLAME_SPIT     = 55071,
};

class npc_primordial_drake : public CreatureScript
{
public:
    npc_primordial_drake() : CreatureScript("npc_primordial_drake") {}

    struct npc_primordial_drakeAI : public QuantumBasicAI
    {
        npc_primordial_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		//uint32 BirthingFlameTimer;
		uint32 FlameSpitTimer;

        void Reset()
		{
			//BirthingFlameTimer = 4*IN_MILLISECONDS;
			FlameSpitTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			//if (BirthingFlameTimer <= diff)
			//{
				//DoCastVictim(SPELL_BIRTHING_FLAME);
				//BirthingFlameTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			//}
			//else BirthingFlameTimer -= diff;

			if (FlameSpitTimer <= diff)
			{
				DoCastAOE(SPELL_FLAME_SPIT);
				FlameSpitTimer = 5*IN_MILLISECONDS;
			}
			else FlameSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_primordial_drakeAI(creature);
    }
};

enum PrimordialDrakeEgg
{
	SPELL_SUMMON_PRIMORDIAL_HATCHLING = 51595,
};

class npc_primordial_drake_egg : public CreatureScript
{
public:
    npc_primordial_drake_egg() : CreatureScript("npc_primordial_drake_egg") {}

    struct npc_primordial_drake_eggAI : public QuantumBasicAI
    {
        npc_primordial_drake_eggAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(SPELL_SUMMON_PRIMORDIAL_HATCHLING);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_primordial_drake_eggAI(creature);
    }
};

enum GoretalonRoc
{
	SPELL_GR_SWOOP = 55082,
};

class npc_goretalon_roc : public CreatureScript
{
public:
    npc_goretalon_roc() : CreatureScript("npc_goretalon_roc") {}

    struct npc_goretalon_rocAI : public QuantumBasicAI
    {
        npc_goretalon_rocAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SwoopTimer;

        void Reset()
        {
			SwoopTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SwoopTimer <= diff)
			{
				DoCastVictim(SPELL_GR_SWOOP);
				SwoopTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SwoopTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goretalon_rocAI(creature);
    }
};

enum GoretalonMatriarch
{
	SPELL_EVASIVE_MANEUVER = 51946,
	SPELL_EYE_PECK         = 49865,
	SPELL_GM_SWOOP         = 55082,
};

class npc_goretalon_matriarch : public CreatureScript
{
public:
    npc_goretalon_matriarch() : CreatureScript("npc_goretalon_matriarch") {}

    struct npc_goretalon_matriarchAI : public QuantumBasicAI
    {
        npc_goretalon_matriarchAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EvasiveManeuverTimer;
		uint32 EyePeckTimer;
		uint32 SwoopTimer;

        void Reset()
        {
			EyePeckTimer = 2*IN_MILLISECONDS;
			SwoopTimer = 4*IN_MILLISECONDS;
			EvasiveManeuverTimer = 5*IN_MILLISECONDS;

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
				EyePeckTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else EyePeckTimer -= diff;

			if (SwoopTimer <= diff)
			{
				DoCastVictim(SPELL_GM_SWOOP);
				SwoopTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SwoopTimer -= diff;

			if (EvasiveManeuverTimer <= diff)
			{
				DoCast(me, SPELL_EVASIVE_MANEUVER);
				EvasiveManeuverTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else EvasiveManeuverTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goretalon_matriarchAI(creature);
    }
};

enum ThalgranBlightbringer
{
	SPELL_DEATHBOLT     = 51854,
	SPELL_RAIN_OF_FIRE  = 54210,
	SPELL_BLIGHTBRINGER = 51841,
};

class npc_thalgran_blightbringer : public CreatureScript
{
public:
    npc_thalgran_blightbringer() : CreatureScript("npc_thalgran_blightbringer") {}

    struct npc_thalgran_blightbringerAI : public QuantumBasicAI
    {
        npc_thalgran_blightbringerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DeathBoltTimer;
		uint32 RainOfFireTimer;

        void Reset()
        {
			DeathBoltTimer = 2*IN_MILLISECONDS;
			RainOfFireTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_BLIGHTBRINGER);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustReachedHome()
		{
			DoCast(me, SPELL_BLIGHTBRINGER);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeathBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEATHBOLT);
					DeathBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else DeathBoltTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAIN_OF_FIRE);
					RainOfFireTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thalgran_blightbringerAI(creature);
    }
};

enum ShardhornRhino
{
	SPELL_RHINO_CHARGE = 55193,
	SPELL_GORE         = 32019,
};

class npc_shardhorn_rhino : public CreatureScript
{
public:
    npc_shardhorn_rhino() : CreatureScript("npc_shardhorn_rhino") {}

    struct npc_shardhorn_rhinoAI : public QuantumBasicAI
    {
        npc_shardhorn_rhinoAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RhinoChargeTimer;
		uint32 GoreTimer;

        void Reset()
        {
			RhinoChargeTimer = 2*IN_MILLISECONDS;
			GoreTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RhinoChargeTimer <= diff)
			{
				DoCast(me, SPELL_RHINO_CHARGE);
				RhinoChargeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RhinoChargeTimer -= diff;

			if (GoreTimer <= diff)
			{
				DoCastVictim(SPELL_GORE);
				GoreTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shardhorn_rhinoAI(creature);
    }
};

enum Dreadsaber
{
	SPELL_DS_CLAW      = 24187,
	SPELL_HFJ_PERIODIC = 44366,
};

class npc_dreadsaber : public CreatureScript
{
public:
    npc_dreadsaber() : CreatureScript("npc_dreadsaber") {}

    struct npc_dreadsaberAI : public QuantumBasicAI
    {
        npc_dreadsaberAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ClawTimer;

        void Reset()
        {
			ClawTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_HFJ_PERIODIC);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ClawTimer <= diff)
			{
				DoCastVictim(SPELL_DS_CLAW);
				ClawTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ClawTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dreadsaberAI(creature);
    }
};

enum FrenzyheartRavager
{
	SPELL_FRENZY       = 53361,
	SPELL_FR_WHIRLWIND = 49807,
};

class npc_frenzyheart_ravager : public CreatureScript
{
public:
    npc_frenzyheart_ravager() : CreatureScript("npc_frenzyheart_ravager") {}

    struct npc_frenzyheart_ravagerAI : public QuantumBasicAI
    {
        npc_frenzyheart_ravagerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WhirlwindTimer;

        void Reset()
        {
			WhirlwindTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_FR_WHIRLWIND);
				WhirlwindTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
			{
				EnterEvadeMode();
				return;
			}

			if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
			{
				EnterEvadeMode();
				return;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzyheart_ravagerAI(creature);
    }
};

enum FrenzyheartHunter
{
	SPELL_FH_SHOOT      = 15547,
	SPELL_FH_MULTI_SHOT = 52270,
};

class npc_frenzyheart_hunter : public CreatureScript
{
public:
    npc_frenzyheart_hunter() : CreatureScript("npc_frenzyheart_hunter") {}

    struct npc_frenzyheart_hunterAI : public QuantumBasicAI
    {
        npc_frenzyheart_hunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 MultiShotTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			MultiShotTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
						DoCast(target, SPELL_FH_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_FH_MULTI_SHOT);
						MultiShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
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
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
						{
							DoCast(target, SPELL_FH_SHOOT);
							ShootTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
						}
					}
				}
				else ShootTimer -= diff;
			}

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
			{
				EnterEvadeMode();
				return;
			}

			if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
			{
				EnterEvadeMode();
				return;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzyheart_hunterAI(creature);
    }
};

enum SerfexTheReaver
{
	SPELL_STAY_SUBMERGED      = 46981,
	SPELL_TUNNEL_BORE_PASSIVE = 29147,
	SPELL_STAND               = 37752,
	SPELL_SF_POISON           = 31747,
};

class npc_serfex_the_reaver : public CreatureScript
{
public:
    npc_serfex_the_reaver() : CreatureScript("npc_serfex_the_reaver") {}

    struct npc_serfex_the_reaverAI : public QuantumBasicAI
    {
        npc_serfex_the_reaverAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonTimer;

        void Reset()
        {
			PoisonTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_STAY_SUBMERGED, true);
			DoCast(me, SPELL_TUNNEL_BORE_PASSIVE, true);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

			me->RemoveAurasDueToSpell(SPELL_STAY_SUBMERGED);
			me->RemoveAurasDueToSpell(SPELL_TUNNEL_BORE_PASSIVE);

			DoCast(me, SPELL_STAND);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SF_POISON);
					PoisonTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_serfex_the_reaverAI(creature);
    }
};

enum AlystrosTheVerdantKeeper
{
	SPELL_TALON_STRIKE  = 51937,
	SPELL_WING_BEAT     = 51938,
	SPELL_LAPSING_DREAM = 51922,
};

class npc_alystros_the_verdant_keeper : public CreatureScript
{
public:
    npc_alystros_the_verdant_keeper() : CreatureScript("npc_alystros_the_verdant_keeper") {}

    struct npc_alystros_the_verdant_keeperAI : public QuantumBasicAI
    {
        npc_alystros_the_verdant_keeperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TalonStrikeTimer;
		uint32 WingBeatTimer;
		uint32 LapsingDreamTimer;

        void Reset()
        {
			TalonStrikeTimer = 2*IN_MILLISECONDS;
			WingBeatTimer = 4*IN_MILLISECONDS;
			LapsingDreamTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TalonStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TALON_STRIKE);
				TalonStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else TalonStrikeTimer -= diff;

			if (WingBeatTimer <= diff)
			{
				DoCastVictim(SPELL_WING_BEAT);
				WingBeatTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else WingBeatTimer -= diff;

			if (LapsingDreamTimer <= diff)
			{
				DoCastAOE(SPELL_LAPSING_DREAM);
				LapsingDreamTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else LapsingDreamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alystros_the_verdant_keeperAI(creature);
    }
};

enum EmeraldSkytalon
{
	SPELL_ES_TALON_STRIKE = 32909,
	SPELL_ES_SWOOP        = 51919,
};

class npc_emerald_skytalon : public CreatureScript
{
public:
    npc_emerald_skytalon() : CreatureScript("npc_emerald_skytalon") {}

    struct npc_emerald_skytalonAI : public QuantumBasicAI
    {
        npc_emerald_skytalonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TalonStrikeTimer;

        void Reset()
        {
			TalonStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_ES_SWOOP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TalonStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_ES_TALON_STRIKE);
				TalonStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else TalonStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emerald_skytalonAI(creature);
    }
};

enum EmeraldLasher
{
	SPELL_DREAM_LASH = 51901,
};

class npc_emerald_lasher : public CreatureScript
{
public:
    npc_emerald_lasher() : CreatureScript("npc_emerald_lasher") {}

    struct npc_emerald_lasherAI : public QuantumBasicAI
    {
        npc_emerald_lasherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DreamLashTimer;

        void Reset()
        {
			DreamLashTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DreamLashTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DREAM_LASH);
					DreamLashTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else DreamLashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emerald_lasherAI(creature);
    }
};

enum SnowplainDisciple
{
	SPELL_SD_FROSTBOLT  = 61730,
	SPELL_RENEWING_BEAM = 52011,
};

class npc_snowplain_disciple : public CreatureScript
{
public:
    npc_snowplain_disciple() : CreatureScript("npc_snowplain_disciple") {}

    struct npc_snowplain_discipleAI : public QuantumBasicAI
    {
        npc_snowplain_discipleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 RenewingBeamTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			RenewingBeamTimer = 2.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_SD_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (RenewingBeamTimer <= diff)
			{
				DoCast(me, SPELL_RENEWING_BEAM);
				RenewingBeamTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RenewingBeamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowplain_discipleAI(creature);
    }
};

enum SnowplainShaman
{
	SPELL_SEARING_TOTEM = 39591,

	NPC_SEARING_TOTEM   = 22895,
};

class npc_snowplain_shaman : public CreatureScript
{
public:
    npc_snowplain_shaman() : CreatureScript("npc_snowplain_shaman") {}

    struct npc_snowplain_shamanAI : public QuantumBasicAI
    {
        npc_snowplain_shamanAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 SearingTotemTimer;

		SummonList Summons;

        void Reset()
        {
			SearingTotemTimer = 2*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SEARING_TOTEM)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SearingTotemTimer <= diff)
			{
				DoCast(me, SPELL_SEARING_TOTEM);
				SearingTotemTimer = urand(10*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else SearingTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowplain_shamanAI(creature);
    }
};

enum SnowfallGladeShaman
{
	SPELL_LIGHTNING_SHIELD = 12550,
	SPELL_FROST_SHOCK      = 12548,
	SPELL_HEALING_WAVE     = 11986,
};

class npc_snowfall_glade_shaman : public CreatureScript
{
public:
    npc_snowfall_glade_shaman() : CreatureScript("npc_snowfall_glade_shaman") {}

    struct npc_snowfall_glade_shamanAI : public QuantumBasicAI
    {
        npc_snowfall_glade_shamanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningShieldTimer;
		uint32 FrostShockTimer;
		uint32 HealingWaveTimer;

        void Reset()
        {
			LightningShieldTimer = 0.5*IN_MILLISECONDS;
			FrostShockTimer = 1*IN_MILLISECONDS;
			HealingWaveTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (LightningShieldTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_LIGHTNING_SHIELD);
				LightningShieldTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else LightningShieldTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Lightning Shield Buff Check in Combat
			if (!me->HasAura(SPELL_LIGHTNING_SHIELD))
				DoCast(me, SPELL_LIGHTNING_SHIELD);

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_SHOCK);
					FrostShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostShockTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_WAVE);
				HealingWaveTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else HealingWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowfall_glade_shamanAI(creature);
    }
};

enum SummonedSearingTotem
{
	SPELL_SEAR = 39592,
};

class npc_summoned_searing_totem : public CreatureScript
{
public:
    npc_summoned_searing_totem() : CreatureScript("npc_summoned_searing_totem") {}

    struct npc_summoned_searing_totemAI : public QuantumBasicAI
    {
        npc_summoned_searing_totemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 SearTimer;

        void Reset()
        {
			SearTimer = 0.5*IN_MILLISECONDS;

			me->SetReactState(REACT_AGGRESSIVE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SEAR);
					SearTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else SearTimer -= diff;

			// No melee attacks
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_summoned_searing_totemAI(creature);
    }
};

enum ChosenZealot
{
	SPELL_CZ_REND = 13445,
	SPELL_FRAILTY = 12530, // spell 60541 wtf?
};

class npc_chosen_zealot : public CreatureScript
{
public:
    npc_chosen_zealot() : CreatureScript("npc_chosen_zealot") {}

    struct npc_chosen_zealotAI : public QuantumBasicAI
    {
        npc_chosen_zealotAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 FrailtyTimer;

        void Reset()
        {
			RendTimer = 2*IN_MILLISECONDS;
			FrailtyTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_CZ_REND);
				RendTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (FrailtyTimer <= diff)
			{
				DoCastVictim(SPELL_FRAILTY);
				FrailtyTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrailtyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chosen_zealotAI(creature);
    }
};

enum DragonboneCondor
{
	SPELL_DC_EVASIVE_MANEUVER = 51946,
};

class npc_dragonbone_condor : public CreatureScript
{
public:
    npc_dragonbone_condor() : CreatureScript("npc_dragonbone_condor") {}

    struct npc_dragonbone_condorAI : public QuantumBasicAI
    {
        npc_dragonbone_condorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EvasiveManeuverTimer;

        void Reset()
        {
			EvasiveManeuverTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EvasiveManeuverTimer <= diff)
			{
				DoCastVictim(SPELL_DC_EVASIVE_MANEUVER);
				EvasiveManeuverTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else EvasiveManeuverTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonbone_condorAI(creature);
    }
};

enum FrigidGhoulAttacker
{
	SPELL_ROT_ARMOR = 50361,
};

class npc_frigid_ghoul_attacker : public CreatureScript
{
public:
    npc_frigid_ghoul_attacker() : CreatureScript("npc_frigid_ghoul_attacker") {}

    struct npc_frigid_ghoul_attackerAI : public QuantumBasicAI
    {
        npc_frigid_ghoul_attackerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RotArmorTimer;

        void Reset()
        {
			RotArmorTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RotArmorTimer <= diff)
			{
				DoCastVictim(SPELL_ROT_ARMOR);
				RotArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RotArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frigid_ghoul_attackerAI(creature);
    }
};

enum RubyWatcher
{
	SPELL_RUBY_BLAST = 49241,
};

class npc_ruby_watcher : public CreatureScript
{
public:
    npc_ruby_watcher() : CreatureScript("npc_ruby_watcher") {}

    struct npc_ruby_watcherAI : public QuantumBasicAI
    {
        npc_ruby_watcherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RubyBlastTimer;

        void Reset()
		{
			RubyBlastTimer = 2*IN_MILLISECONDS;

			me->SetReactState(REACT_AGGRESSIVE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RubyBlastTimer <= diff)
			{
				DoCastVictim(SPELL_RUBY_BLAST);
				RubyBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RubyBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ruby_watcherAI(creature);
    }
};

enum FrigidAbominationAttacker
{
	SPELL_FA_CLEAVE = 40504,
};

class npc_frigid_abomination_attacker : public CreatureScript
{
public:
    npc_frigid_abomination_attacker() : CreatureScript("npc_frigid_abomination_attacker") {}

    struct npc_frigid_abomination_attackerAI : public QuantumBasicAI
    {
        npc_frigid_abomination_attackerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetReactState(REACT_AGGRESSIVE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_FA_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frigid_abomination_attackerAI(creature);
    }
};

enum AllianceConscript
{
	SPELL_AC_SHOOT         = 15620,
	SPELL_AC_CRAZED        = 48147,
	SPELL_SHIELD_BLOCK     = 32587,
	SPELL_AC_HEROIC_STRIKE = 29426,
};

class npc_alliance_conscript : public CreatureScript
{
public:
    npc_alliance_conscript() : CreatureScript("npc_alliance_conscript") {}

    struct npc_alliance_conscriptAI : public QuantumBasicAI
    {
        npc_alliance_conscriptAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrazedTimer;
		uint32 ShieldBlockTimer;
		uint32 HeroicStrikeTimer;

        void Reset()
        {
			CrazedTimer = 2*IN_MILLISECONDS;
			ShieldBlockTimer = 3*IN_MILLISECONDS;
			HeroicStrikeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_AC_SHOOT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrazedTimer <= diff)
			{
				DoCast(me, SPELL_AC_CRAZED);
				CrazedTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CrazedTimer -= diff;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_SHIELD_BLOCK);
				ShieldBlockTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ShieldBlockTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_AC_HEROIC_STRIKE);
				HeroicStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HeroicStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alliance_conscriptAI(creature);
    }
};

enum CaptainIskandar
{
	SPELL_CI_CLEAVE        = 42724,
	SPELL_CI_MORTAL_STRIKE = 15708,
	SPELL_CI_WHIRLWIND     = 38618,
};

class npc_captain_iskandar : public CreatureScript
{
public:
    npc_captain_iskandar() : CreatureScript("npc_captain_iskandar") {}

    struct npc_captain_iskandarAI : public QuantumBasicAI
    {
        npc_captain_iskandarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 MortalStrikeTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 4*IN_MILLISECONDS;
			WhirlwindTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CI_CLEAVE);
				CleaveTimer = 3*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_CI_MORTAL_STRIKE);
				MortalStrikeTimer = 5*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_CI_WHIRLWIND);
				WhirlwindTimer = 7*IN_MILLISECONDS;
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captain_iskandarAI(creature);
    }
};

enum FrigidNecromancer
{
	SPELL_SUMMON_FRIGID_NECROMANCER = 49304, // WTF??
	SPELL_FN_SHADOW_BOLT            = 9613,
	SPELL_FN_BONE_ARMOR             = 50324,
};

class npc_frigid_necromancer : public CreatureScript
{
public:
    npc_frigid_necromancer() : CreatureScript("npc_frigid_necromancer") {}

    struct npc_frigid_necromancerAI : public QuantumBasicAI
    {
        npc_frigid_necromancerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoneArmorTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			BoneArmorTimer = 0.1*IN_MILLISECONDS;
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (BoneArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_FN_BONE_ARMOR);
				BoneArmorTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else BoneArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Bone Armor Buff Check in Combat
			if (!me->HasAura(SPELL_FN_BONE_ARMOR))
				DoCast(me, SPELL_FN_BONE_ARMOR, true);

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FN_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frigid_necromancerAI(creature);
    }
};

enum DahliaSuntouch
{
	SPELL_BANSHEE_CURSE   = 51899,
	SPELL_BANSHEE_WAIL    = 28993,
	SPELL_BANSHEE_SCREECH = 51897,
};

class npc_dahlia_suntouch : public CreatureScript
{
public:
    npc_dahlia_suntouch() : CreatureScript("npc_dahlia_suntouch") {}

    struct npc_dahlia_suntouchAI : public QuantumBasicAI
    {
        npc_dahlia_suntouchAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BansheeCurseTimer;
		uint32 BansheeWailTimer;
		uint32 BansheeScreechTimer;

        void Reset()
        {
			BansheeCurseTimer = 2*IN_MILLISECONDS;
			BansheeWailTimer = 3*IN_MILLISECONDS;
			BansheeScreechTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_BANSHEE_CURSE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BansheeCurseTimer <= diff)
			{
				DoCastVictim(SPELL_BANSHEE_CURSE);
				BansheeCurseTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BansheeCurseTimer -= diff;

			if (BansheeWailTimer <= diff)
			{
				DoCastVictim(SPELL_BANSHEE_WAIL);
				BansheeWailTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else BansheeWailTimer -= diff;

			if (BansheeScreechTimer <= diff)
			{
				DoCastVictim(SPELL_BANSHEE_SCREECH);
				BansheeScreechTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BansheeScreechTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dahlia_suntouchAI(creature);
    }
};

enum RubyGuardian
{
	SPELL_RG_CLEAVE = 40504,
};

class npc_ruby_guardian : public CreatureScript
{
public:
    npc_ruby_guardian() : CreatureScript("npc_ruby_guardian") {}

    struct npc_ruby_guardianAI : public QuantumBasicAI
    {
        npc_ruby_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_RG_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ruby_guardianAI(creature);
    }
};

enum DukeVallenhal
{
	SPELL_BLOOD_PRESENCE_R1 = 50689,
	SPELL_BLOODWORM         = 51873, // 28524
	DV_MOUNT_ID             = 25280,
};

class npc_duke_vallenhal : public CreatureScript
{
public:
    npc_duke_vallenhal() : CreatureScript("npc_duke_vallenhal") {}

    struct npc_duke_vallenhalAI : public QuantumBasicAI
    {
        npc_duke_vallenhalAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BloodwormTimer;

        void Reset()
        {
			BloodwormTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_BLOOD_PRESENCE_R1, true);

			me->Mount(DV_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodwormTimer <= diff)
			{
				DoCastAOE(SPELL_BLOODWORM);
				BloodwormTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else BloodwormTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_duke_vallenhalAI(creature);
    }
};

enum WornBloodworm
{
	SPELL_HEALTH_LEECH_PASSIVE = 58878,
};

class npc_worn_bloodworm : public CreatureScript
{
public:
    npc_worn_bloodworm() : CreatureScript("npc_worn_bloodworm") {}

    struct npc_worn_bloodwormAI : public QuantumBasicAI
    {
        npc_worn_bloodwormAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_HEALTH_LEECH_PASSIVE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_worn_bloodwormAI(creature);
    }
};

enum FrigidGhoul
{
	SPELL_FG_ROT_ARMOR = 50361,
};

class npc_frigid_ghoul : public CreatureScript
{
public:
    npc_frigid_ghoul() : CreatureScript("npc_frigid_ghoul") {}

    struct npc_frigid_ghoulAI : public QuantumBasicAI
    {
        npc_frigid_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RotArmorTimer;

        void Reset()
        {
			RotArmorTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RotArmorTimer <= diff)
			{
				DoCastVictim(SPELL_FG_ROT_ARMOR);
				RotArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RotArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frigid_ghoulAI(creature);
    }
};

// Need Implemented and Translate Text For Russian (Texts Before Complete Nefarian Heads Quest)
#define SAY_1 "Be lifted by <%s>'s accomplishment! Revel in her rallying cry!"
#define SAY_2 "NEFARIAN IS SLAIN! People of Orgrimmar, bow down before the might of <%s> and her allies for they have laid a blow against the Black Dragonflight that is sure to stir the Aspects from their malaise! This defeat shall surely be felt by the father of the Black Flight: Deathwing reels in pain and anguish this day!"

enum OverlordRunthak
{
	SPELL_INTIMIDATING_ROAR = 16508,
	SPELL_OR_REND           = 16509,
	SPELL_OR_STRIKE         = 15580,

	SAY_RUNTHAK_AGGRO_1     = -1520674,
	SAY_RUNTHAK_AGGRO_2     = -1520675,
	SAY_RUNTHAK_AGGRO_3     = -1520676,
};

class npc_overlord_runthak : public CreatureScript
{
public:
    npc_overlord_runthak() : CreatureScript("npc_overlord_runthak") {}

    struct npc_overlord_runthakAI : public QuantumBasicAI
    {
        npc_overlord_runthakAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 StrikeTimer;
		uint32 IntimidatingRoarTimer;

        void Reset()
        {
			RendTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 4*IN_MILLISECONDS;
			IntimidatingRoarTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_RUNTHAK_AGGRO_1, SAY_RUNTHAK_AGGRO_2, SAY_RUNTHAK_AGGRO_3), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_OR_REND);
				RendTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_OR_STRIKE);
				StrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			if (IntimidatingRoarTimer <= diff)
			{
				DoCastAOE(SPELL_INTIMIDATING_ROAR);
				IntimidatingRoarTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else IntimidatingRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overlord_runthakAI(creature);
    }
};

enum ExpeditionaryPriest
{
	SPELL_HOLY_SMITE = 9734,
	SPELL_HEAL       = 11642,
};

class npc_expeditionary_priest : public CreatureScript
{
public:
    npc_expeditionary_priest() : CreatureScript("npc_expeditionary_priest") {}

    struct npc_expeditionary_priestAI : public QuantumBasicAI
    {
        npc_expeditionary_priestAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HolySmiteTimer;
		uint32 HealTimer;

        void Reset()
        {
			HolySmiteTimer = 2*IN_MILLISECONDS;
			HealTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HolySmiteTimer <= diff)
			{
				DoCastVictim(SPELL_HOLY_SMITE);
				HolySmiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HolySmiteTimer -= diff;

			if (HealTimer <= diff)
			{
				DoCast(me, SPELL_HEAL);
				HealTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_expeditionary_priestAI(creature);
    }
};

enum IronSentinel
{
	SPELL_IS_STOMP         = 56490,
	SPELL_IS_THROW_BOULDER = 55811,
};

class npc_iron_sentinel : public CreatureScript
{
public:
    npc_iron_sentinel() : CreatureScript("npc_iron_sentinel") {}

    struct npc_iron_sentinelAI : public QuantumBasicAI
    {
        npc_iron_sentinelAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowBoulderTimer;
		uint32 StompTimer;

        void Reset()
        {
			ThrowBoulderTimer = 2*IN_MILLISECONDS;
			StompTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowBoulderTimer <= diff)
			{
				DoCastVictim(SPELL_IS_THROW_BOULDER);
				ThrowBoulderTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ThrowBoulderTimer -= diff;

			if (StompTimer <= diff)
			{
				DoCastAOE(SPELL_IS_STOMP);
				StompTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_sentinelAI(creature);
    }
};

enum Anathemus
{
	SPELL_BOULDER = 9483,
	SPELL_QUAKE   = 26093,
};

class npc_anathemus : public CreatureScript
{
public:
	npc_anathemus() : CreatureScript("npc_anathemus") {}

    struct npc_anathemusAI : public QuantumBasicAI
    {
        npc_anathemusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoulderTimer;
		uint32 QuakeTimer;

        void Reset()
        {
			BoulderTimer = 2*IN_MILLISECONDS;
			QuakeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoulderTimer <= diff)
			{
				DoCastVictim(SPELL_BOULDER);
				BoulderTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BoulderTimer -= diff;

			if (QuakeTimer <= diff)
			{
				DoCastAOE(SPELL_QUAKE);
				QuakeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else QuakeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anathemusAI(creature);
    }
};

enum WarGolem
{
	SPELL_LOCK_DOWN = 9576,
};

class npc_war_golem : public CreatureScript
{
public:
	npc_war_golem() : CreatureScript("npc_war_golem") {}

    struct npc_war_golemAI : public QuantumBasicAI
    {
        npc_war_golemAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LockDownTimer;

        void Reset()
        {
			LockDownTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LockDownTimer <= diff)
			{
				DoCastVictim(SPELL_LOCK_DOWN);
				LockDownTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else LockDownTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_war_golemAI(creature);
    }
};

enum EarthboundRevenant
{
	SPELL_MACE_SMASH  = 50731,
	SPELL_SHIELD_BASH = 11972,
};

class npc_earthbound_revenant : public CreatureScript
{
public:
	npc_earthbound_revenant() : CreatureScript("npc_earthbound_revenant") {}

    struct npc_earthbound_revenantAI : public QuantumBasicAI
    {
        npc_earthbound_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MaceSmashTimer;
		uint32 ShieldBashTimer;

        void Reset()
        {
			MaceSmashTimer = 2*IN_MILLISECONDS;
			ShieldBashTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MaceSmashTimer <= diff)
			{
				DoCastVictim(SPELL_MACE_SMASH);
				MaceSmashTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MaceSmashTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_earthbound_revenantAI(creature);
    }
};

enum BrittleRevenant
{
	SPELL_BR_MACE_SMASH  = 50731,
	SPELL_BR_SHIELD_BASH = 11972,
};

class npc_brittle_revenant : public CreatureScript
{
public:
	npc_brittle_revenant() : CreatureScript("npc_brittle_revenant") {}

    struct npc_brittle_revenantAI : public QuantumBasicAI
    {
        npc_brittle_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MaceSmashTimer;
		uint32 ShieldBashTimer;

        void Reset()
        {
			MaceSmashTimer = 2*IN_MILLISECONDS;
			ShieldBashTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MaceSmashTimer <= diff)
			{
				DoCastVictim(SPELL_BR_MACE_SMASH);
				MaceSmashTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MaceSmashTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_BR_SHIELD_BASH);
				ShieldBashTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brittle_revenantAI(creature);
    }
};

enum ChilledEarthElemental
{
	SPELL_VERTEX_COLOR_LT_BLUE = 42617,
	SPELL_THROW_ICE            = 43543,
};

class npc_chilled_earth_elemental : public CreatureScript
{
public:
	npc_chilled_earth_elemental() : CreatureScript("npc_chilled_earth_elemental") {}

    struct npc_chilled_earth_elementalAI : public QuantumBasicAI
    {
        npc_chilled_earth_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowIceTimer;

        void Reset()
        {
			ThrowIceTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_VERTEX_COLOR_LT_BLUE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_THROW_ICE, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowIceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_THROW_ICE, true);
					ThrowIceTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ThrowIceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chilled_earth_elementalAI(creature);
    }
};

enum VentureCoExcavator
{
	SPELl_PIERCING_BLOW = 49749,
};

class npc_venture_co_excavator : public CreatureScript
{
public:
	npc_venture_co_excavator() : CreatureScript("npc_venture_co_excavator") {}

    struct npc_venture_co_excavatorAI : public QuantumBasicAI
    {
        npc_venture_co_excavatorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PiercingBlowTimer;

        void Reset()
        {
			PiercingBlowTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PiercingBlowTimer <= diff)
			{
				DoCastVictim(SPELl_PIERCING_BLOW);
				PiercingBlowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PiercingBlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venture_co_excavatorAI(creature);
    }
};

enum ForemanSwindlegrin
{
	SPELL_CHAINSAW_BLADE = 36228,
};

class npc_foreman_swindlegrin : public CreatureScript
{
public:
	npc_foreman_swindlegrin() : CreatureScript("npc_foreman_swindlegrin") {}

    struct npc_foreman_swindlegrinAI : public QuantumBasicAI
    {
        npc_foreman_swindlegrinAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainsawBladeTimer;

        void Reset()
        {
			ChainsawBladeTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChainsawBladeTimer <= diff)
			{
				DoCastVictim(SPELL_CHAINSAW_BLADE);
				ChainsawBladeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ChainsawBladeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_foreman_swindlegrinAI(creature);
    }
};

enum Meatpie
{
	SPELL_M_CONCUSSION_BLOW = 32588,
	SPELL_M_WHIRLWIND       = 49807,
};

class npc_meatpie : public CreatureScript
{
public:
	npc_meatpie() : CreatureScript("npc_meatpie") {}

    struct npc_meatpieAI : public QuantumBasicAI
    {
        npc_meatpieAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ConcussionBlowTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			ConcussionBlowTimer = 2*IN_MILLISECONDS;
			WhirlwindTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ConcussionBlowTimer <= diff)
			{
				DoCastVictim(SPELL_M_CONCUSSION_BLOW);
				ConcussionBlowTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ConcussionBlowTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_M_WHIRLWIND);
				WhirlwindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_meatpieAI(creature);
    }
};

enum BoneguardLieutenant
{
	SPELL_BL_CHARGE            = 63010,
	SPELL_DEFEND               = 64100,
	SPELL_SUNDERING_TRUST_AURA = 62710,
	BL_MOUNT_ID                = 25678,
};

class npc_boneguard_lieutenant : public CreatureScript
{
public:
	npc_boneguard_lieutenant() : CreatureScript("npc_boneguard_lieutenant") {}

    struct npc_boneguard_lieutenantAI : public QuantumBasicAI
    {
        npc_boneguard_lieutenantAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SUNDERING_TRUST_AURA);
			DoCast(me, SPELL_DEFEND);

			me->Mount(BL_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			DoCastVictim(SPELL_BL_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_boneguard_lieutenantAI(creature);
    }
};

enum OilyBeasts
{
	SPELL_OIL_COAT_AURA = 45948,
	SPELL_OILY_COAT     = 50280,
	EVENT_OILY_COAT     = 1,
};

class npc_oil_soaked_caribou : public CreatureScript
{
public:
	npc_oil_soaked_caribou() : CreatureScript("npc_oil_soaked_caribou") {}

    struct npc_oil_soaked_caribouAI : public QuantumBasicAI
    {
        npc_oil_soaked_caribouAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			me->AddAura(SPELL_OIL_COAT_AURA, me);

			events.ScheduleEvent(EVENT_OILY_COAT, 5*IN_MILLISECONDS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->RemoveAura(SPELL_OIL_COAT_AURA);
		}

		void UpdateAI(uint32 const diff)
		{
			UpdateVictim();

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					case EVENT_OILY_COAT:
						DoCast(me, SPELL_OILY_COAT);
						events.ScheduleEvent(EVENT_OILY_COAT, 5*IN_MILLISECONDS);
						break;
				}
			}
			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oil_soaked_caribouAI(creature);
    }
};

class npc_oiled_fledgeling : public CreatureScript
{
public:
	npc_oiled_fledgeling() : CreatureScript("npc_oiled_fledgeling") {}

    struct npc_oiled_fledgelingAI : public QuantumBasicAI
    {
        npc_oiled_fledgelingAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			me->AddAura(SPELL_OIL_COAT_AURA, me);

			events.ScheduleEvent(EVENT_OILY_COAT, 5*IN_MILLISECONDS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->RemoveAura(SPELL_OIL_COAT_AURA);
		}

		void UpdateAI(uint32 const diff)
		{
			UpdateVictim();

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					case EVENT_OILY_COAT:
						DoCast(me, SPELL_OILY_COAT);
						events.ScheduleEvent(EVENT_OILY_COAT, 5*IN_MILLISECONDS);
						break;
				}
			}
			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oiled_fledgelingAI(creature);
    }
};

enum BoneguardCommander
{
	SPELL_BC_SCOURGE_BANNER_BEARER = 59942,
	SPELL_BC_SCOURGE_BANNER_AURA   = 60023,
	SPELL_BC_DEFEND                = 64101,
	SPELL_BC_SHIELD_BREAKER        = 65147,
	SPELL_BC_CHARGE                = 63010,
	BC_MOUNT_ID                    = 25678,
};

class npc_boneguard_commander : public CreatureScript
{
public:
	npc_boneguard_commander() : CreatureScript("npc_boneguard_commander") {}

    struct npc_boneguard_commanderAI : public QuantumBasicAI
    {
        npc_boneguard_commanderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShieldBreakerTimer;

		void Reset()
        {
			ShieldBreakerTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_BC_SCOURGE_BANNER_BEARER);
			DoCast(me, SPELL_BC_SCOURGE_BANNER_AURA);
			DoCast(me, SPELL_BC_DEFEND);

			me->Mount(BC_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			DoCastVictim(SPELL_BL_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBreakerTimer <= diff)
			{
				DoCastVictim(SPELL_BC_SHIELD_BREAKER);
				ShieldBreakerTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ShieldBreakerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_boneguard_commanderAI(creature);
    }
};

enum EbonBladeVindicator
{
	SPELL_EBV_CRAZED        = 48147,
	SPELL_DEATH_GRIP        = 49560,
	SPELL_BLOOD_STRIKE      = 59130,
	SPELL_ICY_TOUCH         = 59131,
	SPELL_EBV_PLAGUE_STRIKE = 59133,
	SPELL_DEATH_COIL        = 59134,
};

class npc_ebon_blade_vindicator : public CreatureScript
{
public:
	npc_ebon_blade_vindicator() : CreatureScript("npc_ebon_blade_vindicator") {}

    struct npc_ebon_blade_vindicatorAI : public QuantumBasicAI
    {
        npc_ebon_blade_vindicatorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrazedTimer;
		uint32 BloodStrikeTimer;
		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;
		uint32 DeathCoilTimer;

        void Reset()
        {
			CrazedTimer = 1*IN_MILLISECONDS;
			BloodStrikeTimer = 2*IN_MILLISECONDS;
			IcyTouchTimer = 3*IN_MILLISECONDS;
			PlagueStrikeTimer = 4*IN_MILLISECONDS;
			DeathCoilTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_DEATH_GRIP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrazedTimer <= diff)
			{
				DoCast(me, SPELL_EBV_CRAZED);
				CrazedTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else CrazedTimer -= diff;

			if (BloodStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BLOOD_STRIKE);
				BloodStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BloodStrikeTimer -= diff;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH);
				IcyTouchTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_EBV_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				DoCastVictim(SPELL_DEATH_COIL);
				DeathCoilTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DeathCoilTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ebon_blade_vindicatorAI(creature);
    }
};

enum SteamBurst
{
	SPELL_COSMETIC_STEAM_AURA = 36151,
};

class npc_steam_burst : public CreatureScript
{
public:
	npc_steam_burst() : CreatureScript("npc_steam_burst") {}

    struct npc_steam_burstAI : public QuantumBasicAI
    {
        npc_steam_burstAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->AddAura(SPELL_COSMETIC_STEAM_AURA, me);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steam_burstAI(creature);
    }
};

enum DragonflayerDefender
{
	SPELL_HARPOON_TOSS = 44188,
	DF_MOUNT_ID        = 28044,
};

class npc_dragonflayer_defender : public CreatureScript
{
public:
	npc_dragonflayer_defender() : CreatureScript("npc_dragonflayer_defender") {}

    struct npc_dragonflayer_defenderAI : public QuantumBasicAI
    {
        npc_dragonflayer_defenderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HarpoonTossTimer;

		void Reset()
        {
			HarpoonTossTimer = 2*IN_MILLISECONDS;

			me->Mount(DF_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_HARPOON_TOSS);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HarpoonTossTimer <= diff)
			{
				DoCastVictim(SPELL_HARPOON_TOSS);
				HarpoonTossTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HarpoonTossTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_defenderAI(creature);
    }
};

enum Varguls
{
	SPELL_VS_DIMINISH_SOUL    = 36789,
	SPELL_VARGUL_PLAGUEBOLT   = 55419,
	SPELL_PLAGUE_SHIELD       = 54512,
	SPELL_RUNE_OF_DESTRUCTION = 56036,
};

class npc_vargul_slayer : public CreatureScript
{
public:
    npc_vargul_slayer() : CreatureScript("npc_vargul_slayer") {}

    struct npc_vargul_slayerAI : public QuantumBasicAI
    {
        npc_vargul_slayerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DiminishSoulTimer;
		uint32 VargulPlagueBoltTimer;

        void Reset()
        {
			VargulPlagueBoltTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H_L);
        }

		void JustReachedHome()
		{
			Reset();
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DiminishSoulTimer = 1.5*IN_MILLISECONDS;
		}

        void UpdateAI(uint32 const diff)
        {
			UpdateVictim();

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DiminishSoulTimer <= diff)
			{
				DoCastVictim(SPELL_VS_DIMINISH_SOUL);
				DiminishSoulTimer = urand(4.4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DiminishSoulTimer -= diff;

			if (VargulPlagueBoltTimer <= diff)
			{
				DoCast(SPELL_VARGUL_PLAGUEBOLT);
				VargulPlagueBoltTimer = 10*IN_MILLISECONDS;
			}
			else VargulPlagueBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vargul_slayerAI(creature);
    }
};

class npc_vargul_runelord : public CreatureScript
{
public:
    npc_vargul_runelord() : CreatureScript("npc_vargul_runelord") {}

    struct npc_vargul_runelordAI : public QuantumBasicAI
    {
        npc_vargul_runelordAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PlagueShieldTimer;
		uint32 RuneOfDestructionTimer;
		uint32 VargulPlagueBoltTimer;

        void Reset()
        {
			PlagueShieldTimer = 1*IN_MILLISECONDS;
			VargulPlagueBoltTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void JustReachedHome()
		{
			Reset();
		}

		void EnterToBattle(Unit* /*who*/)
		{
			RuneOfDestructionTimer = 0.5*IN_MILLISECONDS;
		}

        void UpdateAI(uint32 const diff)
        {
			UpdateVictim();

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VargulPlagueBoltTimer <= diff)
			{
				DoCast(SPELL_VARGUL_PLAGUEBOLT);
				VargulPlagueBoltTimer = 10*IN_MILLISECONDS;
			}
			else VargulPlagueBoltTimer -= diff;

			if (PlagueShieldTimer <= diff)
			{
				DoCast(SPELL_PLAGUE_SHIELD);
				PlagueShieldTimer = 3*MINUTE*IN_MILLISECONDS;
			}
			else PlagueShieldTimer -= diff;

			if (RuneOfDestructionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RUNE_OF_DESTRUCTION);
					RuneOfDestructionTimer = 6*IN_MILLISECONDS;
				}
			}
			else RuneOfDestructionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vargul_runelordAI(creature);
    }
};

enum WaterTerror
{
	SPELL_WATERBOLT = 60869,
};

class npc_water_terror : public CreatureScript
{
public:
    npc_water_terror() : CreatureScript("npc_water_terror") {}

    struct npc_water_terrorAI : public QuantumBasicAI
    {
		npc_water_terrorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterboltTimer;

        void Reset()
		{
			WaterboltTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_WATERBOLT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterboltTimer <= diff)
			{
				DoCastVictim(SPELL_WATERBOLT);
				WaterboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else WaterboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_water_terrorAI(creature);
    }
};

enum SentryWorg
{
	SPELL_TENDON_RIP = 3604,
};

class npc_sentry_worg : public CreatureScript
{
public:
    npc_sentry_worg() : CreatureScript("npc_sentry_worg") {}

    struct npc_sentry_worgAI : public QuantumBasicAI
    {
		npc_sentry_worgAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TendonRipTimer;

        void Reset()
		{
			TendonRipTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TendonRipTimer <= diff)
			{
				DoCastVictim(SPELL_TENDON_RIP);
				TendonRipTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else TendonRipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sentry_worgAI(creature);
    }
};

enum KulgalarOracle
{
	SPELL_ORACLE_MIND_BLAST = 31516,
	SPELL_ORACLE_MIND_FLAY  = 16568,
	SPELL_ORACLE_HEAL       = 11642,
	SPELL_COSMETIC_COMBAT   = 50657,
	SAY_ORACLE_AGGRO        = 0,
};

class npc_kulgalar_oracle : public CreatureScript
{
public:
    npc_kulgalar_oracle() : CreatureScript("npc_kulgalar_oracle") {}

    struct npc_kulgalar_oracleAI : public QuantumBasicAI
    {
		npc_kulgalar_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MindBlastTimer;
		uint32 MindFlayTimer;

        void Reset()
		{
			MindBlastTimer = 1*IN_MILLISECONDS;
			MindFlayTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_ORACLE_AGGRO);
			DoCast(me, SPELL_COSMETIC_COMBAT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MindBlastTimer <= diff)
			{
				DoCastVictim(SPELL_ORACLE_MIND_BLAST);
				MindBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MindBlastTimer -= diff;

			if (MindFlayTimer <= diff)
			{
				DoCastVictim(SPELL_ORACLE_MIND_FLAY);
				MindFlayTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MindFlayTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_ORACLE_HEAL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kulgalar_oracleAI(creature);
    }
};

//Warsong Captain yells: Let the forces of the Lich King gaze upon the carcass of this wretch and know that the Horde will not relent!
//Warsong Captain yells: Marksmen! Front and center! Scourge attack incoming!
//Warsong Captain yells: Marksmen, lock and load!
//Warsong Captain yells: The Nerub'ar have been punished for their transgression!
//Warsong Captain says: Gather, soldiers! Gather and take your trophies from the carapace of the enemy!
//Warsong Captain says: Settle down, soldier!
//Warsong Captain says: We'll be ready for 'em... You did a good job in warning us.

enum WarsongCaptain
{
	SPELL_WC_LESER_HEALING_WAVE = 44256,
	SPELL_WC_CHAIN_HEAL         = 15799,
	SPELL_WC_CHAIN_LIGHTNING    = 12058,
	SPELL_WC_GROUNDING_TOTEM    = 34079,
	SPELL_WC_BLOODLUST          = 45584,
	MOUNT_BLACK_WOLF            = 21241,
};

class npc_warsong_captain : public CreatureScript
{
public:
    npc_warsong_captain() : CreatureScript("npc_warsong_captain") {}

    struct npc_warsong_captainAI : public QuantumBasicAI
    {
		npc_warsong_captainAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainLightningTimer;
		uint32 LeserHealingWaveTimer;
		uint32 ChainHealTimer;
		uint32 GroundingTotemTimer;

        void Reset()
		{
			ChainLightningTimer = 2*IN_MILLISECONDS;
			LeserHealingWaveTimer = 4*IN_MILLISECONDS;
			ChainHealTimer = 6*IN_MILLISECONDS;
			GroundingTotemTimer = 10*IN_MILLISECONDS;

			me->Mount(MOUNT_BLACK_WOLF);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			DoCast(me, SPELL_WC_BLOODLUST);
			DoCast(me, SPELL_WC_GROUNDING_TOTEM);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChainLightningTimer <= diff)
			{
				DoCastVictim(SPELL_WC_CHAIN_LIGHTNING);
				ChainLightningTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ChainLightningTimer -= diff;

			if (LeserHealingWaveTimer <= diff)
			{
				DoCast(me, SPELL_WC_LESER_HEALING_WAVE);
				LeserHealingWaveTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else LeserHealingWaveTimer -= diff;

			if (ChainHealTimer <= diff)
			{
				DoCast(me, SPELL_WC_CHAIN_HEAL);
				ChainHealTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else ChainHealTimer -= diff;

			if (GroundingTotemTimer <= diff)
			{
				DoCast(me, SPELL_WC_GROUNDING_TOTEM);
				GroundingTotemTimer = urand(13*IN_MILLISECONDS, 15*IN_MILLISECONDS);
			}
			else GroundingTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warsong_captainAI(creature);
    }
};

enum WarsongMarksman
{
	SPELL_WM_ENRAGE = 8599,
	SPELL_WM_SHOOT  = 45578,
};

class npc_warsong_marksman : public CreatureScript
{
public:
    npc_warsong_marksman() : CreatureScript("npc_warsong_marksman") {}

    struct npc_warsong_marksmanAI : public QuantumBasicAI
    {
        npc_warsong_marksmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_HOLD_RIFLE);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_WM_SHOOT);
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
					DoCast(target, SPELL_WM_SHOOT);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_WM_ENRAGE, 0))
				{
					DoCast(me, SPELL_WM_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warsong_marksmanAI(creature);
    }
};

enum NerubarSkitterer
{
	SPELL_NS_PIERCING_BLOW = 49749,
};

class npc_nerubar_skitterer : public CreatureScript
{
public:
    npc_nerubar_skitterer() : CreatureScript("npc_nerubar_skitterer") {}

    struct npc_nerubar_skittererAI : public QuantumBasicAI
    {
        npc_nerubar_skittererAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PiercingBlowTimer;

        void Reset()
        {
			PiercingBlowTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PiercingBlowTimer <= diff)
			{
				DoCastVictim(SPELL_NS_PIERCING_BLOW);
				PiercingBlowTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PiercingBlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nerubar_skittererAI(creature);
    }
};

enum NerubarCorpseHarvester
{
	SPELL_NCH_VENOM_SPIT = 45577,
};

class npc_nerubar_corpse_harvester : public CreatureScript
{
public:
    npc_nerubar_corpse_harvester() : CreatureScript("npc_nerubar_corpse_harvester") {}

    struct npc_nerubar_corpse_harvesterAI : public QuantumBasicAI
    {
        npc_nerubar_corpse_harvesterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VenomSpitTimer;

        void Reset()
        {
			VenomSpitTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomSpitTimer <= diff)
			{
				DoCastVictim(SPELL_NCH_VENOM_SPIT);
				VenomSpitTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else VenomSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nerubar_corpse_harvesterAI(creature);
    }
};

enum NerubarWebLord
{
	SPELL_BLINDING_SWARM = 50284,
};

class npc_nerubar_web_lord : public CreatureScript
{
public:
    npc_nerubar_web_lord() : CreatureScript("npc_nerubar_web_lord") {}

    struct npc_nerubar_web_lordAI : public QuantumBasicAI
    {
        npc_nerubar_web_lordAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BlindingSwarmTimer;

        void Reset()
        {
			BlindingSwarmTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BlindingSwarmTimer <= diff)
			{
				DoCastVictim(SPELL_BLINDING_SWARM);
				BlindingSwarmTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BlindingSwarmTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nerubar_web_lordAI(creature);
    }
};

enum WarsongPeon
{
	SAY_RESCUED = 0,
};

class npc_warsong_peon : public CreatureScript
{
public:
    npc_warsong_peon() : CreatureScript("npc_warsong_peon") {}

    struct npc_warsong_peonAI : public QuantumBasicAI
    {
        npc_warsong_peonAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			Talk(SAY_RESCUED);
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
        return new npc_warsong_peonAI(creature);
    }
};

enum BloodsporeMoth
{
	SPELL_POLLEN         = 45610,
	SPELL_BM_WING_BUFFET = 32914,
};

class npc_bloodspore_moth : public CreatureScript
{
public:
    npc_bloodspore_moth() : CreatureScript("npc_bloodspore_moth") {}

    struct npc_bloodspore_mothAI : public QuantumBasicAI
    {
        npc_bloodspore_mothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PollenTimer;
		uint32 WingBuffetTimer;

        void Reset()
        {
			PollenTimer = 2*IN_MILLISECONDS;
			WingBuffetTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ANIMAL_BLOOD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PollenTimer <= diff)
			{
				DoCastAOE(SPELL_POLLEN);
				PollenTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PollenTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_BM_WING_BUFFET);
				WingBuffetTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodspore_mothAI(creature);
    }
};

enum DenVermin
{
	SPELL_DV_THRASH = 3391,
};

class npc_den_vermin : public CreatureScript
{
public:
    npc_den_vermin() : CreatureScript("npc_den_vermin") {}

    struct npc_den_verminAI : public QuantumBasicAI
    {
        npc_den_verminAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DV_THRASH);
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
        return new npc_den_verminAI(creature);
    }
};

enum IsleOfConquestBosses
{
	SPELL_BRUTAL_STRIKE = 58460,
	SPELL_CRUSHING_LEAP = 68506,
	SPELL_DAGGER_THROW  = 67280,
	SPELL_RAGE          = 66776, // After Exit Boss From Room

	AREA_HORDE_KEEP     = 4752,
	AREA_ALLIANCE_KEEP  = 4753,

	SAY_DEFENCE_KEEP    = -1555990,
};

class npc_overlord_agmar : public CreatureScript
{
public:
    npc_overlord_agmar() : CreatureScript("npc_overlord_agmar") {}

    struct npc_overlord_agmarAI : public QuantumBasicAI
    {
        npc_overlord_agmarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BrutalStrikeTimer;
		uint32 DaggerThrowTimer;

        void Reset()
        {
			BrutalStrikeTimer = 2*IN_MILLISECONDS;
			DaggerThrowTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_CRUSHING_LEAP);
			DoSendQuantumText(SAY_DEFENCE_KEEP, me);
		}

		void CheckArea()
		{
			if (me->GetAreaId() == !AREA_HORDE_KEEP)
				DoCast(me, SPELL_RAGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			CheckArea();

			if (BrutalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BRUTAL_STRIKE);
				BrutalStrikeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else BrutalStrikeTimer -= diff;

			if (DaggerThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DAGGER_THROW);
					DaggerThrowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else DaggerThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overlord_agmarAI(creature);
    }
};

class npc_high_commander_halford_wyrmbane : public CreatureScript
{
public:
    npc_high_commander_halford_wyrmbane() : CreatureScript("npc_high_commander_halford_wyrmbane") {}

    struct npc_high_commander_halford_wyrmbaneAI : public QuantumBasicAI
    {
        npc_high_commander_halford_wyrmbaneAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BrutalStrikeTimer;
		uint32 DaggerThrowTimer;

        void Reset()
        {
			BrutalStrikeTimer = 2*IN_MILLISECONDS;
			DaggerThrowTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_CRUSHING_LEAP);
			DoSendQuantumText(SAY_DEFENCE_KEEP, me);
		}

		void CheckArea()
		{
			if (me->GetAreaId() == !AREA_ALLIANCE_KEEP)
				DoCast(me, SPELL_RAGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			CheckArea();

			if (BrutalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BRUTAL_STRIKE);
				BrutalStrikeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else BrutalStrikeTimer -= diff;

			if (DaggerThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DAGGER_THROW);
					DaggerThrowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else DaggerThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_commander_halford_wyrmbaneAI(creature);
    }
};

enum WastesTaskmaster
{
	SPELL_WT_SHADOW_NOVA  = 32712,
	SPELL_WT_SHADOW_SHOCK = 16583,
	SPELL_DESTROY_GHOUL   = 51202,
};

class npc_wastes_taskmaster : public CreatureScript
{
public:
    npc_wastes_taskmaster() : CreatureScript("npc_wastes_taskmaster") {}

    struct npc_wastes_taskmasterAI : public QuantumBasicAI
    {
        npc_wastes_taskmasterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowShockTimer;
		uint32 ShadowNovaTimer;

        void Reset()
		{
			ShadowShockTimer = 1*IN_MILLISECONDS;
			ShadowNovaTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowShockTimer <= diff)
			{
				DoCastVictim(SPELL_WT_SHADOW_SHOCK);
				ShadowShockTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ShadowShockTimer -= diff;

			if (ShadowNovaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WT_SHADOW_NOVA);
					ShadowNovaTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowNovaTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
				DoCast(me, SPELL_DESTROY_GHOUL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wastes_taskmasterAI(creature);
    }
};

enum EmaciatedMammoth
{
	SPELL_EM_TRAMPLE = 51944,
};

class npc_emaciated_mammoth : public CreatureScript
{
public:
    npc_emaciated_mammoth() : CreatureScript("npc_emaciated_mammoth") {}

    struct npc_emaciated_mammothAI : public QuantumBasicAI
    {
        npc_emaciated_mammothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TrampleTimer;

        void Reset()
		{
			TrampleTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_EM_TRAMPLE);
				TrampleTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emaciated_mammothAI(creature);
    }
};

class npc_emaciated_mammoth_bull : public CreatureScript
{
public:
    npc_emaciated_mammoth_bull() : CreatureScript("npc_emaciated_mammoth_bull") {}

    struct npc_emaciated_mammoth_bullAI : public QuantumBasicAI
    {
        npc_emaciated_mammoth_bullAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TrampleTimer;

        void Reset()
		{
			TrampleTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_EM_TRAMPLE);
				TrampleTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emaciated_mammoth_bullAI(creature);
    }
};

enum WyrmrestDrakes
{
	SPELL_FROST_BREATH        = 49317,
	SPELL_AD_FROST_BREATH     = 49111,
	SPELL_INVOKER             = 50287,

	NPC_WYRMREST_TEMPLE_DRAKE = 26925,
	NPC_WYRMREST_GUARDIAN     = 26933,
};

class npc_azure_drake : public CreatureScript
{
public:
    npc_azure_drake() : CreatureScript("npc_azure_drake") {}

    struct npc_azure_drakeAI : public QuantumBasicAI
    {
        npc_azure_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBreathTimer;

        void Reset()
        {
			FrostBreathTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_WYRMREST_TEMPLE_DRAKE || NPC_WYRMREST_GUARDIAN)
				return;
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				me->CastSpell(killer, SPELL_INVOKER, true);
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
				FrostBreathTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_drakeAI(creature);
    }
};

class npc_azure_dragon : public CreatureScript
{
public:
    npc_azure_dragon() : CreatureScript("npc_azure_dragon") {}

    struct npc_azure_dragonAI : public QuantumBasicAI
    {
        npc_azure_dragonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBreathTimer;

        void Reset()
        {
			FrostBreathTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_WYRMREST_TEMPLE_DRAKE || NPC_WYRMREST_GUARDIAN)
				return;
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				me->CastSpell(killer, SPELL_INVOKER, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBreathTimer <= diff)
			{
				DoCastVictim(SPELL_AD_FROST_BREATH);
				FrostBreathTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_dragonAI(creature);
    }
};

enum Ragemane
{
	SPELL_COSMETIC_ORANGE_CLOUD = 54942,
	SPELL_R_CHARGE              = 51492,
	SPELL_FLIPPER_THWACK        = 50169,
	SPELL_R_ENRAGE              = 50420,
};

class npc_ragemane : public CreatureScript
{
public:
    npc_ragemane() : CreatureScript("npc_ragemane") {}

    struct npc_ragemaneAI : public QuantumBasicAI
    {
        npc_ragemaneAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlipperThwackTimer;

        void Reset()
        {
			FlipperThwackTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_COSMETIC_ORANGE_CLOUD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_R_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlipperThwackTimer <= diff)
			{
				DoCastVictim(SPELL_FLIPPER_THWACK);
				FlipperThwackTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FlipperThwackTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_R_ENRAGE, 0))
				{
					DoCast(me, SPELL_R_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ragemaneAI(creature);
    }
};

enum DrakuruProphet
{
	SPELL_PLAGUE_CLOUD = 50366,
};

class npc_drakuru_prophet : public CreatureScript
{
public:
    npc_drakuru_prophet() : CreatureScript("npc_drakuru_prophet") {}

    struct npc_drakuru_prophetAI : public QuantumBasicAI
    {
        npc_drakuru_prophetAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_PLAGUE_CLOUD);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakuru_prophetAI(creature);
    }
};

enum DrakuruBerserker
{
	SPELL_DIRE_STOMP = 54959,
};

class npc_drakuru_berserker : public CreatureScript
{
public:
    npc_drakuru_berserker() : CreatureScript("npc_drakuru_berserker") {}

    struct npc_drakuru_berserkerAI : public QuantumBasicAI
    {
        npc_drakuru_berserkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DireStompTimer;

        void Reset()
        {
			DireStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DireStompTimer <= diff)
			{
				DoCastAOE(SPELL_DIRE_STOMP);
				DireStompTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DireStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakuru_berserkerAI(creature);
    }
};

enum EnchantedTikiWarrior
{
	SPELL_TIKI_VISUAL_1 = 52614,
	SPELL_TIKI_VISUAL_2 = 52617,
	SPELL_TIKI_VISUAL_3 = 52618,
	SPELL_TIKI_VISUAL_4 = 52619,
	SPELL_SPIRIT_BURN   = 54647,
	SPELL_INCORPOREAL   = 54657,

	TIKI_DISPLAY_ID     = 25749,
};

class npc_enchanted_tiki_warrior : public CreatureScript
{
public:
    npc_enchanted_tiki_warrior() : CreatureScript("npc_enchanted_tiki_warrior") {}

    struct npc_enchanted_tiki_warriorAI : public QuantumBasicAI
    {
        npc_enchanted_tiki_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IncorporealTimer;

        void Reset()
        {
			IncorporealTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_TIKI_VISUAL_1);
			DoCast(me, SPELL_TIKI_VISUAL_2);
			DoCast(me, SPELL_TIKI_VISUAL_3);
			DoCast(me, SPELL_TIKI_VISUAL_4);
			DoCast(me, SPELL_SPIRIT_BURN);

			me->SetDisplayId(TIKI_DISPLAY_ID);

			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IncorporealTimer <= diff)
			{
				DoCast(me, SPELL_INCORPOREAL);
				IncorporealTimer = 2*IN_MILLISECONDS;
			}
			else IncorporealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enchanted_tiki_warriorAI(creature);
    }
};

enum Yara
{
	SPELL_Y_STRIKE  = 14516,
	SPELL_SNAP_KICK = 15618,
	SPELL_SPRINT    = 48594,
};

class npc_yara : public CreatureScript
{
public:
    npc_yara() : CreatureScript("npc_yara") {}

    struct npc_yaraAI : public QuantumBasicAI
    {
        npc_yaraAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 SnapKickTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;
			SnapKickTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SPRINT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_Y_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (SnapKickTimer <= diff)
			{
				DoCastVictim(SPELL_SNAP_KICK);
				SnapKickTimer = 6*IN_MILLISECONDS;
			}
			else SnapKickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_yaraAI(creature);
    }
};

enum DrekMaz
{
	SPELL_DM_ENRAGE        = 8599,
	SPELL_DM_MORTAL_STRIKE = 16856,
	SPELL_DM_WHIRLWIND     = 49807,
};

class npc_drek_maz : public CreatureScript
{
public:
    npc_drek_maz() : CreatureScript("npc_drek_maz") {}

    struct npc_drek_mazAI : public QuantumBasicAI
    {
        npc_drek_mazAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;
			WhirlwindTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_DM_MORTAL_STRIKE);
				MortalStrikeTimer = 4*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_DM_WHIRLWIND);
				WhirlwindTimer = 6*IN_MILLISECONDS;
			}
			else WhirlwindTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_DM_ENRAGE, 0))
				{
					DoCast(me, SPELL_DM_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drek_mazAI(creature);
    }
};

enum Tiri
{
	SPELL_CONCUSSION_BLOW = 52719,
	SPELL_RENEW           = 25058,
};

class npc_tiri : public CreatureScript
{
public:
    npc_tiri() : CreatureScript("npc_tiri") {}

    struct npc_tiriAI : public QuantumBasicAI
    {
        npc_tiriAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ConcussionBlowTimer;
		uint32 RenewTimer;

        void Reset()
        {
			ConcussionBlowTimer = 2*IN_MILLISECONDS;
			RenewTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ConcussionBlowTimer <= diff)
			{
				DoCastVictim(SPELL_CONCUSSION_BLOW);
				ConcussionBlowTimer = 5*IN_MILLISECONDS;
			}
			else ConcussionBlowTimer -= diff;

			if (RenewTimer <= diff)
			{
				DoCast(me, SPELL_RENEW);
				RenewTimer = 8*IN_MILLISECONDS;
			}
			else RenewTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tiriAI(creature);
    }
};

enum GundrakSavage
{
	SPELL_SAVAGE = 5515,
};

class npc_gundrak_savage : public CreatureScript
{
public:
    npc_gundrak_savage() : CreatureScript("npc_gundrak_savage") {}

    struct npc_gundrak_savageAI : public QuantumBasicAI
    {
        npc_gundrak_savageAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SavageTimer;

        void Reset()
        {
			SavageTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SavageTimer <= diff)
			{
				DoCast(me, SPELL_SAVAGE);
				SavageTimer = 6*IN_MILLISECONDS;
			}
			else SavageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gundrak_savageAI(creature);
    }
};

enum MamtothDisciple
{
	SPELL_BLUE_RADIATION   = 52574,
	SPELL_MD_MORTAL_STRIKE = 37335,
	SPELL_FEIGN_DEATH      = 29266,
};

class npc_mamtoth_disciple : public CreatureScript
{
public:
    npc_mamtoth_disciple() : CreatureScript("npc_mamtoth_disciple") {}

    struct npc_mamtoth_discipleAI : public QuantumBasicAI
    {
        npc_mamtoth_discipleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_BLUE_RADIATION);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_COWER);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MD_MORTAL_STRIKE);
				MortalStrikeTimer = 5*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mamtoth_discipleAI(creature);
    }
};

class npc_dead_mamtoth_disciple : public CreatureScript
{
public:
    npc_dead_mamtoth_disciple() : CreatureScript("npc_dead_mamtoth_disciple") {}

    struct npc_dead_mamtoth_discipleAI : public QuantumBasicAI
    {
        npc_dead_mamtoth_discipleAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_BLUE_RADIATION);
			DoCast(me, SPELL_FEIGN_DEATH);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dead_mamtoth_discipleAI(creature);
    }
};

enum BloodofMamtoth
{
	SPELL_COAGULATE         = 54580,
	SPELL_BM_BLUE_RADIATION = 54262,
	SPELL_BLUE_BANISH_STATE = 54263,
};

class npc_blood_of_mamtoth : public CreatureScript
{
public:
    npc_blood_of_mamtoth() : CreatureScript("npc_blood_of_mamtoth") {}

    struct npc_blood_of_mamtothAI : public QuantumBasicAI
    {
        npc_blood_of_mamtothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CoagulateTimer;

        void Reset()
        {
			CoagulateTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_BM_BLUE_RADIATION);
			DoCast(me, SPELL_BLUE_BANISH_STATE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CoagulateTimer <= diff)
			{
				DoCast(me, SPELL_COAGULATE);
				CoagulateTimer = 6*IN_MILLISECONDS;
			}
			else CoagulateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blood_of_mamtothAI(creature);
    }
};

enum AltarWarden
{
	SPELL_GUST_OF_WIND   = 54588,
	SPELL_WHIRLING_WINDS = 54589,
	SPELL_ZEPHYR         = 50215,
};

class npc_altar_warden : public CreatureScript
{
public:
    npc_altar_warden() : CreatureScript("npc_altar_warden") {}

    struct npc_altar_wardenAI : public QuantumBasicAI
    {
        npc_altar_wardenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GustOfWindTimer;
		uint32 WhirlingWindsTimer;

        void Reset()
        {
			GustOfWindTimer = 2*IN_MILLISECONDS;
			WhirlingWindsTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ZEPHYR, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GustOfWindTimer <= diff)
			{
				DoCastVictim(SPELL_GUST_OF_WIND);
				GustOfWindTimer = 4*IN_MILLISECONDS;
			}
			else GustOfWindTimer -= diff;

			if (WhirlingWindsTimer <= diff)
			{
				DoCast(me, SPELL_WHIRLING_WINDS);
				WhirlingWindsTimer = 8*IN_MILLISECONDS;
			}
			else WhirlingWindsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_altar_wardenAI(creature);
    }
};

enum ProphetofQuetzlun
{
	SPELL_SERPENT_FORM       = 54601,
	SPELL_LINGERING_PRESENCE = 54606,
};

class npc_prophet_of_quetzlun : public CreatureScript
{
public:
    npc_prophet_of_quetzlun() : CreatureScript("npc_prophet_of_quetzlun") {}

    struct npc_prophet_of_quetzlunAI : public QuantumBasicAI
    {
        npc_prophet_of_quetzlunAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SerpentFormTimer;
		uint32 LingeringPresenceTimer;

        void Reset()
        {
			SerpentFormTimer = 4*IN_MILLISECONDS;
			LingeringPresenceTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SerpentFormTimer <= diff)
			{
				DoCast(me, SPELL_SERPENT_FORM, true);
				SerpentFormTimer = 12*IN_MILLISECONDS;
			}
			else SerpentFormTimer -= diff;

			if (LingeringPresenceTimer <= diff)
			{
				DoCast(me, SPELL_LINGERING_PRESENCE);
				LingeringPresenceTimer = 14*IN_MILLISECONDS;
			}
			else LingeringPresenceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prophet_of_quetzlunAI(creature);
    }
};

enum QuetzlunWorshipper
{
	SPELL_SERPENT_STRIKE = 54594,
};

class npc_quetzlun_worshipper : public CreatureScript
{
public:
    npc_quetzlun_worshipper() : CreatureScript("npc_quetzlun_worshipper") {}

    struct npc_quetzlun_worshipperAI : public QuantumBasicAI
    {
        npc_quetzlun_worshipperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SerpentFormTimer;

        void Reset()
        {
			SerpentFormTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_COWER);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SERPENT_STRIKE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SerpentFormTimer <= diff)
			{
				DoCast(me, SPELL_SERPENT_FORM, true);
				SerpentFormTimer = 14*IN_MILLISECONDS;
			}
			else SerpentFormTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_quetzlun_worshipperAI(creature);
    }
};

enum HighPriestessTuaTua
{
	SPELL_MIND_BLAST        = 13860,
	SPELL_SHADOW_WORD_DEATH = 51818,
	SPELL_SHADOWFORM        = 29406,

	PRIESTESS_SAY_AGGRO     = -1562012,
};

class npc_high_priestess_tua_tua : public CreatureScript
{
public:
    npc_high_priestess_tua_tua() : CreatureScript("npc_high_priestess_tua_tua") {}

    struct npc_high_priestess_tua_tuaAI : public QuantumBasicAI
    {
        npc_high_priestess_tua_tuaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MindBlastTimer;
		uint32 ShadowWordDeathTimer;

        void Reset()
        {
			MindBlastTimer = 1*IN_MILLISECONDS;
			ShadowWordDeathTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(PRIESTESS_SAY_AGGRO, me);
			DoCast(me, SPELL_SHADOWFORM);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MindBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_BLAST);
					MindBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindBlastTimer -= diff;

			if (ShadowWordDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_WORD_DEATH);
					ShadowWordDeathTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else ShadowWordDeathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_priestess_tua_tuaAI(creature);
    }
};

enum ScionofQuetzlun
{
	SPELL_LIGHTNING_BREATH = 38193,
	SPELL_TORMENT          = 54526,
};

class npc_scion_of_quetzlun : public CreatureScript
{
public:
    npc_scion_of_quetzlun() : CreatureScript("npc_scion_of_quetzlun") {}

    struct npc_scion_of_quetzlunAI : public QuantumBasicAI
    {
        npc_scion_of_quetzlunAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBreathTimer;
		uint32 TormentTimer;

        void Reset()
        {
			LightningBreathTimer = 1*IN_MILLISECONDS;
			TormentTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningBreathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LIGHTNING_BREATH);
					LightningBreathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBreathTimer -= diff;

			if (TormentTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TORMENT, true);
					TormentTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else TormentTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scion_of_quetzlunAI(creature);
    }
};

enum ClawofHarkoa
{
	SPELL_FROSTBRAND_ATTACK = 54608,
};

class npc_claw_of_harkoa : public CreatureScript
{
public:
    npc_claw_of_harkoa() : CreatureScript("npc_claw_of_harkoa") {}

    struct npc_claw_of_harkoaAI : public QuantumBasicAI
    {
        npc_claw_of_harkoaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostbrandAttackTimer;

        void Reset()
        {
			FrostbrandAttackTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_DUAL_WIELD, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_FROSTBRAND_ATTACK);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostbrandAttackTimer <= diff)
			{
				DoCastVictim(SPELL_FROSTBRAND_ATTACK);
				FrostbrandAttackTimer = 4*IN_MILLISECONDS;
			}
			else FrostbrandAttackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_claw_of_harkoaAI(creature);
    }
};

enum FrostbroodWhelp
{
	SPELL_FW_FROST_BLAST = 60814,
};

class npc_frostbrood_whelp : public CreatureScript
{
public:
    npc_frostbrood_whelp() : CreatureScript("npc_frostbrood_whelp") {}

    struct npc_frostbrood_whelpAI : public QuantumBasicAI
    {
        npc_frostbrood_whelpAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBlastTimer;

        void Reset()
        {
			FrostBlastTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FW_FROST_BLAST);
					FrostBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostbrood_whelpAI(creature);
    }
};

enum HarkoanSubduer
{
	SPELL_HS_BLIZZARD  = 8364,
	SPELL_HS_FROSTBOLT = 20822,
};

class npc_harkoan_subduer : public CreatureScript
{
public:
    npc_harkoan_subduer() : CreatureScript("npc_harkoan_subduer") {}

    struct npc_harkoan_subduerAI : public QuantumBasicAI
    {
        npc_harkoan_subduerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 BlizzardTimer;

        void Reset()
        {
			FrostboltTimer = 1*IN_MILLISECONDS;
			BlizzardTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_HS_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HS_BLIZZARD, true);
					BlizzardTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_harkoan_subduerAI(creature);
    }
};

enum ProphetofHarkoa
{
	SPELL_PH_CONCUSSION_BLOW = 52719,
};

class npc_prophet_of_harkoa : public CreatureScript
{
public:
    npc_prophet_of_harkoa() : CreatureScript("npc_prophet_of_harkoa") {}

    struct npc_prophet_of_harkoaAI : public QuantumBasicAI
    {
        npc_prophet_of_harkoaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ConcussionBlowTimer;

        void Reset()
        {
			ConcussionBlowTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ConcussionBlowTimer <= diff)
			{
				DoCastVictim(SPELL_PH_CONCUSSION_BLOW);
				ConcussionBlowTimer = 4*IN_MILLISECONDS;
			}
			else ConcussionBlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_prophet_of_harkoaAI(creature);
    }
};

enum JinAlaiWarrior
{
	SPELL_JW_THROW            = 38556,
	SPELL_POISON_TIPPED_SPEAR = 54630,
};

class npc_jin_alai_warrior : public CreatureScript
{
public:
    npc_jin_alai_warrior() : CreatureScript("npc_jin_alai_warrior") {}

    struct npc_jin_alai_warriorAI : public QuantumBasicAI
    {
        npc_jin_alai_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;

        void Reset()
        {
			ThrowTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_POISON_TIPPED_SPEAR);
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
					DoCast(target, SPELL_JW_THROW);
					ThrowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_jin_alai_warriorAI(creature);
    }
};

enum JinAlaiMedicineMan
{
	SPELL_SHADOW_CHANNELING    = 51733,
	SPELL_VIAL_OF_POISON       = 54626,
	SPELL_DRINK_HEALING_POTION = 54633,
};

class npc_jin_alai_medicine_man : public CreatureScript
{
public:
    npc_jin_alai_medicine_man() : CreatureScript("npc_jin_alai_medicine_man") {}

    struct npc_jin_alai_medicine_manAI : public QuantumBasicAI
    {
        npc_jin_alai_medicine_manAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VialOfPoisonTimer;

        void Reset()
        {
			VialOfPoisonTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_SHADOW_CHANNELING);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VialOfPoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VIAL_OF_POISON);
					VialOfPoisonTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else VialOfPoisonTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_DRINK_HEALING_POTION);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_jin_alai_medicine_manAI(creature);
    }
};

enum Kutubesa
{
	SPELL_KUTUBE_SA_TREASURE = 51815,
	SPELL_KS_WHIRLWIND       = 49807,
	SPELL_VICIOUS_REND       = 35144,
	SPELL_KS_HEROIC_LEAP     = 57793,

	KUTUBESA_SAY_AGGRO       = 0,
};

class npc_kutubesa : public CreatureScript
{
public:
    npc_kutubesa() : CreatureScript("npc_kutubesa") {}

    struct npc_kutubesaAI : public QuantumBasicAI
    {
        npc_kutubesaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ViciousRendTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			ViciousRendTimer = 2*IN_MILLISECONDS;
			WhirlwindTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(KUTUBESA_SAY_AGGRO);
			DoCastVictim(SPELL_KS_HEROIC_LEAP);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(SPELL_KUTUBE_SA_TREASURE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ViciousRendTimer <= diff)
			{
				DoCastVictim(SPELL_VICIOUS_REND);
				ViciousRendTimer = 4*IN_MILLISECONDS;
			}
			else ViciousRendTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_KS_WHIRLWIND);
				WhirlwindTimer = 6*IN_MILLISECONDS;
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_kutubesaAI(creature);
    }
};

enum Gawanil
{
	SPELL_GAWANIL_TREASURE  = 51814,
	SPELL_GW_CURSE_OF_AGONY = 14868,
	SPELL_GW_CORRUPTION     = 39212,
	SPELL_GW_SIPHON_LIFE    = 35195,
	SPELL_GW_SHADOW_BOLT    = 20791,

	GAWANIL_SAY_AGGRO       = 0,
};

class npc_gawanil : public CreatureScript
{
public:
    npc_gawanil() : CreatureScript("npc_gawanil") {}

    struct npc_gawanilAI : public QuantumBasicAI
    {
        npc_gawanilAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfAgonyTimer;
		uint32 CorruptionTimer;
		uint32 SiphonLifeTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			CurseOfAgonyTimer = 1*IN_MILLISECONDS;
			CorruptionTimer = 2*IN_MILLISECONDS;
			SiphonLifeTimer = 3*IN_MILLISECONDS;
			ShadowBoltTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(GAWANIL_SAY_AGGRO);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(SPELL_GAWANIL_TREASURE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfAgonyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GW_CURSE_OF_AGONY);
					CurseOfAgonyTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfAgonyTimer -= diff;

			if (CorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GW_CORRUPTION);
					CorruptionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else CorruptionTimer -= diff;

			if (SiphonLifeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GW_SIPHON_LIFE);
					SiphonLifeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else SiphonLifeTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GW_SHADOW_BOLT);
					ShadowBoltTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_gawanilAI(creature);
    }
};

enum ChulotheMad
{
	SPELL_CHULO_THE_MAD_TREASURE = 51813,
	SPELL_STORMSTRIKE            = 51876,
	SPELL_WILDLY_FLAILING        = 50188,

	CHULO_SAY_AGGRO              = 0,
};

class npc_chulo_the_mad : public CreatureScript
{
public:
    npc_chulo_the_mad() : CreatureScript("npc_chulo_the_mad") {}

    struct npc_chulo_the_madAI : public QuantumBasicAI
    {
        npc_chulo_the_madAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StormStrikeTimer;
		uint32 WildlyFlailingTimer;

        void Reset()
        {
			StormStrikeTimer = 2*IN_MILLISECONDS;
			WildlyFlailingTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(CHULO_SAY_AGGRO);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(SPELL_CHULO_THE_MAD_TREASURE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StormStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STORMSTRIKE);
				StormStrikeTimer = 4*IN_MILLISECONDS;
			}
			else StormStrikeTimer -= diff;

			if (WildlyFlailingTimer <= diff)
			{
				DoCastVictim(SPELL_WILDLY_FLAILING);
				WildlyFlailingTimer = 6*IN_MILLISECONDS;
			}
			else WildlyFlailingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_chulo_the_madAI(creature);
    }
};

enum ZimTorgaDefender
{
	SPELL_ICE_BOULDER        = 54673,
	SPELL_TOSS_ICE_BOULDER_1 = 51511,
	SPELL_TOSS_ICE_BOULDER_2 = 51590,
};

class npc_zim_torga_defender : public CreatureScript
{
public:
    npc_zim_torga_defender() : CreatureScript("npc_zim_torga_defender") {}

    struct npc_zim_torga_defenderAI : public QuantumBasicAI
    {
        npc_zim_torga_defenderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IceBoulderTimer;
		uint32 TossIceBoulderTimer;

        void Reset()
        {
			IceBoulderTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			TossIceBoulderTimer = 0;
		}

        void UpdateAI(uint32 const diff)
        {
			if (TossIceBoulderTimer <= diff)
			{
				DoCast(SPELL_TOSS_ICE_BOULDER_1);
				TossIceBoulderTimer = 4*IN_MILLISECONDS;
			}
			else TossIceBoulderTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IceBoulderTimer <= diff)
			{
				DoCastVictim(SPELL_ICE_BOULDER);
				IceBoulderTimer = 5*IN_MILLISECONDS;
			}
			else IceBoulderTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zim_torga_defenderAI(creature);
    }
};

enum ElementalRift
{
	SPELL_ER_FROSTBOLT        = 9672,
	SPELL_ER_CHAIN_LIGHTNING  = 12058,
	SPELL_AIR_RIFT_VISUAL     = 51638,
	SPELL_SUMMON_FROZEN_EARTH = 51648,
};

class npc_elemental_rift : public CreatureScript
{
public:
    npc_elemental_rift() : CreatureScript("npc_elemental_rift") {}

    struct npc_elemental_riftAI : public QuantumBasicAI
    {
        npc_elemental_riftAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 ChainLightningTimer;
		uint32 SummonFrozenEarthTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			ChainLightningTimer = 3*IN_MILLISECONDS;
			SummonFrozenEarthTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_AIR_RIFT_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_FROZEN_EARTH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				DoCastVictim(SPELL_ER_FROSTBOLT, true);
				FrostboltTimer = 3*IN_MILLISECONDS;
			}
			else FrostboltTimer -= diff;

			if (ChainLightningTimer <= diff)
			{
				DoCastVictim(SPELL_ER_CHAIN_LIGHTNING, true);
				ChainLightningTimer = 5*IN_MILLISECONDS;
			}
			else ChainLightningTimer -= diff;

			if (SummonFrozenEarthTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_FROZEN_EARTH);
				SummonFrozenEarthTimer = 10*IN_MILLISECONDS;
			}
			else SummonFrozenEarthTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_elemental_riftAI(creature);
    }
};

enum FrozenEarth
{
	SPELL_ICE_SPIKE = 54532,
};

class npc_frozen_earth : public CreatureScript
{
public:
    npc_frozen_earth() : CreatureScript("npc_frozen_earth") {}

    struct npc_frozen_earthAI : public QuantumBasicAI
    {
        npc_frozen_earthAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IceSpikeTimer;

        void Reset()
        {
			IceSpikeTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IceSpikeTimer <= diff)
			{
				DoCastVictim(SPELL_ICE_SPIKE);
				IceSpikeTimer = 4*IN_MILLISECONDS;
			}
			else IceSpikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_frozen_earthAI(creature);
    }
};

enum ProphetofRhunok
{
	SPELL_PR_CHARGE       = 49758,
	SPELL_PR_VICIOUS_REND = 16095,

	RHUNOK_SAY_AGGRO      = -1562013,
};

class npc_prophet_of_rhunok : public CreatureScript
{
public:
    npc_prophet_of_rhunok() : CreatureScript("npc_prophet_of_rhunok") {}

    struct npc_prophet_of_rhunokAI : public QuantumBasicAI
    {
        npc_prophet_of_rhunokAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ViciousRendTimer;

        void Reset()
        {
			ViciousRendTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_DROWNED);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RHUNOK_SAY_AGGRO, me);
			DoCastVictim(SPELL_PR_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ViciousRendTimer <= diff)
			{
				DoCastVictim(SPELL_PR_VICIOUS_REND);
				ViciousRendTimer = 2*IN_MILLISECONDS;
			}
			else ViciousRendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_prophet_of_rhunokAI(creature);
    }
};

enum HebDrakkars
{
	SPELL_HS_STRIKE          = 11976,
	SPELL_RABIES             = 51951,
	SPELL_HD_STRIKE          = 11976,
	SPELL_HD_WHIRLWIND       = 49807,
	MOUNT_ID_DRAKKAR_STRIKER = 9074,
};

class npc_heb_drakkar_striker : public CreatureScript
{
public:
    npc_heb_drakkar_striker() : CreatureScript("npc_heb_drakkar_striker") {}

    struct npc_heb_drakkar_strikerAI : public QuantumBasicAI
    {
        npc_heb_drakkar_strikerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 RabiesTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;
			RabiesTimer = 4*IN_MILLISECONDS;

			me->Mount(MOUNT_ID_DRAKKAR_STRIKER);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HS_STRIKE);
				StrikeTimer = 3*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (RabiesTimer <= diff)
			{
				DoCastVictim(SPELL_RABIES);
				RabiesTimer = 5*IN_MILLISECONDS;
			}
			else RabiesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_heb_drakkar_strikerAI(creature);
    }
};

class npc_heb_drakkar_headhunter : public CreatureScript
{
public:
    npc_heb_drakkar_headhunter() : CreatureScript("npc_heb_drakkar_headhunter") {}

    struct npc_heb_drakkar_headhunterAI : public QuantumBasicAI
    {
        npc_heb_drakkar_headhunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;
			WhirlwindTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HD_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_HD_WHIRLWIND);
				WhirlwindTimer = 6*IN_MILLISECONDS;
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_heb_drakkar_headhunterAI(creature);
    }
};

enum WitheredArgentFootman
{
	SPELL_ORANGE_RADIATION = 45857,
	SPELL_WITHERED_TOUCH   = 54450,
};

class npc_withered_argent_footman : public CreatureScript
{
public:
    npc_withered_argent_footman() : CreatureScript("npc_withered_argent_footman") {}

    struct npc_withered_argent_footmanAI : public QuantumBasicAI
    {
        npc_withered_argent_footmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WitheredTouchTimer;

        void Reset()
        {
			WitheredTouchTimer = 2*IN_MILLISECONDS;

			me->AddAura(SPELL_ORANGE_RADIATION, me);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* /*killer*/)
		{
			me->RemoveAurasDueToSpell(SPELL_ORANGE_RADIATION);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WitheredTouchTimer <= diff)
			{
				DoCastVictim(SPELL_WITHERED_TOUCH);
				WitheredTouchTimer = 4*IN_MILLISECONDS;
			}
			else WitheredTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_withered_argent_footmanAI(creature);
    }
};

enum HatharBroodmaster
{
	SPELL_HB_SHADOW_BOLT = 9613,
	SPELL_HB_WEB_WRAP    = 54453,
};

class npc_hathar_broodmaster : public CreatureScript
{
public:
    npc_hathar_broodmaster() : CreatureScript("npc_hathar_broodmaster") {}

    struct npc_hathar_broodmasterAI : public QuantumBasicAI
    {
        npc_hathar_broodmasterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltTimer;
		uint32 WebWrapTimer;

        void Reset()
        {
			ShadowBoltTimer = 1*IN_MILLISECONDS;
			WebWrapTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_HB_SHADOW_BOLT);
					ShadowBoltTimer = 3*IN_MILLISECONDS;
				}
			}
			else ShadowBoltTimer -= diff;

			if (WebWrapTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HB_WEB_WRAP);
					WebWrapTimer = 5*IN_MILLISECONDS;
				}
			}
			else WebWrapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_hathar_broodmasterAI(creature);
    }
};

enum BurningSkimmer
{
	SPELL_COWARDICE       = 8224,
	SPELL_FLAMES_COSMETIC = 39199,
};

class npc_burning_skimmer : public CreatureScript
{
public:
    npc_burning_skimmer() : CreatureScript("npc_burning_skimmer") {}

    struct npc_burning_skimmerAI : public QuantumBasicAI
    {
        npc_burning_skimmerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_COWARDICE);
			DoCast(me, SPELL_FLAMES_COSMETIC);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
		return new npc_burning_skimmerAI(creature);
    }
};

enum CaptiveFootman
{
	SAY_FOOTMAN_SPAWN_1 = -1562014,
	SAY_FOOTMAN_SPAWN_2 = -1562015,
	SAY_FOOTMAN_SPAWN_3 = -1562016,
};

class npc_captive_footman : public CreatureScript
{
public:
    npc_captive_footman() : CreatureScript("npc_captive_footman") {}

    struct npc_captive_footmanAI : public QuantumBasicAI
    {
        npc_captive_footmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
        {
			SayTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void UpdateAI(uint32 const diff)
        {
			if (SayTimer <= diff)
			{
				DoSendQuantumText(RAND(SAY_FOOTMAN_SPAWN_1, SAY_FOOTMAN_SPAWN_2, SAY_FOOTMAN_SPAWN_3), me);
				SayTimer = 75*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_captive_footmanAI(creature);
    }
};

enum DrakkariCaptive
{
	SPELL_DC_WITHERED_TOUCH = 54451,
};

class npc_drakkari_captive : public CreatureScript
{
public:
    npc_drakkari_captive() : CreatureScript("npc_drakkari_captive") {}

    struct npc_drakkari_captiveAI : public QuantumBasicAI
    {
        npc_drakkari_captiveAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WitheredTouchTimer;

        void Reset()
        {
			WitheredTouchTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WitheredTouchTimer <= diff)
			{
				DoCastVictim(SPELL_DC_WITHERED_TOUCH);
				WitheredTouchTimer = 5*IN_MILLISECONDS;
			}
			else WitheredTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_drakkari_captiveAI(creature);
    }
};

enum HatharSkimmer
{
	SPELL_HS_JUMP_ATTACK = 54487,
	SPELL_ENCASING_WEBS  = 51152,
};

class npc_hathar_skimmer : public CreatureScript
{
public:
    npc_hathar_skimmer() : CreatureScript("npc_hathar_skimmer") {}

    struct npc_hathar_skimmerAI : public QuantumBasicAI
    {
        npc_hathar_skimmerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EncasingWebsTimer;

        void Reset()
        {
			EncasingWebsTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_HS_JUMP_ATTACK);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EncasingWebsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ENCASING_WEBS);
					EncasingWebsTimer = 4*IN_MILLISECONDS;
				}
			}
			else EncasingWebsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_hathar_skimmerAI(creature);
    }
};

enum HatharNecromagus
{
	SPELL_SUMMON_SCOURGED_ARGENT_FOOTMAN = 51148,
	SPELL_CONVERSION_BEAM                = 54491,
	SPELL_SCOURGING_REANIMATION          = 51139,

	NPC_SCOURGED_ARGENT_FOOTMAN          = 28268,
};

class npc_hathar_necromagus : public CreatureScript
{
public:
    npc_hathar_necromagus() : CreatureScript("npc_hathar_necromagus") {}

    struct npc_hathar_necromagusAI : public QuantumBasicAI
    {
        npc_hathar_necromagusAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ConversionBeamTimer;
		uint32 ScourgingReanimationTimer;

		SummonList Summons;

        void Reset()
        {
			ConversionBeamTimer = 2*IN_MILLISECONDS;
			ScourgingReanimationTimer = 5*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SCOURGED_ARGENT_FOOTMAN)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ConversionBeamTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CONVERSION_BEAM);
					ConversionBeamTimer = 5*IN_MILLISECONDS;
				}
			}
			else ConversionBeamTimer -= diff;

			if (ScourgingReanimationTimer <= diff)
			{
				DoCast(SPELL_SCOURGING_REANIMATION);
				DoCast(me, SPELL_SUMMON_SCOURGED_ARGENT_FOOTMAN, true);
				ScourgingReanimationTimer = 8*IN_MILLISECONDS;
			}
			else ScourgingReanimationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_hathar_necromagusAI(creature);
    }
};

enum ScourgedArgentFootman
{
	SPELL_SA_WITHERED_TOUCH = 54451,
};

class npc_scourged_argent_footman : public CreatureScript
{
public:
    npc_scourged_argent_footman() : CreatureScript("npc_scourged_argent_footman") {}

    struct npc_scourged_argent_footmanAI : public QuantumBasicAI
    {
        npc_scourged_argent_footmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WitheredTouchTimer;

        void Reset()
        {
			WitheredTouchTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WitheredTouchTimer <= diff)
			{
				DoCastVictim(SPELL_SA_WITHERED_TOUCH);
				WitheredTouchTimer = 5*IN_MILLISECONDS;
			}
			else WitheredTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_scourged_argent_footmanAI(creature);
    }
};

enum DrakkariWaterBinder
{
	SPELL_DW_LIGHTNING_BOLT = 9532,
	SPELL_WATER_BUBBLE      = 54399,
};

class npc_drakkari_water_binder : public CreatureScript
{
public:
    npc_drakkari_water_binder() : CreatureScript("npc_drakkari_water_binder") {}

    struct npc_drakkari_water_binderAI : public QuantumBasicAI
    {
        npc_drakkari_water_binderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LigtningBoltTimer;
		uint32 WaterBubbleTimer;

        void Reset()
        {
			LigtningBoltTimer = 1*IN_MILLISECONDS;
			WaterBubbleTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_DANCE);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LigtningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DW_LIGHTNING_BOLT);
					LigtningBoltTimer = 3*IN_MILLISECONDS;
				}
			}
			else LigtningBoltTimer -= diff;

			if (WaterBubbleTimer <= diff)
			{
				DoCastVictim(SPELL_WATER_BUBBLE);
				WaterBubbleTimer = 5*IN_MILLISECONDS;
			}
			else WaterBubbleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_drakkari_water_binderAI(creature);
    }
};

enum CrazedWaterSpirit
{
	SPELL_WATER_WALL = 54400,
};

class npc_crazed_water_spirit : public CreatureScript
{
public:
    npc_crazed_water_spirit() : CreatureScript("npc_crazed_water_spirit") {}

    struct npc_crazed_water_spiritAI : public QuantumBasicAI
    {
        npc_crazed_water_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterWallTimer;

        void Reset()
        {
			WaterWallTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterWallTimer <= diff)
			{
				DoCastVictim(SPELL_WATER_WALL);
				WaterWallTimer = 4*IN_MILLISECONDS;
			}
			else WaterWallTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_crazed_water_spiritAI(creature);
    }
};

enum DisturbedSoul
{
	SPELL_DS_SHADOW_WORD_DEATH = 51818,
};

class npc_disturbed_soul : public CreatureScript
{
public:
    npc_disturbed_soul() : CreatureScript("npc_disturbed_soul") {}

    struct npc_disturbed_soulAI : public QuantumBasicAI
    {
        npc_disturbed_soulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowWordDeathTimer;

        void Reset()
        {
			ShadowWordDeathTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowWordDeathTimer <= diff)
			{
				DoCastVictim(SPELL_DS_SHADOW_WORD_DEATH);
				ShadowWordDeathTimer = 4*IN_MILLISECONDS;
			}
			else ShadowWordDeathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_disturbed_soulAI(creature);
    }
};

enum CarrionEater
{
	SPELL_CE_RAVENOUS_CLAW = 54387,
};

class npc_carrion_eater : public CreatureScript
{
public:
    npc_carrion_eater() : CreatureScript("npc_carrion_eater") {}

    struct npc_carrion_eaterAI : public QuantumBasicAI
    {
        npc_carrion_eaterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RavenousClawTimer;

        void Reset()
        {
			RavenousClawTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_EAT);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RavenousClawTimer <= diff)
			{
				DoCastVictim(SPELL_CE_RAVENOUS_CLAW);
				RavenousClawTimer = 4*IN_MILLISECONDS;
			}
			else RavenousClawTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_carrion_eaterAI(creature);
    }
};

enum RampagingGeist
{
	SPELL_DEVOUR_GUMANOID = 52207,
};

class npc_rampaging_geist : public CreatureScript
{
public:
    npc_rampaging_geist() : CreatureScript("npc_rampaging_geist") {}

    struct npc_rampaging_geistAI : public QuantumBasicAI
    {
        npc_rampaging_geistAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_DEVOUR_GUMANOID);
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
		return new npc_rampaging_geistAI(creature);
    }
};

enum RottingAbomination
{
	SPELL_RA_CLEAVE       = 15496,
	SPELL_RA_SCOURGE_HOOK = 50335,
};

class npc_rotting_abomination : public CreatureScript
{
public:
    npc_rotting_abomination() : CreatureScript("npc_rotting_abomination") {}

    struct npc_rotting_abominationAI : public QuantumBasicAI
    {
        npc_rotting_abominationAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_RA_SCOURGE_HOOK);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_RA_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_rotting_abominationAI(creature);
    }
};

enum MossyRampager
{
	SPELL_MR_RAMPAGE = 54475,
};

class npc_mossy_rampager : public CreatureScript
{
public:
    npc_mossy_rampager() : CreatureScript("npc_mossy_rampager") {}

    struct npc_mossy_rampagerAI : public QuantumBasicAI
    {
        npc_mossy_rampagerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_MR_RAMPAGE, 0))
					DoCast(me, SPELL_MR_RAMPAGE);
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_mossy_rampagerAI(creature);
    }
};

enum LurkingBasilisk
{
	SPELL_LB_VENOMOUS_BITE = 54470,
};

class npc_lurking_basilisk : public CreatureScript
{
public:
    npc_lurking_basilisk() : CreatureScript("npc_lurking_basilisk") {}

    struct npc_lurking_basiliskAI : public QuantumBasicAI
    {
        npc_lurking_basiliskAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VenomousBiteTimer;

        void Reset()
        {
			VenomousBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomousBiteTimer <= diff)
			{
				DoCastVictim(SPELL_LB_VENOMOUS_BITE);
				VenomousBiteTimer = 4*IN_MILLISECONDS;
			}
			else VenomousBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_lurking_basiliskAI(creature);
    }
};

enum SseratusNpcs
{
	SPELL_DS_SHOOT          = 6660,
	SPELL_PS_MIND_BLAST     = 13860,
	SPELL_CS_CLEAVE         = 15496,
	SPELL_PS_MIND_FLAY      = 16568,
	SPELL_BLOOD_OF_SSERATUS = 54482,
	SPELL_SUMMON_DK_SNAKE   = 54484,

	NPC_DRAKKARI_SNAKE      = 29444,
};

class npc_priest_of_sseratus : public CreatureScript
{
public:
    npc_priest_of_sseratus() : CreatureScript("npc_priest_of_sseratus") {}

    struct npc_priest_of_sseratusAI : public QuantumBasicAI
    {
        npc_priest_of_sseratusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MindBlastTimer;
		uint32 MindFlayTimer;
		uint32 BloodOfSseratusTimer;

        void Reset()
        {
			BloodOfSseratusTimer = 1*IN_MILLISECONDS;
			MindBlastTimer = 2*IN_MILLISECONDS;
			MindFlayTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
			// Not in Combat Buff
			if (BloodOfSseratusTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_BLOOD_OF_SSERATUS);
				BloodOfSseratusTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else BloodOfSseratusTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MindBlastTimer <= diff)
			{
				DoCastVictim(SPELL_PS_MIND_BLAST);
				MindBlastTimer = 4*IN_MILLISECONDS;
			}
			else MindBlastTimer -= diff;

			if (MindFlayTimer <= diff)
			{
				DoCastVictim(SPELL_PS_MIND_FLAY);
				MindFlayTimer = 6*IN_MILLISECONDS;
			}
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_priest_of_sseratusAI(creature);
    }
};

class npc_champion_of_sseratus : public CreatureScript
{
public:
    npc_champion_of_sseratus() : CreatureScript("npc_champion_of_sseratus") {}

    struct npc_champion_of_sseratusAI : public QuantumBasicAI
    {
        npc_champion_of_sseratusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 BloodOfSseratusTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			BloodOfSseratusTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

        void UpdateAI(uint32 const diff)
        {
			// Not in Combat Buff
			if (BloodOfSseratusTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_BLOOD_OF_SSERATUS);
				BloodOfSseratusTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else BloodOfSseratusTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CS_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_champion_of_sseratusAI(creature);
    }
};

class npc_drakkari_snake_handler : public CreatureScript
{
public:
    npc_drakkari_snake_handler() : CreatureScript("npc_drakkari_snake_handler") {}

    struct npc_drakkari_snake_handlerAI : public QuantumBasicAI
    {
		npc_drakkari_snake_handlerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShootTimer;
		uint32 SnakeTimer;
		uint32 BloodOfSseratusTimer;

		SummonList Summons;

        void Reset()
        {
			BloodOfSseratusTimer = 1*IN_MILLISECONDS;
			ShootTimer = 2*IN_MILLISECONDS;
			SnakeTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			Summons.DespawnAll();
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_DK_SNAKE);
		}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_DRAKKARI_SNAKE)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (BloodOfSseratusTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_BLOOD_OF_SSERATUS);
				BloodOfSseratusTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else BloodOfSseratusTimer -= diff;

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
						DoCast(target, SPELL_DS_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

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
						DoCast(target, SPELL_DS_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}

			if (SnakeTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_DK_SNAKE, true);
				DoCast(me, SPELL_SUMMON_DK_SNAKE, true);
				DoCast(me, SPELL_SUMMON_DK_SNAKE, true);
				SnakeTimer = 8*IN_MILLISECONDS;
			}
			else SnakeTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_snake_handlerAI(creature);
    }
};

enum DrakkariSnake
{
	SPELL_CRIPPLING_POISON    = 25809,
	SPELL_MIND_NUMBING_POISON = 25810,
};

class npc_drakkari_snake : public CreatureScript
{
public:
    npc_drakkari_snake() : CreatureScript("npc_drakkari_snake") {}

    struct npc_drakkari_snakeAI : public QuantumBasicAI
    {
        npc_drakkari_snakeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CripplingPoisonTimer;
		uint32 MindNumbingPoisonTimer;

        void Reset()
        {
			CripplingPoisonTimer = 1*IN_MILLISECONDS;
			MindNumbingPoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CripplingPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLING_POISON);
				CripplingPoisonTimer = 3*IN_MILLISECONDS;
			}
			else CripplingPoisonTimer -= diff;

			if (MindNumbingPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_MIND_NUMBING_POISON);
				MindNumbingPoisonTimer = 4*IN_MILLISECONDS;
			}
			else MindNumbingPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }

		Unit* GetRandomTarget()
        {
            return SelectTarget(TARGET_RANDOM);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_snakeAI(creature);
    }
};

enum AncientWatcher
{
	SPELL_ENTANGLING_ROOTS = 33844,
	SPELL_AW_TRAMPLE       = 51944,
};

class npc_ancient_watcher : public CreatureScript
{
public:
    npc_ancient_watcher() : CreatureScript("npc_ancient_watcher") {}

    struct npc_ancient_watcherAI : public QuantumBasicAI
    {
        npc_ancient_watcherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EntanglingRootsTimer;
		uint32 TrampleTimer;

        void Reset()
        {
			EntanglingRootsTimer = 1*IN_MILLISECONDS;
			TrampleTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EntanglingRootsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ENTANGLING_ROOTS);
					EntanglingRootsTimer = 4*IN_MILLISECONDS;
				}
			}
			else EntanglingRootsTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_AW_TRAMPLE);
				TrampleTimer = 6*IN_MILLISECONDS;
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancient_watcherAI(creature);
    }
};

enum GroveWalker
{
	SPELL_REJUVENATION = 15981,
	SPELL_THORNS       = 35361,
};

class npc_grove_walker : public CreatureScript
{
public:
    npc_grove_walker() : CreatureScript("npc_grove_walker") {}

    struct npc_grove_walkerAI : public QuantumBasicAI
    {
        npc_grove_walkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RejuvenationTimer;
		uint32 ThornsTimer;

        void Reset()
        {
			RejuvenationTimer = 1*IN_MILLISECONDS;
			ThornsTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THORNS);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RejuvenationTimer <= diff)
			{
				DoCast(me, SPELL_REJUVENATION);
				RejuvenationTimer = 4*IN_MILLISECONDS;
			}
			else RejuvenationTimer -= diff;

			if (ThornsTimer <= diff)
			{
				DoCast(me, SPELL_THORNS);
				ThornsTimer = 8*IN_MILLISECONDS;
			}
			else ThornsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grove_walkerAI(creature);
    }
};

enum UnboundSeer
{
	SPELL_ARCANE_BOLT = 38204,
	SPELL_LEY_CURSE   = 58667,
};

class npc_unbound_seer : public CreatureScript
{
public:
    npc_unbound_seer() : CreatureScript("npc_unbound_seer") {}

    struct npc_unbound_seerAI : public QuantumBasicAI
    {
        npc_unbound_seerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ArcaneBoltTimer;
		uint32 LeyCurseTimer;

        void Reset()
        {
			ArcaneBoltTimer = 0.5*IN_MILLISECONDS;
			LeyCurseTimer = 2.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_BOLT);
					ArcaneBoltTimer = 3*IN_MILLISECONDS;
				}
			}
			else ArcaneBoltTimer -= diff;

			if (LeyCurseTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LEY_CURSE);
					LeyCurseTimer = 5*IN_MILLISECONDS;
				}
			}
			else LeyCurseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_seerAI(creature);
    }
};

enum LostShandaralSpirit
{
	SPELL_MOONFIRE = 15798,
	SPELL_REGROWTH = 16561,
};

class npc_lost_shandaral_spirit : public CreatureScript
{
public:
    npc_lost_shandaral_spirit() : CreatureScript("npc_lost_shandaral_spirit") {}

    struct npc_lost_shandaral_spiritAI : public QuantumBasicAI
    {
        npc_lost_shandaral_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MoonfireTimer;

		bool LowHealth;

        void Reset()
        {
			MoonfireTimer = 1*IN_MILLISECONDS;

			LowHealth = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SITTING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MoonfireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MOONFIRE);
					MoonfireTimer = 3*IN_MILLISECONDS;
				}
			}
			else MoonfireTimer -= diff;

			if (!LowHealth && HealthBelowPct(HEALTH_PERCENT_50))
			{
				DoCast(me, SPELL_REGROWTH);
				LowHealth = true;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lost_shandaral_spiritAI(creature);
    }
};

enum AzureScalebane
{
	SPELL_AZ_STRIKE        = 11976,
	SPELL_AZ_MORTAL_STRIKE = 19643,
};

class npc_azure_scalebane : public CreatureScript
{
public:
    npc_azure_scalebane() : CreatureScript("npc_azure_scalebane") {}

    struct npc_azure_scalebaneAI : public QuantumBasicAI
    {
        npc_azure_scalebaneAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 MortalStrikeTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_AZ_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_AZ_MORTAL_STRIKE);
				MortalStrikeTimer = 6*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_scalebaneAI(creature);
    }
};

enum AzureSpellweaver
{
	SPELL_AS_ARCANE_MISSILES = 34446,
};

class npc_azure_spellweaver : public CreatureScript
{
public:
    npc_azure_spellweaver() : CreatureScript("npc_azure_spellweaver") {}

    struct npc_azure_spellweaverAI : public QuantumBasicAI
    {
        npc_azure_spellweaverAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ArcaneMissilesTimer;

        void Reset()
        {
			ArcaneMissilesTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneMissilesTimer <= diff)
			{
				DoCastVictim(SPELL_AS_ARCANE_MISSILES);
				ArcaneMissilesTimer = 3*IN_MILLISECONDS;
			}
			else ArcaneMissilesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_spellweaverAI(creature);
    }
};

enum DappledStag
{
	SPELL_BUTT = 59110,
};

class npc_dappled_stag : public CreatureScript
{
public:
    npc_dappled_stag() : CreatureScript("npc_dappled_stag") {}

    struct npc_dappled_stagAI : public QuantumBasicAI
    {
        npc_dappled_stagAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ButtTimer;

        void Reset()
        {
			ButtTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ButtTimer <= diff)
			{
				DoCastVictim(SPELL_BUTT);
				ButtTimer = 4*IN_MILLISECONDS;
			}
			else ButtTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dappled_stagAI(creature);
    }
};

enum SinewyWolf
{
	SPELL_SNARL      = 32919,
	SPELL_DASH       = 36589,
	SPELL_THROAT_RIP = 59008,
};

class npc_sinewy_wolf : public CreatureScript
{
public:
    npc_sinewy_wolf() : CreatureScript("npc_sinewy_wolf") {}

    struct npc_sinewy_wolfAI : public QuantumBasicAI
    {
        npc_sinewy_wolfAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SnarlTimer;
		uint32 DashTimer;

        void Reset()
        {
			SnarlTimer = 2*IN_MILLISECONDS;
			DashTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_THROAT_RIP, true);
			DoCast(me, SPELL_DASH, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SnarlTimer <= diff)
			{
				DoCastVictim(SPELL_SNARL);
				SnarlTimer = 4*IN_MILLISECONDS;
			}
			else SnarlTimer -= diff;

			if (DashTimer <= diff)
			{
				DoCast(me, SPELL_DASH);
				DashTimer = 10*IN_MILLISECONDS;
			}
			else DashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sinewy_wolfAI(creature);
    }
};

enum AzureManabeast
{
	SPELL_MANA_BITE = 59105,
};

class npc_azure_manabeast : public CreatureScript
{
public:
    npc_azure_manabeast() : CreatureScript("npc_azure_manabeast") {}

    struct npc_azure_manabeastAI : public QuantumBasicAI
    {
        npc_azure_manabeastAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ManaBiteTimer;

        void Reset()
        {
			ManaBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ManaBiteTimer <= diff)
			{
				DoCastVictim(SPELL_MANA_BITE);
				ManaBiteTimer = 4*IN_MILLISECONDS;
			}
			else ManaBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_manabeastAI(creature);
    }
};

enum UnboundEnt
{
	SPELL_CORRUPTED_REJUVENATION = 58624,
	SPELL_UE_THORNS              = 35361,
};

class npc_unbound_ent : public CreatureScript
{
public:
    npc_unbound_ent() : CreatureScript("npc_unbound_ent") {}

    struct npc_unbound_entAI : public QuantumBasicAI
    {
        npc_unbound_entAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CorruptedRejuvenationTimer;
		uint32 ThornsTimer;

        void Reset()
        {
			CorruptedRejuvenationTimer = 1*IN_MILLISECONDS;
			ThornsTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_UE_THORNS);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorruptedRejuvenationTimer <= diff)
			{
				DoCastVictim(SPELL_CORRUPTED_REJUVENATION);
				CorruptedRejuvenationTimer = 4*IN_MILLISECONDS;
			}
			else CorruptedRejuvenationTimer -= diff;

			if (ThornsTimer <= diff)
			{
				DoCast(me, SPELL_UE_THORNS);
				ThornsTimer = 8*IN_MILLISECONDS;
			}
			else ThornsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_entAI(creature);
    }
};

enum UnboundDryad
{
	SPELL_UD_STRIKE      = 11976,
	SPELL_UD_THROW_SPEAR = 55217,
};

class npc_unbound_dryad : public CreatureScript
{
public:
    npc_unbound_dryad() : CreatureScript("npc_unbound_dryad") {}

    struct npc_unbound_dryadAI : public QuantumBasicAI
    {
        npc_unbound_dryadAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowSpearTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			ThrowSpearTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_UD_THROW_SPEAR);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowSpearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UD_THROW_SPEAR);
					ThrowSpearTimer = 4*IN_MILLISECONDS;
				}
			}
			else ThrowSpearTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_UD_STRIKE);
				StrikeTimer = 6*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_dryadAI(creature);
    }
};

enum UnboundAncient
{
	SPELL_UA_ENTANGLING_ROOTS = 33844,
	SPELL_UA_TRAMPLE          = 51944,
};

class npc_unbound_ancient : public CreatureScript
{
public:
    npc_unbound_ancient() : CreatureScript("npc_unbound_ancient") {}

    struct npc_unbound_ancientAI : public QuantumBasicAI
    {
        npc_unbound_ancientAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EntanglingRootsTimer;
		uint32 TrampleTimer;

        void Reset()
        {
			EntanglingRootsTimer = 1*IN_MILLISECONDS;
			TrampleTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EntanglingRootsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UA_ENTANGLING_ROOTS);
					EntanglingRootsTimer = 4*IN_MILLISECONDS;
				}
			}
			else EntanglingRootsTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_UA_TRAMPLE);
				TrampleTimer = 6*IN_MILLISECONDS;
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_ancientAI(creature);
    }
};

enum UnboundTrickster
{
	SPELL_UT_STRIKE     = 11976,
	SPELL_UT_FIRE_BLAST = 13341,
	SPELL_UT_IGNITE     = 58438,
};

class npc_unbound_trickster : public CreatureScript
{
public:
    npc_unbound_trickster() : CreatureScript("npc_unbound_trickster") {}

    struct npc_unbound_tricksterAI : public QuantumBasicAI
    {
        npc_unbound_tricksterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 FireBlastTimer;
		uint32 IgniteTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;
			FireBlastTimer = 3*IN_MILLISECONDS;
			IgniteTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_UT_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (FireBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UT_FIRE_BLAST);
					FireBlastTimer = 6*IN_MILLISECONDS;
				}
			}
			else FireBlastTimer -= diff;

			if (IgniteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UT_IGNITE);
					IgniteTimer = 8*IN_MILLISECONDS;
				}
			}
			else IgniteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_tricksterAI(creature);
    }
};

enum DispiritedEnt
{
	SPELL_DE_REJUVENATION = 15981,
	SPELL_DE_THORNS       = 35361,
};

class npc_dispirited_ent : public CreatureScript
{
public:
    npc_dispirited_ent() : CreatureScript("npc_dispirited_ent") {}

    struct npc_dispirited_entAI : public QuantumBasicAI
    {
        npc_dispirited_entAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RejuvenationTimer;
		uint32 ThornsTimer;

        void Reset()
        {
			RejuvenationTimer = 1*IN_MILLISECONDS;
			ThornsTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DE_THORNS);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RejuvenationTimer <= diff)
			{
				DoCast(me, SPELL_DE_REJUVENATION);
				RejuvenationTimer = 4*IN_MILLISECONDS;
			}
			else RejuvenationTimer -= diff;

			if (ThornsTimer <= diff)
			{
				DoCast(me, SPELL_DE_THORNS);
				ThornsTimer = 8*IN_MILLISECONDS;
			}
			else ThornsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dispirited_entAI(creature);
    }
};

enum ScoutOrdimbral
{
	SPELL_SO_SHOOT = 6660,
};

class npc_scout_ordimbral : public CreatureScript
{
public:
    npc_scout_ordimbral() : CreatureScript("npc_scout_ordimbral") {}

    struct npc_scout_ordimbralAI : public QuantumBasicAI
    {
		npc_scout_ordimbralAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

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
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SO_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

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
						DoCast(target, SPELL_SO_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scout_ordimbralAI(creature);
    }
};

enum AlanuraFirecloud
{
	SPELL_AF_FIREBALL     = 20823,
	SPELL_FIERY_INTELLECT = 35917,
};

class npc_alanura_firecloud : public CreatureScript
{
public:
    npc_alanura_firecloud() : CreatureScript("npc_alanura_firecloud") {}

    struct npc_alanura_firecloudAI : public QuantumBasicAI
    {
        npc_alanura_firecloudAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FieryIntellectTimer;
		uint32 FireballTimer;

        void Reset()
        {
			FieryIntellectTimer = 1*IN_MILLISECONDS;
			FireballTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FieryIntellectTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_FIERY_INTELLECT);
				FieryIntellectTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FieryIntellectTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AF_FIREBALL);
					FireballTimer = 3.5*IN_MILLISECONDS;
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alanura_firecloudAI(creature);
    }
};

enum ScoutCaptainElsia
{
	SPELL_SCE_SHOOT      = 22907,
	SPELL_SCE_HOOKED_NET = 36827,
};

class npc_scout_captain_elsia : public CreatureScript
{
public:
    npc_scout_captain_elsia() : CreatureScript("npc_scout_captain_elsia") {}

    struct npc_scout_captain_elsiaAI : public QuantumBasicAI
    {
		npc_scout_captain_elsiaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 HookedNetTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			HookedNetTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
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
						DoCast(target, SPELL_SCE_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (HookedNetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SCE_HOOKED_NET);
						HookedNetTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else HookedNetTimer -= diff;

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
						DoCast(target, SPELL_SCE_SHOOT);
						ShootTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scout_captain_elsiaAI(creature);
    }
};

enum Marisalira
{
	SPELL_M_INTELLECT = 35917,
};

class npc_marisalira : public CreatureScript
{
public:
    npc_marisalira() : CreatureScript("npc_marisalira") {}

    struct npc_marisaliraAI : public QuantumBasicAI
    {
        npc_marisaliraAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FieryIntellectTimer;

        void Reset()
        {
			FieryIntellectTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FieryIntellectTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_M_INTELLECT);
				FieryIntellectTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FieryIntellectTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marisaliraAI(creature);
    }
};

enum SkymasterBaeric
{
	SPELL_SB_INTELLECT = 35917,
};

class npc_skymaster_baeric : public CreatureScript
{
public:
    npc_skymaster_baeric() : CreatureScript("npc_skymaster_baeric") {}

    struct npc_skymaster_baericAI : public QuantumBasicAI
    {
        npc_skymaster_baericAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FieryIntellectTimer;

        void Reset()
        {
			FieryIntellectTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FieryIntellectTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_SB_INTELLECT);
				FieryIntellectTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FieryIntellectTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skymaster_baericAI(creature);
    }
};

enum ShandaralWarriorSpirit
{
	SPELL_SHANDARAL_VISUAL = 58429,
	SPELL_SW_HEROIC_STRIKE = 57846,
	SPELL_SW_SHIELD_BASH   = 38233,
	SPELL_SD_MOONFIRE      = 15798,
	SPELL_SD_REGROWTH      = 16561,
	SPELL_SH_SHOOT         = 6660,
	SPELL_SH_WING_CLIP     = 47168,
	SPELL_SH_AIMED_SHOT    = 54615,
};

class npc_shandaral_warrior_spirit : public CreatureScript
{
public:
    npc_shandaral_warrior_spirit() : CreatureScript("npc_shandaral_warrior_spirit") {}

    struct npc_shandaral_warrior_spiritAI : public QuantumBasicAI
    {
        npc_shandaral_warrior_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HeroicStrikeTimer;
		uint32 ShieldBashTimer;

        void Reset()
        {
			HeroicStrikeTimer = 2*IN_MILLISECONDS;
			ShieldBashTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_SHANDARAL_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SW_HEROIC_STRIKE);
				HeroicStrikeTimer = 4*IN_MILLISECONDS;
			}
			else HeroicStrikeTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SW_SHIELD_BASH);
				ShieldBashTimer = 6*IN_MILLISECONDS;
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shandaral_warrior_spiritAI(creature);
    }
};

class npc_shandaral_druid_spirit : public CreatureScript
{
public:
    npc_shandaral_druid_spirit() : CreatureScript("npc_shandaral_druid_spirit") {}

    struct npc_shandaral_druid_spiritAI : public QuantumBasicAI
    {
        npc_shandaral_druid_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MoonfireTimer;

		bool LowHealth;

        void Reset()
        {
			MoonfireTimer = 0.5*IN_MILLISECONDS;

			LowHealth = false;

			DoCast(me, SPELL_SHANDARAL_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MoonfireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SD_MOONFIRE);
					MoonfireTimer = 3*IN_MILLISECONDS;
				}
			}
			else MoonfireTimer -= diff;

			if (!LowHealth && HealthBelowPct(HEALTH_PERCENT_50))
			{
				DoCast(me, SPELL_SD_REGROWTH);
				LowHealth = true;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shandaral_druid_spiritAI(creature);
    }
};

class npc_shandaral_hunter_spirit : public CreatureScript
{
public:
    npc_shandaral_hunter_spirit() : CreatureScript("npc_shandaral_hunter_spirit") {}

    struct npc_shandaral_hunter_spiritAI : public QuantumBasicAI
    {
		npc_shandaral_hunter_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 AimedShotTimer;
		uint32 WingClipTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			AimedShotTimer = 2*IN_MILLISECONDS;
			WingClipTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_SHANDARAL_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
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
						DoCast(target, SPELL_SH_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (AimedShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SH_AIMED_SHOT, true);
						AimedShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else AimedShotTimer -= diff;

			if (WingClipTimer <= diff)
			{
				DoCastVictim(SPELL_SH_WING_CLIP);
				WingClipTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else WingClipTimer -= diff;

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
						DoCast(target, SPELL_SH_SHOOT);
						ShootTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shandaral_hunter_spiritAI(creature);
    }
};

enum SavageHillBrute
{
	SPELL_BRUTE_FORCE = 54678,
};

class npc_savage_hill_brute : public CreatureScript
{
public:
    npc_savage_hill_brute() : CreatureScript("npc_savage_hill_brute") {}

    struct npc_savage_hill_bruteAI : public QuantumBasicAI
    {
        npc_savage_hill_bruteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BruteForceTimer;

        void Reset()
        {
			BruteForceTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SLEEP);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BruteForceTimer <= diff)
			{
				DoCast(me, SPELL_BRUTE_FORCE);
				BruteForceTimer = 4*IN_MILLISECONDS;
			}
			else BruteForceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_savage_hill_bruteAI(creature);
    }
};

enum SavageHillScavenger
{
	SPELL_BONE_TOSS = 50403,
};

class npc_savage_hill_scavenger : public CreatureScript
{
public:
    npc_savage_hill_scavenger() : CreatureScript("npc_savage_hill_scavenger") {}

    struct npc_savage_hill_scavengerAI : public QuantumBasicAI
    {
        npc_savage_hill_scavengerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoneTossTimer;

        void Reset()
        {
			BoneTossTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoneTossTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BONE_TOSS);
					BoneTossTimer = 3*IN_MILLISECONDS;
				}
			}
			else BoneTossTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_savage_hill_scavengerAI(creature);
    }
};

enum SavageHillMystic
{
	SPELL_ARCANE_BARRAGE = 50273,
};

class npc_savage_hill_mystic : public CreatureScript
{
public:
    npc_savage_hill_mystic() : CreatureScript("npc_savage_hill_mystic") {}

    struct npc_savage_hill_mysticAI : public QuantumBasicAI
    {
        npc_savage_hill_mysticAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ArcaneBarrageTimer;

        void Reset()
        {
			ArcaneBarrageTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBarrageTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_BARRAGE);
					ArcaneBarrageTimer = 4*IN_MILLISECONDS;
				}
			}
			else ArcaneBarrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_savage_hill_mysticAI(creature);
    }
};

enum TheCrusadersPinnacle
{
	SPELL_LD_AVENGERS_SHIELD = 37554,
	SPELL_CONSECRATION       = 48819,
	SPELL_EXORCISM           = 48801,
	SPELL_LD_HOLY_LIGHT      = 58053,
	SPELL_BLESSING_OF_KINGS  = 58054,
	SPELL_THREAT_PULSE       = 58110,

	LORD_DALFORS_MOUNT_ID    = 2786,
};

class npc_crusader_lord_dalfors : public CreatureScript
{
public:
    npc_crusader_lord_dalfors() : CreatureScript("npc_crusader_lord_dalfors") {}

    struct npc_crusader_lord_dalforsAI : public QuantumBasicAI
    {
        npc_crusader_lord_dalforsAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ExorcismTimer;
		uint32 ConsecrationTimer;
		uint32 HolyLightTimer;

        void Reset()
        {
			ExorcismTimer = 2*IN_MILLISECONDS;
			ConsecrationTimer = 4*IN_MILLISECONDS;
			HolyLightTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_THREAT_PULSE);

			me->Mount(LORD_DALFORS_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			DoCastVictim(SPELL_LD_AVENGERS_SHIELD);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ExorcismTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_EXORCISM);
					ExorcismTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ExorcismTimer -= diff;

			if (ConsecrationTimer <= diff)
			{
				DoCastAOE(SPELL_CONSECRATION);
				ConsecrationTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ConsecrationTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					DoCast(target, SPELL_LD_HOLY_LIGHT);
					HolyLightTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else HolyLightTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_70))
				DoCast(me, SPELL_LD_HOLY_LIGHT, true);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crusader_lord_dalforsAI(creature);
    }
};

class npc_argent_battle_priest : public CreatureScript
{
public:
    npc_argent_battle_priest() : CreatureScript("npc_argent_battle_priest") {}

    struct npc_argent_battle_priestAI : public QuantumBasicAI
    {
        npc_argent_battle_priestAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HolyLightTimer;

        void Reset()
        {
			HolyLightTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_THREAT_PULSE);
			DoCast(me, SPELL_BLESSING_OF_KINGS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HolyLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					DoCast(target, SPELL_LD_HOLY_LIGHT);
					HolyLightTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else HolyLightTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_70))
				DoCast(me, SPELL_LD_HOLY_LIGHT, true);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_battle_priestAI(creature);
    }
};

enum LonghoofGrazer
{
	SPELL_HEAD_BUTT = 42320,
};

class npc_longhoof_grazer : public CreatureScript
{
public:
    npc_longhoof_grazer() : CreatureScript("npc_longhoof_grazer") {}

    struct npc_longhoof_grazerAI : public QuantumBasicAI
    {
        npc_longhoof_grazerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HeadButtTimer;

        void Reset()
        {
			HeadButtTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeadButtTimer <= diff)
			{
				DoCastVictim(SPELL_HEAD_BUTT);
				HeadButtTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else HeadButtTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_longhoof_grazerAI(creature);
    }
};

enum DuskhowlProwler
{
	SPELL_HOWLING_FJORD_PERIODIC = 44366,
	SPELL_DP_GORE                = 32019,
};

class npc_duskhowl_prowler : public CreatureScript
{
public:
    npc_duskhowl_prowler() : CreatureScript("npc_duskhowl_prowler") {}

    struct npc_duskhowl_prowlerAI : public QuantumBasicAI
    {
        npc_duskhowl_prowlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_HOWLING_FJORD_PERIODIC);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GoreTimer <= diff)
			{
				DoCastVictim(SPELL_DP_GORE);
				GoreTimer = 5*IN_MILLISECONDS;
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_duskhowl_prowlerAI(creature);
    }
};

enum Shoveltusk
{
	SPELL_S_HEAD_BUTT = 42320,
};

class npc_shoveltusk : public CreatureScript
{
public:
    npc_shoveltusk() : CreatureScript("npc_shoveltusk") {}

    struct npc_shoveltuskAI : public QuantumBasicAI
    {
        npc_shoveltuskAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HeadButtTimer;

        void Reset()
        {
			HeadButtTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeadButtTimer <= diff)
			{
				DoCastVictim(SPELL_S_HEAD_BUTT);
				HeadButtTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else HeadButtTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shoveltuskAI(creature);
    }
};

class npc_shoveltusk_calf : public CreatureScript
{
public:
    npc_shoveltusk_calf() : CreatureScript("npc_shoveltusk_calf") {}

    struct npc_shoveltusk_calfAI : public QuantumBasicAI
    {
        npc_shoveltusk_calfAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HeadButtTimer;

        void Reset()
        {
			HeadButtTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeadButtTimer <= diff)
			{
				DoCastVictim(SPELL_S_HEAD_BUTT);
				HeadButtTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else HeadButtTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shoveltusk_calfAI(creature);
    }
};

enum GlacialSpirit
{
	SPELL_GS_FROSTBOLT = 40430,
};

class npc_glacial_spirit : public CreatureScript
{
public:
    npc_glacial_spirit() : CreatureScript("npc_glacial_spirit") {}

    struct npc_glacial_spiritAI : public QuantumBasicAI
    {
        npc_glacial_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_GS_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_glacial_spiritAI(creature);
    }
};

enum WaterRevenant
{
	SPELL_WR_WATER_BOLT = 37252,
};

class npc_water_revenant : public CreatureScript
{
public:
    npc_water_revenant() : CreatureScript("npc_water_revenant") {}

    struct npc_water_revenantAI : public QuantumBasicAI
    {
        npc_water_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterBoltTimer;

        void Reset()
        {
			WaterBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WR_WATER_BOLT);
					WaterBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else WaterBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_water_revenantAI(creature);
    }
};

enum WanderingShadow
{
	SPELL_CURSE_OF_WEAKNESS = 18267,
	SPELL_CHILLING_TOUCH    = 38240,
};

class npc_wandering_shadow : public CreatureScript
{
public:
    npc_wandering_shadow() : CreatureScript("npc_wandering_shadow") {}

    struct npc_wandering_shadowAI : public QuantumBasicAI
    {
        npc_wandering_shadowAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfWeaknessTimer;
		uint32 ChillingTouchTimer;

        void Reset()
        {
			CurseOfWeaknessTimer = 0.5*IN_MILLISECONDS;
			ChillingTouchTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfWeaknessTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_WEAKNESS);
					CurseOfWeaknessTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfWeaknessTimer -= diff;

			if (ChillingTouchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CHILLING_TOUCH);
					ChillingTouchTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ChillingTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wandering_shadowAI(creature);
    }
};

enum ShadowRevenant
{
	SPELL_VERTEX_DARK_RED = 44773,
	SPELL_STRANGULATE     = 51131,
};

class npc_shadow_revenant : public CreatureScript
{
public:
    npc_shadow_revenant() : CreatureScript("npc_shadow_revenant") {}

    struct npc_shadow_revenantAI : public QuantumBasicAI
    {
        npc_shadow_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrangulateTimer;

        void Reset()
        {
			StrangulateTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_VERTEX_DARK_RED);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrangulateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_STRANGULATE);
					StrangulateTimer = 5*IN_MILLISECONDS;
				}
			}
			else StrangulateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadow_revenantAI(creature);
    }
};

enum WhisperingWind
{
	SPELL_WIND_BLAST = 51877,
};

class npc_whispering_wind : public CreatureScript
{
public:
    npc_whispering_wind() : CreatureScript("npc_whispering_wind") {}

    struct npc_whispering_windAI : public QuantumBasicAI
    {
        npc_whispering_windAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WindBlastTimer;

        void Reset()
        {
			WindBlastTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WindBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WIND_BLAST, true);
					WindBlastTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else WindBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_whispering_windAI(creature);
    }
};

enum FlameRevenant
{
	SPELL_FR_FIRE_BLAST = 30516,
};

class npc_flame_revenant : public CreatureScript
{
public:
    npc_flame_revenant() : CreatureScript("npc_flame_revenant") {}

    struct npc_flame_revenantAI : public QuantumBasicAI
    {
        npc_flame_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireBlastTimer;

        void Reset()
        {
			FireBlastTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FR_FIRE_BLAST);
					FireBlastTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FireBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flame_revenantAI(creature);
    }
};

enum RagingFlame
{
	SPELL_RAGING_FLAMES = 35278,
};

class npc_raging_flame : public CreatureScript
{
public:
    npc_raging_flame() : CreatureScript("npc_raging_flame") {}

    struct npc_raging_flameAI : public QuantumBasicAI
    {
        npc_raging_flameAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RagingFlamesTimer;

        void Reset()
        {
			RagingFlamesTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RagingFlamesTimer <= diff)
			{
				DoCastAOE(SPELL_RAGING_FLAMES);
				RagingFlamesTimer = 6*IN_MILLISECONDS;
			}
			else RagingFlamesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raging_flameAI(creature);
    }
};

enum TempestRevenant
{
	SPELL_SEETHING_FLAMES = 56620,
};

class npc_tempest_revenant : public CreatureScript
{
public:
    npc_tempest_revenant() : CreatureScript("npc_tempest_revenant") {}

    struct npc_tempest_revenantAI : public QuantumBasicAI
    {
        npc_tempest_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SeethingFlamesTimer;

        void Reset()
        {
			SeethingFlamesTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SeethingFlamesTimer <= diff)
			{
				DoCastAOE(SPELL_SEETHING_FLAMES);
				SeethingFlamesTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SeethingFlamesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_revenantAI(creature);
    }
};

enum LivingLasher
{
	SPELL_LL_DREAM_LASH = 51901,
};

class npc_living_lasher : public CreatureScript
{
public:
    npc_living_lasher() : CreatureScript("npc_living_lasher") {}

    struct npc_living_lasherAI : public QuantumBasicAI
    {
        npc_living_lasherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DreamLashTimer;

        void Reset()
        {
			DreamLashTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DreamLashTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LL_DREAM_LASH);
					DreamLashTimer = 2*IN_MILLISECONDS;
				}
			}
			else DreamLashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_living_lasherAI(creature);
    }
};

class npc_mature_lasher : public CreatureScript
{
public:
    npc_mature_lasher() : CreatureScript("npc_mature_lasher") {}

    struct npc_mature_lasherAI : public QuantumBasicAI
    {
        npc_mature_lasherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DreamLashTimer;

        void Reset()
        {
			DreamLashTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DreamLashTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LL_DREAM_LASH);
					DreamLashTimer = 2*IN_MILLISECONDS;
				}
			}
			else DreamLashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mature_lasherAI(creature);
    }
};

enum PustulentHorror
{
	SPELL_PH_KNOCK_BACK = 28405,
	SPELL_PH_STOMP      = 63546,
};

class npc_pustulent_horror : public CreatureScript
{
public:
    npc_pustulent_horror() : CreatureScript("npc_pustulent_horror") {}

    struct npc_pustulent_horrorAI : public QuantumBasicAI
    {
        npc_pustulent_horrorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockbackTimer;
		uint32 StompTimer;

        void Reset()
        {
			KnockbackTimer = 2*IN_MILLISECONDS;
			StompTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* attacker)
		{
			me->AI()->AttackStart(attacker);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KnockbackTimer <= diff)
			{
				DoCastVictim(SPELL_PH_KNOCK_BACK);
				KnockbackTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else KnockbackTimer -= diff;

			if (StompTimer <= diff)
			{
				DoCastAOE(SPELL_PH_STOMP);
				StompTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else StompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pustulent_horrorAI(creature);
    }
};

enum CitadelWatcher
{
	SPELL_CW_GARGOYLE_STRIKE = 16564,
};

class npc_citadel_watcher : public CreatureScript
{
public:
    npc_citadel_watcher() : CreatureScript("npc_citadel_watcher") {}

    struct npc_citadel_watcherAI : public QuantumBasicAI
    {
        npc_citadel_watcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SUBMERGED_NEW);
		}

		uint32 GargoyleStrikeTimer;

        void Reset()
        {
			GargoyleStrikeTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GargoyleStrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CW_GARGOYLE_STRIKE);
					GargoyleStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else GargoyleStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_citadel_watcherAI(creature);
    }
};

enum CorpretharGuardian
{
	SPELL_CG_INFECTED_BITE = 60927,
	SPELL_CG_MORTAL_WOUND  = 54378
};

class npc_corprethar_guardian : public CreatureScript
{
public:
    npc_corprethar_guardian() : CreatureScript("npc_corprethar_guardian") {}

    struct npc_corprethar_guardianAI : public QuantumBasicAI
    {
        npc_corprethar_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedBiteTimer;
		uint32 MortalWoundTimer;

        void Reset()
        {
			InfectedBiteTimer = 2*IN_MILLISECONDS;
			MortalWoundTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedBiteTimer <= diff)
			{
				DoCastVictim(SPELL_CG_INFECTED_BITE);
				InfectedBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else InfectedBiteTimer -= diff;

			if (MortalWoundTimer <= diff)
			{
				DoCastVictim(SPELL_CG_MORTAL_WOUND);
				MortalWoundTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalWoundTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_corprethar_guardianAI(creature);
    }
};

enum ConvertedHero
{
	SPELL_CH_MORTAL_STRIKE     = 19643,
	SPELL_INCAPACITATING_SHOUT = 61578,
	SPELL_GRIP_THE_SCOURGE     = 60231,
};

class npc_converted_hero : public CreatureScript
{
public:
    npc_converted_hero() : CreatureScript("npc_converted_hero") {}

    struct npc_converted_heroAI : public QuantumBasicAI
    {
        npc_converted_heroAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer ;
		uint32 IncapacitatingShoutTimer;

        void Reset()
        {
			MortalStrikeTimer  = 2*IN_MILLISECONDS;
			IncapacitatingShoutTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_GRIP_THE_SCOURGE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_CH_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (IncapacitatingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_INCAPACITATING_SHOUT);
				IncapacitatingShoutTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else IncapacitatingShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_converted_heroAI(creature);
    }
};

enum WintergraspCommanders
{
	SPELL_CD_CLEAVE  = 15284,

	MOUNT_ID_ZANNETH = 14337,
};

class npc_commander_dardosh : public CreatureScript
{
public:
    npc_commander_dardosh() : CreatureScript("npc_commander_dardosh") {}

    struct npc_commander_dardoshAI : public QuantumBasicAI
    {
        npc_commander_dardoshAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer ;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CD_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_commander_dardoshAI(creature);
    }
};

class npc_commander_zanneth : public CreatureScript
{
public:
    npc_commander_zanneth() : CreatureScript("npc_commander_zanneth") {}

    struct npc_commander_zannethAI : public QuantumBasicAI
    {
        npc_commander_zannethAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer ;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->Mount(MOUNT_ID_ZANNETH);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
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
				DoCastVictim(SPELL_CD_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_commander_zannethAI(creature);
    }
};

enum TacticalOfficerAhbramis
{
	SPELL_TA_MORTAL_STRIKE = 15708,

	MOUNT_ID_AHBRAMIS      = 6569,
};

class npc_tactical_officer_ahbramis : public CreatureScript
{
public:
    npc_tactical_officer_ahbramis() : CreatureScript("npc_tactical_officer_ahbramis") {}

    struct npc_tactical_officer_ahbramisAI : public QuantumBasicAI
    {
        npc_tactical_officer_ahbramisAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;

			me->Mount(MOUNT_ID_AHBRAMIS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TA_MORTAL_STRIKE);
				MortalStrikeTimer = 4*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tactical_officer_ahbramisAI(creature);
    }
};

class npc_primalist_mulfort : public CreatureScript
{
public:
    npc_primalist_mulfort() : CreatureScript("npc_primalist_mulfort") {}

    struct npc_primalist_mulfortAI : public QuantumBasicAI
    {
        npc_primalist_mulfortAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer ;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CD_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_primalist_mulfortAI(creature);
    }
};

enum VengefulTaunkaSpirit
{
	SPELL_SOUL_SEPERATION = 46695,
};

class npc_vengeful_taunka_spirit : public CreatureScript
{
public:
    npc_vengeful_taunka_spirit() : CreatureScript("npc_vengeful_taunka_spirit") {}

    struct npc_vengeful_taunka_spiritAI : public QuantumBasicAI
    {
        npc_vengeful_taunka_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SOUL_SEPERATION);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
        return new npc_vengeful_taunka_spiritAI(creature);
    }
};

enum ArgentCrusadeRifleman
{
	SPELL_ACR_SHOOT = 15547,
};

class npc_argent_crusade_rifleman : public CreatureScript
{
public:
    npc_argent_crusade_rifleman() : CreatureScript("npc_argent_crusade_rifleman") {}

    struct npc_argent_crusade_riflemanAI : public QuantumBasicAI
    {
		npc_argent_crusade_riflemanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_HOLD_RIFLE);
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
						DoCast(target, SPELL_ACR_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

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
						DoCast(target, SPELL_ACR_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_crusade_riflemanAI(creature);
    }
};

enum SifreldarStormMaiden
{
	SPELL_STORM_CLOUD = 57408,
};

class npc_sifreldar_storm_maiden : public CreatureScript
{
public:
    npc_sifreldar_storm_maiden() : CreatureScript("npc_sifreldar_storm_maiden") {}

    struct npc_sifreldar_storm_maidenAI : public QuantumBasicAI
    {
        npc_sifreldar_storm_maidenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StormCloudTimer;

        void Reset()
        {
			StormCloudTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StormCloudTimer <= diff)
			{
				DoCast(me, SPELL_STORM_CLOUD, true);
				StormCloudTimer = 6*IN_MILLISECONDS;
			}
			else StormCloudTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sifreldar_storm_maidenAI(creature);
    }
};

enum SifreldarRunekeeper
{
	SPELL_SF_FROSTBOLT      = 20792,
	SPELL_REVITALIZING_RUNE = 52714,
};

class npc_sifreldar_runekeeper : public CreatureScript
{
public:
    npc_sifreldar_runekeeper() : CreatureScript("npc_sifreldar_runekeeper") {}

    struct npc_sifreldar_runekeeperAI : public QuantumBasicAI
    {
        npc_sifreldar_runekeeperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 RevitalizingRuneTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			RevitalizingRuneTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
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
					DoCast(target, SPELL_SF_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (RevitalizingRuneTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					if (!target->HasAura(SPELL_REVITALIZING_RUNE))
					{
						DoCast(target, SPELL_REVITALIZING_RUNE, true);
						RevitalizingRuneTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
			}
			else RevitalizingRuneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sifreldar_runekeeperAI(creature);
    }
};

enum IcemaneYeti
{
	SPELL_CRASHING_SLAM = 56402,
};

class npc_icemane_yeti : public CreatureScript
{
public:
    npc_icemane_yeti() : CreatureScript("npc_icemane_yeti") {}

    struct npc_icemane_yetiAI : public QuantumBasicAI
    {
        npc_icemane_yetiAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrashingSlamTimer;

        void Reset()
        {
			CrashingSlamTimer = 1.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrashingSlamTimer <= diff)
			{
				DoCastAOE(SPELL_CRASHING_SLAM);
				CrashingSlamTimer = 4*IN_MILLISECONDS;
			}
			else CrashingSlamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icemane_yetiAI(creature);
    }
};

enum Protodrakes
{
	SPELL_SUMMON_PROTO_DRAKE_MOUNT    = 54513,
	SPELL_FLIGHT                      = 57403,
};

class npc_frigid_proto_drake : public CreatureScript
{
public:
    npc_frigid_proto_drake() : CreatureScript("npc_frigid_proto_drake") { }

    struct npc_frigid_proto_drakeAI : public VehicleAI
    {
        npc_frigid_proto_drakeAI(Creature* creature) : VehicleAI(creature)
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void Reset(){}

        void PassengerBoarded(Unit* passenger, int8* /*seatId*/, bool apply)
        {
            if (passenger && apply)
			{
				if (Player* player = passenger->ToPlayer())
				{
					player->ExitVehicle();
					player->CastSpell(player, SPELL_SUMMON_PROTO_DRAKE_MOUNT, true);
				}
			}
        }
		
		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frigid_proto_drakeAI(creature);
    }
};

class npc_njorndar_proto_drake_vehicle : public CreatureScript
{
public:
    npc_njorndar_proto_drake_vehicle() : CreatureScript("npc_njorndar_proto_drake_vehicle") { }

    struct npc_njorndar_proto_drake_vehicleAI : public VehicleAI
    {
        npc_njorndar_proto_drake_vehicleAI(Creature* creature) : VehicleAI(creature)
		{
			DoCast(me, SPELL_FLIGHT, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void Reset(){}

		void PassengerBoarded(Unit* /*passenger*/, int8* /*seatId*/, bool apply){}
		
		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_njorndar_proto_drake_vehicleAI(creature);
    }
};

enum HyldsmeetDrakerider
{
	SPELL_HD_MORTAL_STRIKE   = 32736,
};

class npc_hyldsmeet_drakerider : public CreatureScript
{
public:
    npc_hyldsmeet_drakerider() : CreatureScript("npc_hyldsmeet_drakerider") {}

    struct npc_hyldsmeet_drakeriderAI : public QuantumBasicAI
    {
        npc_hyldsmeet_drakeriderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;

        void Reset()
        {
			MortalStrikeTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HD_MORTAL_STRIKE);
				MortalStrikeTimer = 4*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hyldsmeet_drakeriderAI(creature);
    }
};

enum BunnyData
{
	SPELL_PROTO_DRAKE_CHAIN_CHANNEL = 55244,
	SPELL_KIRGARAAK_BEAM            = 56379,

	SPELL_HELM_SPARKLE              = 56494,
	SPELL_HELM_SPARKLE_LARGE        = 56507,
	SPELL_HELM_GIGANTIC_SPARKLE     = 61333,
	SPELL_HELM_EXTRA_SPARKLE        = 61334,

	NPC_CAPTIVE_PROTO_DRAKE         = 29708,
};

class npc_captive_proto_drake_beam_bunny : public CreatureScript
{
public:
    npc_captive_proto_drake_beam_bunny() : CreatureScript("npc_captive_proto_drake_beam_bunny") {}

    struct npc_captive_proto_drake_beam_bunnyAI : public QuantumBasicAI
    {
        npc_captive_proto_drake_beam_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DrakeChainsTimer;

        void Reset()
        {
			DrakeChainsTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (DrakeChainsTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* drake = me->FindCreatureWithDistance(NPC_CAPTIVE_PROTO_DRAKE, 50.0f))
				{
					me->SetFacingToObject(drake);
					DoCast(drake, SPELL_PROTO_DRAKE_CHAIN_CHANNEL, true);
					DrakeChainsTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else DrakeChainsTimer -= diff;

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captive_proto_drake_beam_bunnyAI(creature);
    }
};

class npc_ice_spike_target_bunny : public CreatureScript
{
public:
    npc_ice_spike_target_bunny() : CreatureScript("npc_ice_spike_target_bunny") {}

    struct npc_ice_spike_target_bunnyAI : public QuantumBasicAI
    {
        npc_ice_spike_target_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_HELM_SPARKLE, true);
			DoCast(me, SPELL_HELM_SPARKLE_LARGE, true);
			DoCast(me, SPELL_HELM_GIGANTIC_SPARKLE, true);
			DoCast(me, SPELL_HELM_EXTRA_SPARKLE, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ice_spike_target_bunnyAI(creature);
    }
};

enum IceboundRevenant
{
	SPELL_IR_AVALANCHE = 55216,
};

class npc_icebound_revenant : public CreatureScript
{
public:
    npc_icebound_revenant() : CreatureScript("npc_icebound_revenant") {}

    struct npc_icebound_revenantAI : public QuantumBasicAI
    {
        npc_icebound_revenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 AvalancheTimer;

        void Reset()
        {
			AvalancheTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AvalancheTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IR_AVALANCHE);
					AvalancheTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else AvalancheTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icebound_revenantAI(creature);
    }
};

enum StormforgedDecimator
{
	SPELL_STORM_PUNCH  = 56352,
	SPELL_SD_WAR_STOMP = 35238,
};

class npc_stormforged_decimator : public CreatureScript
{
public:
    npc_stormforged_decimator() : CreatureScript("npc_stormforged_decimator") {}

    struct npc_stormforged_decimatorAI : public QuantumBasicAI
    {
        npc_stormforged_decimatorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StormPunchTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			StormPunchTimer = 1*IN_MILLISECONDS;
			WarStompTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StormPunchTimer <= diff)
			{
				DoCastVictim(SPELL_STORM_PUNCH, true);
				StormPunchTimer = 5*IN_MILLISECONDS;
			}
			else StormPunchTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_SD_WAR_STOMP);
				WarStompTimer = 8*IN_MILLISECONDS;
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_decimatorAI(creature);
    }
};

enum TorsegTheExiled
{
	SPELL_TTE_WAR_STOMP  = 33707,
	SPELL_BACKBREAKER    = 53437,
	SPELL_COSMETIC_SLEEP = 55474,
};

class npc_torseg_the_exiled : public CreatureScript
{
public:
    npc_torseg_the_exiled() : CreatureScript("npc_torseg_the_exiled") {}

    struct npc_torseg_the_exiledAI : public QuantumBasicAI
    {
        npc_torseg_the_exiledAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BackbreakerTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			BackbreakerTimer = 2*IN_MILLISECONDS;
			WarStompTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_COSMETIC_SLEEP, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SLEEP);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BackbreakerTimer <= diff)
			{
				DoCastVictim(SPELL_BACKBREAKER);
				BackbreakerTimer = 6*IN_MILLISECONDS;
			}
			else BackbreakerTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_TTE_WAR_STOMP);
				WarStompTimer = 8*IN_MILLISECONDS;
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_torseg_the_exiledAI(creature);
    }
};

enum CrystalwebSpitter
{
	SPELL_CS_WEB        = 12023,
	SPELL_CRYSTAL_SPIKE = 57616,
};

class npc_crystalweb_spitter : public CreatureScript
{
public:
    npc_crystalweb_spitter() : CreatureScript("npc_crystalweb_spitter") {}

    struct npc_crystalweb_spitterAI : public QuantumBasicAI
    {
        npc_crystalweb_spitterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrystalSpikeTimer;
		uint32 WebTimer;

        void Reset()
        {
			CrystalSpikeTimer = 0.5*IN_MILLISECONDS;
			WebTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrystalSpikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRYSTAL_SPIKE);
					CrystalSpikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CrystalSpikeTimer -= diff;

			if (WebTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CS_WEB);
					WebTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else WebTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalweb_spitterAI(creature);
    }
};

enum CrystalwebWeaver
{
	SPELL_CRYSTAL_CHAINS = 47698,
};

class npc_crystalweb_weaver : public CreatureScript
{
public:
    npc_crystalweb_weaver() : CreatureScript("npc_crystalweb_weaver") {}

    struct npc_crystalweb_weaverAI : public QuantumBasicAI
    {
        npc_crystalweb_weaverAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrystalChainsTimer;

        void Reset()
        {
			CrystalChainsTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrystalChainsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRYSTAL_CHAINS);
					CrystalChainsTimer = 6*IN_MILLISECONDS;
				}
			}
			else CrystalChainsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalweb_weaverAI(creature);
    }
};

enum SnowblindDigger
{
	SPELL_PUNCTURE_WOUND  = 48374,
	SPELL_SD_SUNDER_ARMOR = 50370,
};

class npc_snowblind_digger : public CreatureScript
{
public:
    npc_snowblind_digger() : CreatureScript("npc_snowblind_digger") {}

    struct npc_snowblind_diggerAI : public QuantumBasicAI
    {
        npc_snowblind_diggerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SunderArmorTimer;
		uint32 PunctureWoundTimer;

        void Reset()
        {
			SunderArmorTimer = 1*IN_MILLISECONDS;
			PunctureWoundTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SD_SUNDER_ARMOR);
				SunderArmorTimer = 4*IN_MILLISECONDS;
			}
			else SunderArmorTimer -= diff;

			if (PunctureWoundTimer <= diff)
			{
				DoCastVictim(SPELL_PUNCTURE_WOUND);
				PunctureWoundTimer = 6*IN_MILLISECONDS;
			}
			else PunctureWoundTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowblind_diggerAI(creature);
    }
};

enum IcetipCrawler
{
	SPELL_ICETIP_POISON = 57617,
};

class npc_icetip_crawler : public CreatureScript
{
public:
    npc_icetip_crawler() : CreatureScript("npc_icetip_crawler") {}

    struct npc_icetip_crawlerAI : public QuantumBasicAI
    {
        npc_icetip_crawlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IcetipPoisonTimer;

        void Reset()
        {
			IcetipPoisonTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcetipPoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ICETIP_POISON);
					IcetipPoisonTimer = 4*IN_MILLISECONDS;
				}
			}
			else IcetipPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icetip_crawlerAI(creature);
    }
};

enum AlgarTheChosen
{
	SPELL_DREADFUL_ROAR = 42729,
	ALGAR_MOUNT_ID      = 26645,
};

class npc_algar_the_chosen : public CreatureScript
{
public:
    npc_algar_the_chosen() : CreatureScript("npc_algar_the_chosen") {}

    struct npc_algar_the_chosenAI : public QuantumBasicAI
    {
        npc_algar_the_chosenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DreadfulRoarTimer;

        void Reset()
        {
			DreadfulRoarTimer = 2*IN_MILLISECONDS;

			me->Mount(ALGAR_MOUNT_ID);

			me->GetMotionMaster()->MoveRandom(25.0f);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->RemoveMount();

			me->GetMotionMaster()->MoveFall();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DreadfulRoarTimer <= diff)
			{
				DoCastAOE(SPELL_DREADFUL_ROAR);
				DreadfulRoarTimer = 5*IN_MILLISECONDS;
			}
			else DreadfulRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_algar_the_chosenAI(creature);
    }
};

enum ServantOfDrakuru
{
	SPELL_GUT_RIP          = 52401,
	SPELL_SD_ROT_ARMOR     = 50361,
	SPELL_STUNNING_FORCE   = 52402,
	SPELL_FEROCIOUS_ENRAGE = 52400,
};

class npc_servant_of_drakuru : public CreatureScript
{
public:
    npc_servant_of_drakuru() : CreatureScript("npc_servant_of_drakuru") {}

    struct npc_servant_of_drakuruAI : public QuantumBasicAI
    {
        npc_servant_of_drakuruAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RotArmorTimer;
		uint32 GutRipTimer;
		uint32 StunningForceTimer;

        void Reset()
        {
			RotArmorTimer = 2*IN_MILLISECONDS;
			GutRipTimer = 3*IN_MILLISECONDS;
			StunningForceTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RotArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SD_ROT_ARMOR);
				RotArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RotArmorTimer -= diff;

			if (GutRipTimer <= diff)
			{
				DoCastVictim(SPELL_GUT_RIP);
				GutRipTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GutRipTimer -= diff;

			if (StunningForceTimer <= diff)
			{
				DoCastVictim(SPELL_STUNNING_FORCE);
				StunningForceTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else StunningForceTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_FEROCIOUS_ENRAGE, 0))
				{
					DoCast(me, SPELL_FEROCIOUS_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_servant_of_drakuruAI(creature);
    }
};

enum Blightguard
{
	SPELL_BLIGHTGUARD_TRIGGER = 52068,
	SPELL_INVISIBLE           = 30991,
	SPELL_SHADOWSTRIKE        = 33914,
};

class npc_blightguard : public CreatureScript
{
public:
    npc_blightguard() : CreatureScript("npc_blightguard") {}

    struct npc_blightguardAI : public QuantumBasicAI
    {
        npc_blightguardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowstrikeTimer;

        void Reset()
        {
			ShadowstrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_INVISIBLE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_INVISIBLE);

			DoCast(me, SPELL_BLIGHTGUARD_TRIGGER);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowstrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SHADOWSTRIKE);
				ShadowstrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ShadowstrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blightguardAI(creature);
    }
};

enum FlyingFiend
{
	SPELL_FF_GARGOYLE_STRIKE = 16564,
};

class npc_flying_fiend : public CreatureScript
{
public:
    npc_flying_fiend() : CreatureScript("npc_flying_fiend") {}

    struct npc_flying_fiendAI : public QuantumBasicAI
    {
        npc_flying_fiendAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GargoyleStrikeTimer;

        void Reset()
		{
			GargoyleStrikeTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GargoyleStrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FF_GARGOYLE_STRIKE);
					GargoyleStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else GargoyleStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flying_fiendAI(creature);
    }
};

enum LootCrazedPoacher
{
	SPELL_LCP_STEALTH   = 34189,

	SAY_POACHER_AGGRO_1 = -1300031,
	SAY_POACHER_AGGRO_2 = -1300032,
	SAY_POACHER_AGGRO_3 = -1300033,
	SAY_POACHER_AGGRO_4 = -1300034,
	SAY_POACHER_AGGRO_5 = -1300035,
	SAY_POACHER_AGGRO_6 = -1300036,
	SAY_POACHER_AGGRO_7 = -1300037,
	SAY_POACHER_AGGRO_8 = -1300038,
};

class npc_loot_crazed_poacher : public CreatureScript
{
public:
    npc_loot_crazed_poacher() : CreatureScript("npc_loot_crazed_poacher") {}

    struct npc_loot_crazed_poacherAI : public QuantumBasicAI
    {
        npc_loot_crazed_poacherAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_LCP_STEALTH);

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* who)
		{
			me->RemoveAurasDueToSpell(SPELL_LCP_STEALTH);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_POACHER_AGGRO_1, SAY_POACHER_AGGRO_2, SAY_POACHER_AGGRO_3, SAY_POACHER_AGGRO_4,
				SAY_POACHER_AGGRO_5, SAY_POACHER_AGGRO_6, SAY_POACHER_AGGRO_7, SAY_POACHER_AGGRO_8), me);
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
        return new npc_loot_crazed_poacherAI(creature);
    }
};

enum ForgottenCaptain
{
	SPELL_FORGOTTEN_AURA             = 48143,
	SPELL_WINTERGLADE_INVISIBILITY_B = 48357,
	SPELL_FC_STORMHAMMER             = 51591,

	FORGOTTEN_CAPTAIN_MOUNT_ID       = 24447,
};

class npc_forgotten_captain : public CreatureScript
{
public:
    npc_forgotten_captain() : CreatureScript("npc_forgotten_captain") {}

    struct npc_forgotten_captainAI : public QuantumBasicAI
    {
        npc_forgotten_captainAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StormhammerTimer;

        void Reset()
        {
			StormhammerTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_FORGOTTEN_AURA, true);
			DoCast(me, SPELL_WINTERGLADE_INVISIBILITY_B, true);

			me->Mount(FORGOTTEN_CAPTAIN_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StormhammerTimer <= diff)
			{
				DoCastVictim(SPELL_FC_STORMHAMMER);
				StormhammerTimer = 4*IN_MILLISECONDS;
			}
			else StormhammerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_captainAI(creature);
    }
};

enum ScaleswornElite
{
	SPELL_HOVER_ANIM   = 57764,
	SPELL_ICE_SHARD    = 61269,
	SPELL_ARCANE_SURGE = 61272,
};

class npc_scalesworn_elite : public CreatureScript
{
public:
    npc_scalesworn_elite() : CreatureScript("npc_scalesworn_elite") {}

    struct npc_scalesworn_eliteAI : public QuantumBasicAI
    {
        npc_scalesworn_eliteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IceShardTimer;
		uint32 ArcaneSurgeTimer;

        void Reset()
		{
			IceShardTimer = 0.5*IN_MILLISECONDS;
			ArcaneSurgeTimer = 2*IN_MILLISECONDS;

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

			if (IceShardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ICE_SHARD);
					IceShardTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else IceShardTimer -= diff;

			if (ArcaneSurgeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_SURGE);
					ArcaneSurgeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ArcaneSurgeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scalesworn_eliteAI(creature);
    }
};

enum ScourgingCrystal
{
	SPELL_SCOURGING_BEAM   = 43872, // ??
	SPELL_PURPLE_SHIELD_X3 = 43874,
};

class npc_scourging_crystal : public CreatureScript
{
public:
    npc_scourging_crystal() : CreatureScript("npc_scourging_crystal") {}

    struct npc_scourging_crystalAI : public QuantumBasicAI
    {
        npc_scourging_crystalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
        {
			DoCast(me, SPELL_PURPLE_SHIELD_X3);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourging_crystalAI(creature);
    }
};

enum Necrotech
{
	SPELL_NT_SHADOW_BOLT       = 9613,
	SPELL_NT_DRAIN_LIFE        = 17173,
	SPELL_NT_CURSE_OF_WEAKNESS = 18267,

	NPC_SCOURGE_CRYSTAL        = 24464,
};

class npc_necrotech : public CreatureScript
{
public:
    npc_necrotech() : CreatureScript("npc_necrotech") {}

    struct npc_necrotechAI : public QuantumBasicAI
    {
        npc_necrotechAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NecroticPurpleBeamTimer;
		uint32 CurseOfWeaknessTimer;
		uint32 ShadowBoltTimer;
		uint32 DrainLifeTimer;

        void Reset()
        {
			NecroticPurpleBeamTimer = 0.1*IN_MILLISECONDS;
			CurseOfWeaknessTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 1*IN_MILLISECONDS;
			DrainLifeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (NecroticPurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* crystal = me->FindCreatureWithDistance(NPC_SCOURGE_CRYSTAL, 35.0f))
				{
					me->SetFacingToObject(crystal);
					DoCast(crystal, SPELL_NECROTIC_PURPLE_BEAM, true);
					NecroticPurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else NecroticPurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfWeaknessTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NT_CURSE_OF_WEAKNESS);
					CurseOfWeaknessTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfWeaknessTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NT_SHADOW_BOLT);
					ShadowBoltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (DrainLifeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NT_DRAIN_LIFE);
					DrainLifeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else DrainLifeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_necrotechAI(creature);
    }
};

enum CoprousTheDefiled
{
	SPELL_KNOCKDOWN   = 5164,
	SPELL_ACID_GEYSER = 38971,
};

class npc_coprous_the_defiled : public CreatureScript
{
public:
    npc_coprous_the_defiled() : CreatureScript("npc_coprous_the_defiled") {}

    struct npc_coprous_the_defiledAI : public QuantumBasicAI
    {
        npc_coprous_the_defiledAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockdownTimer;
		uint32 AcidGeyserTimer;

        void Reset()
        {
			KnockdownTimer = 0.5*IN_MILLISECONDS;
			AcidGeyserTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else KnockdownTimer -= diff;

			if (AcidGeyserTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ACID_GEYSER);
					AcidGeyserTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else AcidGeyserTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coprous_the_defiledAI(creature);
    }
};

enum HebJin
{
	SPELL_GROUND_SMASH = 12734,
	SPELL_PUMMEL       = 12555,
	SPELL_THUNDERCLAP  = 15548,

	SAY_HEBJIN_SPAWN   = -1420081,
	SAY_HEBJIN_AGGRO   = -1420082,
};

class npc_hebjin : public CreatureScript
{
public:
    npc_hebjin() : CreatureScript("npc_hebjin") {}

    struct npc_hebjinAI : public QuantumBasicAI
    {
        npc_hebjinAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GroundSmashTimer;
		uint32 PummelTimer;
		uint32 ThunderclapTimer;

        void Reset()
        {
			GroundSmashTimer = 1*IN_MILLISECONDS;
			PummelTimer = 2*IN_MILLISECONDS;
			ThunderclapTimer = 3*IN_MILLISECONDS;

			DoSendQuantumText(SAY_HEBJIN_SPAWN, me);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_HEBJIN_AGGRO, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GroundSmashTimer <= diff)
			{
				DoCastAOE(SPELL_GROUND_SMASH);
				GroundSmashTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else GroundSmashTimer -= diff;

			if (PummelTimer <= diff)
			{
				DoCastVictim(SPELL_PUMMEL);
				PummelTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else PummelTimer -= diff;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERCLAP);
				ThunderclapTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else ThunderclapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hebjinAI(creature);
    }
};

enum EnchantingCosmetic
{
	SPELL_COSMETIC_ENCHANT_CAST = 60888,
};

class npc_fael_morningsong : public CreatureScript
{
public:
    npc_fael_morningsong() : CreatureScript("npc_fael_morningsong") {}

    struct npc_fael_morningsongAI : public QuantumBasicAI
    {
        npc_fael_morningsongAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CastEnchantTimer;

        void Reset()
        {
			CastEnchantTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (CastEnchantTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_COSMETIC_ENCHANT_CAST);
				CastEnchantTimer = 3*IN_MILLISECONDS;
			}
			else CastEnchantTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fael_morningsongAI(creature);
    }
};

class npc_enchanter_nalthanis : public CreatureScript
{
public:
    npc_enchanter_nalthanis() : CreatureScript("npc_enchanter_nalthanis") {}

    struct npc_enchanter_nalthanisAI : public QuantumBasicAI
    {
        npc_enchanter_nalthanisAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CastEnchantTimer;

        void Reset()
        {
			CastEnchantTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (CastEnchantTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_COSMETIC_ENCHANT_CAST);
				CastEnchantTimer = 3*IN_MILLISECONDS;
			}
			else CastEnchantTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enchanter_nalthanisAI(creature);
    }
};

enum MarshCaribou
{
	SPELL_MC_GORE = 32019,
};

class npc_marsh_caribou : public CreatureScript
{
public:
    npc_marsh_caribou() : CreatureScript("npc_marsh_caribou") {}

    struct npc_marsh_caribouAI : public QuantumBasicAI
    {
        npc_marsh_caribouAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ANIMAL_BLOOD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GoreTimer <= diff)
			{
				DoCastVictim(SPELL_MC_GORE);
				GoreTimer = 5*IN_MILLISECONDS;
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marsh_caribouAI(creature);
    }
};

enum MarshFawn
{
	SPELL_MF_GORE = 32019,
};

class npc_marsh_fawn : public CreatureScript
{
public:
    npc_marsh_fawn() : CreatureScript("npc_marsh_fawn") {}

    struct npc_marsh_fawnAI : public QuantumBasicAI
    {
        npc_marsh_fawnAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ANIMAL_BLOOD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GoreTimer <= diff)
			{
				DoCastVictim(SPELL_MF_GORE);
				GoreTimer = 5*IN_MILLISECONDS;
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marsh_fawnAI(creature);
    }
};

enum MammothCalf
{
	SPELL_MC_TRAMPLE = 15550,
};

class npc_mammoth_calf : public CreatureScript
{
public:
    npc_mammoth_calf() : CreatureScript("npc_mammoth_calf") {}

    struct npc_mammoth_calfAI : public QuantumBasicAI
    {
        npc_mammoth_calfAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TrampleTimer;

        void Reset()
        {
			TrampleTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ANIMAL_BLOOD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_MC_TRAMPLE);
				TrampleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mammoth_calfAI(creature);
    }
};

enum WoolyMammoth
{
	SPELL_WM_TRAMPLE = 15550,
};

class npc_wooly_mammoth : public CreatureScript
{
public:
    npc_wooly_mammoth() : CreatureScript("npc_wooly_mammoth") {}

    struct npc_wooly_mammothAI : public QuantumBasicAI
    {
        npc_wooly_mammothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TrampleTimer;

        void Reset()
        {
			TrampleTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ANIMAL_BLOOD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_WM_TRAMPLE);
				TrampleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wooly_mammothAI(creature);
    }
};

enum WoolyMammothBull
{
	SPELL_MAMMOTH_CHARGE  = 46315,
	SPELL_THUNDERING_ROAR = 46316,
};

class npc_wooly_mammoth_bull : public CreatureScript
{
public:
    npc_wooly_mammoth_bull() : CreatureScript("npc_wooly_mammoth_bull") {}

    struct npc_wooly_mammoth_bullAI : public QuantumBasicAI
    {
        npc_wooly_mammoth_bullAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThunderingRoarTimer;

        void Reset()
        {
			ThunderingRoarTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_MAMMOTH_CHARGE);
		}

		void JustDied(Unit* killer)
		{
			DoCast(killer, SPELL_ANIMAL_BLOOD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderingRoarTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERING_ROAR);
				ThunderingRoarTimer = 9*IN_MILLISECONDS;
			}
			else ThunderingRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wooly_mammoth_bullAI(creature);
    }
};

enum GjalerbronRuneCaster
{
	SPELL_GR_FROSTBOLT    = 9672,
	SPELL_FREEZING_CIRCLE = 34787,
	SPELL_RUNE_WARD       = 43453,
};

class npc_gjalerbron_rune_caster : public CreatureScript
{
public:
    npc_gjalerbron_rune_caster() : CreatureScript("npc_gjalerbron_rune_caster") {}

    struct npc_gjalerbron_rune_casterAI : public QuantumBasicAI
    {
        npc_gjalerbron_rune_casterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 FreezingCircleTimer;
		uint32 RuneWardTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FreezingCircleTimer = 2*IN_MILLISECONDS;
			RuneWardTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_GR_FROSTBOLT, false);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FreezingCircleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FREEZING_CIRCLE, true);
					FreezingCircleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FreezingCircleTimer -= diff;

			if (RuneWardTimer <= diff)
			{
				DoCast(me, SPELL_RUNE_WARD);
				RuneWardTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else RuneWardTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gjalerbron_rune_casterAI(creature);
    }
};

enum AzureManashaper
{
	SPELL_AZURE_CHANNEL       = 59069,
	SPELL_AM_ARCANE_BLAST     = 10833,
	SPELL_AM_SLOW             = 25603,

	NPC_AZURE_CHANNEL_STALKER = 31400,
};

class npc_azure_manashaper : public CreatureScript
{
public:
    npc_azure_manashaper() : CreatureScript("npc_azure_manashaper") {}

    struct npc_azure_manashaperAI : public QuantumBasicAI
    {
        npc_azure_manashaperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChannelTimer;
		uint32 ArcaneBlastTimer;
		uint32 SlowTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			ChannelTimer = 0.1*IN_MILLISECONDS;
			ArcaneBlastTimer = 0.5*IN_MILLISECONDS;
			SlowTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ChannelTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* stalker = me->FindCreatureWithDistance(NPC_AZURE_CHANNEL_STALKER, 35.0f))
				{
					me->SetFacingToObject(stalker);
					DoCast(stalker, SPELL_AZURE_CHANNEL, true);
					ChannelTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else ChannelTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AM_ARCANE_BLAST);
					ArcaneBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ArcaneBlastTimer -= diff;

			if (SlowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AM_SLOW);
					SlowTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_manashaperAI(creature);
    }
};

enum CultistSaboteur
{
	SPELL_THROW_DYNAMITE = 40062,
};

class npc_cultist_saboteur : public CreatureScript
{
public:
    npc_cultist_saboteur() : CreatureScript("npc_cultist_saboteur") {}

    struct npc_cultist_saboteurAI : public QuantumBasicAI
    {
        npc_cultist_saboteurAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowDynamiteTimer;

        void Reset()
        {
			ThrowDynamiteTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowDynamiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_THROW_DYNAMITE);
					ThrowDynamiteTimer = 5*IN_MILLISECONDS;
				}
			}
			else ThrowDynamiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cultist_saboteurAI(creature);
    }
};

enum UnboundCorrupter
{
	SPELL_UC_ARCANE_BOLT   = 38204,
	SPELL_UC_LEY_CURSE     = 58667,
	SPELL_TRANSFERED_POWER = 58270,
};

class npc_unbound_corrupter : public CreatureScript
{
public:
    npc_unbound_corrupter() : CreatureScript("npc_unbound_corrupter") {}

    struct npc_unbound_corrupterAI : public QuantumBasicAI
    {
        npc_unbound_corrupterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ArcaneBoltTimer;
		uint32 LeyCurseTimer;

        void Reset()
        {
			ArcaneBoltTimer = 0.5*IN_MILLISECONDS;
			LeyCurseTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UC_ARCANE_BOLT);
					ArcaneBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ArcaneBoltTimer -= diff;

			if (LeyCurseTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UC_LEY_CURSE);
					LeyCurseTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else LeyCurseTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_TRANSFERED_POWER, 0))
					DoCast(me, SPELL_TRANSFERED_POWER, true);

				DoSpellAttackIfReady(SPELL_UC_ARCANE_BOLT);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_corrupterAI(creature);
    }
};

enum BurningDepthsNecrolyte
{
	NPC_BONE_TARGET_BUNNY    = 27402,

	SPELL_DESTRUCTIVE_STRIKE = 51430,
	SPELL_OBSIDIAN_ESSENCE   = 48616,
};

class npc_burning_depths_necrolyte : public CreatureScript
{
public:
    npc_burning_depths_necrolyte() : CreatureScript("npc_burning_depths_necrolyte") {}

    struct npc_burning_depths_necrolyteAI : public QuantumBasicAI
    {
        npc_burning_depths_necrolyteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NecroticPurpleBeamTimer;
		uint32 DestructiveStrikeTimer;

        void Reset()
        {
			NecroticPurpleBeamTimer = 0.1*IN_MILLISECONDS;
			DestructiveStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (NecroticPurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* BoneTarget = me->FindCreatureWithDistance(NPC_BONE_TARGET_BUNNY, 25.0f))
				{
					me->SetFacingToObject(BoneTarget);
					me->InterruptSpell(CURRENT_CHANNELED_SPELL);
					DoCast(BoneTarget, SPELL_NECROTIC_PURPLE_BEAM, true);
					NecroticPurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else NecroticPurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DestructiveStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_DESTRUCTIVE_STRIKE);
				DestructiveStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DestructiveStrikeTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_40))
			{
				if (!me->HasAuraEffect(SPELL_OBSIDIAN_ESSENCE, 0))
					DoCast(me, SPELL_OBSIDIAN_ESSENCE, true);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burning_depths_necrolyteAI(creature);
    }
};

enum OnslaughtRavenBishop
{
	SPELL_FLOCK_CALL            = 40427,
	SPELL_BLESSING_OF_THE_LIGHT = 54323,
	SPELL_RAVEN_FLOCK           = 50740,
	SPELL_RAVEN_HEAL            = 50750,
};

class npc_onslaught_raven_bishop : public CreatureScript
{
public:
    npc_onslaught_raven_bishop() : CreatureScript("npc_onslaught_raven_bishop") {}

    struct npc_onslaught_raven_bishopAI : public QuantumBasicAI
    {
        npc_onslaught_raven_bishopAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RavenFlockTimer;
		uint32 FlockCallTimer;
		uint32 RavenHealTimer;

        void Reset()
        {
			RavenFlockTimer = 0.5*IN_MILLISECONDS;
			FlockCallTimer = 2*IN_MILLISECONDS;
			RavenHealTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_BLESSING_OF_THE_LIGHT, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RavenFlockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAVEN_FLOCK);
					RavenFlockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else RavenFlockTimer -= diff;

			if (FlockCallTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLOCK_CALL);
					FlockCallTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else FlockCallTimer -= diff;

			if (RavenHealTimer <= diff)
			{
				DoCast(me, SPELL_RAVEN_HEAL, true);
				RavenHealTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else RavenHealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onslaught_raven_bishopAI(creature);
    }
};

enum OnslaughtDarkweaver
{
	SPELL_OD_DEMON_ARMOR  = 13787,
	SPELL_OD_SHADOW_BOLT  = 12739,
	SPELL_METAMORPHOSIS   = 54840,
	SPELL_SUMMON_IMP      = 11939,
	SPELL_SUMMON_SUCCUBUS = 8722,

	NPC_IMP_MINION        = 12922,
	NPC_SUCCUBUS_MINION   = 10928,
};

class npc_onslaught_darkweaver : public CreatureScript
{
public:
    npc_onslaught_darkweaver() : CreatureScript("npc_onslaught_darkweaver") {}

    struct npc_onslaught_darkweaverAI : public QuantumBasicAI
    {
        npc_onslaught_darkweaverAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShadowBoltTimer;

		SummonList Summons;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_SUCCUBUS, true);

			me->SummonCreature(NPC_IMP_MINION, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_IMP_MINION:
				case NPC_SUCCUBUS_MINION:
					Summons.Summon(summon);
					summon->SetCurrentFaction(me->GetFaction());
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
						summon->SetInCombatWith(target);
					break;
				default:
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
			if (!me->HasAura(SPELL_OD_DEMON_ARMOR) && !me->IsInCombatActive())
				DoCast(me, SPELL_OD_DEMON_ARMOR);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_OD_DEMON_ARMOR))
				DoCast(me, SPELL_OD_DEMON_ARMOR, true);

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_OD_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAura(SPELL_METAMORPHOSIS))
					DoCast(me, SPELL_METAMORPHOSIS, true);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onslaught_darkweaverAI(creature);
    }
};

enum AnubarCultist
{
	SPELL_AC_SHADOW_BOLT   = 9613,
	SPELL_ZEAL             = 51605,
};

class npc_anubar_cultist : public CreatureScript
{
public:
    npc_anubar_cultist() : CreatureScript("npc_anubar_cultist") {}

    struct npc_anubar_cultistAI : public QuantumBasicAI
    {
        npc_anubar_cultistAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltTimer;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(me, SPELL_ZEAL, true);
					DoCast(target, SPELL_AC_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_cultistAI(creature);
    }
};

enum AnubarBlightbeast
{
	SPELL_AB_POISON_BOLT  = 21971,
	SPELL_EMPOWER         = 47257, // Need More info
	SPELL_BLIGHTED_SHRIEK = 47443,
};

class npc_anubar_blightbeast : public CreatureScript
{
public:
    npc_anubar_blightbeast() : CreatureScript("npc_anubar_blightbeast") {}

    struct npc_anubar_blightbeastAI : public QuantumBasicAI
    {
        npc_anubar_blightbeastAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonBoltTimer;
		uint32 BlightedShriekTimer;

        void Reset()
        {
			PoisonBoltTimer = 0.5*IN_MILLISECONDS;
			BlightedShriekTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AB_POISON_BOLT);
					PoisonBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else PoisonBoltTimer -= diff;

			if (BlightedShriekTimer <= diff)
			{
				DoCastAOE(SPELL_BLIGHTED_SHRIEK);
				BlightedShriekTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else BlightedShriekTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_blightbeastAI(creature);
    }
};

enum BinkieBrightgear
{
	SPELL_CREATE_INGENEERING_ITEM = 45643,
};

class npc_binkie_brightgear : public CreatureScript
{
public:
    npc_binkie_brightgear() : CreatureScript("npc_binkie_brightgear") {}

    struct npc_binkie_brightgearAI : public QuantumBasicAI
    {
        npc_binkie_brightgearAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CreateIngeneeringItemTimer;

        void Reset()
        {
			CreateIngeneeringItemTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			if (CreateIngeneeringItemTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_CREATE_INGENEERING_ITEM);
				CreateIngeneeringItemTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else CreateIngeneeringItemTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_binkie_brightgearAI(creature);
    }
};

enum FrostbroodSpawn
{
	SPELL_FS_FROST_BREATH = 60667,
};

class npc_frostbrood_spawn : public CreatureScript
{
public:
    npc_frostbrood_spawn() : CreatureScript("npc_frostbrood_spawn") {}

    struct npc_frostbrood_spawnAI : public QuantumBasicAI
    {
        npc_frostbrood_spawnAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBreathTimer;

        void Reset()
        {
			FrostBreathTimer = 2*IN_MILLISECONDS;

			me->SetCanFly(true);
			me->SetDisableGravity(true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetCanFly(false);
			me->SetDisableGravity(false);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBreathTimer <= diff)
			{
				DoCastVictim(SPELL_FS_FROST_BREATH);
				FrostBreathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostbrood_spawnAI(creature);
    }
};

enum DeepJormungar
{
	SPELL_DJ_STAND     = 37752,
	SPELL_DJ_ACID_SPIT = 61597,
	SPELL_DJ_SWEEP     = 61598,
};

class npc_deep_jormungar : public CreatureScript
{
public:
    npc_deep_jormungar() : CreatureScript("npc_deep_jormungar") {}

    struct npc_deep_jormungarAI : public QuantumBasicAI
    {
        npc_deep_jormungarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 AcidSpitTimer;
		uint32 SweepTimer;

        void Reset()
        {
			AcidSpitTimer = 0.5*IN_MILLISECONDS;
			SweepTimer = 2*IN_MILLISECONDS;

			me->SetStandState(UNIT_STAND_STATE_SUBMERGED);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

			me->SetStandState(UNIT_STAND_STATE_STAND);

			DoCast(me, SPELL_STAND);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AcidSpitTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DJ_ACID_SPIT);
					AcidSpitTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else AcidSpitTimer -= diff;

			if (SweepTimer <= diff)
			{
				DoCastAOE(SPELL_DJ_SWEEP);
				SweepTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else SweepTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deep_jormungarAI(creature);
    }
};

enum Crypts
{
	SPELL_CRYPT_SCARABS = 31600,
};

class npc_crypt_crawler : public CreatureScript
{
public:
    npc_crypt_crawler() : CreatureScript("npc_crypt_crawler") {}

    struct npc_crypt_crawlerAI : public QuantumBasicAI
    {
        npc_crypt_crawlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CryptScarabsTimer;

        void Reset()
        {
			CryptScarabsTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CryptScarabsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRYPT_SCARABS);
					CryptScarabsTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CryptScarabsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crypt_crawlerAI(creature);
    }
};

class npc_risen_crypt_lord : public CreatureScript
{
public:
    npc_risen_crypt_lord() : CreatureScript("npc_risen_crypt_lord") {}

    struct npc_risen_crypt_lordAI : public QuantumBasicAI
    {
        npc_risen_crypt_lordAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CryptScarabsTimer;

        void Reset()
        {
			CryptScarabsTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CryptScarabsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRYPT_SCARABS);
					CryptScarabsTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CryptScarabsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_crypt_lordAI(creature);
    }
};

enum PlaguedScavenger
{
	SPELL_PS_INFECTED_BITE = 49861,
};

class npc_plagued_scavenger : public CreatureScript
{
public:
    npc_plagued_scavenger() : CreatureScript("npc_plagued_scavenger") {}

    struct npc_plagued_scavengerAI : public QuantumBasicAI
    {
        npc_plagued_scavengerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedBiteTimer;

        void Reset()
		{
			InfectedBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedBiteTimer <= diff)
			{
				DoCastVictim(SPELL_PS_INFECTED_BITE);
				InfectedBiteTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else InfectedBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plagued_scavengerAI(creature);
    }
};

class npc_reckless_scavenger : public CreatureScript
{
public:
    npc_reckless_scavenger() : CreatureScript("npc_reckless_scavenger") {}

    struct npc_reckless_scavengerAI : public QuantumBasicAI
    {
        npc_reckless_scavengerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedBiteTimer;

        void Reset()
		{
			InfectedBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedBiteTimer <= diff)
			{
				DoCastVictim(SPELL_PS_INFECTED_BITE);
				InfectedBiteTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else InfectedBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reckless_scavengerAI(creature);
    }
};

enum FarshireMilitia
{
	SPELL_FM_CLEAVE = 15284,
};

class npc_farshire_militia : public CreatureScript
{
public:
    npc_farshire_militia() : CreatureScript("npc_farshire_militia") {}

    struct npc_farshire_militiaAI : public QuantumBasicAI
    {
        npc_farshire_militiaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_FM_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_farshire_militiaAI(creature);
    }
};

enum GeraldGreen
{
	SPELL_GR_CLEAVE = 40504,
	SPELL_GR_ENRAGE = 41447,
};

class npc_gerald_green : public CreatureScript
{
public:
    npc_gerald_green() : CreatureScript("npc_gerald_green") {}

    struct npc_gerald_greenAI : public QuantumBasicAI
    {
        npc_gerald_greenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_GR_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_GR_ENRAGE, 0))
				{
					DoCast(me, SPELL_GR_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gerald_greenAI(creature);
    }
};

enum CultistNecrolyte
{
	SPELL_CN_SHADOW_BOLT       = 9613,
	SPELL_CN_CURSE_OF_AGONY    = 18266,
	SPELL_CN_SHADOW_CHANNELING = 45104,
};

class npc_cultist_necrolyte : public CreatureScript
{
public:
    npc_cultist_necrolyte() : CreatureScript("npc_cultist_necrolyte") {}

    struct npc_cultist_necrolyteAI : public QuantumBasicAI
    {
        npc_cultist_necrolyteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltTimer;
		uint32 CurseOfAgonyTimer;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			CurseOfAgonyTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_CN_SHADOW_CHANNELING);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
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
					DoCast(target, SPELL_CN_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (CurseOfAgonyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CN_CURSE_OF_AGONY);
					CurseOfAgonyTimer = urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else CurseOfAgonyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cultist_necrolyteAI(creature);
    }
};

enum MagnataurAlpha
{
	SPELL_MA_STOMP         = 48131,
	SPELL_MA_BRUTAL_STRIKE = 58460,
};

class npc_magnataur_alpha : public CreatureScript
{
public:
    npc_magnataur_alpha() : CreatureScript("npc_magnataur_alpha") {}

    struct npc_magnataur_alphaAI : public QuantumBasicAI
    {
        npc_magnataur_alphaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BrutalStrikeTimer;
		uint32 StompTimer;

        void Reset()
		{
			BrutalStrikeTimer = 2*IN_MILLISECONDS;
			StompTimer = 4*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_MA_BRUTAL_STRIKE);
				BrutalStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else BrutalStrikeTimer -= diff;

			if (StompTimer <= diff)
			{
				DoCastAOE(SPELL_MA_STOMP);
				StompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else StompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magnataur_alphaAI(creature);
    }
};

enum WastesScavenger
{
	SPELL_WS_BONE_TOSS = 50403,
};

class npc_wastes_scavenger : public CreatureScript
{
public:
	npc_wastes_scavenger() : CreatureScript("npc_wastes_scavenger") {}

    struct npc_wastes_scavengerAI : public QuantumBasicAI
    {
		npc_wastes_scavengerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoneTossTimer;

        void Reset()
        {
			BoneTossTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoneTossTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WS_BONE_TOSS);
					BoneTossTimer = 3*IN_MILLISECONDS;
				}
			}
			else BoneTossTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_wastes_scavengerAI(creature);
    }
};

enum RottingStormGiant
{
	SPELL_FLESH_CONSTRUCT_TEST_1 = 50389,
	SPELL_FLESH_CONSTRUCT_TEST_2 = 50390,
	SPELL_FORGED_THUNDER         = 50405,
};

class npc_rotting_storm_giant : public CreatureScript
{
public:
    npc_rotting_storm_giant() : CreatureScript("npc_rotting_storm_giant") {}

    struct npc_rotting_storm_giantAI : public QuantumBasicAI
    {
        npc_rotting_storm_giantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ForgedThunderTimer;

        void Reset()
		{
			ForgedThunderTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_FLESH_CONSTRUCT_TEST_1, true);
			DoCast(me, SPELL_FLESH_CONSTRUCT_TEST_2, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			me->RemoveAurasDueToSpell(SPELL_FLESH_CONSTRUCT_TEST_1);
			me->RemoveAurasDueToSpell(SPELL_FLESH_CONSTRUCT_TEST_2);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ForgedThunderTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FORGED_THUNDER);
					ForgedThunderTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ForgedThunderTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rotting_storm_giantAI(creature);
    }
};

enum ThiassiTheLightningBringer
{
	SPELL_TL_WAR_STOMP         = 15593,
	SPELL_THASSIS_STORMBOLT    = 50456,
	SPELL_SHROUD_OF_LIGHTNING  = 50494,

	NPC_GRAND_NECROLORD_ANTIOK = 28006,
};

class npc_thiassi_the_lightning_bringer : public CreatureScript
{
public:
    npc_thiassi_the_lightning_bringer() : CreatureScript("npc_thiassi_the_lightning_bringer") {}

    struct npc_thiassi_the_lightning_bringerAI : public QuantumBasicAI
    {
        npc_thiassi_the_lightning_bringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			Reset();
		}

		uint32 ThiassisStormboltTimer;
		uint32 WarStompTimer;

        void Reset()
		{
			ThiassisStormboltTimer = 0.5*IN_MILLISECONDS;
			WarStompTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			me->AI()->AttackStart(who);
		}

		void JustDied(Unit* /*killer*/)
		{
			// Remove Shield For Antiok After Death Thiassi
			if (Creature* antiok = me->FindCreatureWithDistance(NPC_GRAND_NECROLORD_ANTIOK, 35.0f, true))
				antiok->RemoveAurasDueToSpell(SPELL_SHROUD_OF_LIGHTNING);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThiassisStormboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_THASSIS_STORMBOLT);
					ThiassisStormboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ThiassisStormboltTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_TL_WAR_STOMP);
				WarStompTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thiassi_the_lightning_bringerAI(creature);
    }
};

enum GrandNecrolordAntiok
{
	SPELL_RIDE_VEHICLE            = 46598,
	SPELL_GA_SEED_OF_CORRUTION    = 32863,
	SPELL_SHADOW_BOLT_1           = 50455,
	SPELL_SHADOW_BOLT_2           = 55984,
	SPELL_SCREAM_OF_CHAOS         = 50497,
	SPELL_FLESH_HARVEST           = 50501,

	NPC_THIASSI_LIGHTNING_BRINGER = 28018,

	SAY_ANTIOK_AGGRO              = -1420106,
};

class npc_grand_necrolord_antiok : public CreatureScript
{
public:
    npc_grand_necrolord_antiok() : CreatureScript("npc_grand_necrolord_antiok") {}

    struct npc_grand_necrolord_antiokAI : public QuantumBasicAI
    {
        npc_grand_necrolord_antiokAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SeedOfCorruptionTimer;
		uint32 ScreamOfChaosTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			SeedOfCorruptionTimer = 0.5*IN_MILLISECONDS;
			ScreamOfChaosTimer = 2.5*IN_MILLISECONDS;
			ShadowBoltTimer = 3*IN_MILLISECONDS;

			// Rebuff Shield for Antiok
			if (Creature* thiassi = me->FindCreatureWithDistance(NPC_THIASSI_LIGHTNING_BRINGER, 35.0f))
			{
				if (thiassi->IsAlive())
				{
					DoCast(me, SPELL_RIDE_VEHICLE, true);
					DoCast(me, SPELL_SHROUD_OF_LIGHTNING, true);
				}
			}

			DoCast(me, SPELL_RIDE_VEHICLE, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_ANTIOK_AGGRO, me);
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
					DoCast(target, SPELL_GA_SEED_OF_CORRUTION);
					SeedOfCorruptionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SeedOfCorruptionTimer -= diff;

			if (ScreamOfChaosTimer <= diff)
			{
				DoCastAOE(SPELL_SCREAM_OF_CHAOS);
				ScreamOfChaosTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ScreamOfChaosTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAND(SPELL_SHADOW_BOLT_1, SPELL_SHADOW_BOLT_2), true);
					ShadowBoltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grand_necrolord_antiokAI(creature);
    }
};


enum ColdwindWasteHuntress
{
	SPELL_CW_EYE_PECK = 49865,
};

class npc_coldwind_waste_huntress : public CreatureScript
{
public:
    npc_coldwind_waste_huntress() : CreatureScript("npc_coldwind_waste_huntress") {}

    struct npc_coldwind_waste_huntressAI : public QuantumBasicAI
    {
        npc_coldwind_waste_huntressAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EyePeckTimer;

        void Reset()
        {
			EyePeckTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_CW_EYE_PECK);
				EyePeckTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else EyePeckTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coldwind_waste_huntressAI(creature);
    }
};

enum DecrepitNecromancer
{
	SPELL_DN_SHADOW_BOLT     = 9613,
	SPELL_DN_CONVERSION_BEAM = 50659,

	NPC_CRYSTAL_STALKER      = 27180,
};

class npc_decrepit_necromancer : public CreatureScript
{
public:
    npc_decrepit_necromancer() : CreatureScript("npc_decrepit_necromancer") {}

    struct npc_decrepit_necromancerAI : public QuantumBasicAI
    {
        npc_decrepit_necromancerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NecroticPurpleBeamTimer;
		uint32 ShadowBoltTimer;
		uint32 ConversionBeamTimer;

        void Reset()
        {
			NecroticPurpleBeamTimer = 0.2*IN_MILLISECONDS;
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			ConversionBeamTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (NecroticPurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* stalker = me->FindCreatureWithDistance(NPC_CRYSTAL_STALKER, 35.0f))
				{
					me->SetFacingToObject(stalker);
					DoCast(stalker, SPELL_NECROTIC_PURPLE_BEAM, true);
					NecroticPurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else NecroticPurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DN_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ConversionBeamTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DN_CONVERSION_BEAM);
					ConversionBeamTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else ConversionBeamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_decrepit_necromancerAI(creature);
    }
};

enum HulkingAtrocity
{
	SPELL_HA_INFECTED_BITE = 49861,
};

class npc_hulking_atrocity : public CreatureScript
{
public:
    npc_hulking_atrocity() : CreatureScript("npc_hulking_atrocity") {}

    struct npc_hulking_atrocityAI : public QuantumBasicAI
    {
        npc_hulking_atrocityAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedBiteTimer;

        void Reset()
		{
			InfectedBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedBiteTimer <= diff)
			{
				DoCastVictim(SPELL_HA_INFECTED_BITE);
				InfectedBiteTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else InfectedBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hulking_atrocityAI(creature);
    }
};

enum ReanimatedDrakkariTribesman
{
	SPELL_CRAZED_HUNGER = 3151,
	SPELL_CANNIBALIZE   = 50642,
};

class npc_reanimated_drakkari_tribesman : public CreatureScript
{
public:
    npc_reanimated_drakkari_tribesman() : CreatureScript("npc_reanimated_drakkari_tribesman") {}

    struct npc_reanimated_drakkari_tribesmanAI : public QuantumBasicAI
    {
        npc_reanimated_drakkari_tribesmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrazedHungerTimer;
		uint32 CannibalizeTimer;

        void Reset()
		{
			CrazedHungerTimer = 0.5*IN_MILLISECONDS;
			CannibalizeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CANNIBALIZE);
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

			if (CrazedHungerTimer <= diff)
			{
				DoCast(me, SPELL_CRAZED_HUNGER);
				CrazedHungerTimer = 6*IN_MILLISECONDS;
			}
			else CrazedHungerTimer -= diff;

			if (CannibalizeTimer <= diff)
			{
				DoCast(me, SPELL_CANNIBALIZE);
				CannibalizeTimer = 8*IN_MILLISECONDS;
			}
			else CannibalizeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reanimated_drakkari_tribesmanAI(creature);
    }
};

enum BatteredDrakkariBerserker
{
	SPELL_BD_KNOCKDOWN = 37592,
	SPELL_BD_ENRAGE    = 50420,
};

class npc_battered_drakkari_berserker : public CreatureScript
{
public:
    npc_battered_drakkari_berserker() : CreatureScript("npc_battered_drakkari_berserker") {}

    struct npc_battered_drakkari_berserkerAI : public QuantumBasicAI
    {
        npc_battered_drakkari_berserkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockdownTimer;

        void Reset()
		{
			KnockdownTimer = 0.5*IN_MILLISECONDS;

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

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_BD_KNOCKDOWN);
				KnockdownTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else KnockdownTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_BD_ENRAGE, 0))
				{
					DoCast(me, SPELL_BD_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_battered_drakkari_berserkerAI(creature);
    }
};

enum TormentedDrakkari
{
	SPELL_TD_CRAZED_HUNGER  = 3151,
	SPELL_TD_KICK           = 43518,
	SPELL_TD_TORMENTED_ROAR = 50636,
	SPELL_TD_CANNIBALIZE    = 50642,
};

class npc_tormented_drakkari : public CreatureScript
{
public:
    npc_tormented_drakkari() : CreatureScript("npc_tormented_drakkari") {}

    struct npc_tormented_drakkariAI : public QuantumBasicAI
    {
        npc_tormented_drakkariAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrazedHungerTimer;
		uint32 CannibalizeTimer;
		uint32 KickTimer;
		uint32 TormentedRoarTimer;

        void Reset()
		{
			CrazedHungerTimer = 0.5*IN_MILLISECONDS;
			CannibalizeTimer = 2*IN_MILLISECONDS;
			KickTimer = 4*IN_MILLISECONDS;
			TormentedRoarTimer = 6*IN_MILLISECONDS;

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

			if (CrazedHungerTimer <= diff)
			{
				DoCast(me, SPELL_TD_CRAZED_HUNGER);
				CrazedHungerTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CrazedHungerTimer -= diff;

			if (CannibalizeTimer <= diff)
			{
				DoCast(me, SPELL_TD_CANNIBALIZE);
				CannibalizeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else CannibalizeTimer -= diff;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_TD_KICK);
				KickTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else KickTimer -= diff;

			if (TormentedRoarTimer <= diff)
			{
				DoCast(me, SPELL_TD_TORMENTED_ROAR);
				TormentedRoarTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else TormentedRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tormented_drakkariAI(creature);
    }
};

enum SnowfallElk
{
	SPELL_PUNCTURE = 15976,
};

class npc_snowfall_elk : public CreatureScript
{
public:
    npc_snowfall_elk() : CreatureScript("npc_snowfall_elk") { }

    struct npc_snowfall_elkAI : public QuantumBasicAI
    {
        npc_snowfall_elkAI(Creature* creature) : QuantumBasicAI (creature){}

		uint32 PunctureTimer;

        void Reset()
        {
			PunctureTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PunctureTimer <= diff)
			{
				DoCastVictim(SPELL_PUNCTURE);
				PunctureTimer = 4*IN_MILLISECONDS;
			}
			else PunctureTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowfall_elkAI(creature);
    }
};

enum ArcticGrizzly
{
	SPELL_SUMMON_ARCTIC_GRIZZLY_CUB = 47212,
	SPELL_AG_ENRAGE                 = 15716,

	NPC_ARCTIC_GRIZZLY_CUB          = 26613,
};

class npc_arctic_grizzly : public CreatureScript
{
public:
    npc_arctic_grizzly() : CreatureScript("npc_arctic_grizzly") {}

    struct npc_arctic_grizzlyAI : public QuantumBasicAI
    {
		npc_arctic_grizzlyAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		SummonList Summons;

        void Reset()
		{
			Summons.DespawnAll();

			DoCast(me, SPELL_SUMMON_ARCTIC_GRIZZLY_CUB);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_ARCTIC_GRIZZLY_CUB)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_AG_ENRAGE, 0))
				{
					DoCast(me, SPELL_AG_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arctic_grizzlyAI(creature);
    }
};

enum IronThaneFuryhammer
{
	SPELL_FURYHAMMERS_IMMUNITY = 47922,
	SPELL_EMP                  = 47911, // SPELLHIT FOR SHIELD
	SPELL_FURYHAMMER           = 61575,
};

class npc_iron_thane_furyhammer : public CreatureScript
{
public:
    npc_iron_thane_furyhammer() : CreatureScript("npc_iron_thane_furyhammer") { }

    struct npc_iron_thane_furyhammerAI : public QuantumBasicAI
    {
        npc_iron_thane_furyhammerAI(Creature* creature) : QuantumBasicAI (creature){}

		uint32 FuryhammerTimer;

        void Reset()
        {
			FuryhammerTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_FURYHAMMERS_IMMUNITY);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_EMP)
				me->RemoveAurasDueToSpell(SPELL_FURYHAMMERS_IMMUNITY);
		}

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FuryhammerTimer <= diff)
			{
				DoCast(me, SPELL_FURYHAMMER);
				FuryhammerTimer = 8*IN_MILLISECONDS;
			}
			else FuryhammerTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_thane_furyhammerAI(creature);
    }
};

enum TheAnvil
{
	SPELL_MOLTEN_BLAST = 61577,
};

class npc_the_anvil : public CreatureScript
{
public:
    npc_the_anvil() : CreatureScript("npc_the_anvil") { }

    struct npc_the_anvilAI : public QuantumBasicAI
    {
        npc_the_anvilAI(Creature* creature) : QuantumBasicAI (creature){}

		uint32 MoltenBlastTimer;

        void Reset()
        {
			MoltenBlastTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MoltenBlastTimer <= diff)
			{
				DoCastAOE(SPELL_MOLTEN_BLAST);
				MoltenBlastTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else MoltenBlastTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_anvilAI(creature);
    }
};

enum BinderMurdis
{
	SPELL_BM_FLAME_SHOCK     = 15039,
	SPELL_BM_RUNE_OF_BINDING = 48599,
};

class npc_binder_murdis : public CreatureScript
{
public:
    npc_binder_murdis() : CreatureScript("npc_binder_murdis") {}

    struct npc_binder_murdisAI : public QuantumBasicAI
    {
        npc_binder_murdisAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlameShockTimer;
		uint32 RuneOfBindingTimer;

        void Reset()
        {
			FlameShockTimer = 0.5*IN_MILLISECONDS;
			RuneOfBindingTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BM_FLAME_SHOCK);
					FlameShockTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FlameShockTimer -= diff;

			if (RuneOfBindingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BM_RUNE_OF_BINDING, true);
					RuneOfBindingTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else RuneOfBindingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_binder_murdisAI(creature);
    }
};

enum RunedOrb
{
	SPELL_ELEMENTAL_PRISON  = 43312,
	SPELL_RUNED_ORB_CHANNEL = 43546,
};

class npc_runed_orb : public CreatureScript
{
public:
    npc_runed_orb() : CreatureScript("npc_runed_orb") {}

    struct npc_runed_orbAI : public QuantumBasicAI
    {
        npc_runed_orbAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 OrbChannelTimer;

        void Reset()
        {
			OrbChannelTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (OrbChannelTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_ELEMENTAL_PRISON, true);
				DoCast(me, SPELL_RUNED_ORB_CHANNEL, true);
				OrbChannelTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else OrbChannelTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runed_orbAI(creature);
    }
};

enum IronRuneStonecaller
{
	SPELL_IRS_FIREBALL = 9053,
	SPELL_FIRE_CHANNEL = 45845,
};

class npc_iron_rune_stonecaller : public CreatureScript
{
public:
    npc_iron_rune_stonecaller() : CreatureScript("npc_iron_rune_stonecaller") {}

    struct npc_iron_rune_stonecallerAI : public QuantumBasicAI
    {
        npc_iron_rune_stonecallerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireballTimer;

        void Reset()
        {
			FireballTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_FIRE_CHANNEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustReachedHome()
		{
			Reset();
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
					DoCast(target, SPELL_IRS_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_stonecallerAI(creature);
    }
};

enum IronRuneSentinel
{
	SPELL_RUNE_OF_DETONATION = 48416,
};

class npc_iron_rune_sentinel : public CreatureScript
{
public:
    npc_iron_rune_sentinel() : CreatureScript("npc_iron_rune_sentinel") {}

    struct npc_iron_rune_sentinelAI : public QuantumBasicAI
    {
        npc_iron_rune_sentinelAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RuneOfDetonationTimer;

        void Reset()
        {
			RuneOfDetonationTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RuneOfDetonationTimer <= diff)
			{
				DoCast(me, SPELL_RUNE_OF_DETONATION);
				RuneOfDetonationTimer = 6*IN_MILLISECONDS;
			}
			else RuneOfDetonationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_sentinelAI(creature);
    }
};

enum IronRuneBinder
{
	SPELL_IR_FLAME_SHOCK     = 15039,
	SPELL_IR_RUNE_OF_BINDING = 48599,
};

class npc_iron_rune_binder : public CreatureScript
{
public:
    npc_iron_rune_binder() : CreatureScript("npc_iron_rune_binder") {}

    struct npc_iron_rune_binderAI : public QuantumBasicAI
    {
        npc_iron_rune_binderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlameShockTimer;
		uint32 RuneOfBindingTimer;

        void Reset()
        {
			FlameShockTimer = 0.5*IN_MILLISECONDS;
			RuneOfBindingTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IR_FLAME_SHOCK);
					FlameShockTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FlameShockTimer -= diff;

			if (RuneOfBindingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IR_RUNE_OF_BINDING, true);
					RuneOfBindingTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else RuneOfBindingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_binderAI(creature);
    }
};

enum IronRuneWeaver
{
	SPELL_RUNE_WEAVER_CHANNEL = 47463,
	SPELL_RUNE_WEAVING        = 52713,

	NPC_DIRECTIONAL_RUNE      = 26785,
};

class npc_iron_rune_weaver : public CreatureScript
{
public:
    npc_iron_rune_weaver() : CreatureScript("npc_iron_rune_weaver") {}

    struct npc_iron_rune_weaverAI : public QuantumBasicAI
    {
        npc_iron_rune_weaverAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RuneChannelTimer;
		uint32 RuneWeavingTimer;

        void Reset()
        {
			RuneChannelTimer = 0.1*IN_MILLISECONDS;
			RuneWeavingTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (RuneChannelTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* rune = me->FindCreatureWithDistance(NPC_DIRECTIONAL_RUNE, 25.0f))
				{
					me->SetFacingToObject(rune);
					DoCast(rune, SPELL_RUNE_WEAVER_CHANNEL, true);
					RuneChannelTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else RuneChannelTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RuneWeavingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RUNE_WEAVING);
					RuneWeavingTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else RuneWeavingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_weaverAI(creature);
    }
};

enum OverseerLochli
{
	SPELL_OL_THUNDERSTORM = 52717,
};

class npc_overseer_lochli : public CreatureScript
{
public:
    npc_overseer_lochli() : CreatureScript("npc_overseer_lochli") {}

    struct npc_overseer_lochliAI : public QuantumBasicAI
    {
        npc_overseer_lochliAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThunderstormTimer;

        void Reset()
        {
			ThunderstormTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderstormTimer <= diff)
			{
				DoCastAOE(SPELL_OL_THUNDERSTORM);
				ThunderstormTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ThunderstormTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overseer_lochliAI(creature);
    }
};

enum OverseerKorgan
{
	SPELL_OK_REVITALIZING_RUNE = 52714,
	SPELL_OK_CALL_LIGHTNING    = 32018,
};

class npc_overseer_korgan : public CreatureScript
{
public:
    npc_overseer_korgan() : CreatureScript("npc_overseer_korgan") {}

    struct npc_overseer_korganAI : public QuantumBasicAI
    {
        npc_overseer_korganAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CallLightningTimer;
		uint32 RevitalizingRuneTimer;

        void Reset()
        {
			CallLightningTimer = 0.5*IN_MILLISECONDS;
			RevitalizingRuneTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CallLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_OK_CALL_LIGHTNING);
					CallLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CallLightningTimer -= diff;

			if (RevitalizingRuneTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					if (!target->HasAura(SPELL_OK_REVITALIZING_RUNE))
					{
						DoCast(target, SPELL_OK_REVITALIZING_RUNE, true);
						RevitalizingRuneTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
			}
			else RevitalizingRuneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overseer_korganAI(creature);
    }
};

enum OverseerBrunon
{
	SPELL_OB_THUNDERSTORM = 55825,
};

class npc_overseer_brunon : public CreatureScript
{
public:
    npc_overseer_brunon() : CreatureScript("npc_overseer_brunon") {}

    struct npc_overseer_brunonAI : public QuantumBasicAI
    {
        npc_overseer_brunonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThunderstormTimer;

        void Reset()
        {
			ThunderstormTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderstormTimer <= diff)
			{
				DoCastAOE(SPELL_OB_THUNDERSTORM);
				ThunderstormTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ThunderstormTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overseer_brunonAI(creature);
    }
};

enum OverseerDurval
{
	SPELL_OD_RUNE_OF_DESTRUCTION = 52715,
};

class npc_overseer_durval : public CreatureScript
{
public:
    npc_overseer_durval() : CreatureScript("npc_overseer_durval") {}

    struct npc_overseer_durvalAI : public QuantumBasicAI
    {
        npc_overseer_durvalAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RuneOfDestructionTimer;

        void Reset()
        {
			RuneOfDestructionTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RuneOfDestructionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_OD_RUNE_OF_DESTRUCTION);
					RuneOfDestructionTimer = 6*IN_MILLISECONDS;
				}
			}
			else RuneOfDestructionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overseer_durvalAI(creature);
    }
};

enum SilvercoatStag
{
	SPELL_SC_BUTT = 59110,
};

class npc_silvercoat_stag : public CreatureScript
{
public:
    npc_silvercoat_stag() : CreatureScript("npc_silvercoat_stag") {}

    struct npc_silvercoat_stagAI : public QuantumBasicAI
    {
        npc_silvercoat_stagAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ButtTimer;

        void Reset()
        {
			ButtTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ButtTimer <= diff)
			{
				DoCastVictim(SPELL_SC_BUTT);
				ButtTimer = 4*IN_MILLISECONDS;
			}
			else ButtTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silvercoat_stagAI(creature);
    }
};

enum DrakkariOracle
{
	SPELL_DO_LIGHTNING_BOLT = 9532,
	SPELL_DO_WARPED_MIND    = 52430,
	SPELL_DO_WARPED_BODY    = 52431,
};

class npc_drakkari_oracle : public CreatureScript
{
public:
    npc_drakkari_oracle() : CreatureScript("npc_drakkari_oracle") {}

    struct npc_drakkari_oracleAI : public QuantumBasicAI
    {
        npc_drakkari_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 WarpedMindTimer;
		uint32 WarpedBodyTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			WarpedMindTimer = 3*IN_MILLISECONDS;
			WarpedBodyTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SIT_CHAIR_LOW);
        }

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
					DoCast(target, SPELL_DO_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (WarpedMindTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DO_WARPED_MIND);
					WarpedMindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else WarpedMindTimer -= diff;

			if (WarpedBodyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DO_WARPED_BODY);
					WarpedBodyTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else WarpedBodyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_drakkari_oracleAI(creature);
    }
};

enum GuardianSerpent
{
	SPELL_TAIL_LASH = 34811,
};

class npc_guardian_serpent : public CreatureScript
{
public:
    npc_guardian_serpent() : CreatureScript("npc_guardian_serpent") {}

    struct npc_guardian_serpentAI : public QuantumBasicAI
    {
        npc_guardian_serpentAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TailLashTimer;

        void Reset()
		{
			TailLashTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TailLashTimer <= diff)
			{
				DoCastVictim(SPELL_TAIL_LASH);
				TailLashTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TailLashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guardian_serpentAI(creature);
    }
};

enum Valkyrs
{
	SPELL_VG_SMITE = 71841,
	SPELL_VP_SMITE = 71842,
};

class npc_valkyr_guardian : public CreatureScript
{
public:
    npc_valkyr_guardian() : CreatureScript("npc_valkyr_guardian") {}

    struct npc_valkyr_guardianAI : public QuantumBasicAI
    {
        npc_valkyr_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SmiteTimer;

        void Reset()
		{
			SmiteTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VG_SMITE);
					SmiteTimer = urand(1.5*IN_MILLISECONDS, 2*IN_MILLISECONDS);
				}
			}
			else SmiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valkyr_guardianAI(creature);
    }
};

class npc_valkyr_protector : public CreatureScript
{
public:
    npc_valkyr_protector() : CreatureScript("npc_valkyr_protector") {}

    struct npc_valkyr_protectorAI : public QuantumBasicAI
    {
        npc_valkyr_protectorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SmiteTimer;

        void Reset()
		{
			SmiteTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VP_SMITE);
					SmiteTimer = urand(1.5*IN_MILLISECONDS, 2*IN_MILLISECONDS);
				}
			}
			else SmiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valkyr_protectorAI(creature);
    }
};

enum Infinites
{
	SPELL_CM_SHADOW_BOLT        = 9613,
	SPELL_CM_SHADOW_BLAST       = 38085,
	SPELL_ID_ENRAGE             = 8599,
	SPELL_ID_REND               = 14118,
	SPELL_ID_MORTAL_STRIKE      = 19643,
	SPELL_IA_KIDNEY_SHOT        = 32864,
	SPELL_IA_CHOP               = 43410,
};

class npc_infinite_assailant : public CreatureScript
{
public:
    npc_infinite_assailant() : CreatureScript("npc_infinite_assailant") {}

    struct npc_infinite_assailantAI : public QuantumBasicAI
    {
        npc_infinite_assailantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChopTimer;
		uint32 KidneyShotTimer;

        void Reset()
        {
			ChopTimer = 2*IN_MILLISECONDS;
			KidneyShotTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChopTimer <= diff)
			{
				DoCastVictim(SPELL_IA_CHOP);
				ChopTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ChopTimer -= diff;

			if (KidneyShotTimer <= diff)
			{
				DoCastVictim(SPELL_IA_KIDNEY_SHOT);
				KidneyShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else KidneyShotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_assailantAI(creature);
    }
};

class npc_infinite_destroyer : public CreatureScript
{
public:
    npc_infinite_destroyer() : CreatureScript("npc_infinite_destroyer") {}

    struct npc_infinite_destroyerAI : public QuantumBasicAI
    {
        npc_infinite_destroyerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 MortalStrikeTimer;

        void Reset()
        {
			RendTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 4*IN_MILLISECONDS;

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

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_ID_REND);
				RendTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_ID_MORTAL_STRIKE);
				MortalStrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_ID_ENRAGE, 0))
				{
					DoCast(me, SPELL_ID_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_destroyerAI(creature);
    }
};

class npc_infinite_chrono_magus : public CreatureScript
{
public:
    npc_infinite_chrono_magus() : CreatureScript("npc_infinite_chrono_magus") {}

    struct npc_infinite_chrono_magusAI : public QuantumBasicAI
    {
        npc_infinite_chrono_magusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltTimer;
		uint32 ShadowBlastTimer;

        void Reset()
		{
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			ShadowBlastTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_CM_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ShadowBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CM_SHADOW_BLAST);
					ShadowBlastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_chrono_magusAI(creature);
    }
};

enum InfiniteEradicator
{
	SPELL_IE_BATTLING_THROUGH_TIME = 4368,
	SPELL_IE_HASTEN                = 31458,
	SPELL_IE_WING_BUFFET           = 31475,
	SPELL_IE_TEMPORAL_VORTEX       = 52657,
};

class npc_infinite_eradicator : public CreatureScript
{
public:
    npc_infinite_eradicator() : CreatureScript("npc_infinite_eradicator") {}

    struct npc_infinite_eradicatorAI : public QuantumBasicAI
    {
        npc_infinite_eradicatorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TemporalVortexTimer;
		uint32 HastenTimer;
		uint32 WingBuffetTimer;

        void Reset()
		{
			TemporalVortexTimer = 0.5*IN_MILLISECONDS;
			HastenTimer = 3*IN_MILLISECONDS;
			WingBuffetTimer = 5*IN_MILLISECONDS;

			me->SetCorpseDelay(5);

			DoCast(me, SPELL_IE_BATTLING_THROUGH_TIME);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TemporalVortexTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IE_TEMPORAL_VORTEX, true);
					TemporalVortexTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else TemporalVortexTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCast(SPELL_IE_WING_BUFFET);
				WingBuffetTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WingBuffetTimer -= diff;

			if (HastenTimer <= diff)
			{
				DoCast(me, SPELL_IE_HASTEN);
				HastenTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else HastenTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_eradicatorAI(creature);
    }
};

enum InfiniteTimebreaker
{
	SPELL_IT_BATTLING_THROUGH_TIME = 4368,
	SPELL_IT_TIME_STOP             = 60074,
	SPELL_IT_ENRAGE                = 60075,
};

class npc_infinite_timebreaker : public CreatureScript
{
public:
    npc_infinite_timebreaker() : CreatureScript("npc_infinite_timebreaker") {}

    struct npc_infinite_timebreakerAI : public QuantumBasicAI
    {
        npc_infinite_timebreakerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TimeStopTimer;
		uint32 EnrageTimer;

        void Reset()
		{
			TimeStopTimer = 1*IN_MILLISECONDS;
			EnrageTimer = 3*IN_MILLISECONDS;

			me->SetCorpseDelay(5);

			DoCast(me, SPELL_IT_BATTLING_THROUGH_TIME);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(65.0f);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TimeStopTimer <= diff)
			{
				DoCastAOE(SPELL_IT_TIME_STOP);
				TimeStopTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else TimeStopTimer -= diff;

			if (EnrageTimer <= diff)
			{
				if (!me->HasAuraEffect(SPELL_IT_ENRAGE, 0))
				{
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
					DoCast(me, SPELL_IT_ENRAGE);
					EnrageTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_timebreakerAI(creature);
    }
};

enum TempusWyrm
{
	SPELL_TW_BATTLING_THROUGH_TIME = 4368,
	SPELL_TW_TIME_SHOCK            = 60076,
};

class npc_tempus_wyrm : public CreatureScript
{
public:
    npc_tempus_wyrm() : CreatureScript("npc_tempus_wyrm") {}

    struct npc_tempus_wyrmAI : public QuantumBasicAI
    {
        npc_tempus_wyrmAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TimeShockTimer;

        void Reset()
		{
			TimeShockTimer = 0.5*IN_MILLISECONDS;

			me->SetCorpseDelay(5);

			DoCast(me, SPELL_TW_BATTLING_THROUGH_TIME);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TimeShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TW_TIME_SHOCK);
					TimeShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else TimeShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempus_wyrmAI(creature);
    }
};

enum BronzeShrineWarden
{
	SPELL_BS_BATTLING_THROUGH_TIME = 4368,
	SPELL_BS_SAND_BREATH           = 20716,
	SPELL_BS_STOP_TIME             = 60077,
};

class npc_bronze_shrine_warden : public CreatureScript
{
public:
    npc_bronze_shrine_warden() : CreatureScript("npc_bronze_shrine_warden") {}

    struct npc_bronze_shrine_wardenAI : public QuantumBasicAI
    {
        npc_bronze_shrine_wardenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SandBreathTimer;
		uint32 StopTimeTimer;

        void Reset()
		{
			SandBreathTimer = 1*IN_MILLISECONDS;
			StopTimeTimer = 3*IN_MILLISECONDS;

			me->SetCorpseDelay(5);

			DoCast(me, SPELL_BS_BATTLING_THROUGH_TIME);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(65.0f);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SandBreathTimer <= diff)
			{
				DoCast(SPELL_BS_SAND_BREATH);
				SandBreathTimer = 4*IN_MILLISECONDS;
			}
			else SandBreathTimer -= diff;

			if (StopTimeTimer <= diff)
			{
				DoCastAOE(SPELL_BS_STOP_TIME);
				StopTimeTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else StopTimeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bronze_shrine_wardenAI(creature);
    }
};

enum Nozdormu
{
	SPELL_HOVER_ANIM_OVERRIDE = 57764,
};

class npc_nozdormu_bs : public CreatureScript
{
public:
    npc_nozdormu_bs() : CreatureScript("npc_nozdormu_bs") {}

    struct npc_nozdormu_bsAI : public QuantumBasicAI
    {
        npc_nozdormu_bsAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetCorpseDelay(5);

			DoCast(me, SPELL_HOVER_ANIM_OVERRIDE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
        return new npc_nozdormu_bsAI(creature);
    }
};

enum ScourgeDeathspeaker
{
	SPELL_FLAME_OF_THE_SEER = 52281,
	SPELL_SD_FIREBALL       = 52282,
	SPELL_FIRE_BEAM         = 49119,

	NPC_SEER_STALKER        = 27452,
};

class npc_scourge_deathspeaker : public CreatureScript
{
public:
    npc_scourge_deathspeaker() : CreatureScript("npc_scourge_deathspeaker") {}

    struct npc_scourge_deathspeakerAI : public QuantumBasicAI
    {
        npc_scourge_deathspeakerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireBeamTimer;
		uint32 FireballTimer;

        void Reset()
        {
			FireBeamTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FireBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* stalker = me->FindCreatureWithDistance(NPC_SEER_STALKER, 50.0f))
				{
					me->SetFacingToObject(stalker);
					DoCast(me, SPELL_FLAME_OF_THE_SEER, true);
					DoCast(stalker, SPELL_FIRE_BEAM, true);
					FireBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else FireBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_FLAME_OF_THE_SEER))
				DoCast(me, SPELL_FLAME_OF_THE_SEER, true);

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SD_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_deathspeakerAI(creature);
    }
};

enum DrakkariPlagueSpreader
{
	SPELL_PLAGUE_INFECTED = 52230,
};

class npc_drakkari_plague_spreader : public CreatureScript
{
public:
    npc_drakkari_plague_spreader() : CreatureScript("npc_drakkari_plague_spreader") {}

    struct npc_drakkari_plague_spreaderAI : public QuantumBasicAI
    {
        npc_drakkari_plague_spreaderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PlagueInfectedTimer;

        void Reset()
		{
			PlagueInfectedTimer = 1*IN_MILLISECONDS;

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

			if (PlagueInfectedTimer <= diff)
			{
				DoCastVictim(SPELL_PLAGUE_INFECTED);
				PlagueInfectedTimer = 5*IN_MILLISECONDS;
			}
			else PlagueInfectedTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_plague_spreaderAI(creature);
    }
};

enum DiseasedDrakkari
{
	SPELL_FEVERED_DISEASE  = 34363,
	SPELL_CANNIBALIZE_MEAT = 48141,
};

class npc_diseased_drakkari : public CreatureScript
{
public:
    npc_diseased_drakkari() : CreatureScript("npc_diseased_drakkari") {}

    struct npc_diseased_drakkariAI : public QuantumBasicAI
    {
        npc_diseased_drakkariAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FeveredDiseaseTimer;

        void Reset()
		{
			FeveredDiseaseTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_CANNIBALIZE_MEAT);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CANNIBALIZE);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
				return;

			if (FeveredDiseaseTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEVERED_DISEASE);
					FeveredDiseaseTimer = 5*IN_MILLISECONDS;
				}
			}
			else FeveredDiseaseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_diseased_drakkariAI(creature);
    }
};

enum GraymistHunter
{
	SPELL_PREDATOR_DUMMY = 44366,
	SPELL_GH_GORE        = 32019,
};

class npc_graymist_hunter : public CreatureScript
{
public:
    npc_graymist_hunter() : CreatureScript("npc_graymist_hunter") {}

    struct npc_graymist_hunterAI : public QuantumBasicAI
    {
        npc_graymist_hunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_PREDATOR_DUMMY);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GoreTimer <= diff)
			{
				DoCastVictim(SPELL_GH_GORE);
				GoreTimer = 5*IN_MILLISECONDS;
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_graymist_hunterAI(creature);
    }
};

enum EntropicOoze
{
	SPELL_MALLEABLE_OOZE = 52328,
	SPELL_CRUDE_OOZE     = 52334,
};

class npc_entropic_ooze : public CreatureScript
{
public:
    npc_entropic_ooze() : CreatureScript("npc_entropic_ooze") {}

    struct npc_entropic_oozeAI : public QuantumBasicAI
    {
        npc_entropic_oozeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MalleableOozeTimer;
		uint32 CrudeOozeTimer;

        void Reset()
        {
			MalleableOozeTimer = 0.2*IN_MILLISECONDS;
			CrudeOozeTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MalleableOozeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MALLEABLE_OOZE);
					MalleableOozeTimer = 4*IN_MILLISECONDS;
				}
			}
			else MalleableOozeTimer -= diff;

			if (CrudeOozeTimer <= diff)
			{
				DoCastVictim(SPELL_CRUDE_OOZE);
				CrudeOozeTimer = 6*IN_MILLISECONDS;
			}
			else CrudeOozeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_entropic_oozeAI(creature);
    }
};

enum SaroniteHorror
{
	SPELL_SEETHING_EVIL = 52342,
};

class npc_saronite_horror : public CreatureScript
{
public:
    npc_saronite_horror() : CreatureScript("npc_saronite_horror") {}

    struct npc_saronite_horrorAI : public QuantumBasicAI
    {
        npc_saronite_horrorAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
			// Out of Combat
			if (!me->HasAura(SPELL_SEETHING_EVIL))
				DoCast(me, SPELL_SEETHING_EVIL);

            if (!UpdateVictim())
                return;

			// In Combat
			if (!me->HasAura(SPELL_SEETHING_EVIL))
				DoCast(me, SPELL_SEETHING_EVIL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_saronite_horrorAI(creature);
    }
};

enum ImageOfDrakuru
{
	SPELL_SPIRIT_PARCTICLES_GREEN = 43167,
	SPELL_DRAKURU_QUEST_INVIS     = 47119,
	SPELL_DRAKURU_HOVER           = 57764,
};

class npc_image_of_drakuru : public CreatureScript
{
public:
    npc_image_of_drakuru() : CreatureScript("npc_image_of_drakuru") {}

    struct npc_image_of_drakuruAI : public QuantumBasicAI
    {
        npc_image_of_drakuruAI(Creature* creature) : QuantumBasicAI(creature)
		{
			Reset();
		}

        void Reset()
        {
			DoCast(me, SPELL_DRAKURU_QUEST_INVIS, true);
			DoCast(me, SPELL_SPIRIT_PARCTICLES_GREEN, true);
			DoCast(me, SPELL_DRAKURU_HOVER, true);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 65536);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
        return new npc_image_of_drakuruAI(creature);
    }
};

enum CultistInfiltrator
{
	SPELL_SUMMON_TICKING_BOMB = 54962,
};

class npc_cultist_infiltrator : public CreatureScript
{
public:
    npc_cultist_infiltrator() : CreatureScript("npc_cultist_infiltrator") {}

    struct npc_cultist_infiltratorAI : public QuantumBasicAI
    {
        npc_cultist_infiltratorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TickingTimeBombTimer;

        void Reset()
        {
			TickingTimeBombTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TickingTimeBombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SUMMON_TICKING_BOMB);
					TickingTimeBombTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else TickingTimeBombTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cultist_infiltratorAI(creature);
    }
};

enum DestroyedWarMachine
{
	SPELL_DW_FEIGN_DEATH = 29266,
};

class npc_destroyed_war_machine : public CreatureScript
{
public:
    npc_destroyed_war_machine() : CreatureScript("npc_destroyed_war_machine") {}

    struct npc_destroyed_war_machineAI : public QuantumBasicAI
    {
        npc_destroyed_war_machineAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_DW_FEIGN_DEATH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_destroyed_war_machineAI(creature);
    }
};

enum SlainAllianceSoldier
{
	SPELL_SA_FEIGN_DEATH_ROOT = 31261,
};

class npc_slain_alliance_soldier : public CreatureScript
{
public:
    npc_slain_alliance_soldier() : CreatureScript("npc_slain_alliance_soldier") {}

    struct npc_slain_alliance_soldierAI : public QuantumBasicAI
    {
        npc_slain_alliance_soldierAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SA_FEIGN_DEATH_ROOT);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slain_alliance_soldierAI(creature);
    }
};

enum TamedJormungar
{
	SPELL_JORMUNGAR_PHASE = 56526,
};

class npc_tamed_jormungar : public CreatureScript
{
public:
    npc_tamed_jormungar() : CreatureScript("npc_tamed_jormungar") {}

    struct npc_tamed_jormungarAI : public VehicleAI
    {
        npc_tamed_jormungarAI(Creature* creature) : VehicleAI(creature){}

        void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void OnCharmed(bool apply)
        {
			VehicleAI::OnCharmed(apply);
        }

		void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
		{
			if (passenger && apply)
				DoCast(me, SPELL_JORMUNGAR_PHASE, true);
			else
				me->DespawnAfterAction();
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
        return new npc_tamed_jormungarAI(creature);
    }
};

enum Stormrider
{
	SPELL_SR_FORKED_LIGHTNING = 12549,
	SPELL_SR_AGGRO_ENERGIZE   = 47693,
	SPELL_SR_STORMSTRIKE      = 51876,
};

class npc_stormrider : public CreatureScript
{
public:
    npc_stormrider() : CreatureScript("npc_stormrider") {}

    struct npc_stormriderAI : public QuantumBasicAI
    {
        npc_stormriderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ForkedLightningTimer;
		uint32 StormstrikeTimer;

        void Reset()
        {
			ForkedLightningTimer = 0.5*IN_MILLISECONDS;
			StormstrikeTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_HOVER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SR_AGGRO_ENERGIZE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ForkedLightningTimer <= diff)
			{
				DoCast(SPELL_SR_FORKED_LIGHTNING);
				ForkedLightningTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ForkedLightningTimer -= diff;

			if (StormstrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SR_STORMSTRIKE);
				StormstrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else StormstrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormriderAI(creature);
    }
};

enum SnowdriftJormungar
{
	SPELL_CORROSIVE_POISON = 50293,
};

class npc_snowdrift_jormungar : public CreatureScript
{
public:
    npc_snowdrift_jormungar() : CreatureScript("npc_snowdrift_jormungar") {}

    struct npc_snowdrift_jormungarAI : public QuantumBasicAI
    {
        npc_snowdrift_jormungarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CorrosivePoisonTimer;

        void Reset()
        {
			CorrosivePoisonTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorrosivePoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CORROSIVE_POISON);
					CorrosivePoisonTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else CorrosivePoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowdrift_jormungarAI(creature);
    }
};

enum StormforgedTaskmaster
{
	SPELL_SF_LIGHTNING_CHARGED = 52701,
	SPELL_SF_STORMSTRIKE       = 51876,
};

class npc_stormforged_taskmaster : public CreatureScript
{
public:
    npc_stormforged_taskmaster() : CreatureScript("npc_stormforged_taskmaster") {}

    struct npc_stormforged_taskmasterAI : public QuantumBasicAI
    {
        npc_stormforged_taskmasterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StormStrikeTimer;

        void Reset()
        {
			StormStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_SF_LIGHTNING_CHARGED))
				DoCast(me, SPELL_SF_LIGHTNING_CHARGED, true);

			if (StormStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SF_STORMSTRIKE);
				StormStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else StormStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_taskmasterAI(creature);
    }
};

enum StormforgedChampion
{
	SPELL_SC_LIGHTNING_CHARGED = 52701,
	SPELL_SC_STORMSTRIKE       = 51876,
};

class npc_stormforged_champion : public CreatureScript
{
public:
    npc_stormforged_champion() : CreatureScript("npc_stormforged_champion") {}

    struct npc_stormforged_championAI : public QuantumBasicAI
    {
        npc_stormforged_championAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StormStrikeTimer;

        void Reset()
        {
			StormStrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_SC_LIGHTNING_CHARGED))
				DoCast(me, SPELL_SC_LIGHTNING_CHARGED, true);

			if (StormStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SC_STORMSTRIKE);
				StormStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else StormStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_championAI(creature);
    }
};

enum
{
	SPELL_STORMFORGED_BEAM = 54430,
};

enum IronColossus
{
	SPELL_GROUND_SLAM = 61673,

	EMOTE_GROUND_SLAM = -1420119,
};

class npc_iron_colossus_sp : public CreatureScript
{
public:
    npc_iron_colossus_sp() : CreatureScript("npc_iron_colossus_sp") {}

    struct npc_iron_colossus_spAI : public QuantumBasicAI
    {
        npc_iron_colossus_spAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GroundSlamTimer;

        void Reset()
        {
			GroundSlamTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GroundSlamTimer <= diff)
			{
				DoSendQuantumText(EMOTE_GROUND_SLAM, me);
				DoCast(SPELL_GROUND_SLAM);
				GroundSlamTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else GroundSlamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_colossus_spAI(creature);
    }
};

enum HyldnirOverseer
{
	SPELL_HO_BATTLE_SHOUT = 30931,
	SPELL_HO_SNAP_KICK    = 46182,
};

class npc_hyldnir_overseer : public CreatureScript
{
public:
    npc_hyldnir_overseer() : CreatureScript("npc_hyldnir_overseer") {}

    struct npc_hyldnir_overseerAI : public QuantumBasicAI
    {
        npc_hyldnir_overseerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BattleShoutTimer;
		uint32 SnapKickTimer;

        void Reset()
        {
			BattleShoutTimer = 0.5*IN_MILLISECONDS;
			SnapKickTimer = 2*IN_MILLISECONDS;

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

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_HO_BATTLE_SHOUT);
				BattleShoutTimer = 8*IN_MILLISECONDS;
			}
			else BattleShoutTimer -= diff;

			if (SnapKickTimer <= diff)
			{
				DoCastVictim(SPELL_HO_SNAP_KICK);
				SnapKickTimer = 6*IN_MILLISECONDS;
			}
			else SnapKickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hyldnir_overseerAI(creature);
    }
};

void AddSC_northrend_npc_scripts()
{
	new npc_wyrm_reanimator();
	new npc_cultist_corrupter();
	new npc_vrykul_necrolord();
	new npc_hulking_abomination();
	new npc_malefic_necromancer();
	new npc_undying_minion();
	new npc_scourge_converter();
	new npc_scourge_banner_bearer();
	new npc_vargul_wanderer();
	new npc_scourge_soulbinder();
	new npc_pustulent_colossus();
	new npc_necrotic_webspinner();
	new npc_skeletal_archmage();
	new npc_thexal_deathchill();
	new npc_harbinger_of_horror();
	new npc_orbaz_bloodbane();
	new npc_warsong_raider();
	new npc_plagued_fiend();
	new npc_hulking_horror();
	new npc_valiance_commando();
	new npc_iceskin_sentry();
	new npc_lumbering_atrocity();
	new npc_saronite_shaper();
	new npc_skeletal_runesmith();
	new npc_umbral_brute();
	new npc_shambling_zombie();
	new npc_rokir();
	new npc_sapph();
	new npc_spiked_ghoul();
	new npc_intrepid_ghoul();
	new npc_scourgebeak_fleshripper();
	new npc_skeletal_constructor();
	new npc_master_summoner_zarod();
	new npc_cultist_shard_watcher();
	new npc_ymirjar_element_shaper();
	new npc_ymirheim_chosen_warrior();
	new npc_savage_proto_drake();
	new npc_ymirheim_spear_gun();
	new npc_ymirheim_defender();
	new npc_argent_champion_ic();
	new npc_frostbrood_destroyer();
	new npc_highlord_tirion_fordring_av();
	new npc_reanimated_captain();
	new npc_halof_the_deathbringer();
	new npc_scourge_drudge();
	new npc_frosthowl_screecher();
	new npc_hailscorn();
	new npc_urgreth_of_the_thousand_tombs();
	new npc_bythius_the_flesh_shaper();
	new npc_forgotten_depths_ambusher();
	new npc_carrion_fleshstripper();
	new npc_forgotten_depths_slayer();
	new npc_plague_drenched_ghoul();
	new npc_rampaging_ghoul();
	new npc_skeletal_craftsman();
	new npc_wrathstrike_gargoyle();
	new npc_mangal_crocolisk();
	new npc_monstrous_wight();
	new npc_longneck_grazer();
	new npc_sparktouched_warrior();
	new npc_sparktouched_oracle();
	new npc_servant_of_freya();
	new npc_empreror_cobra();
	new npc_oracle_soo_nee();
	new npc_rainspeaker_warrior();
	new npc_rainspeaker_oracle();
	new npc_frenzyheart_berserker();
	new npc_broodmother_slivina();
	new npc_mistwhisper_lightning_cloud();
	new npc_bonescythe_ravager();
	new npc_shattertusk_bull();
	new npc_perch_guardian();
	new npc_sholazar_guardian();
	new npc_overlook_sentry();
	new npc_nesingwary_game_warden();
	new npc_venomtip();
	new npc_bittertide_hydra();
	new npc_primordial_drake();
	new npc_primordial_drake_egg();
	new npc_goretalon_roc();
	new npc_goretalon_matriarch();
	new npc_thalgran_blightbringer();
	new npc_shardhorn_rhino();
	new npc_dreadsaber();
	new npc_frenzyheart_ravager();
	new npc_frenzyheart_hunter();
	new npc_serfex_the_reaver();
	new npc_alystros_the_verdant_keeper();
	new npc_emerald_skytalon();
	new npc_emerald_lasher();
	new npc_snowplain_disciple();
	new npc_snowplain_shaman();
	new npc_snowfall_glade_shaman();
	new npc_summoned_searing_totem();
	new npc_chosen_zealot();
	new npc_dragonbone_condor();
	new npc_frigid_ghoul_attacker();
	new npc_ruby_watcher();
	new npc_frigid_abomination_attacker();
	new npc_alliance_conscript();
	new npc_captain_iskandar();
	new npc_frigid_necromancer();
	new npc_dahlia_suntouch();
	new npc_ruby_guardian();
	new npc_duke_vallenhal();
	new npc_worn_bloodworm();
	new npc_frigid_ghoul();
	new npc_overlord_runthak();
	new npc_expeditionary_priest();
	new npc_iron_sentinel();
	new npc_anathemus();
	new npc_war_golem();
	new npc_earthbound_revenant();
	new npc_brittle_revenant();
	new npc_chilled_earth_elemental();
	new npc_venture_co_excavator();
	new npc_foreman_swindlegrin();
	new npc_meatpie();
	new npc_boneguard_lieutenant();
	new npc_oil_soaked_caribou();
	new npc_oiled_fledgeling();
	new npc_boneguard_commander();
	new npc_ebon_blade_vindicator();
	new npc_steam_burst();
	new npc_dragonflayer_defender();
	new npc_vargul_slayer();
	new npc_vargul_runelord();
	new npc_water_terror();
	new npc_sentry_worg();
	new npc_kulgalar_oracle();
	new npc_warsong_captain();
	new npc_warsong_marksman();
	new npc_nerubar_skitterer();
	new npc_nerubar_corpse_harvester();
	new npc_nerubar_web_lord();
	new npc_warsong_peon();
	new npc_bloodspore_moth();
	new npc_den_vermin();
	new npc_overlord_agmar(); // Bg Boss
	new npc_high_commander_halford_wyrmbane(); // Bg Boss
	new npc_wastes_taskmaster();
	new npc_emaciated_mammoth();
	new npc_emaciated_mammoth_bull();
	new npc_azure_drake();
	new npc_azure_dragon();
	new npc_ragemane();
	new npc_drakuru_prophet();
	new npc_drakuru_berserker();
	new npc_enchanted_tiki_warrior();
	new npc_yara();
	new npc_drek_maz();
	new npc_tiri();
	new npc_gundrak_savage();
	new npc_mamtoth_disciple();
	new npc_dead_mamtoth_disciple();
	new npc_blood_of_mamtoth();
	new npc_altar_warden();
	new npc_prophet_of_quetzlun();
	new npc_quetzlun_worshipper();
	new npc_high_priestess_tua_tua();
	new npc_scion_of_quetzlun();
	new npc_claw_of_harkoa();
	new npc_frostbrood_whelp();
	new npc_harkoan_subduer();
	new npc_prophet_of_harkoa();
	new npc_jin_alai_warrior();
	new npc_jin_alai_medicine_man();
	new npc_kutubesa();
	new npc_gawanil();
	new npc_chulo_the_mad();
	new npc_zim_torga_defender();
	new npc_elemental_rift();
	new npc_frozen_earth();
	new npc_prophet_of_rhunok();
	new npc_heb_drakkar_striker();
	new npc_heb_drakkar_headhunter();
	new npc_withered_argent_footman();
	new npc_hathar_broodmaster();
	new npc_burning_skimmer();
	new npc_captive_footman();
	new npc_drakkari_captive();
	new npc_hathar_skimmer();
	new npc_hathar_necromagus();
	new npc_scourged_argent_footman();
	new npc_drakkari_water_binder();
	new npc_crazed_water_spirit();
	new npc_disturbed_soul();
	new npc_carrion_eater();
	new npc_rampaging_geist();
	new npc_rotting_abomination();
	new npc_mossy_rampager();
	new npc_lurking_basilisk();
	new npc_priest_of_sseratus();
	new npc_champion_of_sseratus();
	new npc_drakkari_snake_handler();
	new npc_drakkari_snake();
	new npc_ancient_watcher();
	new npc_grove_walker();
	new npc_unbound_seer();
	new npc_lost_shandaral_spirit();
	new npc_azure_scalebane();
	new npc_azure_spellweaver();
	new npc_dappled_stag();
	new npc_sinewy_wolf();
	new npc_azure_manabeast();
	new npc_unbound_ent();
	new npc_unbound_dryad();
	new npc_unbound_ancient();
	new npc_unbound_trickster();
	new npc_dispirited_ent();
	new npc_scout_ordimbral();
	new npc_alanura_firecloud();
	new npc_scout_captain_elsia();
	new npc_marisalira();
	new npc_skymaster_baeric();
	new npc_shandaral_warrior_spirit();
	new npc_shandaral_druid_spirit();
	new npc_shandaral_hunter_spirit();
	new npc_savage_hill_brute();
	new npc_savage_hill_scavenger();
	new npc_savage_hill_mystic();
	new npc_crusader_lord_dalfors();
	new npc_argent_battle_priest();
	new npc_longhoof_grazer();
	new npc_duskhowl_prowler();
	new npc_shoveltusk();
	new npc_shoveltusk_calf();
	new npc_glacial_spirit();
	new npc_water_revenant();
	new npc_wandering_shadow();
	new npc_shadow_revenant();
	new npc_whispering_wind();
	new npc_flame_revenant();
	new npc_raging_flame();
	new npc_tempest_revenant();
	new npc_living_lasher();
	new npc_mature_lasher();
	new npc_pustulent_horror();
	new npc_citadel_watcher();
	new npc_corprethar_guardian();
	new npc_converted_hero();
	new npc_commander_dardosh();
	new npc_commander_zanneth();
	new npc_tactical_officer_ahbramis();
	new npc_primalist_mulfort();
	new npc_vengeful_taunka_spirit();
	new npc_argent_crusade_rifleman();
	new npc_sifreldar_storm_maiden();
	new npc_sifreldar_runekeeper();
	new npc_icemane_yeti();
	new npc_frigid_proto_drake();
	new npc_njorndar_proto_drake_vehicle();
	new npc_hyldsmeet_drakerider();
	new npc_captive_proto_drake_beam_bunny();
	new npc_ice_spike_target_bunny();
	new npc_icebound_revenant();
	new npc_stormforged_decimator();
	new npc_torseg_the_exiled();
	new npc_crystalweb_spitter();
	new npc_crystalweb_weaver();
	new npc_snowblind_digger();
	new npc_icetip_crawler();
	new npc_algar_the_chosen();
	new npc_servant_of_drakuru();
	new npc_blightguard();
	new npc_flying_fiend();
	new npc_loot_crazed_poacher();
	new npc_forgotten_captain();
	new npc_scalesworn_elite();
	new npc_scourging_crystal();
	new npc_necrotech();
	new npc_coprous_the_defiled();
	new npc_hebjin();
	new npc_fael_morningsong();
	new npc_enchanter_nalthanis();
	new npc_marsh_caribou();
	new npc_marsh_fawn();
	new npc_mammoth_calf();
	new npc_wooly_mammoth();
	new npc_wooly_mammoth_bull();
	new npc_gjalerbron_rune_caster();
	new npc_azure_manashaper();
	new npc_cultist_saboteur();
	new npc_unbound_corrupter();
	new npc_burning_depths_necrolyte();
	new npc_onslaught_raven_bishop();
	new npc_onslaught_darkweaver();
	new npc_anubar_cultist();
	new npc_anubar_blightbeast();
	new npc_binkie_brightgear();
	new npc_frostbrood_spawn();
	new npc_deep_jormungar();
	new npc_crypt_crawler();
	new npc_risen_crypt_lord();
	new npc_plagued_scavenger();
	new npc_reckless_scavenger();
	new npc_farshire_militia();
	new npc_gerald_green();
	new npc_cultist_necrolyte();
	new npc_magnataur_alpha();
	new npc_wastes_scavenger();
	new npc_rotting_storm_giant();
	new npc_thiassi_the_lightning_bringer();
	new npc_grand_necrolord_antiok();
	new npc_coldwind_waste_huntress();
	new npc_decrepit_necromancer();
	new npc_hulking_atrocity();
	new npc_reanimated_drakkari_tribesman();
	new npc_battered_drakkari_berserker();
	new npc_tormented_drakkari();
	new npc_snowfall_elk();
	new npc_arctic_grizzly();
	new npc_iron_thane_furyhammer();
	new npc_the_anvil();
	new npc_binder_murdis();
	new npc_runed_orb();
	new npc_iron_rune_stonecaller();
	new npc_iron_rune_sentinel();
	new npc_iron_rune_binder();
	new npc_iron_rune_weaver();
	new npc_overseer_lochli();
	new npc_overseer_korgan();
	new npc_overseer_brunon();
	new npc_overseer_durval();
	new npc_silvercoat_stag();
	new npc_drakkari_oracle();
	new npc_guardian_serpent();
	new npc_valkyr_guardian();
	new npc_valkyr_protector();
	new npc_infinite_assailant();
	new npc_infinite_destroyer();
	new npc_infinite_chrono_magus();
	new npc_infinite_eradicator();
	new npc_infinite_timebreaker();
	new npc_tempus_wyrm();
	new npc_bronze_shrine_warden();
	new npc_nozdormu_bs();
	new npc_scourge_deathspeaker();
	new npc_drakkari_plague_spreader();
	new npc_diseased_drakkari();
	new npc_graymist_hunter();
	new npc_entropic_ooze();
	new npc_saronite_horror();
	new npc_image_of_drakuru();
	new npc_cultist_infiltrator();
	new npc_destroyed_war_machine();
	new npc_slain_alliance_soldier();
	new npc_tamed_jormungar();
	new npc_stormrider();
	new npc_snowdrift_jormungar();
	new npc_stormforged_taskmaster();
	new npc_stormforged_champion();
	new npc_iron_colossus_sp();
	new npc_hyldnir_overseer();
}