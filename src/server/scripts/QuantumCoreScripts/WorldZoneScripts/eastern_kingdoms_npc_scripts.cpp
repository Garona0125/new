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

enum GlobalCreatueSpells
{
	SPELL_DUAL_WIELD = 674,
};

enum Sounds
{
	SOUND_WOLF_HOWL  = 1018,
	SOUND_WOLF_AGGRO = 1108,
};

enum BeastMaster
{
	SPELL_BM_SHOOT           = 6660,
	SPELL_BM_HEADCRACK       = 3148,
	SPELL_SKULLSPLITTER_PET  = 3621,

	NPC_SKULLSPLITTER_PANTER = 756,
};

class npc_skullsplitter_beastmaster : public CreatureScript
{
public:
    npc_skullsplitter_beastmaster() : CreatureScript("npc_skullsplitter_beastmaster") {}

    struct npc_skullsplitter_beastmasterAI : public QuantumBasicAI
    {
		npc_skullsplitter_beastmasterAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShootTimer;
		uint32 HeadCrackTimer;

		SummonList Summons;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			HeadCrackTimer = 2*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SKULLSPLITTER_PET);
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
			if (summon->GetEntry() == NPC_SKULLSPLITTER_PANTER)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
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
						DoCast(target, SPELL_BM_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (HeadCrackTimer <= diff)
				{
					DoCastVictim(SPELL_BM_HEADCRACK);
					HeadCrackTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
				else HeadCrackTimer -= diff;

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
						DoCast(target, SPELL_BM_SHOOT);
						ShootTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skullsplitter_beastmasterAI(creature);
    }
};

enum SkullsplitterScout
{
	SPELL_SS_HEAD_CRACK = 3148,
};

class npc_skullsplitter_scout : public CreatureScript
{
public:
    npc_skullsplitter_scout() : CreatureScript("npc_skullsplitter_scout") {}

    struct npc_skullsplitter_scoutAI : public QuantumBasicAI
    {
        npc_skullsplitter_scoutAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HeadCrackTimer;

        void Reset()
        {
			HeadCrackTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeadCrackTimer <= diff)
			{
				DoCastVictim(SPELL_SS_HEAD_CRACK);
				HeadCrackTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HeadCrackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skullsplitter_scoutAI(creature);
    }
};

enum VentureCoTinkerer
{
	SPELL_COMPACT_HARVEST_REAPER = 7979,
	SPELL_FROST_ARMOR            = 12544,
	SPELL_FIREBALL               = 20823,

	NPC_COMPACT_GOLEM            = 2676,
};

class npc_venture_co_tinkerer : public CreatureScript
{
public:
    npc_venture_co_tinkerer() : CreatureScript("npc_venture_co_tinkerer") {}

    struct npc_venture_co_tinkererAI : public QuantumBasicAI
    {
		npc_venture_co_tinkererAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 FrostArmorTimer;
		uint32 FireballTimer;

		SummonList Summons;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_COMPACT_HARVEST_REAPER);
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_COMPACT_GOLEM)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
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

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venture_co_tinkererAI(creature);
    }
};

enum VentureCoForeman
{
	SPELL_DEVOTION_AURA = 8258,
};

class npc_venture_co_foreman : public CreatureScript
{
public:
    npc_venture_co_foreman() : CreatureScript("npc_venture_co_foreman") {}

    struct npc_venture_co_foremanAI : public QuantumBasicAI
    {
        npc_venture_co_foremanAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_DEVOTION_AURA);
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
        return new npc_venture_co_foremanAI(creature);
    }
};

enum VentureCoStripMiner
{
	SPELL_DYNAMITE = 8800,
};

class npc_venture_co_strip_miner : public CreatureScript
{
public:
    npc_venture_co_strip_miner() : CreatureScript("npc_venture_co_strip_miner") {}

    struct npc_venture_co_strip_minerAI : public QuantumBasicAI
    {
        npc_venture_co_strip_minerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DynamiteTimer;

        void Reset()
        {
			DynamiteTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DynamiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 40)
					{
						DoCast(target, SPELL_DYNAMITE);
						DynamiteTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else DynamiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venture_co_strip_minerAI(creature);
    }
};

enum BoulderfistMagus
{
	SPELL_BLOODLUST         = 6742,
	SPELL_FIREBLAST         = 20795,
	SPELL_FROSTBOLT         = 20822,
	SPELL_FROST_NOVA        = 46555,

	SAY_BOULDERFIST_AGGRO_1 = -1562004,
	SAY_BOULDERFIST_AGGRO_2 = -1562005,
};

class npc_boulderfist_magus : public CreatureScript
{
public:
    npc_boulderfist_magus() : CreatureScript("npc_boulderfist_magus") {}

    struct npc_boulderfist_magusAI : public QuantumBasicAI
    {
        npc_boulderfist_magusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireblastTimer;
		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FireblastTimer = 0.5*IN_MILLISECONDS;
			FrostboltTimer = 1500;
			FrostNovaTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_BLOODLUST);

			DoSendQuantumText(RAND(SAY_BOULDERFIST_AGGRO_1, SAY_BOULDERFIST_AGGRO_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireblastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBLAST);
					FireblastTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else FireblastTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

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
        return new npc_boulderfist_magusAI(creature);
    }
};

enum BoulderfistShaman
{
	SPELL_SEARING_TOTEM  = 6364,
	SPELL_LIGHTNING_BOLT = 9532,
	SPELL_HEALING_WAVE   = 11986,
};

class npc_boulderfist_shaman : public CreatureScript
{
public:
    npc_boulderfist_shaman() : CreatureScript("npc_boulderfist_shaman") {}

    struct npc_boulderfist_shamanAI : public QuantumBasicAI
    {
        npc_boulderfist_shamanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 HealingWaveTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			HealingWaveTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SEARING_TOTEM, true);
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
					DoCast(target, SPELL_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

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
        return new npc_boulderfist_shamanAI(creature);
    }
};

enum ZanzilWitchDoctor
{
	SPELL_SHADOW_BOLT     = 9613,
	SPELL_SCORCHING_TOTEM = 15038,
	NPC_SCORCHING_TOTEM   = 9637,
};

class npc_zanzil_witch_doctor : public CreatureScript
{
public:
    npc_zanzil_witch_doctor() : CreatureScript("npc_zanzil_witch_doctor") {}

    struct npc_zanzil_witch_doctorAI : public QuantumBasicAI
    {
        npc_zanzil_witch_doctorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

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
			DoCast(me, SPELL_SCORCHING_TOTEM);
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SCORCHING_TOTEM)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
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
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zanzil_witch_doctorAI(creature);
    }
};

enum ZanzilZombie
{
	SPELL_CONTAGION_OF_ROT = 7102,
};

class npc_zanzil_zombie : public CreatureScript
{
public:
    npc_zanzil_zombie() : CreatureScript("npc_zanzil_zombie") {}

    struct npc_zanzil_zombieAI : public QuantumBasicAI
    {
        npc_zanzil_zombieAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ContagionOfRotTimer;

        void Reset()
        {
			ContagionOfRotTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ContagionOfRotTimer <= diff)
			{
				DoCastVictim(SPELL_CONTAGION_OF_ROT);
				ContagionOfRotTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else ContagionOfRotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zanzil_zombieAI(creature);
    }
};

enum ZanzilHunter
{
	SPELL_THROW = 10277,
};

class npc_zanzil_hunter : public CreatureScript
{
public:
    npc_zanzil_hunter() : CreatureScript("npc_zanzil_hunter") {}

    struct npc_zanzil_hunterAI : public QuantumBasicAI
    {
        npc_zanzil_hunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_THROW);
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
					DoCast(target, SPELL_THROW);
					ThrowTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zanzil_hunterAI(creature);
    }
};

enum ZanzilNaga
{
	SPELL_HAMSTRING = 9080,
	SPELL_PUMMEL    = 12555,
};

class npc_zanzil_naga : public CreatureScript
{
public:
    npc_zanzil_naga() : CreatureScript("npc_zanzil_naga") {}

    struct npc_zanzil_nagaAI : public QuantumBasicAI
    {
        npc_zanzil_nagaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HamstringTimer;
		uint32 PummelTimer;

        void Reset()
        {
			HamstringTimer = 1*IN_MILLISECONDS;
			PummelTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
				HamstringTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			if (PummelTimer <= diff)
			{
				DoCastVictim(SPELL_PUMMEL);
				PummelTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PummelTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zanzil_nagaAI(creature);
    }
};

enum Dragonhawks
{
	SPELL_FEATHER_BURST = 29117,
};

class npc_crazed_dragonhawk : public CreatureScript
{
public:
    npc_crazed_dragonhawk() : CreatureScript("npc_crazed_dragonhawk") {}

    struct npc_crazed_dragonhawkAI : public QuantumBasicAI
    {
        npc_crazed_dragonhawkAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FeatherBurstTimer;

        void Reset()
        {
			FeatherBurstTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FeatherBurstTimer <= diff)
			{
				DoCastVictim(SPELL_FEATHER_BURST);
				FeatherBurstTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FeatherBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crazed_dragonhawkAI(creature);
    }
};

class npc_feral_dragonhawk_hatchling : public CreatureScript
{
public:
    npc_feral_dragonhawk_hatchling() : CreatureScript("npc_feral_dragonhawk_hatchling") {}

    struct npc_feral_dragonhawk_hatchlingAI : public QuantumBasicAI
    {
        npc_feral_dragonhawk_hatchlingAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FeatherBurstTimer;

        void Reset()
        {
			FeatherBurstTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FeatherBurstTimer <= diff)
			{
				DoCastVictim(SPELL_FEATHER_BURST);
				FeatherBurstTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FeatherBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_feral_dragonhawk_hatchlingAI(creature);
    }
};

enum DarnassianScout
{
	SPELL_PROWL = 8152,
};

class npc_darnassian_scout : public CreatureScript
{
public:
    npc_darnassian_scout() : CreatureScript("npc_darnassian_scout") {}

    struct npc_darnassian_scoutAI : public QuantumBasicAI
    {
        npc_darnassian_scoutAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_PROWL, true);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_PROWL);
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
        return new npc_darnassian_scoutAI(creature);
    }
};

enum Springpaws
{
	SPELL_SS_PROWL = 8152,
};

class npc_springpaw_stalker : public CreatureScript
{
public:
    npc_springpaw_stalker() : CreatureScript("npc_springpaw_stalker") {}

    struct npc_springpaw_stalkerAI : public QuantumBasicAI
    {
        npc_springpaw_stalkerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SS_PROWL, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_SS_PROWL);
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
        return new npc_springpaw_stalkerAI(creature);
    }
};

class npc_elder_springpaw : public CreatureScript
{
public:
    npc_elder_springpaw() : CreatureScript("npc_elder_springpaw") {}

    struct npc_elder_springpawAI : public QuantumBasicAI
    {
        npc_elder_springpawAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SS_PROWL, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_SS_PROWL);
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
        return new npc_elder_springpawAI(creature);
    }
};

enum SilvermoonApprentice
{
	SPELL_SA_FIREBALL = 20811,
};

class npc_silvermoon_apprentice : public CreatureScript
{
public:
    npc_silvermoon_apprentice() : CreatureScript("npc_silvermoon_apprentice") {}

    struct npc_silvermoon_apprenticeAI : public QuantumBasicAI
    {
        npc_silvermoon_apprenticeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireballTimer;

        void Reset()
        {
			FireballTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				DoCastVictim(SPELL_SA_FIREBALL);
				FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silvermoon_apprenticeAI(creature);
    }
};

enum PlaguebonePillager
{
	SPELL_STRIKE = 11976,
};

class npc_plaguebone_pillager : public CreatureScript
{
public:
    npc_plaguebone_pillager() : CreatureScript("npc_plaguebone_pillager") {}

    struct npc_plaguebone_pillagerAI : public QuantumBasicAI
    {
        npc_plaguebone_pillagerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plaguebone_pillagerAI(creature);
    }
};

enum RotlimbCannibal
{
	SPELL_BLACK_ROT = 16448,
};

class npc_rotlimb_cannibal : public CreatureScript
{
public:
    npc_rotlimb_cannibal() : CreatureScript("npc_rotlimb_cannibal") {}

    struct npc_rotlimb_cannibalAI : public QuantumBasicAI
    {
        npc_rotlimb_cannibalAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BlackRotTimer;

        void Reset()
        {
			BlackRotTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BlackRotTimer <= diff)
			{
				DoCastVictim(SPELL_BLACK_ROT);
				BlackRotTimer = 4*IN_MILLISECONDS;
			}
			else BlackRotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rotlimb_cannibalAI(creature);
    }
};

enum Wretcheds
{
	SPELL_SINISTER_STRIKE   = 14873,
	SPELL_BITTER_WITHDRAWAL = 29098,
	SPELL_CRIPPLING_POISON  = 25809,

	SAY_WRETCHEDS_AGGRO_1   = -1156302,
	SAY_WRETCHEDS_AGGRO_2   = -1156303,
	SAY_WRETCHEDS_AGGRO_3   = -1156304,
};

class npc_wretched_thug : public CreatureScript
{
public:
    npc_wretched_thug() : CreatureScript("npc_wretched_thug") {}

    struct npc_wretched_thugAI : public QuantumBasicAI
    {
        npc_wretched_thugAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BitterWithdrawalTimer;

        void Reset()
        {
			BitterWithdrawalTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_WRETCHEDS_AGGRO_1, SAY_WRETCHEDS_AGGRO_2, SAY_WRETCHEDS_AGGRO_3), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BitterWithdrawalTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BITTER_WITHDRAWAL);
					BitterWithdrawalTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BitterWithdrawalTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_thugAI(creature);
    }
};

class npc_wretched_urchin : public CreatureScript
{
public:
    npc_wretched_urchin() : CreatureScript("npc_wretched_urchin") {}

    struct npc_wretched_urchinAI : public QuantumBasicAI
    {
        npc_wretched_urchinAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CripplingPoisonTimer;

        void Reset()
        {
			CripplingPoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_WRETCHEDS_AGGRO_1, SAY_WRETCHEDS_AGGRO_2, SAY_WRETCHEDS_AGGRO_3), me);
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
				CripplingPoisonTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CripplingPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_urchinAI(creature);
    }
};

class npc_wretched_hooligan : public CreatureScript
{
public:
    npc_wretched_hooligan() : CreatureScript("npc_wretched_hooligan") {}

    struct npc_wretched_hooliganAI : public QuantumBasicAI
    {
        npc_wretched_hooliganAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BitterWithdrawalTimer;
		uint32 SinisterStrikeTimer;

        void Reset()
        {
			BitterWithdrawalTimer = 0.5*IN_MILLISECONDS;
			SinisterStrikeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_WRETCHEDS_AGGRO_1, SAY_WRETCHEDS_AGGRO_2, SAY_WRETCHEDS_AGGRO_3), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BitterWithdrawalTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BITTER_WITHDRAWAL);
					BitterWithdrawalTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BitterWithdrawalTimer -= diff;

			if (SinisterStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SINISTER_STRIKE);
				SinisterStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SinisterStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_hooliganAI(creature);
    }
};

enum ManaStalker
{
	SPELL_MOONFIRE = 15798,
};

class npc_mana_stalker : public CreatureScript
{
public:
    npc_mana_stalker() : CreatureScript("npc_mana_stalker") {}

    struct npc_mana_stalkerAI : public QuantumBasicAI
    {
        npc_mana_stalkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MoonfireTimer;

        void Reset()
        {
			MoonfireTimer = 0.5*IN_MILLISECONDS;

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
					MoonfireTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MoonfireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_stalkerAI(creature);
    }
};

enum Manawraith
{
	SPELL_FAERIE_FIRE    = 25602,
	SPELL_ARCANE_RESIDUE = 29109,
};

class npc_manawraith : public CreatureScript
{
public:
    npc_manawraith() : CreatureScript("npc_manawraith") {}

    struct npc_manawraithAI : public QuantumBasicAI
    {
        npc_manawraithAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FaerieFireTimer;

        void Reset()
        {
			FaerieFireTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			killer->CastSpell(killer, SPELL_ARCANE_RESIDUE, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FaerieFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FAERIE_FIRE);
					FaerieFireTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FaerieFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_manawraithAI(creature);
    }
};

enum LeyKeeperVelania
{
	SPELL_LK_FIREBALL = 20811,
};

class npc_ley_keeper_velania : public CreatureScript
{
public:
    npc_ley_keeper_velania() : CreatureScript("npc_ley_keeper_velania") {}

    struct npc_ley_keeper_velaniaAI : public QuantumBasicAI
    {
        npc_ley_keeper_velaniaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireballTimer;

        void Reset()
        {
			FireballTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_LK_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ley_keeper_velaniaAI(creature);
    }
};

enum RotlimbMarauder
{
	SPELL_DISEASE_TOUCH = 3234,
};

class npc_rotlimb_marauder : public CreatureScript
{
public:
    npc_rotlimb_marauder() : CreatureScript("npc_rotlimb_marauder") {}

    struct npc_rotlimb_marauderAI : public QuantumBasicAI
    {
        npc_rotlimb_marauderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DiseaseTouchTimer;

        void Reset()
        {
			DiseaseTouchTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DiseaseTouchTimer <= diff)
			{
				DoCastVictim(SPELL_DISEASE_TOUCH);
				DiseaseTouchTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else DiseaseTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rotlimb_marauderAI(creature);
    }
};

enum Darkwraith
{
	SPELL_ENRAGE = 8599,
};

class npc_darkwraith : public CreatureScript
{
public:
    npc_darkwraith() : CreatureScript("npc_darkwraith") {}

    struct npc_darkwraithAI : public QuantumBasicAI
    {
        npc_darkwraithAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

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
        return new npc_darkwraithAI(creature);
    }
};

class npc_asngershade : public CreatureScript
{
public:
    npc_asngershade() : CreatureScript("npc_asngershade") {}

    struct npc_asngershadeAI : public QuantumBasicAI
    {
        npc_asngershadeAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

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
        return new npc_asngershadeAI(creature);
    }
};

enum Gorlash
{
	SPELL_TRAMPLE = 5568,
};

class npc_gorlash : public CreatureScript
{
public:
    npc_gorlash() : CreatureScript("npc_gorlash") {}

    struct npc_gorlashAI : public QuantumBasicAI
    {
        npc_gorlashAI(Creature* creature) : QuantumBasicAI(creature){}

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
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = 6*IN_MILLISECONDS;
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gorlashAI(creature);
    }
};

enum SiouxsieTheBansheeEvent
{
	EVENT_EDWARDS_INTRO      = 1,
	EVENT_SIOUXSIE_OUTRO     = 2,
	EVENT_EDWARDS_OUTRO      = 3,

	NPC_SIOUXSIE_THE_BANSHEE = 27928,

	SAY_EDWARDS_DIALOG_1     = -1420007,
	SAY_EDWARDS_DIALOG_2     = -1420008,
	SAY_EDWARDS_DIALOG_3     = -1420009,
	SAY_EDWARDS_DIALOG_4     = -1420010,
	SAY_EDWARDS_END_DIALOG   = -1420011,

	SAY_SIOUXSIE_DIALOG_1    = -1420012,
	SAY_SIOUXSIE_DIALOG_2    = -1420013,
	SAY_SIOUXSIE_DIALOG_3    = -1420014,
	SAY_SIOUXSIE_DIALOG_4    = -1420015,
};

class npc_squire_edwards_eh : public CreatureScript
{
public:
	npc_squire_edwards_eh() : CreatureScript("npc_squire_edwards_eh") {}

	struct npc_squire_edwards_ehAI : public QuantumBasicAI
	{
		npc_squire_edwards_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.ScheduleEvent(EVENT_EDWARDS_INTRO, 3*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_EDWARDS_INTRO:
						DoSendQuantumText(RAND(SAY_EDWARDS_DIALOG_1, SAY_EDWARDS_DIALOG_2, SAY_EDWARDS_DIALOG_3, SAY_EDWARDS_DIALOG_4), me);
						events.ScheduleEvent(EVENT_SIOUXSIE_OUTRO, 2*IN_MILLISECONDS);
						break;
					case EVENT_SIOUXSIE_OUTRO:
						if (Creature* Siouxsie = me->FindCreatureWithDistance(NPC_SIOUXSIE_THE_BANSHEE, 55.0f))
						{
							DoSendQuantumText(RAND(SAY_SIOUXSIE_DIALOG_1, SAY_SIOUXSIE_DIALOG_2, SAY_SIOUXSIE_DIALOG_3, SAY_SIOUXSIE_DIALOG_4), Siouxsie);
						}
						events.ScheduleEvent(EVENT_EDWARDS_OUTRO, 2*IN_MILLISECONDS);
						break;
					case EVENT_EDWARDS_OUTRO:
						DoSendQuantumText(SAY_EDWARDS_END_DIALOG, me);
						events.ScheduleEvent(EVENT_EDWARDS_INTRO, 3*MINUTE*IN_MILLISECONDS);
						break;
				}
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_squire_edwards_ehAI(creature);
	}
};

enum Amalthazad
{
	NPC_DISCIPLE_OF_FROST        = 28490,
	NPC_FRIGID_BONES             = 29210,

	FACTION_FOR_COMBAT           = 189,

	SPELL_DF_ICY_TOUCH           = 45477,
	SPELL_DF_MIND_FREEZE         = 47528,
	SPELL_DF_HOWLING_BLAST       = 49184,
	SPELL_DF_HUNGERING_COLD      = 51209,
	SPELL_START_LESSON           = 52582,

	// Lesson 1
	EVENT_AMAL_THAZAD_LESSON_1_1 = 1,
	EVENT_AMAL_THAZAD_LESSON_1_2 = 2,
	EVENT_AMAL_THAZAD_LESSON_1_3 = 3,
	EVENT_AMAL_THAZAD_LESSON_1_4 = 4,
	EVENT_AMAL_THAZAD_LESSON_1_5 = 5,
	// Lesson 2

	SAY_AMAL_THAZAD_LESSON_1_1   = -1420019, // You are not overwhelmed yet disciple...let the hungering cold of death halt this onslaught.
	SAY_AMAL_THAZAD_LESSON_1_2   = -1420020, // Frozen solid, ripe for slaughter. Let loose a howling blast and shatter them to pieces, disciple.
	SAY_AMAL_THAZAD_LESSON_1_3   = -1420021, // You have more learning ahead of you, disciple, return to your studies.
};

Position const DisciplieCombatPos[1] =
{
	{2419.37f, -5526.9f, 377.046f, 3.7233f},
};

class npc_amal_thazad_eh : public CreatureScript
{
public:
	npc_amal_thazad_eh() : CreatureScript("npc_amal_thazad_eh") { }

    struct npc_amal_thazad_ehAI : public QuantumBasicAI
    {
        npc_amal_thazad_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.ScheduleEvent(EVENT_AMAL_THAZAD_LESSON_1_1, 5*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_AMAL_THAZAD_LESSON_1_1:
						DoSendQuantumText(SAY_AMAL_THAZAD_LESSON_1_1, me);
						DoCast(me, SPELL_START_LESSON, true);
						me->SummonCreature(NPC_FRIGID_BONES, 2413.38f, -5531.86f, 377.027f, 0.5504f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS);

						if (Creature* disciplie = me->FindCreatureWithDistance(NPC_DISCIPLE_OF_FROST, 55.0f, true))
						{
							disciplie->SetWalk(true);
							disciplie->SetSpeed(MOVE_WALK, 1.0f, true);
							disciplie->GetMotionMaster()->MovePoint(0, DisciplieCombatPos[0]);
						}

						events.ScheduleEvent(EVENT_AMAL_THAZAD_LESSON_1_2, 7*IN_MILLISECONDS);
						break;
					case EVENT_AMAL_THAZAD_LESSON_1_2:
						DoCast(me, SPELL_START_LESSON, true);
						if (Creature* disciplie = me->FindCreatureWithDistance(NPC_DISCIPLE_OF_FROST, 5.0f, true))
						{
							Creature* frigid = disciplie->FindCreatureWithDistance(NPC_FRIGID_BONES, 10.0f, true);
							disciplie->SetFacingToObject(frigid);
						}
						events.ScheduleEvent(EVENT_AMAL_THAZAD_LESSON_1_3, 5*IN_MILLISECONDS);
						break;
					case EVENT_AMAL_THAZAD_LESSON_1_3:
						DoCast(me, SPELL_START_LESSON, true);
						if (Creature* disciplie = me->FindCreatureWithDistance(NPC_DISCIPLE_OF_FROST, 5.0f, true))
						{
							Creature* frigid = disciplie->FindCreatureWithDistance(NPC_FRIGID_BONES, 10.0f, true);
							frigid->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_UNARMED);
							frigid->SetCurrentFaction(FACTION_FOR_COMBAT);
							frigid->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
							frigid->AI()->AttackStart(disciplie);

							disciplie->AI()->AttackStart(frigid);

						}
						events.ScheduleEvent(EVENT_AMAL_THAZAD_LESSON_1_4, 25*IN_MILLISECONDS);
						break;
					case EVENT_AMAL_THAZAD_LESSON_1_4:
						DoCast(me, SPELL_START_LESSON, true);
						DoSendQuantumText(SAY_AMAL_THAZAD_LESSON_1_2, me);
						events.ScheduleEvent(EVENT_AMAL_THAZAD_LESSON_1_5, 7*IN_MILLISECONDS);
						break;
					case EVENT_AMAL_THAZAD_LESSON_1_5:
						DoCast(me, SPELL_START_LESSON, true);
						DoSendQuantumText(SAY_AMAL_THAZAD_LESSON_1_3, me);

						if (Creature* frigid = me->FindCreatureWithDistance(NPC_FRIGID_BONES, 35.0f, true))
							frigid->Kill(frigid);

						if (Creature* disciplie = me->FindCreatureWithDistance(NPC_DISCIPLE_OF_FROST, 5.0f, true))
							disciplie->AI()->EnterEvadeMode();

						events.ScheduleEvent(EVENT_AMAL_THAZAD_LESSON_1_1, 1*MINUTE *IN_MILLISECONDS);
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amal_thazad_ehAI(creature);
    }
};

class npc_disciple_of_frost_eh : public CreatureScript
{
public:
    npc_disciple_of_frost_eh() : CreatureScript("npc_disciple_of_frost_eh") {}

    struct npc_disciple_of_frost_ehAI : public QuantumBasicAI
    {
        npc_disciple_of_frost_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HowlingBlastTimer;
		uint32 IcyTouchTimer;
		uint32 HungeringColdTimer;
		uint32 MindFreezeTimer;

        void Reset()
        {
			HowlingBlastTimer = 1*IN_MILLISECONDS;
			HungeringColdTimer = 3*IN_MILLISECONDS;
			IcyTouchTimer = 4*IN_MILLISECONDS;
			MindFreezeTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_DF_MIND_FREEZE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HowlingBlastTimer <= diff)
			{
				DoCastVictim(SPELL_DF_HOWLING_BLAST);
				HowlingBlastTimer = 6*IN_MILLISECONDS;
			}
			else HowlingBlastTimer -= diff;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_DF_ICY_TOUCH);
				IcyTouchTimer = 8*IN_MILLISECONDS;
			}
			else IcyTouchTimer -= diff;

			if (HungeringColdTimer <= diff)
			{
				DoCastVictim(SPELL_DF_HUNGERING_COLD);
				HungeringColdTimer = 10*IN_MILLISECONDS;
			}
			else HungeringColdTimer -= diff;

			if (MindFreezeTimer <= diff)
			{
				DoCastVictim(SPELL_DF_MIND_FREEZE);
				MindFreezeTimer = 12*IN_MILLISECONDS;
			}
			else MindFreezeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_disciple_of_frost_ehAI(creature);
    }
};

enum ShatteredSunMarksman
{
	SPELL_SM_SHOOT     = 42580,

	NPC_MARKSMAN_BUNNY = 25192,
};

class npc_shattered_sun_marksman : public CreatureScript
{
public:
    npc_shattered_sun_marksman() : CreatureScript("npc_shattered_sun_marksman") {}

    struct npc_shattered_sun_marksmanAI : public QuantumBasicAI
    {
        npc_shattered_sun_marksmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ShootTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* bunny = me->FindCreatureWithDistance(NPC_MARKSMAN_BUNNY, 100.0f))
				{
					DoCast(bunny, SPELL_SM_SHOOT);
					ShootTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_sun_marksmanAI(creature);
    }
};

enum DrywhiskerSurveyor
{
	SPELL_DS_FROST_ARMOR = 12544,
	SPELL_DS_FROSTBOLT   = 20822,
	SPELL_DS_FROST_NOVA  = 1225,
};

class npc_drywhisker_surveyor : public CreatureScript
{
public:
    npc_drywhisker_surveyor() : CreatureScript("npc_drywhisker_surveyor") {}

    struct npc_drywhisker_surveyorAI : public QuantumBasicAI
    {
        npc_drywhisker_surveyorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FrostArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_DS_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_DS_FROST_ARMOR))
				DoCast(me, SPELL_DS_FROST_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DS_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_DS_FROST_NOVA);
				FrostNovaTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drywhisker_surveyorAI(creature);
    }
};

enum DarkIronShadowcaster
{
	SPELL_DS_IMMOLATE    = 2941,
	SPELL_DS_SHADOW_BOLT = 20816,
};

class npc_dark_iron_shadowcaster : public CreatureScript
{
public:
    npc_dark_iron_shadowcaster() : CreatureScript("npc_dark_iron_shadowcaster") {}

    struct npc_dark_iron_shadowcasterAI : public QuantumBasicAI
    {
        npc_dark_iron_shadowcasterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ImmolateTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			ImmolateTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 1500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_DS_IMMOLATE);
					ImmolateTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ImmolateTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DS_SHADOW_BOLT);
					ShadowBoltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_iron_shadowcasterAI(creature);
    }
};

enum SyndicatePathstalker
{
	SPELL_SP_SHOOT = 6660,
};

class npc_syndicate_pathstalker : public CreatureScript
{
public:
    npc_syndicate_pathstalker() : CreatureScript("npc_syndicate_pathstalker") {}

    struct npc_syndicate_pathstalkerAI : public QuantumBasicAI
    {
        npc_syndicate_pathstalkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

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
						DoCast(target, SPELL_SP_SHOOT);
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
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
						{
							DoCast(target, SPELL_SP_SHOOT);
							ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
						}
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syndicate_pathstalkerAI(creature);
    }
};

class npc_coldridge_mountaineer : public CreatureScript
{
public:
    npc_coldridge_mountaineer() : CreatureScript("npc_coldridge_mountaineer") {}

    struct npc_coldridge_mountaineerAI : public QuantumBasicAI
    {
        npc_coldridge_mountaineerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
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
						DoCast(target, SPELL_SP_SHOOT);
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
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
						{
							DoCast(target, SPELL_SP_SHOOT);
							ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
						}
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coldridge_mountaineerAI(creature);
    }
};

enum SurenaCaledon
{
	SPELL_SC_FROST_ARMOR = 12544,
	SPELL_SC_FIREBALL    = 20793,
};

class npc_surena_caledon : public CreatureScript
{
public:
    npc_surena_caledon() : CreatureScript("npc_surena_caledon") {}

    struct npc_surena_caledonAI : public QuantumBasicAI
    {
        npc_surena_caledonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FireballTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FrostArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_SC_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_SC_FROST_ARMOR))
				DoCast(me, SPELL_SC_FROST_ARMOR, true);

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SC_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_surena_caledonAI(creature);
    }
};

enum HighvaleOutrunner
{
	SPELL_HO_SHOOT = 6660,
};

class npc_highvale_outrunner : public CreatureScript
{
public:
    npc_highvale_outrunner() : CreatureScript("npc_highvale_outrunner") {}

    struct npc_highvale_outrunnerAI : public QuantumBasicAI
    {
		npc_highvale_outrunnerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

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
						DoCast(target, SPELL_HO_SHOOT);
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
						DoCast(target, SPELL_HO_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highvale_outrunnerAI(creature);
    }
};

enum HighvaleScout
{
	SPELL_STRONG_CLEAVE = 8255,
};

class npc_highvale_scout : public CreatureScript
{
public:
    npc_highvale_scout() : CreatureScript("npc_highvale_scout") {}

    struct npc_highvale_scoutAI : public QuantumBasicAI
    {
        npc_highvale_scoutAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrongCleaveTimer;

        void Reset()
        {
			StrongCleaveTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrongCleaveTimer <= diff)
			{
				DoCastVictim(SPELL_STRONG_CLEAVE);
				StrongCleaveTimer = 4*IN_MILLISECONDS;
			}
			else StrongCleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highvale_scoutAI(creature);
    }
};

enum HighvaleMarksman
{
	SPELL_HM_SHOOT   = 6660,
	SPELL_MULTI_SHOT = 14443,
};

class npc_highvale_marksman : public CreatureScript
{
public:
    npc_highvale_marksman() : CreatureScript("npc_highvale_marksman") {}

    struct npc_highvale_marksmanAI : public QuantumBasicAI
    {
		npc_highvale_marksmanAI(Creature* creature) : QuantumBasicAI(creature){}

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
						DoCast(target, SPELL_HM_SHOOT);
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

				if (MultiShotTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_MULTI_SHOT);
						MultiShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else MultiShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highvale_marksmanAI(creature);
    }
};

enum HighvaleRanger
{
	SPELL_HR_FAERIE_FIRE = 6950,
};

class npc_highvale_ranger : public CreatureScript
{
public:
    npc_highvale_ranger() : CreatureScript("npc_highvale_ranger") {}

    struct npc_highvale_rangerAI : public QuantumBasicAI
    {
        npc_highvale_rangerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FaerieFireTimer;

        void Reset()
        {
			FaerieFireTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FaerieFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HR_FAERIE_FIRE);
					FaerieFireTimer = 4*IN_MILLISECONDS;
				}
			}
			else FaerieFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highvale_rangerAI(creature);
    }
};

enum SaltscaleOracle
{
	SPELL_SC_LIGHTNING_BOLT = 9532,
	SPELL_SC_HEALING_WAVE   = 11986,
};

class npc_saltscale_oracle : public CreatureScript
{
public:
    npc_saltscale_oracle() : CreatureScript("npc_saltscale_oracle") {}

    struct npc_saltscale_oracleAI : public QuantumBasicAI
    {
        npc_saltscale_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 HealingWaveTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			HealingWaveTimer = 3500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_SC_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				DoCast(me, SPELL_SC_HEALING_WAVE);
				HealingWaveTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else HealingWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_saltscale_oracleAI(creature);
    }
};

enum FrostwolfBowman
{
	SPELL_FB_SHOOT          = 22121,
	SPELL_SELF_ROOT_FOREVER = 42716,
};

class npc_frostwolf_bowman : public CreatureScript
{
public:
    npc_frostwolf_bowman() : CreatureScript("npc_frostwolf_bowman") {}

    struct npc_frostwolf_bowmanAI : public QuantumBasicAI
    {
		npc_frostwolf_bowmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_SELF_ROOT_FOREVER);
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
						DoCast(target, SPELL_FB_SHOOT);
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
						DoCast(target, SPELL_FB_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostwolf_bowmanAI(creature);
    }
};

enum AcherusGeist
{
	SPELL_DEVOUR_HUMANOID = 52207,
};

class npc_acherus_geist : public CreatureScript
{
public:
    npc_acherus_geist() : CreatureScript("npc_acherus_geist") {}

    struct npc_acherus_geistAI : public QuantumBasicAI
    {
        npc_acherus_geistAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DevourHumanoidTimer;

        void Reset()
		{
			DevourHumanoidTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DevourHumanoidTimer <= diff)
			{
				DoCastVictim(SPELL_DEVOUR_HUMANOID);
				DevourHumanoidTimer = 9*IN_MILLISECONDS;
			}
			else DevourHumanoidTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_acherus_geistAI(creature);
    }
};

enum SyndicateConjuror
{
	SPELL_SC_SHADOW_BOLT     = 9613,
	SPELL_SHADOW_BOLT_VOLLEY = 9081,
	SPELL_DEMON_ARMOR        = 13787,
	SPELL_BRIGHT_CAMPFIRE    = 25085,
	SPELL_SUMMON_VOIDWALKER  = 43896,

	NPC_MINOR_VOIDWALKER     = 24476,
};

class npc_syndicate_conjuror : public CreatureScript
{
public:
    npc_syndicate_conjuror() : CreatureScript("npc_syndicate_conjuror") {}

    struct npc_syndicate_conjurorAI : public QuantumBasicAI
    {
        npc_syndicate_conjurorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 DemonArmorTimer;
		uint32 ShadowBoltTimer;

		SummonList Summons;

        void Reset()
		{
			DemonArmorTimer = 0.1*IN_MILLISECONDS;
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_BRIGHT_CAMPFIRE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_VOIDWALKER, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_MINOR_VOIDWALKER)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (DemonArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_DEMON_ARMOR);
				DemonArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else DemonArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Demon Armor Buff Check in Combat
			if (!me->HasAura(SPELL_DEMON_ARMOR))
				DoCast(me, SPELL_DEMON_ARMOR, true);

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
        return new npc_syndicate_conjurorAI(creature);
    }
};

class npc_minor_voidwalker : public CreatureScript
{
public:
    npc_minor_voidwalker() : CreatureScript("npc_minor_voidwalker") {}

    struct npc_minor_voidwalkerAI : public QuantumBasicAI
    {
        npc_minor_voidwalkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltVolleyTimer;

        void Reset()
		{
			ShadowBoltVolleyTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
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

			if (HealthBelowPct(HEALTH_PERCENT_5))
				me->GetMotionMaster()->MoveFleeing(me->GetVictim(), 5*IN_MILLISECONDS);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_minor_voidwalkerAI(creature);
    }
};

enum JagueroStalker
{
	SPELL_THRASH  = 3417,
	SPELL_STEALTH = 30831,
};

class npc_jaguero_stalker : public CreatureScript
{
public:
    npc_jaguero_stalker() : CreatureScript("npc_jaguero_stalker") {}

    struct npc_jaguero_stalkerAI : public QuantumBasicAI
    {
        npc_jaguero_stalkerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_STEALTH, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
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
        return new npc_jaguero_stalkerAI(creature);
    }
};

enum WeakenedMorbentFel
{
	SPELL_TOUCH_OF_DEATH    = 3108,
	SPELL_PRESENCE_OF_DEATH = 3109,
};

class npc_weakened_morbent_fel : public CreatureScript
{
public:
    npc_weakened_morbent_fel() : CreatureScript("npc_weakened_morbent_fel") {}

    struct npc_weakened_morbent_felAI : public QuantumBasicAI
    {
        npc_weakened_morbent_felAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TouchOfDeathTimer;
		uint32 PresenceOfDeathTimer;

        void Reset()
		{
			TouchOfDeathTimer = 0.5*IN_MILLISECONDS;
			PresenceOfDeathTimer = 2500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TouchOfDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TOUCH_OF_DEATH);
					TouchOfDeathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else TouchOfDeathTimer -= diff;

			if (PresenceOfDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PRESENCE_OF_DEATH);
					PresenceOfDeathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PresenceOfDeathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_weakened_morbent_felAI(creature);
    }
};

enum StormpikeRamRider
{
	SPELL_SR_CLEAVE        = 15284,
	SPELL_SR_MORTAL_STRIKE = 16856,

	RIDER_MOUNT_ID         = 2786,
};

class npc_stormpike_ram_rider_commander : public CreatureScript
{
public:
    npc_stormpike_ram_rider_commander() : CreatureScript("npc_stormpike_ram_rider_commander") {}

    struct npc_stormpike_ram_rider_commanderAI : public QuantumBasicAI
    {
        npc_stormpike_ram_rider_commanderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 MortalStrikeTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 3*IN_MILLISECONDS;

			me->Mount(RIDER_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();
		}

        void UpdateAI(const uint32 diff)
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

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SR_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormpike_ram_rider_commanderAI(creature);
    }
};

enum Ghouls
{
	SPELL_DEAFENING_SCREECH = 3589,
	SPELL_GHOUL_ROT         = 12539,
	SPELL_CURSE_OF_TONGUES  = 13338,
	SPELL_HEALTH_FUNNEL     = 16569,

	NPC_DARROWSHIRE_SPIRIT  = 11064,
};

class npc_cannibal_ghoul : public CreatureScript
{
public:
    npc_cannibal_ghoul() : CreatureScript("npc_cannibal_ghoul") { }

    struct npc_cannibal_ghoulAI : public QuantumBasicAI
    {
        npc_cannibal_ghoulAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 DeafeningScreechTimer;
		uint32 HealthFunnelTimer;

        void Reset()
		{
			DeafeningScreechTimer = 1*IN_MILLISECONDS;
			HealthFunnelTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
                me->SummonCreature(NPC_DARROWSHIRE_SPIRIT, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60*IN_MILLISECONDS);
        }

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeafeningScreechTimer <= diff)
			{
				DoCastAOE(SPELL_DEAFENING_SCREECH);
				DeafeningScreechTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DeafeningScreechTimer -= diff;

			if (HealthFunnelTimer <= diff)
			{
				DoCast(SPELL_HEALTH_FUNNEL);
				HealthFunnelTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else HealthFunnelTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cannibal_ghoulAI(creature);
    }
};

class npc_gibbering_ghoul : public CreatureScript
{
public:
    npc_gibbering_ghoul() : CreatureScript("npc_gibbering_ghoul") { }

    struct npc_gibbering_ghoulAI : public QuantumBasicAI
    {
        npc_gibbering_ghoulAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 CurseOfTonguesTimer;

        void Reset()
		{
			CurseOfTonguesTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
                me->SummonCreature(NPC_DARROWSHIRE_SPIRIT, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60*IN_MILLISECONDS);
        }

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfTonguesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_TONGUES);
					CurseOfTonguesTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfTonguesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gibbering_ghoulAI(creature);
    }
};

class npc_diseased_flayer : public CreatureScript
{
public:
    npc_diseased_flayer() : CreatureScript("npc_diseased_flayer") { }

    struct npc_diseased_flayerAI : public QuantumBasicAI
    {
        npc_diseased_flayerAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 GhoulRotTimer;

        void Reset()
		{
			GhoulRotTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
                me->SummonCreature(NPC_DARROWSHIRE_SPIRIT, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60*IN_MILLISECONDS);
        }

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GhoulRotTimer <= diff)
			{
				DoCast(me, SPELL_GHOUL_ROT);
				GhoulRotTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else GhoulRotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_diseased_flayerAI(creature);
    }
};

enum PutridGargoyles
{
	SPELL_PUTRID_BILE = 16574,
	SPELL_SONIC_BURST = 8281,
};

class npc_putrid_gargoyle : public CreatureScript
{
public:
    npc_putrid_gargoyle() : CreatureScript("npc_putrid_gargoyle") { }

    struct npc_putrid_gargoyleAI : public QuantumBasicAI
    {
        npc_putrid_gargoyleAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 PutridBileTimer;

        void Reset()
		{
			PutridBileTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PutridBileTimer <= diff)
			{
				DoCast(me, SPELL_PUTRID_BILE);
				PutridBileTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else PutridBileTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_putrid_gargoyleAI(creature);
    }
};

class npc_putrid_shrieker : public CreatureScript
{
public:
    npc_putrid_shrieker() : CreatureScript("npc_putrid_shrieker") { }

    struct npc_putrid_shriekerAI : public QuantumBasicAI
    {
        npc_putrid_shriekerAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 PutridBileTimer;
		uint32 SonicBurstTimer;

        void Reset()
		{
			PutridBileTimer = 0.5*IN_MILLISECONDS;
			SonicBurstTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SonicBurstTimer <= diff)
			{
				DoCastAOE(SPELL_SONIC_BURST);
				SonicBurstTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SonicBurstTimer -= diff;

			if (PutridBileTimer <= diff)
			{
				DoCast(me, SPELL_PUTRID_BILE);
				PutridBileTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else PutridBileTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_putrid_shriekerAI(creature);
    }
};

enum AddledLeper
{
	SPELL_HEAL          = 2055,
	SPELL_AL_SHOOT      = 6660,
	SPELL_BATTLE_STANCE = 7165,
	SPELL_AL_HAMSTRING  = 9080,
};

class npc_addled_leper : public CreatureScript
{
public:
    npc_addled_leper() : CreatureScript("npc_addled_leper") { }

    struct npc_addled_leperAI : public QuantumBasicAI
    {
        npc_addled_leperAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 HamstringTimer;
		uint32 HealTimer;

        void Reset()
		{
			HamstringTimer = 1*IN_MILLISECONDS;
			HealTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_BATTLE_STANCE, true);
			DoCastVictim(SPELL_AL_SHOOT, true);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_AL_HAMSTRING);
				HamstringTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			if (HealTimer <= diff)
			{
				DoCast(me, SPELL_HEAL);
				HealTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_addled_leperAI(creature);
    }
};

enum DugganWildhammer
{
	SPELL_DW_SHOOT  = 15547,
	SPELL_DW_CLEAVE = 15284,
};

class npc_duggan_wildhammer : public CreatureScript
{
public:
    npc_duggan_wildhammer() : CreatureScript("npc_duggan_wildhammer") {}

    struct npc_duggan_wildhammerAI : public QuantumBasicAI
    {
        npc_duggan_wildhammerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 CleaveTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			CleaveTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_DW_SHOOT);
					ShootTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_DW_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_duggan_wildhammerAI(creature);
    }
};

enum NathanosBlightcaller
{
	SPELL_BACKHAND       = 6253,
	SPELL_NB_MULTI_SHOT  = 18651,
	SPELL_PSYCHIC_SCREAM = 13704,
	SPELL_SHADOW_SHOOT   = 18649,
};

class npc_nathanos_blightcaller : public CreatureScript
{
public:
    npc_nathanos_blightcaller() : CreatureScript("npc_nathanos_blightcaller") {}

    struct npc_nathanos_blightcallerAI : public QuantumBasicAI
    {
		npc_nathanos_blightcallerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowShootTimer;
		uint32 MultiShotTimer;
		uint32 PsychicScreamTimer;
		uint32 BackHandTimer;

        void Reset()
        {
			ShadowShootTimer = 0.5*IN_MILLISECONDS;
			MultiShotTimer = 1500;
			PsychicScreamTimer = 3*IN_MILLISECONDS;
			BackHandTimer = 5*IN_MILLISECONDS;

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

			if (ShadowShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SHADOW_SHOOT);
						ShadowShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShadowShootTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_NB_MULTI_SHOT);
						MultiShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else MultiShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (PsychicScreamTimer <= diff)
				{
					DoCastAOE(SPELL_PSYCHIC_SCREAM);
					PsychicScreamTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
				else PsychicScreamTimer -= diff;

				if (BackHandTimer <= diff)
				{
					DoCastAOE(SPELL_PSYCHIC_SCREAM);
					BackHandTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
				else BackHandTimer -= diff;

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (ShadowShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_SHADOW_SHOOT);
						ShadowShootTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
					}
				}
				else ShadowShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nathanos_blightcallerAI(creature);
    }
};

enum BluegillRaider
{
	SPELL_BR_THROW = 10277,
	SPELL_BLR_NET  = 12024,
};

class npc_bluegill_raider : public CreatureScript
{
public:
    npc_bluegill_raider() : CreatureScript("npc_bluegill_raider") {}

    struct npc_bluegill_raiderAI : public QuantumBasicAI
    {
		npc_bluegill_raiderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;
		uint32 NetTimer;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;
			NetTimer = 1500;

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

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_BR_THROW);
						ThrowTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ThrowTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_BLR_NET);
						NetTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else NetTimer -= diff;

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

				if (ThrowTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_BR_THROW);
						ThrowTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
				else ThrowTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bluegill_raiderAI(creature);
    }
};

enum BloodsailMage
{
	SPELL_FIRE_SHIELD = 2601,
	SPELL_BM_FIREBALL = 20823,
};

class npc_bloodsail_mage : public CreatureScript
{
public:
    npc_bloodsail_mage() : CreatureScript("npc_bloodsail_mage") {}

    struct npc_bloodsail_mageAI : public QuantumBasicAI
    {
        npc_bloodsail_mageAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireShieldTimer;
		uint32 FireballTimer;

        void Reset()
        {
			FireShieldTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireShieldTimer <= diff)
			{
				DoCast(me, SPELL_FIRE_SHIELD);
				FireShieldTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FireShieldTimer -= diff;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BM_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_mageAI(creature);
    }
};

enum BloodsailRaider
{
	SPELL_BR_NET = 6533,
};

class npc_bloodsail_raider : public CreatureScript
{
public:
    npc_bloodsail_raider() : CreatureScript("npc_bloodsail_raider") {}

    struct npc_bloodsail_raiderAI : public QuantumBasicAI
    {
        npc_bloodsail_raiderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NetTimer;

        void Reset()
        {
			NetTimer = 0.5*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BR_NET);
					NetTimer = 5*IN_MILLISECONDS;
				}
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_raiderAI(creature);
    }
};

enum BloodsailWarlock
{
	SPELL_CURSE_OF_WEAKNESS = 11980,
	SPELL_BW_SHADOW_BOLT    = 20825,
	SPELL_SUMMON_IMP        = 11939,
	SPELL_SUMMON_SUCCUBUS   = 8722,

	NPC_IMP_MINION          = 12922,
	NPC_SUCCUBUS_MINION     = 10928,
};

class npc_bloodsail_warlock : public CreatureScript
{
public:
    npc_bloodsail_warlock() : CreatureScript("npc_bloodsail_warlock") {}

    struct npc_bloodsail_warlockAI : public QuantumBasicAI
    {
		npc_bloodsail_warlockAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 CurseOfWeaknessTimer;
		uint32 ShadowBoltTimer;

		SummonList Summons;

        void Reset()
        {
			CurseOfWeaknessTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 1*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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
					summon->SetCurrentFaction(119);
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
						summon->SetInCombatWith(target);

					Summons.Summon(summon);
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

			if (CurseOfWeaknessTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(me, SPELL_CURSE_OF_WEAKNESS);
					CurseOfWeaknessTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else CurseOfWeaknessTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BW_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_warlockAI(creature);
    }
};

enum BloodsailSwashbuckler
{
	SPELL_DISARM = 6713,
	SPELL_KICK   = 11978,
};

class npc_bloodsail_swashbuckler : public CreatureScript
{
public:
    npc_bloodsail_swashbuckler() : CreatureScript("npc_bloodsail_swashbuckler") {}

    struct npc_bloodsail_swashbucklerAI : public QuantumBasicAI
    {
        npc_bloodsail_swashbucklerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DisarmTimer;
		uint32 KickTimer;

        void Reset()
        {
			DisarmTimer = 1*IN_MILLISECONDS;
			KickTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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

			if (DisarmTimer <= diff)
			{
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = 5*IN_MILLISECONDS;
			}
			else DisarmTimer -= diff;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = 7*IN_MILLISECONDS;
			}
			else KickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_swashbucklerAI(creature);
    }
};

enum BloodsailSeaDog
{
	SPELL_POISON   = 744,
	SPELL_GOUGE    = 12540,
	SPELL_BACKSTAB = 37685,
};

class npc_bloodsail_sea_dog : public CreatureScript
{
public:
    npc_bloodsail_sea_dog() : CreatureScript("npc_bloodsail_sea_dog") {}

    struct npc_bloodsail_sea_dogAI : public QuantumBasicAI
    {
        npc_bloodsail_sea_dogAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonTimer;
		uint32 GougeTimer;
		uint32 BackstabTimer;

        void Reset()
        {
			PoisonTimer = 1*IN_MILLISECONDS;
			GougeTimer = 2*IN_MILLISECONDS;
			BackstabTimer = 3*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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
				PoisonTimer = 4*IN_MILLISECONDS;
			}
			else PoisonTimer -= diff;

			if (GougeTimer <= diff)
			{
				DoCastVictim(SPELL_GOUGE);
				GougeTimer = 6*IN_MILLISECONDS;
			}
			else GougeTimer -= diff;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_BACKSTAB);
				BackstabTimer = 8*IN_MILLISECONDS;
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_sea_dogAI(creature);
    }
};

enum BloodsailDeckhand
{
	SPELL_REND            = 11977,
	SPELL_PIERCING_STRIKE = 37998,
};

class npc_bloodsail_deckhand : public CreatureScript
{
public:
    npc_bloodsail_deckhand() : CreatureScript("npc_bloodsail_deckhand") {}

    struct npc_bloodsail_deckhandAI : public QuantumBasicAI
    {
        npc_bloodsail_deckhandAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 PiercingStrikeTimer;

        void Reset()
        {
			RendTimer = 1*IN_MILLISECONDS;
			PiercingStrikeTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_REND);
				RendTimer = 4*IN_MILLISECONDS;
			}
			else RendTimer -= diff;

			if (PiercingStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCING_STRIKE);
				PiercingStrikeTimer = 6*IN_MILLISECONDS;
			}
			else PiercingStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_deckhandAI(creature);
    }
};

enum BloodsailSwabby
{
	SPELL_POISONOUS_STAB = 7357,
	SPELL_BS_KICK        = 11978,
};

class npc_bloodsail_swabby : public CreatureScript
{
public:
    npc_bloodsail_swabby() : CreatureScript("npc_bloodsail_swabby") {}

    struct npc_bloodsail_swabbyAI : public QuantumBasicAI
    {
        npc_bloodsail_swabbyAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KickTimer;
		uint32 PoisonousStabTimer;

        void Reset()
        {
			KickTimer = 1*IN_MILLISECONDS;
			PoisonousStabTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_BS_KICK);
				KickTimer = 4*IN_MILLISECONDS;
			}
			else KickTimer -= diff;

			if (PoisonousStabTimer <= diff)
			{
				DoCastVictim(SPELL_POISONOUS_STAB);
				PoisonousStabTimer = 6*IN_MILLISECONDS;
			}
			else PoisonousStabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_swabbyAI(creature);
    }
};

enum BloodsailElderMagus
{
	SPELL_BE_FIREBALL = 9053,
	SPELL_FLAMESTRIKE = 11829,
};

class npc_bloodsail_elder_magus : public CreatureScript
{
public:
    npc_bloodsail_elder_magus() : CreatureScript("npc_bloodsail_elder_magus") {}

    struct npc_bloodsail_elder_magusAI : public QuantumBasicAI
    {
        npc_bloodsail_elder_magusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlameStrikeTimer;
		uint32 FireballTimer;

        void Reset()
        {
			FlameStrikeTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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
					DoCast(target, SPELL_BE_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (FlameStrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAMESTRIKE, true);
					FlameStrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else FlameStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsail_elder_magusAI(creature);
    }
};

enum FleetMasterFirallon
{
	SPELL_BATTLE_SHOUT = 32064,

	SAY_FIRALION_AGGRO = -1000298,
};

class npc_fleet_master_firallon : public CreatureScript
{
public:
    npc_fleet_master_firallon() : CreatureScript("npc_fleet_master_firallon") {}

    struct npc_fleet_master_firallonAI : public QuantumBasicAI
    {
        npc_fleet_master_firallonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BattleShoutTimer;

        void Reset()
        {
			BattleShoutTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_FIRALION_AGGRO, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_BATTLE_SHOUT);
				BattleShoutTimer = 6*IN_MILLISECONDS;
			}
			else BattleShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fleet_master_firallonAI(creature);
    }
};

enum Ironpatch
{
	SPELL_IMPROVED_BLOCKING = 3639,
	SPELL_DEFENSIVE_STANCE  = 7164,
	SPELL_SHIELD_BASH       = 11972,
};

class npc_ironpatch : public CreatureScript
{
public:
    npc_ironpatch() : CreatureScript("npc_ironpatch") {}

    struct npc_ironpatchAI : public QuantumBasicAI
    {
        npc_ironpatchAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ImprovedBlockingTimer;
		uint32 ShieldBashTimer;

        void Reset()
        {
			ImprovedBlockingTimer = 0.2*IN_MILLISECONDS;
			ShieldBashTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DEFENSIVE_STANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImprovedBlockingTimer <= diff)
			{
				DoCast(me, SPELL_IMPROVED_BLOCKING);
				ImprovedBlockingTimer = 9*IN_MILLISECONDS;
			}
			else ImprovedBlockingTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = 5*IN_MILLISECONDS;
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ironpatchAI(creature);
    }
};

enum CaptainKeelhaul
{
	SPELL_CK_SHOOT          = 6660,
	SPELL_EXPLODING_SHOT    = 7896,
	SPELL_DEMORAIZING_SHOUT = 13730,
};

class npc_captain_keelhaul : public CreatureScript
{
public:
    npc_captain_keelhaul() : CreatureScript("npc_captain_keelhaul") {}

    struct npc_captain_keelhaulAI : public QuantumBasicAI
    {
        npc_captain_keelhaulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 ExplodindShotTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
        {
			ShootTimer = 0.1*IN_MILLISECONDS;
			ExplodindShotTimer = 0.5*IN_MILLISECONDS;
			DemoralizingShoutTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
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

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CK_SHOOT);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (ExplodindShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_EXPLODING_SHOT);
					ExplodindShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ExplodindShotTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORAIZING_SHOUT);
				DemoralizingShoutTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captain_keelhaulAI(creature);
    }
};

enum GarrSalthoof
{
	SPELL_GS_BACKHAND    = 6253,
	SPELL_RUSHING_CHARGE = 8260,
};

class npc_garr_salthoof : public CreatureScript
{
public:
    npc_garr_salthoof() : CreatureScript("npc_garr_salthoof") {}

    struct npc_garr_salthoofAI : public QuantumBasicAI
    {
        npc_garr_salthoofAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RushingChargeTimer;
		uint32 BackhandTimer;

        void Reset()
        {
			RushingChargeTimer = 0.1*IN_MILLISECONDS;
			BackhandTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
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

			if (RushingChargeTimer <= diff)
			{
				DoCast(me, SPELL_RUSHING_CHARGE);
				RushingChargeTimer = 4*IN_MILLISECONDS;
			}
			else RushingChargeTimer -= diff;

			if (BackhandTimer <= diff)
			{
				DoCastVictim(SPELL_GS_BACKHAND);
				BackhandTimer = 6*IN_MILLISECONDS;
			}
			else BackhandTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_garr_salthoofAI(creature);
    }
};

enum PrettyBoyDuncan
{
	SPELL_SINISTER_PB_STRIKE = 14873,
};

class npc_pretty_boy_duncan : public CreatureScript
{
public:
    npc_pretty_boy_duncan() : CreatureScript("npc_pretty_boy_duncan") {}

    struct npc_pretty_boy_duncanAI : public QuantumBasicAI
    {
        npc_pretty_boy_duncanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SinisterStrikeTimer;

        void Reset()
        {
			SinisterStrikeTimer = 1*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SinisterStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SINISTER_PB_STRIKE);
				SinisterStrikeTimer = 4*IN_MILLISECONDS;
			}
			else SinisterStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pretty_boy_duncanAI(creature);
    }
};

enum CaptainStillwater
{
	SPELL_SLOW             = 11436,
	SPELL_ARCANE_EXPLOSION = 11975,
};

class npc_captain_stillwater : public CreatureScript
{
public:
    npc_captain_stillwater() : CreatureScript("npc_captain_stillwater") {}

    struct npc_captain_stillwaterAI : public QuantumBasicAI
    {
        npc_captain_stillwaterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SlowTimer;
		uint32 ArcaneExplosionTimer;

        void Reset()
        {
			SlowTimer = 0.2*IN_MILLISECONDS;
			ArcaneExplosionTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SlowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SLOW);
					SlowTimer = 4*IN_MILLISECONDS;
				}
			}
			else SlowTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
			{
				DoCast(me, SPELL_ARCANE_EXPLOSION, true);
				ArcaneExplosionTimer = 6*IN_MILLISECONDS;
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captain_stillwaterAI(creature);
    }
};

enum Brutus
{
	SPELL_CRUSH_ARMOR  = 33661,
	SPELL_GROUND_SMASH = 38785,
};

class npc_brutus : public CreatureScript
{
public:
    npc_brutus() : CreatureScript("npc_brutus") {}

    struct npc_brutusAI : public QuantumBasicAI
    {
        npc_brutusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrushArmorTimer;
		uint32 GroundSmashTimer;

        void Reset()
        {
			CrushArmorTimer = 1*IN_MILLISECONDS;
			GroundSmashTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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

			if (CrushArmorTimer <= diff)
			{
				DoCastVictim(SPELL_CRUSH_ARMOR);
				CrushArmorTimer = 4*IN_MILLISECONDS;
			}
			else CrushArmorTimer -= diff;

			if (GroundSmashTimer <= diff)
			{
				DoCast(me, SPELL_GROUND_SMASH);
				GroundSmashTimer = 8*IN_MILLISECONDS;
			}
			else GroundSmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brutusAI(creature);
    }
};

enum WeldonBarov
{
	SPELL_WB_HAMSTRING          = 9080,
	SPELL_WB_CLEAVE             = 15284,
	SPELL_WB_STRIKE             = 15580,
	SPELL_WB_DEMORALIZING_SHOUT = 16244,
};

class npc_weldon_barov : public CreatureScript
{
public:
    npc_weldon_barov() : CreatureScript("npc_weldon_barov") {}

    struct npc_weldon_barovAI : public QuantumBasicAI
    {
        npc_weldon_barovAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HamstringTimer;
		uint32 CleaveTimer;
		uint32 StrikeTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
        {
			HamstringTimer = 1*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 3*IN_MILLISECONDS;
			DemoralizingShoutTimer = 4*IN_MILLISECONDS;

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

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_WB_HAMSTRING);
				HamstringTimer = 4*IN_MILLISECONDS;
			}
			else HamstringTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_WB_CLEAVE);
				CleaveTimer = 6*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_WB_STRIKE);
				StrikeTimer = 8*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_WB_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = 10*IN_MILLISECONDS;
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_weldon_barovAI(creature);
    }
};

enum HighPriestessMacDonnell
{
	SPELL_HOLY_SMITE                  = 9481,
	SPELL_HOLY_FIRE                   = 15267,
	SPELL_POWER_WORD_FORTITUDE_RANK_5 = 10937,
	SPELL_POWER_WORD_FORTITUDE_RANK_6 = 10938,
};

class npc_high_priestess_macdonnell : public CreatureScript
{
public:
    npc_high_priestess_macdonnell() : CreatureScript("npc_high_priestess_macdonnell") {}

    struct npc_high_priestess_macdonnellAI : public QuantumBasicAI
    {
        npc_high_priestess_macdonnellAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PowerWordFortitudeTimer;
		uint32 HolySmiteTimer;
		uint32 HolyFireTimer;
		uint32 ResetTimer;

		bool Buffed;

        void Reset()
        {
			PowerWordFortitudeTimer = 0.1*IN_MILLISECONDS;
			HolySmiteTimer = 0.5*IN_MILLISECONDS;
			HolyFireTimer = 3*IN_MILLISECONDS;
			ResetTimer = 60*IN_MILLISECONDS;

			Buffed = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (Buffed || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 20.0f))
				return;

			if (Buffed == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 20.0f))
			{
				DoCast(who, SPELL_POWER_WORD_FORTITUDE_RANK_6, true);
				Buffed = true;
			}
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PowerWordFortitudeTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_POWER_WORD_FORTITUDE_RANK_5);
				PowerWordFortitudeTimer = 5*MINUTE*IN_MILLISECONDS;
			}
			else PowerWordFortitudeTimer -= diff;

			if (ResetTimer <= diff && !me->IsInCombatActive())
			{
				Reset();
				ResetTimer = 60*IN_MILLISECONDS;
			}
			else ResetTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HolySmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HOLY_SMITE);
					HolySmiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else HolySmiteTimer -= diff;

			if (HolyFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HOLY_FIRE);
					HolyFireTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else HolyFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_priestess_macdonnellAI(creature);
    }
};

enum FlintShadowmore
{
	SPELL_FS_SLICE_AND_DICE = 6434,
	SPELL_FS_GOUGE          = 12540,
	SPELL_FS_BACKSTAB       = 15582,
};

class npc_flint_shadowmore : public CreatureScript
{
public:
    npc_flint_shadowmore() : CreatureScript("npc_flint_shadowmore") {}

    struct npc_flint_shadowmoreAI : public QuantumBasicAI
    {
        npc_flint_shadowmoreAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SliceAndDiceTimer;
		uint32 GougeTimer;
		uint32 BackstabTimer;

        void Reset()
		{
			SliceAndDiceTimer = 0.2*IN_MILLISECONDS;
			GougeTimer = 2*IN_MILLISECONDS;
			BackstabTimer = 3*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SliceAndDiceTimer <= diff)
			{
				DoCast(me, SPELL_FS_SLICE_AND_DICE);
				SliceAndDiceTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else SliceAndDiceTimer -= diff;

			if (GougeTimer <= diff)
			{
				DoCastVictim(SPELL_FS_GOUGE);
				GougeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GougeTimer -= diff;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_FS_BACKSTAB);
				BackstabTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flint_shadowmoreAI(creature);
    }
};

enum SyndicateWatchman
{
	SPELL_TORCH_BURST = 3582,
};

class npc_syndicate_watchman : public CreatureScript
{
public:
    npc_syndicate_watchman() : CreatureScript("npc_syndicate_watchman") {}

    struct npc_syndicate_watchmanAI : public QuantumBasicAI
    {
        npc_syndicate_watchmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TorchBurstTimer;

        void Reset()
		{
			TorchBurstTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TorchBurstTimer <= diff)
			{
				DoCast(me, SPELL_TORCH_BURST);
				TorchBurstTimer = 10*IN_MILLISECONDS;
			}
			else TorchBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syndicate_watchmanAI(creature);
    }
};

enum SkeletalFlayer
{
	SPELL_SF_THRASH = 3417,
};

class npc_skeletal_flayer : public CreatureScript
{
public:
    npc_skeletal_flayer() : CreatureScript("npc_skeletal_flayer") {}

    struct npc_skeletal_flayerAI : public QuantumBasicAI
    {
        npc_skeletal_flayerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SF_THRASH);
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
        return new npc_skeletal_flayerAI(creature);
    }
};

enum SlaveringGhoul
{
	SPELL_SG_THRASH    = 3417,
	SPELL_TOXIC_SALIVA = 7125,
};

class npc_slavering_ghoul : public CreatureScript
{
public:
    npc_slavering_ghoul() : CreatureScript("npc_slavering_ghoul") {}

    struct npc_slavering_ghoulAI : public QuantumBasicAI
    {
        npc_slavering_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ToxicSalivaTimer;

        void Reset()
		{
			ToxicSalivaTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SF_THRASH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (ToxicSalivaTimer <= diff)
			{
				DoCastVictim(SPELL_TOXIC_SALIVA);
				ToxicSalivaTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ToxicSalivaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slavering_ghoulAI(creature);
    }
};

enum DefiasBrotherhood
{
	SPELL_SNAP_KICK      = 8646,
	SPELL_DR_FROSTBOLT   = 13322,
	SPELL_DR_FROST_ARMOR = 12544,

	SAY_DEFIAS_AGGRO_1 = -1000299,
	SAY_DEFIAS_AGGRO_2 = -1000300,
	SAY_DEFIAS_AGGRO_3 = -1000301,
	SAY_DEFIAS_AGGRO_4 = -1000302,
	SAY_DEFIAS_AGGRO_5 = -1000303,
};

class npc_defias_bandit : public CreatureScript
{
public:
    npc_defias_bandit() : CreatureScript("npc_defias_bandit") {}

    struct npc_defias_banditAI : public QuantumBasicAI
    {
        npc_defias_banditAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SnapKickTimer;

        void Reset()
		{
			SnapKickTimer = 1*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_DEFIAS_AGGRO_1, SAY_DEFIAS_AGGRO_2, SAY_DEFIAS_AGGRO_3, SAY_DEFIAS_AGGRO_4, SAY_DEFIAS_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SnapKickTimer <= diff)
			{
				DoCastVictim(SPELL_SNAP_KICK);
				SnapKickTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SnapKickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_banditAI(creature);
    }
};

class npc_defias_rogue_wizard : public CreatureScript
{
public:
    npc_defias_rogue_wizard() : CreatureScript("npc_defias_rogue_wizard") {}

    struct npc_defias_rogue_wizardAI : public QuantumBasicAI
    {
        npc_defias_rogue_wizardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FrostboltTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FrostboltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_DEFIAS_AGGRO_1, SAY_DEFIAS_AGGRO_2, SAY_DEFIAS_AGGRO_3, SAY_DEFIAS_AGGRO_4, SAY_DEFIAS_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (FrostArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_DR_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_DR_FROST_ARMOR))
				DoCast(me, SPELL_DR_FROST_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DR_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_rogue_wizardAI(creature);
    }
};

enum ChillwindLieutenant
{
	SPELL_GUST_OF_WIND = 6982,
	SPELL_FROST_SHOCK  = 23115,
};

class npc_chillwind_lieutenant : public CreatureScript
{
public:
    npc_chillwind_lieutenant() : CreatureScript("npc_chillwind_lieutenant") {}

    struct npc_chillwind_lieutenantAI : public QuantumBasicAI
    {
        npc_chillwind_lieutenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostsShockTimer;
		uint32 GustOfWindTimer;

        void Reset()
        {
			FrostsShockTimer = 0.5*IN_MILLISECONDS;
			GustOfWindTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostsShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_SHOCK);
					FrostsShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostsShockTimer -= diff;

			if (GustOfWindTimer <= diff)
			{
				DoCastAOE(SPELL_GUST_OF_WIND);
				GustOfWindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GustOfWindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chillwind_lieutenantAI(creature);
    }
};

enum FrigidLieutenant
{
	SPELL_FROST_BREATH = 3131,
};

class npc_frigid_lieutenant : public CreatureScript
{
public:
    npc_frigid_lieutenant() : CreatureScript("npc_frigid_lieutenant") {}

    struct npc_frigid_lieutenantAI : public QuantumBasicAI
    {
        npc_frigid_lieutenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBreathTimer;

        void Reset()
        {
			FrostBreathTimer = 2*IN_MILLISECONDS;

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
				FrostBreathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frigid_lieutenantAI(creature);
    }
};

enum GlacialLieutenant
{
	SPELL_GL_FROST_NOVA  = 14907,
	SPELL_GL_FROST_SHOCK = 15089,
};

class npc_glacial_lieutenant : public CreatureScript
{
public:
    npc_glacial_lieutenant() : CreatureScript("npc_glacial_lieutenant") {}

    struct npc_glacial_lieutenantAI : public QuantumBasicAI
    {
        npc_glacial_lieutenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostShockTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FrostShockTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GL_FROST_SHOCK);
					FrostShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostShockTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_GL_FROST_NOVA);
				FrostNovaTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_glacial_lieutenantAI(creature);
    }
};

enum SyndicateMagus
{
	SPELL_SM_BLIZZARD    = 8364,
	SPELL_SM_FROSTBOLT   = 9672,
	SPELL_SM_FROST_ARMOR = 12544,
};

class npc_syndicate_magus : public CreatureScript
{
public:
    npc_syndicate_magus() : CreatureScript("npc_syndicate_magus") {}

    struct npc_syndicate_magusAI : public QuantumBasicAI
    {
        npc_syndicate_magusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 BlizzardTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			BlizzardTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat
			if (!me->HasAura(SPELL_SM_FROST_ARMOR) && !me->IsInCombatActive())
				DoCast(me, SPELL_SM_FROST_ARMOR);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_SM_FROST_ARMOR))
				DoCast(me, SPELL_SM_FROST_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SM_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SM_BLIZZARD, true);
					BlizzardTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syndicate_magusAI(creature);
    }
};

enum RumblingExile
{
	SPELL_GROUND_TREMOR = 6524,
};

class npc_rumbling_exile : public CreatureScript
{
public:
    npc_rumbling_exile() : CreatureScript("npc_rumbling_exile") {}

    struct npc_rumbling_exileAI : public QuantumBasicAI
    {
        npc_rumbling_exileAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GroundTremorTimer;

        void Reset()
        {
			GroundTremorTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GroundTremorTimer <= diff)
			{
				DoCastAOE(SPELL_GROUND_TREMOR);
				GroundTremorTimer = 6*IN_MILLISECONDS;
			}
			else GroundTremorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rumbling_exileAI(creature);
    }
};

enum DaggerspineRaider
{
	SPELL_DR_NET    = 6533,
	SPELL_DR_CHARGE = 20508,
};

class npc_daggerspine_raider : public CreatureScript
{
public:
    npc_daggerspine_raider() : CreatureScript("npc_daggerspine_raider") {}

    struct npc_daggerspine_raiderAI : public QuantumBasicAI
    {
        npc_daggerspine_raiderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChargeTimer;
		uint32 NetTimer;

        void Reset()
        {
			ChargeTimer = 0.2*IN_MILLISECONDS;
			NetTimer = 1*IN_MILLISECONDS;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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

			if (ChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DR_CHARGE, true);
					ChargeTimer = 5*IN_MILLISECONDS;
				}
			}
			else ChargeTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DR_NET);
					NetTimer = 7*IN_MILLISECONDS;
				}
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daggerspine_raiderAI(creature);
    }
};

enum DaggerspineSorceress
{
	SPELL_DS_LIGHTNING_SHIELD = 8134,
	SPELL_DS_LIGHTNING_BOLT   = 9532,
};

class npc_daggerspine_sorceress : public CreatureScript
{
public:
    npc_daggerspine_sorceress() : CreatureScript("npc_daggerspine_sorceress") {}

    struct npc_daggerspine_sorceressAI : public QuantumBasicAI
    {
        npc_daggerspine_sorceressAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat
			if (!me->HasAura(SPELL_DS_LIGHTNING_SHIELD) && !me->IsInCombatActive())
				DoCast(me, SPELL_DS_LIGHTNING_SHIELD);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_DS_LIGHTNING_SHIELD))
				DoCast(me, SPELL_DS_LIGHTNING_SHIELD, true);

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DS_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daggerspine_sorceressAI(creature);
    }
};

enum DaggerspineShorehunter
{
	SPELL_DS_THROW = 10277,
	SPELL_DS_NET   = 12024,
};

class npc_daggerspine_shorehunter : public CreatureScript
{
public:
    npc_daggerspine_shorehunter() : CreatureScript("npc_daggerspine_shorehunter") {}

    struct npc_daggerspine_shorehunterAI : public QuantumBasicAI
    {
		npc_daggerspine_shorehunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;
		uint32 NetTimer;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;
			NetTimer = 1500;

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

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_DS_THROW);
						ThrowTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ThrowTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_DS_NET);
						NetTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else NetTimer -= diff;

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

				if (ThrowTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_DS_THROW);
						ThrowTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
				else ThrowTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daggerspine_shorehunterAI(creature);
    }
};

enum LieutenantHaggerdin
{
	SPELL_LH_ENRAGE       = 8599,
	SPELL_LH_SHIELD_BLOCK = 12169,
	SPELL_LH_REVENGE      = 19130,
};

class npc_lieutenant_haggerdin : public CreatureScript
{
public:
    npc_lieutenant_haggerdin() : CreatureScript("npc_lieutenant_haggerdin") {}

    struct npc_lieutenant_haggerdinAI : public QuantumBasicAI
    {
        npc_lieutenant_haggerdinAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShieldBlockTimer;
		uint32 RevengeTimer;

        void Reset()
        {
			ShieldBlockTimer = 0.5*IN_MILLISECONDS;
			RevengeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
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

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_LH_SHIELD_BLOCK);
				ShieldBlockTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ShieldBlockTimer -= diff;

			if (RevengeTimer <= diff)
			{
				DoCastVictim(SPELL_LH_REVENGE);
				RevengeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RevengeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_LH_ENRAGE, 0))
				{
					DoCast(me, SPELL_LH_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lieutenant_haggerdinAI(creature);
    }
};

enum StormpikeBattleguard
{
	SPELL_SB_IMPROVED_BLOCKING = 3248,
	SPELL_SB_REND              = 16509,
	SPELL_SB_CHARGE            = 22120,
	SPELL_SB_STRIKE            = 22591,
};

class npc_stormpike_battleguard : public CreatureScript
{
public:
    npc_stormpike_battleguard() : CreatureScript("npc_stormpike_battleguard") {}

    struct npc_stormpike_battleguardAI : public QuantumBasicAI
    {
        npc_stormpike_battleguardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChargeTimer;
		uint32 ImprovedBlockingTimer;
		uint32 RendTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			ChargeTimer = 0.2*IN_MILLISECONDS;
			ImprovedBlockingTimer = 1*IN_MILLISECONDS;
			RendTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
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
					DoCast(target, SPELL_SB_CHARGE);
					ChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ChargeTimer -= diff;

			if (ImprovedBlockingTimer <= diff)
			{
				DoCast(me, SPELL_SB_IMPROVED_BLOCKING);
				ImprovedBlockingTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ImprovedBlockingTimer -= diff;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_SB_REND);
				RendTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SB_STRIKE);
				StrikeTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormpike_battleguardAI(creature);
    }
};

enum SouthshoreGuard
{
	SPELL_SG_ENRAGE       = 8599,
	SPELL_SG_SHIELD_BLOCK = 12169,
	SPELL_SG_REVENGE      = 12170,
};

class npc_southshore_guard : public CreatureScript
{
public:
    npc_southshore_guard() : CreatureScript("npc_southshore_guard") {}

    struct npc_southshore_guardAI : public QuantumBasicAI
    {
        npc_southshore_guardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShieldBlockTimer;
		uint32 RevengeTimer;

        void Reset()
        {
			ShieldBlockTimer = 0.5*IN_MILLISECONDS;
			RevengeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
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

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_SG_SHIELD_BLOCK);
				ShieldBlockTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ShieldBlockTimer -= diff;

			if (RevengeTimer <= diff)
			{
				DoCastVictim(SPELL_SG_REVENGE);
				RevengeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RevengeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_SG_ENRAGE, 0))
				{
					DoCast(me, SPELL_SG_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_southshore_guardAI(creature);
    }
};

enum ThanthaldisSnowgleam
{
	SPELL_TS_CLEAVE = 19642,
	SPELL_TS_STRIKE = 19644,
};

class npc_thanthaldis_snowgleam : public CreatureScript
{
public:
    npc_thanthaldis_snowgleam() : CreatureScript("npc_thanthaldis_snowgleam") {}

    struct npc_thanthaldis_snowgleamAI : public QuantumBasicAI
    {
        npc_thanthaldis_snowgleamAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
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
				DoCastVictim(SPELL_TS_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TS_STRIKE);
				StrikeTimer = 6*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thanthaldis_snowgleamAI(creature);
    }
};

enum SyndicateThief
{
	SPELL_ST_POISON   = 744,
	SPELL_ST_DISARM   = 6713,
	SPELL_ST_BACKSTAB = 7159,
	SPELL_ST_STRIKE   = 13584,

	SAY_FOOTPAD_AGGRO = -1420091,
};

class npc_syndicate_thief : public CreatureScript
{
public:
    npc_syndicate_thief() : CreatureScript("npc_syndicate_thief") {}

    struct npc_syndicate_thiefAI : public QuantumBasicAI
    {
        npc_syndicate_thiefAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonTimer;
		uint32 DisarmTimer;
		uint32 BackstabTimer;

        void Reset()
        {
			PoisonTimer = 1*IN_MILLISECONDS;
			DisarmTimer = 2*IN_MILLISECONDS;
			BackstabTimer = 3*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_ST_POISON);
				PoisonTimer = 4*IN_MILLISECONDS;
			}
			else PoisonTimer -= diff;

			if (DisarmTimer <= diff)
			{
				DoCastVictim(SPELL_ST_DISARM);
				DisarmTimer = 6*IN_MILLISECONDS;
			}
			else DisarmTimer -= diff;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_ST_BACKSTAB);
				BackstabTimer = 8*IN_MILLISECONDS;
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syndicate_thiefAI(creature);
    }
};

class npc_syndicate_footpad : public CreatureScript
{
public:
    npc_syndicate_footpad() : CreatureScript("npc_syndicate_footpad") {}

    struct npc_syndicate_footpadAI : public QuantumBasicAI
    {
        npc_syndicate_footpadAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonTimer;
		uint32 BackstabTimer;

        void Reset()
        {
			PoisonTimer = 1*IN_MILLISECONDS;
			BackstabTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_ST_POISON);
				PoisonTimer = 4*IN_MILLISECONDS;
			}
			else PoisonTimer -= diff;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_ST_BACKSTAB);
				BackstabTimer = 8*IN_MILLISECONDS;
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syndicate_footpadAI(creature);
    }
};

class npc_drunken_footpad : public CreatureScript
{
public:
    npc_drunken_footpad() : CreatureScript("npc_drunken_footpad") {}

    struct npc_drunken_footpadAI : public QuantumBasicAI
    {
        npc_drunken_footpadAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoSendQuantumText(SAY_FOOTPAD_AGGRO, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_ST_STRIKE);
				StrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drunken_footpadAI(creature);
    }
};

enum GrayBear
{
	SPELL_GB_ENRAGE       = 8599,
	SPELL_SUNDERING_SWIPE = 35147,
};

class npc_gray_bear : public CreatureScript
{
public:
    npc_gray_bear() : CreatureScript("npc_gray_bear") {}

    struct npc_gray_bearAI : public QuantumBasicAI
    {
        npc_gray_bearAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SunderingSwipeTimer;

        void Reset()
        {
			SunderingSwipeTimer = 1*IN_MILLISECONDS;

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

			if (SunderingSwipeTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDERING_SWIPE);
				SunderingSwipeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SunderingSwipeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_GB_ENRAGE, 0))
				{
					DoCast(me, SPELL_GB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gray_bearAI(creature);
    }
};

class npc_vicious_gray_bear : public CreatureScript
{
public:
    npc_vicious_gray_bear() : CreatureScript("npc_vicious_gray_bear") {}

    struct npc_vicious_gray_bearAI : public QuantumBasicAI
    {
        npc_vicious_gray_bearAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SunderingSwipeTimer;

        void Reset()
        {
			SunderingSwipeTimer = 1*IN_MILLISECONDS;

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

			if (SunderingSwipeTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDERING_SWIPE);
				SunderingSwipeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SunderingSwipeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_GB_ENRAGE, 0))
				{
					DoCast(me, SPELL_GB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vicious_gray_bearAI(creature);
    }
};

class npc_elder_gray_bear : public CreatureScript
{
public:
    npc_elder_gray_bear() : CreatureScript("npc_elder_gray_bear") {}

    struct npc_elder_gray_bearAI : public QuantumBasicAI
    {
        npc_elder_gray_bearAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SunderingSwipeTimer;

        void Reset()
        {
			SunderingSwipeTimer = 1*IN_MILLISECONDS;

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

			if (SunderingSwipeTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDERING_SWIPE);
				SunderingSwipeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SunderingSwipeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_GB_ENRAGE, 0))
				{
					DoCast(me, SPELL_GB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elder_gray_bearAI(creature);
    }
};

enum ForestMossCreeper
{
	SPELL_CORROSIVE_POISON = 3396,
};

class npc_forest_moss_creeper : public CreatureScript
{
public:
    npc_forest_moss_creeper() : CreatureScript("npc_forest_moss_creeper") {}

    struct npc_forest_moss_creeperAI : public QuantumBasicAI
    {
        npc_forest_moss_creeperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CorrosivePoison;

        void Reset()
        {
			CorrosivePoison = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorrosivePoison <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CORROSIVE_POISON);
					CorrosivePoison = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CorrosivePoison -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forest_moss_creeperAI(creature);
    }
};

class npc_giant_moss_creeper : public CreatureScript
{
public:
    npc_giant_moss_creeper() : CreatureScript("npc_giant_moss_creeper") {}

    struct npc_giant_moss_creeperAI : public QuantumBasicAI
    {
        npc_giant_moss_creeperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CorrosivePoison;

        void Reset()
        {
			CorrosivePoison = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorrosivePoison <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CORROSIVE_POISON);
					CorrosivePoison = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CorrosivePoison -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_giant_moss_creeperAI(creature);
    }
};

enum FerociousYeti
{
	SPELL_FY_ENRAGE = 8599,
};

class npc_ferocious_yeti : public CreatureScript
{
public:
    npc_ferocious_yeti() : CreatureScript("npc_ferocious_yeti") {}

    struct npc_ferocious_yetiAI : public QuantumBasicAI
    {
        npc_ferocious_yetiAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_FY_ENRAGE, 0))
				{
					DoCast(me, SPELL_FY_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ferocious_yetiAI(creature);
    }
};

class npc_cave_yeti : public CreatureScript
{
public:
    npc_cave_yeti() : CreatureScript("npc_cave_yeti") {}

    struct npc_cave_yetiAI : public QuantumBasicAI
    {
        npc_cave_yetiAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_FY_ENRAGE, 0))
				{
					DoCast(me, SPELL_FY_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cave_yetiAI(creature);
    }
};

enum StarvingMountainLion
{
	SPELL_SM_PROWL = 24450,
};

class npc_starving_mountain_lion : public CreatureScript
{
public:
    npc_starving_mountain_lion() : CreatureScript("npc_starving_mountain_lion") {}

    struct npc_starving_mountain_lionAI : public QuantumBasicAI
    {
        npc_starving_mountain_lionAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SM_PROWL);

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
        return new npc_starving_mountain_lionAI(creature);
    }
};

class npc_feral_mountain_lion : public CreatureScript
{
public:
    npc_feral_mountain_lion() : CreatureScript("npc_feral_mountain_lion") {}

    struct npc_feral_mountain_lionAI : public QuantumBasicAI
    {
        npc_feral_mountain_lionAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SM_PROWL);

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
        return new npc_feral_mountain_lionAI(creature);
    }
};

enum TornFinOracle
{
	SPELL_TF_LIGHTNING_BOLT = 9532,
	SPELL_TF_HEALING_WAVE   = 939,
};

class npc_torn_fin_oracle : public CreatureScript
{
public:
    npc_torn_fin_oracle() : CreatureScript("npc_torn_fin_oracle") {}

    struct npc_torn_fin_oracleAI : public QuantumBasicAI
    {
        npc_torn_fin_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 HealingWaveTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			HealingWaveTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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
					DoCast(target, SPELL_TF_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				DoCast(me, SPELL_TF_HEALING_WAVE);
				HealingWaveTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else HealingWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_torn_fin_oracleAI(creature);
    }
};

enum TornFinTidehunter
{
	SPELL_TF_FROSTBOLT  = 9672,
	SPELL_TF_FROST_NOVA = 11831,
};

class npc_torn_fin_tidehunter : public CreatureScript
{
public:
    npc_torn_fin_tidehunter() : CreatureScript("npc_torn_fin_tidehunter") {}

    struct npc_torn_fin_tidehunterAI : public QuantumBasicAI
    {
        npc_torn_fin_tidehunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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
					DoCast(target, SPELL_TF_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_TF_FROST_NOVA, true);
				FrostNovaTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_torn_fin_tidehunterAI(creature);
    }
};

enum TornFinMuckdweller
{
	SPELL_INFECTED_WOUND = 3427,
};

class npc_torn_fin_muckdweller : public CreatureScript
{
public:
    npc_torn_fin_muckdweller() : CreatureScript("npc_torn_fin_muckdweller") {}

    struct npc_torn_fin_muckdwellerAI : public QuantumBasicAI
    {
        npc_torn_fin_muckdwellerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedWoundTimer;

        void Reset()
        {
			InfectedWoundTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedWoundTimer <= diff)
			{
				DoCastVictim(SPELL_INFECTED_WOUND);
				InfectedWoundTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else InfectedWoundTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_torn_fin_muckdwellerAI(creature);
    }
};

enum TornFinCoastrunner
{
	SPELL_TF_THROW = 10277,
};

class npc_torn_fin_coastrunner : public CreatureScript
{
public:
    npc_torn_fin_coastrunner() : CreatureScript("npc_torn_fin_coastrunner") {}

    struct npc_torn_fin_coastrunnerAI : public QuantumBasicAI
    {
        npc_torn_fin_coastrunnerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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
					DoCast(target, SPELL_TF_THROW);
					ThrowTimer = 3*IN_MILLISECONDS;
				}
			}
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_torn_fin_coastrunnerAI(creature);
    }
};

enum HillsbradSentry
{
	SPELL_HS_CHARGE       = 22120,
	SPELL_HS_BATTLE_SHOUT = 32064,
};

class npc_hillsbrad_sentry : public CreatureScript
{
public:
    npc_hillsbrad_sentry() : CreatureScript("npc_hillsbrad_sentry") {}

    struct npc_hillsbrad_sentryAI : public QuantumBasicAI
    {
        npc_hillsbrad_sentryAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChargeTimer;
		uint32 BattleShoutTimer;

        void Reset()
        {
			ChargeTimer = 0.2*IN_MILLISECONDS;
			BattleShoutTimer = 1*IN_MILLISECONDS;

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

			if (ChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HS_CHARGE);
					ChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ChargeTimer -= diff;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_HS_BATTLE_SHOUT);
				BattleShoutTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else BattleShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hillsbrad_sentryAI(creature);
    }
};

enum HillsbradMiner
{
	SPELL_HM_DEFENSIVE_STANCE = 7164,
	SPELL_HM_SUNDER_ARMOR     = 11971,
};

class npc_hillsbrad_miner : public CreatureScript
{
public:
    npc_hillsbrad_miner() : CreatureScript("npc_hillsbrad_miner") {}

    struct npc_hillsbrad_minerAI : public QuantumBasicAI
    {
        npc_hillsbrad_minerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DefensiveStanceTimer;
		uint32 SunderArmorTimer;

        void Reset()
        {
			DefensiveStanceTimer = 0.2*IN_MILLISECONDS;
			SunderArmorTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
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

			if (DefensiveStanceTimer <= diff)
			{
				DoCast(me, SPELL_HM_DEFENSIVE_STANCE);
				DefensiveStanceTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DefensiveStanceTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_HM_SUNDER_ARMOR);
				SunderArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else SunderArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hillsbrad_minerAI(creature);
    }
};

enum DunGarokSoldier
{
	SPELL_DS_DEFENSIVE_STANCE = 7164,
	SPELL_DS_SHIELD_BASH      = 11972,
};

class npc_dun_garok_soldier : public CreatureScript
{
public:
    npc_dun_garok_soldier() : CreatureScript("npc_dun_garok_soldier") {}

    struct npc_dun_garok_soldierAI : public QuantumBasicAI
    {
        npc_dun_garok_soldierAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DefensiveStanceTimer;
		uint32 ShieldBashTimer;

        void Reset()
        {
			DefensiveStanceTimer = 0.2*IN_MILLISECONDS;
			ShieldBashTimer = 1*IN_MILLISECONDS;

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

			if (DefensiveStanceTimer <= diff)
			{
				DoCast(me, SPELL_DS_DEFENSIVE_STANCE);
				DefensiveStanceTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DefensiveStanceTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_DS_SHIELD_BASH);
				ShieldBashTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dun_garok_soldierAI(creature);
    }
};

enum MinerHackett
{
	SPELL_MH_CLEAVE = 15284,
};

class npc_miner_hackett : public CreatureScript
{
public:
    npc_miner_hackett() : CreatureScript("npc_miner_hackett") {}

    struct npc_miner_hackettAI : public QuantumBasicAI
    {
        npc_miner_hackettAI(Creature* creature) : QuantumBasicAI(creature){}

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
				DoCastVictim(SPELL_MH_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_miner_hackettAI(creature);
    }
};

enum ForemanBonds
{
	SPELL_FB_DEVOTION_AURA     = 643,
	SPELL_FB_HAMMER_OF_JUSTICE = 5588,

	NPC_DUN_GAROK_SOLDIER      = 7360,
};

class npc_foreman_bonds : public CreatureScript
{
public:
    npc_foreman_bonds() : CreatureScript("npc_foreman_bonds") {}

    struct npc_foreman_bondsAI : public QuantumBasicAI
    {
		npc_foreman_bondsAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 HammerOfJusticeTimer;

		bool Defenders;

		SummonList Summons;

        void Reset()
        {
			HammerOfJusticeTimer = 2*IN_MILLISECONDS;

			Summons.DespawnAll();

			Defenders = true;

			DoCast(me, SPELL_FB_DEVOTION_AURA);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_DUN_GAROK_SOLDIER)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfJusticeTimer <= diff)
			{
				DoCastVictim(SPELL_FB_HAMMER_OF_JUSTICE);
				HammerOfJusticeTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else HammerOfJusticeTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!Defenders)
				{
					me->SummonCreature(NPC_DUN_GAROK_SOLDIER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 35*IN_MILLISECONDS);
					me->SummonCreature(NPC_DUN_GAROK_SOLDIER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 35*IN_MILLISECONDS);
					Defenders = false;
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_foreman_bondsAI(creature);
    }
};

enum RabidDireWolf
{
	SPELL_DIRE_WOLF_VISUAL = 31332,
	SPELL_RABIES           = 3150,
};

class npc_rabid_dire_wolf : public CreatureScript
{
public:
    npc_rabid_dire_wolf() : CreatureScript("npc_rabid_dire_wolf") {}

    struct npc_rabid_dire_wolfAI : public QuantumBasicAI
    {
        npc_rabid_dire_wolfAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RabiesTimer;

        void Reset()
        {
			RabiesTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_DIRE_WOLF_VISUAL);

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

			if (RabiesTimer <= diff)
			{
				DoCastVictim(SPELL_RABIES);
				RabiesTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RabiesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rabid_dire_wolfAI(creature);
    }
};

enum ShoreCrawler
{
	SPELL_SC_PIERCE_ARMOR = 6016,
	SPELL_SC_WATER_BUBBLE = 7383,
	SPELL_SC_SAND_BREATH  = 20716,
};

class npc_shore_crawler : public CreatureScript
{
public:
    npc_shore_crawler() : CreatureScript("npc_shore_crawler") {}

    struct npc_shore_crawlerAI : public QuantumBasicAI
    {
        npc_shore_crawlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PierceArmorTimer;

        void Reset()
        {
			PierceArmorTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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
				PierceArmorTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shore_crawlerAI(creature);
    }
};

class npc_sand_crawler : public CreatureScript
{
public:
    npc_sand_crawler() : CreatureScript("npc_sand_crawler") {}

    struct npc_sand_crawlerAI : public QuantumBasicAI
    {
        npc_sand_crawlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SandBreathTimer;

        void Reset()
        {
			SandBreathTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SandBreathTimer <= diff)
			{
				DoCastVictim(SPELL_SC_SAND_BREATH);
				SandBreathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SandBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sand_crawlerAI(creature);
    }
};

class npc_sea_crawler : public CreatureScript
{
public:
    npc_sea_crawler() : CreatureScript("npc_sea_crawler") {}

    struct npc_sea_crawlerAI : public QuantumBasicAI
    {
        npc_sea_crawlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterBubbleTimer;
		uint32 SandBreathTimer;

        void Reset()
        {
			WaterBubbleTimer = 0.2*IN_MILLISECONDS;
			SandBreathTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterBubbleTimer <= diff)
			{
				DoCast(me, SPELL_SC_WATER_BUBBLE, true);
				WaterBubbleTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else WaterBubbleTimer -= diff;

			if (SandBreathTimer <= diff)
			{
				DoCastVictim(SPELL_SC_SAND_BREATH);
				SandBreathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SandBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sea_crawlerAI(creature);
    }
};

enum MurlocOracle
{
	SPELL_MO_SMITE             = 9734,
	SPELL_MO_POWER_WORD_SHIELD = 11835,
};

class npc_murloc_oracle : public CreatureScript
{
public:
    npc_murloc_oracle() : CreatureScript("npc_murloc_oracle") {}

    struct npc_murloc_oracleAI : public QuantumBasicAI
    {
        npc_murloc_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SmiteTimer;
		uint32 PowerWordShieldTimer;

        void Reset()
        {
			SmiteTimer = 0.5*IN_MILLISECONDS;
			PowerWordShieldTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_MO_POWER_WORD_SHIELD, true);
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
					DoCast(target, SPELL_MO_SMITE);
					SmiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SmiteTimer -= diff;

			if (PowerWordShieldTimer <= diff)
			{
				DoCast(me, SPELL_MO_POWER_WORD_SHIELD, true);
				PowerWordShieldTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else PowerWordShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_oracleAI(creature);
    }
};

enum MurlocTidehunter
{
	SPELL_MT_FROST_NOVA = 11831,
	SPELL_MT_WATER_BOLT = 32011,
};

class npc_murloc_tidehunter : public CreatureScript
{
public:
    npc_murloc_tidehunter() : CreatureScript("npc_murloc_tidehunter") {}

    struct npc_murloc_tidehunterAI : public QuantumBasicAI
    {
        npc_murloc_tidehunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterBoltTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			WaterBoltTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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
					DoCast(target, SPELL_MT_WATER_BOLT);
					WaterBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else WaterBoltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_MT_FROST_NOVA, true);
				FrostNovaTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_tidehunterAI(creature);
    }
};

enum MurlocWarrior
{
	SPELL_MW_POISONOUS_STAB = 7357,
	SPELL_MW_KICK           = 11978,
};

class npc_murloc_warrior : public CreatureScript
{
public:
    npc_murloc_warrior() : CreatureScript("npc_murloc_warrior") {}

    struct npc_murloc_warriorAI : public QuantumBasicAI
    {
        npc_murloc_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KickTimer;
		uint32 PoisonousStabTimer;

        void Reset()
        {
			KickTimer = 1*IN_MILLISECONDS;
			PoisonousStabTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_MW_KICK);
				KickTimer = 4*IN_MILLISECONDS;
			}
			else KickTimer -= diff;

			if (PoisonousStabTimer <= diff)
			{
				DoCastVictim(SPELL_MW_POISONOUS_STAB);
				PoisonousStabTimer = 6*IN_MILLISECONDS;
			}
			else PoisonousStabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_warriorAI(creature);
    }
};

enum MurlocHunter
{
	SPELL_SUMMON_CRAWLER = 8656,
	SPELL_MH_THROW       = 10277,

	NPC_CRAWLER          = 6250,
};

class npc_murloc_hunter : public CreatureScript
{
public:
    npc_murloc_hunter() : CreatureScript("npc_murloc_hunter") {}

    struct npc_murloc_hunterAI : public QuantumBasicAI
    {
        npc_murloc_hunterAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ThrowTimer;

		SummonList Summons;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_SUMMON_CRAWLER);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summoned)
		{
			if (summoned->GetEntry() == NPC_CRAWLER)
			{
				summoned->SetReactState(REACT_DEFENSIVE);
				Summons.Summon(summoned);
			}
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
					DoCast(target, SPELL_MH_THROW);
					ThrowTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_hunterAI(creature);
    }
};

enum MurlocNetter
{
	SPELL_MN_SUNDER_ARMOR = 11971,
	SPELL_MN_NET          = 12024,
};

class npc_murloc_netter : public CreatureScript
{
public:
    npc_murloc_netter() : CreatureScript("npc_murloc_netter") {}

    struct npc_murloc_netterAI : public QuantumBasicAI
    {
        npc_murloc_netterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NetTimer;
		uint32 SunderArmorTimer;

        void Reset()
        {
			NetTimer = 0.5*IN_MILLISECONDS;
			SunderArmorTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MN_NET);
					NetTimer = 5*IN_MILLISECONDS;
				}
			}
			else NetTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_MN_SUNDER_ARMOR);
				SunderArmorTimer = 3*IN_MILLISECONDS;
			}
			else SunderArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_netterAI(creature);
    }
};

enum MurlocMinorOracle
{
	SPELL_MO_HEALING_WAVE   = 332,
	SPELL_MO_LIGHTNING_BOLT = 9532,
	SPELL_MO_WATER_SHIELD   = 34827,
};

class npc_murloc_minor_oracle : public CreatureScript
{
public:
    npc_murloc_minor_oracle() : CreatureScript("npc_murloc_minor_oracle") {}

    struct npc_murloc_minor_oracleAI : public QuantumBasicAI
    {
        npc_murloc_minor_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 WaterShieldTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			WaterShieldTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_MO_WATER_SHIELD, true);
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
					DoCast(target, SPELL_MO_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (WaterShieldTimer <= diff)
			{
				DoCast(me, SPELL_MO_WATER_SHIELD, true);
				WaterShieldTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else WaterShieldTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_MO_HEALING_WAVE);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_minor_oracleAI(creature);
    }
};

enum MurlocCoastrunner
{
	SPELL_MC_POISONOUS_STAB = 7357,
	SPELL_MC_KICK           = 11978,
};

class npc_murloc_coastrunner : public CreatureScript
{
public:
    npc_murloc_coastrunner() : CreatureScript("npc_murloc_coastrunner") {}

    struct npc_murloc_coastrunnerAI : public QuantumBasicAI
    {
        npc_murloc_coastrunnerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KickTimer;
		uint32 PoisonousStabTimer;

        void Reset()
        {
			KickTimer = 1*IN_MILLISECONDS;
			PoisonousStabTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_MC_KICK);
				KickTimer = 4*IN_MILLISECONDS;
			}
			else KickTimer -= diff;

			if (PoisonousStabTimer <= diff)
			{
				DoCastVictim(SPELL_MC_POISONOUS_STAB);
				PoisonousStabTimer = 6*IN_MILLISECONDS;
			}
			else PoisonousStabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_coastrunnerAI(creature);
    }
};

enum MurlocRaider
{
	SPELL_MR_NET                 = 6533,
	SPELL_MR_DEMORALIZING_MMRRGL = 50267,
};

class npc_murloc_raider : public CreatureScript
{
public:
    npc_murloc_raider() : CreatureScript("npc_murloc_raider") {}

    struct npc_murloc_raiderAI : public QuantumBasicAI
    {
        npc_murloc_raiderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NetTimer;
		uint32 DemoralizingMrglTimer;

        void Reset()
        {
			NetTimer = 0.5*IN_MILLISECONDS;
			DemoralizingMrglTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MR_NET);
					NetTimer = 5*IN_MILLISECONDS;
				}
			}
			else NetTimer -= diff;

			if (DemoralizingMrglTimer <= diff)
			{
				DoCastAOE(SPELL_MR_DEMORALIZING_MMRRGL);
				DemoralizingMrglTimer = 8*IN_MILLISECONDS;
			}
			else DemoralizingMrglTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murloc_raiderAI(creature);
    }
};

enum OldMurkEye
{
	SPELL_VOLATILE_INFECTION = 3584,
};

class npc_old_murk_eye : public CreatureScript
{
public:
    npc_old_murk_eye() : CreatureScript("npc_old_murk_eye") {}

    struct npc_old_murk_eyeAI : public QuantumBasicAI
    {
        npc_old_murk_eyeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VolatileInfectionTimer;

        void Reset()
        {
			VolatileInfectionTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VolatileInfectionTimer <= diff)
			{
				DoCastVictim(SPELL_VOLATILE_INFECTION);
				VolatileInfectionTimer = 5*IN_MILLISECONDS;
			}
			else VolatileInfectionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_old_murk_eyeAI(creature);
    }
};

enum Slark
{
	SPELL_S_THRASH       = 3417,
	SPELL_S_WATER_SHIELD = 34827,
};

class npc_slark : public CreatureScript
{
public:
    npc_slark() : CreatureScript("npc_slark") {}

    struct npc_slarkAI : public QuantumBasicAI
    {
        npc_slarkAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterShieldTimer;

        void Reset()
        {
			WaterShieldTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_S_THRASH, true);
			DoCast(me, SPELL_MO_WATER_SHIELD, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterShieldTimer <= diff)
			{
				DoCast(me, SPELL_S_WATER_SHIELD, true);
				WaterShieldTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else WaterShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slarkAI(creature);
    }
};

enum Brack
{
	SPELL_B_PIERCE_ARMOR = 6016,
	SPELL_B_HAMSTRING    = 9080,
	SPELL_B_STRIKE       = 11976,
};

class npc_brack : public CreatureScript
{
public:
    npc_brack() : CreatureScript("npc_brack") {}

    struct npc_brackAI : public QuantumBasicAI
    {
        npc_brackAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PierceArmorTimer;
		uint32 HamstringTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			PierceArmorTimer = 1*IN_MILLISECONDS;
			HamstringTimer = 3*IN_MILLISECONDS;
			StrikeTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
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

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_B_PIERCE_ARMOR);
				PierceArmorTimer = 4*IN_MILLISECONDS;
			}
			else PierceArmorTimer -= diff;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_B_HAMSTRING);
				HamstringTimer = 6*IN_MILLISECONDS;
			}
			else HamstringTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_B_STRIKE);
				StrikeTimer = 8*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brackAI(creature);
    }
};

enum CoyotePackleader
{
	SPELL_FURIOUS_HOWL   = 3149,
	SPELL_FESTERING_BITE = 16460,
};

class npc_coyote_packleader : public CreatureScript
{
public:
    npc_coyote_packleader() : CreatureScript("npc_coyote_packleader") {}

    struct npc_coyote_packleaderAI : public QuantumBasicAI
    {
        npc_coyote_packleaderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HowlTimer;
		uint32 FuriousHowlTimer;
		uint32 FesteringBiteTimer;

        void Reset()
        {
			HowlTimer = 0.5*IN_MILLISECONDS;
			FuriousHowlTimer = 0.5*IN_MILLISECONDS;
			FesteringBiteTimer = 3*IN_MILLISECONDS;

			me->GetMotionMaster()->MoveRandom(10.0f);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(SOUND_WOLF_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (HowlTimer <= diff && !me->IsInCombatActive())
			{
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_CREATURE_SPECIAL);
				me->PlayDistanceSound(SOUND_WOLF_HOWL);
				HowlTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else HowlTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FuriousHowlTimer <= diff)
			{
				DoCast(SPELL_FURIOUS_HOWL);
				FuriousHowlTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FuriousHowlTimer -= diff;

			if (FesteringBiteTimer <= diff)
			{
				DoCastVictim(SPELL_FESTERING_BITE);
				FesteringBiteTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FesteringBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coyote_packleaderAI(creature);
    }
};

enum Coyote
{
	SPELL_C_FESTERING_BITE = 16460,
};

class npc_coyote : public CreatureScript
{
public:
    npc_coyote() : CreatureScript("npc_coyote") {}

    struct npc_coyoteAI : public QuantumBasicAI
    {
        npc_coyoteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HowlTimer;
		uint32 FesteringBiteTimer;

        void Reset()
        {
			HowlTimer = 2*IN_MILLISECONDS;
			FesteringBiteTimer = 2*IN_MILLISECONDS;

			me->GetMotionMaster()->MoveRandom(10.0f);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(SOUND_WOLF_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (HowlTimer <= diff && !me->IsInCombatActive())
			{
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_CREATURE_SPECIAL);
				me->PlayDistanceSound(SOUND_WOLF_HOWL);
				HowlTimer = 1.5*MINUTE*IN_MILLISECONDS;
			}
			else HowlTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FesteringBiteTimer <= diff)
			{
				DoCastVictim(SPELL_FESTERING_BITE);
				FesteringBiteTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FesteringBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coyoteAI(creature);
    }
};

enum Goretusk
{
	SPELL_G_RUSHING_CHARGE = 6268,
};

class npc_goretusk : public CreatureScript
{
public:
    npc_goretusk() : CreatureScript("npc_goretusk") {}

    struct npc_goretuskAI : public QuantumBasicAI
    {
        npc_goretuskAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RushingChargeTimer;

        void Reset()
		{
			RushingChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RushingChargeTimer <= diff)
			{
				DoCast(me, SPELL_G_RUSHING_CHARGE);
				RushingChargeTimer = 4*IN_MILLISECONDS;
			}
			else RushingChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goretuskAI(creature);
    }
};

enum YoungGoretusk
{
	SPELL_YG_RUSHING_CHARGE = 6268,
};

class npc_young_goretusk : public CreatureScript
{
public:
    npc_young_goretusk() : CreatureScript("npc_young_goretusk") {}

    struct npc_young_goretuskAI : public QuantumBasicAI
    {
        npc_young_goretuskAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RushingChargeTimer;

        void Reset()
		{
			RushingChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RushingChargeTimer <= diff)
			{
				DoCast(me, SPELL_YG_RUSHING_CHARGE);
				RushingChargeTimer = 4*IN_MILLISECONDS;
			}
			else RushingChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_young_goretuskAI(creature);
    }
};

enum GreatGoretusk
{
	SPELL_GG_RUSHING_CHARGE = 6268,
};

class npc_great_goretusk : public CreatureScript
{
public:
    npc_great_goretusk() : CreatureScript("npc_great_goretusk") {}

    struct npc_great_goretuskAI : public QuantumBasicAI
    {
        npc_great_goretuskAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RushingChargeTimer;

        void Reset()
		{
			RushingChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RushingChargeTimer <= diff)
			{
				DoCast(me, SPELL_GG_RUSHING_CHARGE);
				RushingChargeTimer = 4*IN_MILLISECONDS;
			}
			else RushingChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_great_goretuskAI(creature);
    }
};

enum DustDevil
{
	SPELL_DD_GUST_OF_WIND = 6982,
};

class npc_dust_devil : public CreatureScript
{
public:
    npc_dust_devil() : CreatureScript("npc_dust_devil") {}

    struct npc_dust_devilAI : public QuantumBasicAI
    {
        npc_dust_devilAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GustOfWindTimer;

        void Reset()
        {
			GustOfWindTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GustOfWindTimer <= diff)
			{
				DoCastAOE(SPELL_DD_GUST_OF_WIND);
				GustOfWindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GustOfWindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dust_devilAI(creature);
    }
};

enum Fleshripper
{
	SPELL_MUSCLE_TEAR = 12166,
};

class npc_fleshripper : public CreatureScript
{
public:
    npc_fleshripper() : CreatureScript("npc_fleshripper") {}

    struct npc_fleshripperAI : public QuantumBasicAI
    {
        npc_fleshripperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MuscleTearTimer;

        void Reset()
        {
			MuscleTearTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MuscleTearTimer <= diff)
			{
				DoCastVictim(SPELL_MUSCLE_TEAR);
				MuscleTearTimer = 3*IN_MILLISECONDS;
			}
			else MuscleTearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fleshripperAI(creature);
    }
};

enum GreaterFleshripper
{
	SPELL_GF_MUSCLE_TEAR = 12166,
};

class npc_greater_fleshripper : public CreatureScript
{
public:
    npc_greater_fleshripper() : CreatureScript("npc_greater_fleshripper") {}

    struct npc_greater_fleshripperAI : public QuantumBasicAI
    {
        npc_greater_fleshripperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MuscleTearTimer;

        void Reset()
        {
			MuscleTearTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MuscleTearTimer <= diff)
			{
				DoCastVictim(SPELL_GF_MUSCLE_TEAR);
				MuscleTearTimer = 3*IN_MILLISECONDS;
			}
			else MuscleTearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_fleshripperAI(creature);
    }
};

enum YoungFleshripper
{
	SPELL_YF_MUSCLE_TEAR = 12166,
};

class npc_young_fleshripper : public CreatureScript
{
public:
    npc_young_fleshripper() : CreatureScript("npc_young_fleshripper") {}

    struct npc_young_fleshripperAI : public QuantumBasicAI
    {
        npc_young_fleshripperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MuscleTearTimer;

        void Reset()
        {
			MuscleTearTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MuscleTearTimer <= diff)
			{
				DoCastVictim(SPELL_YF_MUSCLE_TEAR);
				MuscleTearTimer = 3*IN_MILLISECONDS;
			}
			else MuscleTearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_young_fleshripperAI(creature);
    }
};

enum HarvestGolem
{
	SPELL_HG_STRIKE = 11976,
};

class npc_harvest_golem : public CreatureScript
{
public:
    npc_harvest_golem() : CreatureScript("npc_harvest_golem") {}

    struct npc_harvest_golemAI : public QuantumBasicAI
    {
        npc_harvest_golemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_PERIODIC_LEECH, true);
		}

		uint32 StrikeTimer;

        void Reset()
        {
			StrikeTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_HG_STRIKE);
				StrikeTimer = 5*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_harvest_golemAI(creature);
    }
};

enum HarvestWatcher
{
	SPELL_HW_WIDE_SLASH = 7342,
};

class npc_harvest_watcher : public CreatureScript
{
public:
    npc_harvest_watcher() : CreatureScript("npc_harvest_watcher") {}

    struct npc_harvest_watcherAI : public QuantumBasicAI
    {
        npc_harvest_watcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_PERIODIC_LEECH, true);
		}

		uint32 WideSlashTimer;

        void Reset()
        {
			WideSlashTimer = 2*IN_MILLISECONDS;

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

			if (WideSlashTimer <= diff)
			{
				DoCastVictim(SPELL_HW_WIDE_SLASH);
				WideSlashTimer = 5*IN_MILLISECONDS;
			}
			else WideSlashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_harvest_watcherAI(creature);
    }
};

enum Patchwerk
{
	SPELL_PATCH_FRENZY = 28131,
};

class boss_patchwerk_eh : public CreatureScript
{
public:
    boss_patchwerk_eh() : CreatureScript("boss_patchwerk_eh") {}

    struct boss_patchwerk_ehAI : public QuantumBasicAI
    {
        boss_patchwerk_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrenzyTimer;

        void Reset()
        {
			FrenzyTimer = 5*IN_MILLISECONDS;

			me->SetCorpseDelay(0);

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

			if (FrenzyTimer <= diff)
			{
				DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				DoCast(me, SPELL_PATCH_FRENZY);
				FrenzyTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrenzyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_patchwerk_ehAI(creature);
    }
};

enum ValkyrBattleMaiden
{
	SPELL_TOUCH_OF_THE_VALKYR = 57547,
};

class npc_valkyr_battle_maiden_eh : public CreatureScript
{
public:
    npc_valkyr_battle_maiden_eh() : CreatureScript("npc_valkyr_battle_maiden_eh") {}

    struct npc_valkyr_battle_maiden_ehAI : public QuantumBasicAI
    {
        npc_valkyr_battle_maiden_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TouchOfTheValkyrTimer;

        void Reset()
        {
			TouchOfTheValkyrTimer = 0.5*IN_MILLISECONDS;

			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
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
        return new npc_valkyr_battle_maiden_ehAI(creature);
    }
};

enum TerrifyingAbomination
{
	SPELL_TF_SCOURGE_HOOK = 50335,
	SPELL_TF_CLEAVE       = 15496,
};

class npc_terrifying_abomination : public CreatureScript
{
public:
    npc_terrifying_abomination() : CreatureScript("npc_terrifying_abomination") {}

    struct npc_terrifying_abominationAI : public QuantumBasicAI
    {
        npc_terrifying_abominationAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_TF_SCOURGE_HOOK);

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
				DoCastVictim(SPELL_TF_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_terrifying_abominationAI(creature);
    }
};

enum HornCall
{
	SOUND_HORN_CALL = 14969,
};

class npc_valkyr_battle_maiden_chpt1_eh : public CreatureScript
{
public:
    npc_valkyr_battle_maiden_chpt1_eh() : CreatureScript("npc_valkyr_battle_maiden_chpt1_eh") {}

    struct npc_valkyr_battle_maiden_chpt1_ehAI : public QuantumBasicAI
    {
        npc_valkyr_battle_maiden_chpt1_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HornCallTimer;

        void Reset()
        {
			HornCallTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, 257);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (HornCallTimer <= diff && !me->IsInCombatActive())
			{
				me->PlayDistanceSound(SOUND_HORN_CALL);
				me->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_01);
				HornCallTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else HornCallTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valkyr_battle_maiden_chpt1_ehAI(creature);
    }
};

enum Coldwraith
{
	SPELL_FROST_CHANNEL = 45846,
	SPELL_CW_FROSTBOLT  = 15043,
};

class npc_coldwraith_eh : public CreatureScript
{
public:
    npc_coldwraith_eh() : CreatureScript("npc_coldwraith_eh") {}

    struct npc_coldwraith_ehAI : public QuantumBasicAI
    {
        npc_coldwraith_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_FROST_CHANNEL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
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
					DoCast(target, SPELL_CW_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coldwraith_ehAI(creature);
    }
};

enum SmallCragBoar
{
	SPELL_BOAR_CHARGE = 44530,
};

class npc_small_crag_boar : public CreatureScript
{
public:
    npc_small_crag_boar() : CreatureScript("npc_small_crag_boar") {}

    struct npc_small_crag_boarAI : public QuantumBasicAI
    {
        npc_small_crag_boarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoarChargeTimer;

        void Reset()
        {
			BoarChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoarChargeTimer <= diff)
			{
				DoCast(me, SPELL_BOAR_CHARGE);
				BoarChargeTimer = 4*IN_MILLISECONDS;
			}
			else BoarChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_small_crag_boarAI(creature);
    }
};

void AddSC_eastern_kingdoms_npc_scripts()
{
	new npc_skullsplitter_beastmaster();
	new npc_skullsplitter_scout();
	new npc_venture_co_tinkerer();
	new npc_venture_co_foreman();
	new npc_venture_co_strip_miner();
	new npc_boulderfist_magus();
	new npc_boulderfist_shaman();
	new npc_zanzil_witch_doctor();
	new npc_zanzil_zombie();
	new npc_zanzil_hunter();
	new npc_zanzil_naga();
	new npc_crazed_dragonhawk();
	new npc_feral_dragonhawk_hatchling();
	new npc_darnassian_scout();
	new npc_springpaw_stalker();
	new npc_elder_springpaw();
	new npc_silvermoon_apprentice();
	new npc_plaguebone_pillager();
	new npc_rotlimb_cannibal();
	new npc_wretched_thug();
	new npc_wretched_urchin();
	new npc_wretched_hooligan();
	new npc_mana_stalker();
	new npc_manawraith();
	new npc_ley_keeper_velania();
	new npc_rotlimb_marauder();
	new npc_darkwraith();
	new npc_asngershade();
	new npc_gorlash();
	new npc_squire_edwards_eh();
	new npc_amal_thazad_eh();
	new npc_disciple_of_frost_eh();
	new npc_shattered_sun_marksman();
	new npc_drywhisker_surveyor();
	new npc_dark_iron_shadowcaster();
	new npc_syndicate_pathstalker();
	new npc_coldridge_mountaineer();
	new npc_surena_caledon();
	new npc_highvale_outrunner();
	new npc_highvale_scout();
	new npc_highvale_marksman();
	new npc_highvale_ranger();
	new npc_saltscale_oracle();
	new npc_frostwolf_bowman();
	new npc_acherus_geist();
	new npc_syndicate_conjuror();
	new npc_minor_voidwalker();
	new npc_jaguero_stalker();
	new npc_weakened_morbent_fel();
	new npc_stormpike_ram_rider_commander();
	new npc_cannibal_ghoul();
	new npc_gibbering_ghoul();
	new npc_diseased_flayer();
	new npc_putrid_gargoyle();
	new npc_putrid_shrieker();
	new npc_addled_leper();
	new npc_duggan_wildhammer();
	new npc_nathanos_blightcaller();
	new npc_bluegill_raider();
	new npc_bloodsail_mage();
	new npc_bloodsail_raider();
	new npc_bloodsail_warlock();
	new npc_bloodsail_swashbuckler();
	new npc_bloodsail_sea_dog();
	new npc_bloodsail_deckhand();
	new npc_bloodsail_swabby();
	new npc_bloodsail_elder_magus();
	new npc_fleet_master_firallon();
	new npc_ironpatch();
	new npc_captain_keelhaul();
	new npc_garr_salthoof();
	new npc_pretty_boy_duncan();
	new npc_captain_stillwater();
	new npc_brutus();
	new npc_weldon_barov();
	new npc_high_priestess_macdonnell();
	new npc_flint_shadowmore();
	new npc_syndicate_watchman();
	new npc_skeletal_flayer();
	new npc_slavering_ghoul();
	new npc_defias_bandit();
	new npc_defias_rogue_wizard();
	new npc_chillwind_lieutenant();
	new npc_frigid_lieutenant();
	new npc_glacial_lieutenant();
	new npc_syndicate_magus();
	new npc_rumbling_exile();
	new npc_daggerspine_raider();
	new npc_daggerspine_sorceress();
	new npc_daggerspine_shorehunter();
	new npc_lieutenant_haggerdin();
	new npc_stormpike_battleguard();
	new npc_southshore_guard();
	new npc_thanthaldis_snowgleam();
	new npc_syndicate_thief();
	new npc_syndicate_footpad();
	new npc_drunken_footpad();
	new npc_gray_bear();
	new npc_vicious_gray_bear();
	new npc_elder_gray_bear();
	new npc_forest_moss_creeper();
	new npc_giant_moss_creeper();
	new npc_ferocious_yeti();
	new npc_cave_yeti();
	new npc_starving_mountain_lion();
	new npc_feral_mountain_lion();
	new npc_torn_fin_oracle();
	new npc_torn_fin_tidehunter();
	new npc_torn_fin_muckdweller();
	new npc_torn_fin_coastrunner();
	new npc_hillsbrad_sentry();
	new npc_hillsbrad_miner();
	new npc_dun_garok_soldier();
	new npc_miner_hackett();
	new npc_foreman_bonds();
	new npc_rabid_dire_wolf();
	new npc_shore_crawler();
	new npc_sand_crawler();
	new npc_sea_crawler();
	new npc_murloc_oracle();
	new npc_murloc_tidehunter();
	new npc_murloc_warrior();
	new npc_murloc_hunter();
	new npc_murloc_netter();
	new npc_murloc_minor_oracle();
	new npc_murloc_coastrunner();
	new npc_murloc_raider();
	new npc_old_murk_eye();
	new npc_slark();
	new npc_brack();
	new npc_coyote_packleader();
	new npc_coyote();
	new npc_goretusk();
	new npc_young_goretusk();
	new npc_great_goretusk();
	new npc_dust_devil();
	new npc_fleshripper();
	new npc_greater_fleshripper();
	new npc_young_fleshripper();
	new npc_harvest_golem();
	new npc_harvest_watcher();
	new boss_patchwerk_eh();
	new npc_valkyr_battle_maiden_eh();
	new npc_terrifying_abomination();
	new npc_valkyr_battle_maiden_chpt1_eh();
	new npc_coldwraith_eh();
	new npc_small_crag_boar();
}