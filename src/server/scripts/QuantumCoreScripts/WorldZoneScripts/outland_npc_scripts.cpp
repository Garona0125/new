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
#include "QuantumGossip.h"

enum WarpChaser
{
	SPELL_PHASING_INVISIBILITY = 32942,
	SPELL_WARP                 = 32920,
	SPELL_WC_VENOMOUS_BITE     = 32739,
	SPELL_WARP_CHARGE          = 37417,
};

class npc_warp_chaser : public CreatureScript
{
public:
    npc_warp_chaser() : CreatureScript("npc_warp_chaser") {}

    struct npc_warp_chaserAI : public QuantumBasicAI
    {
        npc_warp_chaserAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VenomousBiteTimer;
		uint32 WarpTimer;

        void Reset()
        {
			DoCast(me, SPELL_PHASING_INVISIBILITY);

			VenomousBiteTimer = 2*IN_MILLISECONDS;
			WarpTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_WARP_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomousBiteTimer <= diff)
			{
				DoCastVictim(SPELL_WC_VENOMOUS_BITE);
				VenomousBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else VenomousBiteTimer -= diff;

			if (WarpTimer <= diff)
			{
				DoCast(me, SPELL_WARP);
				WarpTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WarpTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warp_chaserAI(creature);
    }
};

enum TalbukStag
{
	SPELL_TS_GORE = 32019,
};

class npc_talbuk_stag : public CreatureScript
{
public:
	npc_talbuk_stag() : CreatureScript("npc_talbuk_stag") {}

    struct npc_talbuk_stagAI : public QuantumBasicAI
    {
        npc_talbuk_stagAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_TS_GORE);
				GoreTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_talbuk_stagAI(creature);
    }
};

enum Clefthoof
{
	SPELL_HOOF_STOMP = 32023,
};

class npc_clefthoof : public CreatureScript
{
public:
	npc_clefthoof() : CreatureScript("npc_clefthoof") {}

    struct npc_clefthoofAI : public QuantumBasicAI
    {
        npc_clefthoofAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HoofStompTimer;

        void Reset()
        {
			HoofStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HoofStompTimer <= diff)
			{
				DoCastAOE(SPELL_HOOF_STOMP);
				HoofStompTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else HoofStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_clefthoofAI(creature);
    }
};

class npc_clefthoof_bull : public CreatureScript
{
public:
	npc_clefthoof_bull() : CreatureScript("npc_clefthoof_bull") {}

    struct npc_clefthoof_bullAI : public QuantumBasicAI
    {
        npc_clefthoof_bullAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HoofStompTimer;

        void Reset()
        {
			HoofStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HoofStompTimer <= diff)
			{
				DoCastAOE(SPELL_HOOF_STOMP);
				HoofStompTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else HoofStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_clefthoof_bullAI(creature);
    }
};

enum TalbukPatriarch
{
	SPELL_TP_GORE       = 32019,
	SPELL_TALBUK_STRIKE = 32020,
};

class npc_talbuk_patriarch : public CreatureScript
{
public:
	npc_talbuk_patriarch() : CreatureScript("npc_talbuk_patriarch") {}

    struct npc_talbuk_patriarchAI : public QuantumBasicAI
    {
        npc_talbuk_patriarchAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;
		uint32 TalbukStrikeTimer;

        void Reset()
        {
			GoreTimer = 2*IN_MILLISECONDS;
			TalbukStrikeTimer = 4*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_TP_GORE);
				GoreTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else GoreTimer -= diff;

			if (TalbukStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TALBUK_STRIKE);
				TalbukStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TalbukStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_talbuk_patriarchAI(creature);
    }
};

class npc_talbuk_thorngrazer : public CreatureScript
{
public:
	npc_talbuk_thorngrazer() : CreatureScript("npc_talbuk_thorngrazer") {}

    struct npc_talbuk_thorngrazerAI : public QuantumBasicAI
    {
        npc_talbuk_thorngrazerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;
		uint32 TalbukStrikeTimer;

        void Reset()
        {
			GoreTimer = 2*IN_MILLISECONDS;
			TalbukStrikeTimer = 4*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_TP_GORE);
				GoreTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else GoreTimer -= diff;

			if (TalbukStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TALBUK_STRIKE);
				TalbukStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TalbukStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_talbuk_thorngrazerAI(creature);
    }
};

enum InjuredTalbuk
{
	SPELL_IT_GORE = 32019,
};

class npc_injured_talbuk : public CreatureScript
{
public:
	npc_injured_talbuk() : CreatureScript("npc_injured_talbuk") {}

    struct npc_injured_talbukAI : public QuantumBasicAI
    {
        npc_injured_talbukAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_IT_GORE);
				GoreTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_talbukAI(creature);
    }
};

enum WildElekk
{
	SPELL_WE_GORE = 32019,
};

class npc_wild_elekk : public CreatureScript
{
public:
	npc_wild_elekk() : CreatureScript("npc_wild_elekk") {}

    struct npc_wild_elekkAI : public QuantumBasicAI
    {
        npc_wild_elekkAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GoreTimer;

        void Reset()
        {
			GoreTimer = 2*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_WE_GORE);
				GoreTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else GoreTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wild_elekkAI(creature);
    }
};

enum Windroc
{
	SPELL_EAGLE_CLAW = 30285,
};

class npc_windroc : public CreatureScript
{
public:
	npc_windroc() : CreatureScript("npc_windroc") {}

    struct npc_windrocAI : public QuantumBasicAI
    {
        npc_windrocAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EagleClawTimer;

        void Reset()
        {
			EagleClawTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EagleClawTimer <= diff)
			{
				DoCastVictim(SPELL_EAGLE_CLAW);
				EagleClawTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else EagleClawTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_windrocAI(creature);
    }
};

enum DustHowler
{
	SPELL_HOWLING_DUST = 32017,
};

class npc_dust_howler : public CreatureScript
{
public:
	npc_dust_howler() : CreatureScript("npc_dust_howler") {}

    struct npc_dust_howlerAI : public QuantumBasicAI
    {
        npc_dust_howlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HowlingDustTimer;

        void Reset()
        {
			HowlingDustTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HowlingDustTimer <= diff)
			{
				DoCastVictim(SPELL_HOWLING_DUST);
				HowlingDustTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else HowlingDustTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dust_howlerAI(creature);
    }
};

enum BoulderfistCrusher
{
	SPELL_PULVERIZE = 742,
};

class npc_boulderfist_crusher : public CreatureScript
{
public:
	npc_boulderfist_crusher() : CreatureScript("npc_boulderfist_crusher") {}

    struct npc_boulderfist_crusherAI : public QuantumBasicAI
    {
        npc_boulderfist_crusherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PulverizeTimer;

        void Reset()
        {
			PulverizeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PulverizeTimer <= diff)
			{
				DoCastAOE(SPELL_PULVERIZE);
				PulverizeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PulverizeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_boulderfist_crusherAI(creature);
    }
};

enum BoulderfistMystic
{
	SPELL_BM_LIGHTNING_BOLT = 9532,
	SPELL_BM_EARTH_SHOCK    = 13281,
	SPELL_HEALING_TOUCH     = 11431,
};

class npc_boulderfist_mystic : public CreatureScript
{
public:
	npc_boulderfist_mystic() : CreatureScript("npc_boulderfist_mystic") {}

    struct npc_boulderfist_mysticAI : public QuantumBasicAI
    {
        npc_boulderfist_mysticAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 EarthShockTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			EarthShockTimer = 2*IN_MILLISECONDS;

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
					DoCast(target, SPELL_BM_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (EarthShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BM_EARTH_SHOCK);
					EarthShockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else EarthShockTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, SPELL_HEALING_TOUCH);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_boulderfist_mysticAI(creature);
    }
};

enum WindyreedScavenger
{
	SPELL_WS_THROW = 10277,
	SPELL_WS_NET   = 12024,
};

class npc_windyreed_scavenger : public CreatureScript
{
public:
	npc_windyreed_scavenger() : CreatureScript("npc_windyreed_scavenger") {}

    struct npc_windyreed_scavengerAI : public QuantumBasicAI
    {
        npc_windyreed_scavengerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;
		uint32 NetTimer;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;
			NetTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_WS_THROW);
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
					DoCast(target, SPELL_WS_THROW);
					ThrowTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ThrowTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WS_NET);
					NetTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_windyreed_scavengerAI(creature);
    }
};

enum WindyreedWretch
{
	SPELL_DISEASE_TOUCH = 3234,
	SPELL_CREEPING_MOLD = 6278,
};

class npc_windyreed_wretch : public CreatureScript
{
public:
	npc_windyreed_wretch() : CreatureScript("npc_windyreed_wretch") {}

    struct npc_windyreed_wretchAI : public QuantumBasicAI
    {
        npc_windyreed_wretchAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DiseaseTouchTimer;
		uint32 CreepingMoldTimer;

        void Reset()
        {
			DiseaseTouchTimer = 2*IN_MILLISECONDS;
			CreepingMoldTimer = 4*IN_MILLISECONDS;
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

			if (CreepingMoldTimer <= diff)
			{
				DoCastVictim(SPELL_CREEPING_MOLD);
				CreepingMoldTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CreepingMoldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_windyreed_wretchAI(creature);
    }
};

enum Akama
{
	SPELL_AK_CHAIN_LIGHTNING    = 39945,
	SPELL_AK_DESTRUCTIVE_POISON = 40874,
	SPELL_AK_STEALTH            = 34189,
};

class npc_distraction_of_akama : public CreatureScript
{
public:
    npc_distraction_of_akama() : CreatureScript("npc_distraction_of_akama") {}

    struct npc_distraction_of_akamaAI : public QuantumBasicAI
    {
        npc_distraction_of_akamaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainLightningTimer;
		uint32 DestructivePoisonTimer;

        void Reset()
        {
			ChainLightningTimer = 0.5*IN_MILLISECONDS;
			DestructivePoisonTimer = 3*IN_MILLISECONDS;

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
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AK_CHAIN_LIGHTNING);
					ChainLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ChainLightningTimer -= diff;

			if (DestructivePoisonTimer <= diff)
			{
				DoCastVictim(SPELL_AK_DESTRUCTIVE_POISON);
				DestructivePoisonTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DestructivePoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_distraction_of_akamaAI(creature);
    }
};

enum ShadowlordDeathwail
{
	NPC_SOULSTEALER         = 22061,
	NPC_FELLFIRE_SUMMONER   = 22121,
	NPC_DEATHWAIL_TRIGGER   = 22096,
	SPELL_SUMMONER_FELLFIRE = 35769,
	SPELL_SD_DEATH_COIL     = 32709,
	SPELL_SD_FEAR           = 27641,
	SPELL_SD_FEL_FIREBALL   = 38312, // Spell Doesn`t work
	SPELL_SD_SHADOW_BOLT    = 12471,
	SPELL_SD_SHADOW_VOLLEY  = 15245,
	DEATHWAIL_MOUNT_ID      = 20684,
};

#define YELL_AGGRO "You will never get the Heart of Fury! Its power belongs to Illidan!"
#define YELL_DEATH "Master... I\'ve failed you..."

class npc_shadowlord_deathwail : public CreatureScript
{
public:
    npc_shadowlord_deathwail() : CreatureScript("npc_shadowlord_deathwail") {}

    struct npc_shadowlord_deathwailAI : public QuantumBasicAI
    {
        npc_shadowlord_deathwailAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 FearTimer;
		uint32 ShadowBoltTimer;
		uint32 DeathCoilTimer;
		uint32 ShadowVolleyTimer;

		SummonList Summons;

        void Reset()
        {
			FearTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 3*IN_MILLISECONDS;
			DeathCoilTimer = 4*IN_MILLISECONDS;
			ShadowVolleyTimer = 6*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->Mount(DEATHWAIL_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->MonsterYell(YELL_AGGRO, LANG_UNIVERSAL, 0);
			me->RemoveMount();
			me->SummonCreature(NPC_FELLFIRE_SUMMONER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 180000);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->MonsterYell(YELL_DEATH, LANG_UNIVERSAL, 0);
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_FELLFIRE_SUMMONER)
			{
				Summons.Summon(summon);

				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.f))
					summon->SetInCombatWith(target);
			}
		}

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
					DoCast(target, SPELL_SD_FEAR);
					FearTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FearTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SD_SHADOW_BOLT);
					ShadowBoltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SD_DEATH_COIL);
					DeathCoilTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else DeathCoilTimer -= diff;

			if (ShadowVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SD_SHADOW_VOLLEY);
					ShadowVolleyTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else ShadowVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowlord_deathwailAI(creature);
    }
};

class npc_felfire_summoner : public CreatureScript
{
public:
    npc_felfire_summoner() : CreatureScript("npc_felfire_summoner") {}

    struct npc_felfire_summonerAI : public QuantumBasicAI
    {
        npc_felfire_summonerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SUMMONER_FELLFIRE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felfire_summonerAI(creature);
    }
};

enum GanargWarpTinker
{
	SPELL_MANA_BOMB    = 36846,
	SPELL_STEAL_WEAPON = 36208,
};

class npc_ganarg_warp_tinker : public CreatureScript
{
public:
    npc_ganarg_warp_tinker() : CreatureScript("npc_ganarg_warp_tinker") {}

    struct npc_ganarg_warp_tinkerAI : public QuantumBasicAI
    {
        npc_ganarg_warp_tinkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StealWeaponTimer;
		uint32 ManaBombTimer;

        void Reset()
        {
			StealWeaponTimer = 2*IN_MILLISECONDS;
			ManaBombTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StealWeaponTimer <= diff)
			{
				DoCastVictim(SPELL_STEAL_WEAPON);
				StealWeaponTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else StealWeaponTimer -= diff;

			if (ManaBombTimer <= diff)
			{
				DoCastVictim(SPELL_MANA_BOMB);
				ManaBombTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ManaBombTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ganarg_warp_tinkerAI(creature);
    }
};

enum MoargWarpMaster
{
	SPELL_SLIME_SPRAY = 36486,
};

class npc_moarg_warp_master : public CreatureScript
{
public:
    npc_moarg_warp_master() : CreatureScript("npc_moarg_warp_master") {}

    struct npc_moarg_warp_masterAI : public QuantumBasicAI
    {
        npc_moarg_warp_masterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SlimeSprayTimer;

        void Reset()
        {
			SlimeSprayTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SlimeSprayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SLIME_SPRAY);
					SlimeSprayTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SlimeSprayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_moarg_warp_masterAI(creature);
    }
};

enum ManaBeast
{
	SPELL_MANA_BURN = 36484,
};

class npc_mana_beast : public CreatureScript
{
public:
    npc_mana_beast() : CreatureScript("npc_mana_beast") {}

    struct npc_mana_beastAI : public QuantumBasicAI
    {
        npc_mana_beastAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ManaBurnTimer;

        void Reset()
        {
			ManaBurnTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MANA_BURN);
					ManaBurnTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ManaBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_beastAI(creature);
    }
};

enum LashhanWindwalker
{
	SPELL_LIGHTNING_SHIELD = 12550,
	SPELL_LIGHTNING_BOLT   = 9532,
};

class npc_lashhan_windwalker : public CreatureScript
{
public:
    npc_lashhan_windwalker() : CreatureScript("npc_lashhan_windwalker") {}

    struct npc_lashhan_windwalkerAI : public QuantumBasicAI
    {
        npc_lashhan_windwalkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 LightningShieldTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			LightningShieldTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_LIGHTNING_SHIELD);
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

			if (LightningShieldTimer <= diff)
			{
				DoCast(me, SPELL_LIGHTNING_SHIELD);
				LightningShieldTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else LightningShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lashhan_windwalkerAI(creature);
    }
};

enum ArchMageXintor
{
	SPELL_ARCANE_MISSILES = 15736,
	SPELL_AX_FIREBALL     = 20823,
	SPELL_AX_FROSTBOLT    = 29457,
	SPELL_AX_BLIZZARD     = 29458,
	SPELL_AX_PYROBLAST    = 29459,
	SPELL_ICE_BARRIER     = 33245,
	SPELL_AX_GHOST_VISUAL = 28002,

	SAY_AX_BLIZZARD       = 0,
	SAY_AX_PYROBLAST      = 1,
	SAY_AX_FIREBALL       = 2,
	SAY_AX_ICE_BARRIER    = 3,
	SAY_AX_FROSTBOLT      = 4,
};

class npc_arch_mage_xintor : public CreatureScript
{
public:
	npc_arch_mage_xintor() : CreatureScript("npc_arch_mage_xintor") {}

    struct npc_arch_mage_xintorAI : public QuantumBasicAI
    {
        npc_arch_mage_xintorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ArcaneMissilesTimer;
		uint32 FireballTimer;
		uint32 FrostboltTimer;
		uint32 BlizzardTimer;
		uint32 PyroblastTimer;
		uint32 IceBarrierTimer;

        void Reset()
        {
			ArcaneMissilesTimer = 2*IN_MILLISECONDS;
			FireballTimer = 3*IN_MILLISECONDS;
			FrostboltTimer = 4*IN_MILLISECONDS;
			BlizzardTimer = 5*IN_MILLISECONDS;
			PyroblastTimer = 6*IN_MILLISECONDS;
			IceBarrierTimer = 7*IN_MILLISECONDS;

			DoCast(me, SPELL_AX_GHOST_VISUAL);

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
				DoCastVictim(SPELL_ARCANE_MISSILES);
				ArcaneMissilesTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ArcaneMissilesTimer -= diff;

			if (FireballTimer <= diff)
			{
				DoCastVictim(SPELL_AX_FIREBALL);
				Talk(SAY_AX_FIREBALL);
				FireballTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FireballTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				DoCastVictim(SPELL_AX_FROSTBOLT);
				Talk(SAY_AX_FROSTBOLT);
				FrostboltTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else FrostboltTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				DoCastAOE(SPELL_AX_BLIZZARD);
				Talk(SAY_AX_BLIZZARD);
				BlizzardTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else BlizzardTimer -= diff;

			if (PyroblastTimer <= diff)
			{
				DoCastVictim(SPELL_AX_PYROBLAST);
				Talk(SAY_AX_PYROBLAST);
				PyroblastTimer = urand(12*IN_MILLISECONDS, 13*IN_MILLISECONDS);
			}
			else PyroblastTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
			{
				if (!me->HasAuraEffect(SPELL_ICE_BARRIER, 0))
				{
					DoCast(me, SPELL_ICE_BARRIER);
					Talk(SAY_AX_ICE_BARRIER);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arch_mage_xintorAI(creature);
    }
};

enum BladespireOgres
{
	SPELL_BLOODMAUL_INTOXICATION = 35240,
	SPELL_BS_LIGHTNING_BOLT      = 26098,
	SPELL_BS_LIGHTNING_SHIELD    = 12550,
	SPELL_FIRE_NOVA_TOTEM        = 32062,
	SPELL_TENDERIZE              = 37596,
	SPELL_MEAT_SLAP              = 37597,
	SPELL_MIGHTY_CHARGE          = 37777,
	SPELL_THUNDERCLAP            = 8078,
	SPELL_CLEAVE                 = 15496,
};

class npc_bladespire_shaman : public CreatureScript
{
public:
    npc_bladespire_shaman() : CreatureScript("npc_bladespire_shaman") {}

    struct npc_bladespire_shamanAI : public QuantumBasicAI
    {
        npc_bladespire_shamanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningShieldTimer;
		uint32 LightningBoltTimer;
		uint32 BloodmaulIntoxicationTimer;
		uint32 FireNovaTotemTimer;

        void Reset()
        {
			LightningShieldTimer = 0.1*IN_MILLISECONDS;
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			BloodmaulIntoxicationTimer = 3*IN_MILLISECONDS;
			FireNovaTotemTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (LightningShieldTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_BS_LIGHTNING_SHIELD);
				LightningShieldTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else LightningShieldTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Lightning Shield Buff Check in Combat
			if (!me->HasAura(SPELL_BS_LIGHTNING_SHIELD))
				DoCast(me, SPELL_BS_LIGHTNING_SHIELD);

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BS_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (BloodmaulIntoxicationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLOODMAUL_INTOXICATION);
					BloodmaulIntoxicationTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else BloodmaulIntoxicationTimer -= diff;

			if (FireNovaTotemTimer <= diff)
			{
				DoCast(me, SPELL_FIRE_NOVA_TOTEM);
				FireNovaTotemTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FireNovaTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bladespire_shamanAI(creature);
    }
};

class npc_bladespire_cook : public CreatureScript
{
public:
    npc_bladespire_cook() : CreatureScript("npc_bladespire_cook") {}

    struct npc_bladespire_cookAI : public QuantumBasicAI
    {
        npc_bladespire_cookAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TenderizeTimer;
		uint32 MeatSlapTimer;
		uint32 BloodmaulIntoxicationTimer;

        void Reset()
        {
			TenderizeTimer = 1*IN_MILLISECONDS;
			BloodmaulIntoxicationTimer = 2*IN_MILLISECONDS;
			MeatSlapTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TenderizeTimer <= diff)
			{
				DoCastVictim(SPELL_TENDERIZE);
				TenderizeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else TenderizeTimer -= diff;

			if (BloodmaulIntoxicationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLOODMAUL_INTOXICATION);
					BloodmaulIntoxicationTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else BloodmaulIntoxicationTimer -= diff;

			if (MeatSlapTimer <= diff)
			{
				DoCastVictim(SPELL_MEAT_SLAP);
				MeatSlapTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MeatSlapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bladespire_cookAI(creature);
    }
};

class npc_bladespire_champion : public CreatureScript
{
public:
    npc_bladespire_champion() : CreatureScript("npc_bladespire_champion") {}

    struct npc_bladespire_championAI : public QuantumBasicAI
    {
        npc_bladespire_championAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThunderclapTimer;
		uint32 BloodmaulIntoxicationTimer;

        void Reset()
        {
			ThunderclapTimer = 2*IN_MILLISECONDS;
			BloodmaulIntoxicationTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERCLAP);
				ThunderclapTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ThunderclapTimer -= diff;

			if (BloodmaulIntoxicationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLOODMAUL_INTOXICATION);
					BloodmaulIntoxicationTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else BloodmaulIntoxicationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bladespire_championAI(creature);
    }
};

class npc_bladespire_sober_defender : public CreatureScript
{
public:
    npc_bladespire_sober_defender() : CreatureScript("npc_bladespire_sober_defender") {}

    struct npc_bladespire_sober_defenderAI : public QuantumBasicAI
    {
        npc_bladespire_sober_defenderAI(Creature* creature) : QuantumBasicAI(creature){}

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
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bladespire_sober_defenderAI(creature);
    }
};

enum Lakaan
{
	SPELL_CORRUPTED_WATER_ELEMENTAL = 36826,
	SPELL_WATER_BUBBLE_VISUAL       = 51248, // Need to find original spell id
	SPELL_BLUE_BEAM                 = 30945,
	SPELL_WATER_BOLT                = 32011,

	NPC_CORRUPT_WATER_TOTEM         = 21420,
	NPC_CORRUPTED_WATER_ELEMENTAL   = 21428,
};

class npc_lakaan : public CreatureScript
{
public:
    npc_lakaan() : CreatureScript("npc_lakaan") {}

    struct npc_lakaanAI : public QuantumBasicAI
    {
        npc_lakaanAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 WaterBubbleTimer;
		uint32 WaterBoltTimer;
		uint32 SummonWaterElementalTimer;

		SummonList Summons;

        void Reset()
        {
			WaterBubbleTimer = 1*IN_MILLISECONDS;
			WaterBoltTimer = 2*IN_MILLISECONDS;
			SummonWaterElementalTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_BLUE_BEAM, true);

			TotemRespawner();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_WATER_BUBBLE_VISUAL);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
			TotemDespawner();
		}

		void TotemRespawner()
		{
			std::list<Creature*> CorruptedWaterTotem;
			me->GetCreatureListWithEntryInGrid(CorruptedWaterTotem, NPC_CORRUPT_WATER_TOTEM, 500.0f);

			if (!CorruptedWaterTotem.empty())
			{
				for (std::list<Creature*>::const_iterator itr = CorruptedWaterTotem.begin(); itr != CorruptedWaterTotem.end(); ++itr)
				{
					if (Creature* totem = *itr)
						totem->Respawn();
				}
			}
		}

		void TotemDespawner()
		{
			std::list<Creature*> CorruptedWaterTotem;
			me->GetCreatureListWithEntryInGrid(CorruptedWaterTotem, NPC_CORRUPT_WATER_TOTEM, 500.0f);

			if (!CorruptedWaterTotem.empty())
			{
				for (std::list<Creature*>::const_iterator itr = CorruptedWaterTotem.begin(); itr != CorruptedWaterTotem.end(); ++itr)
				{
					if (Creature* totem = *itr)
						totem->DisappearAndDie();
				}
			}
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_CORRUPTED_WATER_ELEMENTAL)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (WaterBubbleTimer <= diff)
			{
				DoCast(me, SPELL_WATER_BUBBLE_VISUAL, true);
				WaterBubbleTimer = 15*IN_MILLISECONDS;
			}
			else WaterBubbleTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WATER_BOLT);
					WaterBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else WaterBoltTimer -= diff;

			if (SummonWaterElementalTimer <= diff)
			{
				DoCast(me, SPELL_CORRUPTED_WATER_ELEMENTAL, true);
				DoCast(me, SPELL_CORRUPTED_WATER_ELEMENTAL, true);
				DoCast(me, SPELL_CORRUPTED_WATER_ELEMENTAL, true);
				SummonWaterElementalTimer = 5*IN_MILLISECONDS;
			}
			else SummonWaterElementalTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lakaanAI(creature);
    }
};

enum DemonHunterSupplicant
{
	SPELL_SPRINT  = 32720,
	SPELL_EVASION = 37683,
};

class npc_demon_hunter_supplicant : public CreatureScript
{
public:
    npc_demon_hunter_supplicant() : CreatureScript("npc_demon_hunter_supplicant") {}

    struct npc_demon_hunter_supplicantAI : public QuantumBasicAI
    {
        npc_demon_hunter_supplicantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SprintTimer;
		uint32 EvasionTimer;

        void Reset()
        {
			SprintTimer = 0.5*IN_MILLISECONDS;
			EvasionTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SprintTimer <= diff)
			{
				DoCast(me, SPELL_SPRINT);
				SprintTimer = 6*IN_MILLISECONDS;
			}
			else SprintTimer -= diff;

			if (EvasionTimer <= diff)
			{
				DoCast(me, SPELL_EVASION);
				EvasionTimer = 8*IN_MILLISECONDS;
			}
			else EvasionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_demon_hunter_supplicantAI(creature);
    }
};

enum MegzegNukklebust
{
	SPELL_MN_THUNDERCLAP = 15588,
	SPELL_MN_WHIRLWIND   = 15589,
};

class npc_megzeg_nukklebust : public CreatureScript
{
public:
    npc_megzeg_nukklebust() : CreatureScript("npc_megzeg_nukklebust") {}

    struct npc_megzeg_nukklebustAI : public QuantumBasicAI
    {
        npc_megzeg_nukklebustAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThunderclapTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			ThunderclapTimer = 2*IN_MILLISECONDS;
			WhirlwindTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, 257);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_MN_THUNDERCLAP);
				ThunderclapTimer = 4*IN_MILLISECONDS;
			}
			else ThunderclapTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_MN_WHIRLWIND);
				WhirlwindTimer = 6*IN_MILLISECONDS;
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_megzeg_nukklebustAI(creature);
    }
};

enum ThrallmarMages
{
	SPELL_AT_FROST_NOVA         = 12674,
	SPELL_AT_FROSTBOLT          = 15530,
	SPELL_AT_ARCANE_MISSILES    = 22273,
	SPELL_AT_BLIZZARD           = 33634,
};

class npc_arcanist_torseldori : public CreatureScript
{
public:
    npc_arcanist_torseldori() : CreatureScript("npc_arcanist_torseldori") {}

    struct npc_arcanist_torseldoriAI : public QuantumBasicAI
    {
        npc_arcanist_torseldoriAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostNovaTimer;
		uint32 FrostboltTimer;
		uint32 BlizzardTimer;
		uint32 ArcaneMissilesTimer;

        void Reset()
        {
			FrostNovaTimer = 2*IN_MILLISECONDS;
			FrostboltTimer = 3*IN_MILLISECONDS;
			BlizzardTimer = 5*IN_MILLISECONDS;
			ArcaneMissilesTimer = 7*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_AT_FROST_NOVA);
				FrostNovaTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AT_FROSTBOLT);
					FrostboltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AT_BLIZZARD);
					BlizzardTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			if (ArcaneMissilesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AT_ARCANE_MISSILES);
					ArcaneMissilesTimer = urand(12*IN_MILLISECONDS, 13*IN_MILLISECONDS);
				}
			}
			else ArcaneMissilesTimer -= diff;

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
        return new npc_arcanist_torseldoriAI(creature);
    }
};

class npc_bloodmage : public CreatureScript
{
public:
    npc_bloodmage() : CreatureScript("npc_bloodmage") {}

    struct npc_bloodmageAI : public QuantumBasicAI
    {
        npc_bloodmageAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostNovaTimer;
		uint32 FrostboltTimer;
		uint32 BlizzardTimer;
		uint32 ArcaneMissilesTimer;

        void Reset()
        {
			FrostNovaTimer = 2*IN_MILLISECONDS;
			FrostboltTimer = 3*IN_MILLISECONDS;
			BlizzardTimer = 5*IN_MILLISECONDS;
			ArcaneMissilesTimer = 7*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_AT_FROST_NOVA);
				FrostNovaTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AT_FROSTBOLT);
					FrostboltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AT_BLIZZARD);
					BlizzardTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			if (ArcaneMissilesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AT_ARCANE_MISSILES);
					ArcaneMissilesTimer = urand(12*IN_MILLISECONDS, 13*IN_MILLISECONDS);
				}
			}
			else ArcaneMissilesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodmageAI(creature);
    }
};

enum SergeantShatterskull
{
	SPELL_SS_FRENZY        = 8269,
	SPELL_SS_MORTAL_STRIKE = 16856,
	SPELL_SS_KNOCK_AWAY    = 18670,
};

class npc_sergeant_shatterskull : public CreatureScript
{
public:
    npc_sergeant_shatterskull() : CreatureScript("npc_sergeant_shatterskull") {}

    struct npc_sergeant_shatterskullAI : public QuantumBasicAI
    {
        npc_sergeant_shatterskullAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 KnockAwayTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;
			KnockAwayTimer = 4*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_SS_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_SS_KNOCK_AWAY);
				KnockAwayTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else KnockAwayTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_SS_FRENZY, 0))
				{
					DoCast(me, SPELL_SS_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sergeant_shatterskullAI(creature);
    }
};

enum BonechewerRidingWolf
{
	SPELL_FURIOUS_HOWL   = 3149,
	SPELL_FESTERING_BITE = 16460,
};

class npc_bonechewer_riding_wolf : public CreatureScript
{
public:
    npc_bonechewer_riding_wolf() : CreatureScript("npc_bonechewer_riding_wolf") {}

    struct npc_bonechewer_riding_wolfAI : public QuantumBasicAI
    {
        npc_bonechewer_riding_wolfAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FuriousHowlTimer;
		uint32 FesteringBiteTimer;

        void Reset()
        {
			FuriousHowlTimer = 1*IN_MILLISECONDS;
			FesteringBiteTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
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
        return new npc_bonechewer_riding_wolfAI(creature);
    }
};

enum CoilskarMyrmidon
{
	SPELL_BOILING_BLOOD = 38027,
};

class npc_coilskar_myrmidon : public CreatureScript
{
public:
    npc_coilskar_myrmidon() : CreatureScript("npc_coilskar_myrmidon") {}

    struct npc_coilskar_myrmidonAI : public QuantumBasicAI
    {
        npc_coilskar_myrmidonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoilingBloodTimer;

        void Reset()
        {
			BoilingBloodTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoilingBloodTimer <= diff)
			{
				DoCastVictim(SPELL_BOILING_BLOOD);
				BoilingBloodTimer = 5*IN_MILLISECONDS;
			}
			else BoilingBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilskar_myrmidonAI(creature);
    }
};

enum CoilskarSorceress
{
	SPELL_CS_WATER_BOLT     = 32011,
	SPELL_CS_VISCOUS_SHIELD = 38026,
};

class npc_coilskar_sorceress : public CreatureScript
{
public:
    npc_coilskar_sorceress() : CreatureScript("npc_coilskar_sorceress") {}

    struct npc_coilskar_sorceressAI : public QuantumBasicAI
    {
        npc_coilskar_sorceressAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterBoltTimer;
		uint32 ViscousShield;

        void Reset()
        {
			WaterBoltTimer = 1*IN_MILLISECONDS;
			ViscousShield = 3*IN_MILLISECONDS;

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
					DoCast(target, SPELL_CS_WATER_BOLT);
					WaterBoltTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else WaterBoltTimer -= diff;

			if (ViscousShield <= diff)
			{
				DoCast(me, SPELL_CS_VISCOUS_SHIELD);
				ViscousShield = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ViscousShield -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilskar_sorceressAI(creature);
    }
};

enum CoilskarCobra
{
	SPELL_POISON_SPIT = 38030,
};

class npc_coilskar_cobra : public CreatureScript
{
public:
    npc_coilskar_cobra() : CreatureScript("npc_coilskar_cobra") {}

    struct npc_coilskar_cobraAI : public QuantumBasicAI
    {
        npc_coilskar_cobraAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonSpitTimer;

        void Reset()
        {
			PoisonSpitTimer = 1*IN_MILLISECONDS;

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
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POISON_SPIT);
					PoisonSpitTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else PoisonSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilskar_cobraAI(creature);
    }
};

enum VilewingChimaera
{
	SPELL_VH_POISON_SPIT = 16552,
};

class npc_vilewing_chimaera : public CreatureScript
{
public:
    npc_vilewing_chimaera() : CreatureScript("npc_vilewing_chimaera") {}

    struct npc_vilewing_chimaeraAI : public QuantumBasicAI
    {
        npc_vilewing_chimaeraAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonSpitTimer;

        void Reset()
        {
			PoisonSpitTimer = 1*IN_MILLISECONDS;

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
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VH_POISON_SPIT);
					PoisonSpitTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else PoisonSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vilewing_chimaeraAI(creature);
    }
};

enum AshtongueWarrior
{
	SPELL_AW_FRENZY        = 8269,
	SPELL_AW_MORTAL_STRIKE = 16856,
	SPELL_AW_KNOCK_AWAY    = 18670,
};

class npc_ashtongue_warrior : public CreatureScript
{
public:
    npc_ashtongue_warrior() : CreatureScript("npc_ashtongue_warrior") {}

    struct npc_ashtongue_warriorAI : public QuantumBasicAI
    {
        npc_ashtongue_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 KnockAwayTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;
			KnockAwayTimer = 4*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_AW_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_AW_KNOCK_AWAY);
				KnockAwayTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else KnockAwayTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_AW_FRENZY, 0))
				{
					DoCast(me, SPELL_AW_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashtongue_warriorAI(creature);
    }
};

enum AshtongueShaman
{
	SPELL_AS_LIGHTNING_SHIELD   = 12550,
	SPELL_AS_FIRE_NOVA_TOTEM    = 32062,
	SPELL_AS_BLOODLUST          = 37067,
	// Texts
	SAY_ASHTONGUE_SHAMAN_WORK_1 =  -1462010,
	SAY_ASHTONGUE_SHAMAN_WORK_2 =  -1462011,
	SAY_ASHTONGUE_SHAMAN_WORK_3 =  -1462012,
};

class npc_ashtongue_shaman : public CreatureScript
{
public:
    npc_ashtongue_shaman() : CreatureScript("npc_ashtongue_shaman") {}

    struct npc_ashtongue_shamanAI : public QuantumBasicAI
    {
        npc_ashtongue_shamanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RandomSayTimer;
		uint32 FireNovaTotemTimer;
		uint32 BloodlustTimer;

        void Reset()
        {
			RandomSayTimer = 1*IN_MILLISECONDS;
			FireNovaTotemTimer = 2*IN_MILLISECONDS;
			BloodlustTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (RandomSayTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(RAND(SAY_ASHTONGUE_SHAMAN_WORK_1, SAY_ASHTONGUE_SHAMAN_WORK_2, SAY_ASHTONGUE_SHAMAN_WORK_3), me);
				RandomSayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else RandomSayTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireNovaTotemTimer <= diff)
			{
				DoCast(me, SPELL_AS_FIRE_NOVA_TOTEM);
				FireNovaTotemTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FireNovaTotemTimer -= diff;

			if (BloodlustTimer <= diff)
			{
				DoCast(me, SPELL_AS_BLOODLUST);
				BloodlustTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else BloodlustTimer -= diff;

			if (!me->HasAura(SPELL_AS_LIGHTNING_SHIELD))
				DoCast(me, SPELL_AS_LIGHTNING_SHIELD);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashtongue_shamanAI(creature);
    }
};

enum AshtongueWorker
{
	SPELL_AW_PIERCE_ARMOR = 6016,
};

class npc_ashtongue_worker : public CreatureScript
{
public:
    npc_ashtongue_worker() : CreatureScript("npc_ashtongue_worker") {}

    struct npc_ashtongue_workerAI : public QuantumBasicAI
    {
        npc_ashtongue_workerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PierceArmorTimer;

        void Reset()
        {
			PierceArmorTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_AW_PIERCE_ARMOR);
				PierceArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashtongue_workerAI(creature);
    }
};

enum ServantOfIllidan
{
	SPELL_SI_REND          = 11977,
	SPELL_SI_MORTAL_STRIKE = 32736,
};

class npc_illidari_overseer : public CreatureScript
{
public:
    npc_illidari_overseer() : CreatureScript("npc_illidari_overseer") {}

    struct npc_illidari_overseerAI : public QuantumBasicAI
    {
        npc_illidari_overseerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 MortalStrikeTimer;

        void Reset()
        {
			RendTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 4*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_SI_REND);
				RendTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SI_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_illidari_overseerAI(creature);
    }
};

enum ElekkDemolisher
{
	SPELL_RAMPAGING_STOMP = 38045,
};

class npc_elekk_demolisher : public CreatureScript
{
public:
    npc_elekk_demolisher() : CreatureScript("npc_elekk_demolisher") {}

    struct npc_elekk_demolisherAI : public QuantumBasicAI
    {
        npc_elekk_demolisherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RampagingStompTimer;

        void Reset()
        {
			RampagingStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RampagingStompTimer <= diff)
			{
				DoCastAOE(SPELL_RAMPAGING_STOMP);
				RampagingStompTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RampagingStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elekk_demolisherAI(creature);
    }
};

enum RedeemedSpirits
{
	SPELL_SPAWN_ELEMENTAL = 25035,
};

class npc_redeemed_spirit : public CreatureScript
{
public:
    npc_redeemed_spirit() : CreatureScript("npc_redeemed_spirit") {}

    struct npc_redeemed_spiritAI : public QuantumBasicAI
    {
        npc_redeemed_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SpawnTimer;

        void Reset()
        {
			SpawnTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SpawnTimer <= diff)
			{
				DoCast(me, SPELL_SPAWN_ELEMENTAL);
				SpawnTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SpawnTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_redeemed_spiritAI(creature);
    }
};

enum PainmistressGabrissa
{
	SPELL_GABRISSA_CHANNEL = 35782,
	SPELL_CURSE_OF_PAIN    = 38048,
	SPELL_SUBSERVIENCE     = 38169,
};

class npc_painmistress_gabrissa : public CreatureScript
{
public:
    npc_painmistress_gabrissa() : CreatureScript("npc_painmistress_gabrissa") {}

    struct npc_painmistress_gabrissaAI : public QuantumBasicAI
    {
        npc_painmistress_gabrissaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfPainTimer;
		uint32 SubservienceTimer;

        void Reset()
        {
			CurseOfPainTimer = 1*IN_MILLISECONDS;
			SubservienceTimer = 3*IN_MILLISECONDS;

			DoCast(SPELL_GABRISSA_CHANNEL);

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

			if (CurseOfPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_PAIN);
					CurseOfPainTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else CurseOfPainTimer -= diff;

			if (SubservienceTimer <= diff)
			{
				DoCastAOE(SPELL_SUBSERVIENCE);
				SubservienceTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else SubservienceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_painmistress_gabrissaAI(creature);
    }
};

enum Unyieldings
{
	SPELL_GHOST_VISUAL = 22650,
	SPELL_SHIELD_BASH  = 11972,
	SPELL_STRIKE       = 11976,
	SPELL_FIREBALL     = 9053,
	SPELL_FLAMESTRIKE  = 11829,
	SPELL_FROST_NOVA   = 11831,
	SPELL_CHARGE       = 22911,
	SPELL_HEAL_OTHER   = 33910,
};

class npc_unyielding_footman : public CreatureScript
{
public:
    npc_unyielding_footman() : CreatureScript("npc_unyielding_footman") {}

    struct npc_unyielding_footmanAI : public QuantumBasicAI
    {
        npc_unyielding_footmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShieldBashTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			ShieldBashTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_GHOST_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

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

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unyielding_footmanAI(creature);
    }
};

class npc_unyielding_sorcerer : public CreatureScript
{
public:
    npc_unyielding_sorcerer() : CreatureScript("npc_unyielding_sorcerer") {}

    struct npc_unyielding_sorcererAI : public QuantumBasicAI
    {
        npc_unyielding_sorcererAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireballTimer;
		uint32 FlamestrikeTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FireballTimer = 1*IN_MILLISECONDS;
			FlamestrikeTimer = 3*IN_MILLISECONDS;
			FrostNovaTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_GHOST_VISUAL);

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
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = 3*IN_MILLISECONDS;
				}
			}
			else FireballTimer -= diff;

			if (FlamestrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAMESTRIKE);
					FlamestrikeTimer = 5*IN_MILLISECONDS;
				}
			}
			else FlamestrikeTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = 7*IN_MILLISECONDS;
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unyielding_sorcererAI(creature);
    }
};

class npc_unyielding_knight : public CreatureScript
{
public:
    npc_unyielding_knight() : CreatureScript("npc_unyielding_knight") {}

    struct npc_unyielding_knightAI : public QuantumBasicAI
    {
        npc_unyielding_knightAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChargeTimer;

        void Reset()
        {
			ChargeTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_GHOST_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_CHARGE);
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

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, SPELL_HEAL_OTHER);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unyielding_knightAI(creature);
    }
};

enum SporeggarSpawn
{
	SPELL_SPORESKIN = 35230,
};

class npc_sporeggar_spawn : public CreatureScript
{
public:
    npc_sporeggar_spawn() : CreatureScript("npc_sporeggar_spawn") {}

    struct npc_sporeggar_spawnAI : public QuantumBasicAI
    {
        npc_sporeggar_spawnAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SPORESKIN);

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
        return new npc_sporeggar_spawnAI(creature);
    }
};

enum StarvingGiants
{
	SPELL_CHOKING_VINES = 35244,
	SPELL_WAR_STOMP     = 35238,
};

class npc_starving_bog_lord : public CreatureScript
{
public:
    npc_starving_bog_lord() : CreatureScript("npc_starving_bog_lord") {}

    struct npc_starving_bog_lordAI : public QuantumBasicAI
    {
        npc_starving_bog_lordAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChokingVinesTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			ChokingVinesTimer = 2*IN_MILLISECONDS;
			WarStompTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChokingVinesTimer <= diff)
			{
				DoCastVictim(SPELL_CHOKING_VINES);
				ChokingVinesTimer = 3*IN_MILLISECONDS;
			}
			else ChokingVinesTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = 7*IN_MILLISECONDS;
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_starving_bog_lordAI(creature);
    }
};

class npc_starving_fungal_giant : public CreatureScript
{
public:
    npc_starving_fungal_giant() : CreatureScript("npc_starving_fungal_giant") {}

    struct npc_starving_fungal_giantAI : public QuantumBasicAI
    {
        npc_starving_fungal_giantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChokingVinesTimer;

        void Reset()
        {
			ChokingVinesTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChokingVinesTimer <= diff)
			{
				DoCastVictim(SPELL_CHOKING_VINES);
				ChokingVinesTimer = 3*IN_MILLISECONDS;
			}
			else ChokingVinesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_starving_fungal_giantAI(creature);
    }
};

enum MarshWalker
{
	SPELL_LIGHTNING_TETHER = 33628,
};

class npc_marsh_walker : public CreatureScript
{
public:
    npc_marsh_walker() : CreatureScript("npc_marsh_walker") {}

    struct npc_marsh_walkerAI : public QuantumBasicAI
    {
        npc_marsh_walkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningTetherTimer;

        void Reset()
        {
			LightningTetherTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningTetherTimer <= diff)
			{
				DoCastVictim(SPELL_LIGHTNING_TETHER);
				LightningTetherTimer = 4*IN_MILLISECONDS;
			}
			else LightningTetherTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marsh_walkerAI(creature);
    }
};

enum MarshlightBleeder
{
	SPELL_VIBRANT_BLOOD = 35329,
	SPELL_MD_REND       = 13443,
};

class npc_marshlight_bleeder : public CreatureScript
{
public:
    npc_marshlight_bleeder() : CreatureScript("npc_marshlight_bleeder") {}

    struct npc_marshlight_bleederAI : public QuantumBasicAI
    {
        npc_marshlight_bleederAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 VibrantBloodTimer;

        void Reset()
        {
			RendTimer = 2*IN_MILLISECONDS;
			VibrantBloodTimer = 3*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_MD_REND);
				RendTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (VibrantBloodTimer <= diff)
			{
				DoCastVictim(SPELL_VIBRANT_BLOOD);
				VibrantBloodTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else VibrantBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marshlight_bleederAI(creature);
    }
};

enum GreaterSporebat
{
	SPELL_SPORE_CLOUD = 35394,
};

class npc_greater_sporebat : public CreatureScript
{
public:
    npc_greater_sporebat() : CreatureScript("npc_greater_sporebat") {}

    struct npc_greater_sporebatAI : public QuantumBasicAI
    {
        npc_greater_sporebatAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SporeCloudTimer;

        void Reset()
        {
			SporeCloudTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SporeCloudTimer <= diff)
			{
				DoCastVictim(SPELL_SPORE_CLOUD);
				SporeCloudTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SporeCloudTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_sporebatAI(creature);
    }
};

enum BloodscaleEnchantress
{
	SPELL_BE_ICE_BARRIER     = 33245,
	SPELL_BE_ARCANE_MISSILES = 34446,
	SPELL_BE_FREEZING_CIRCLE = 34787,
};

class npc_bloodscale_enchantress : public CreatureScript
{
public:
    npc_bloodscale_enchantress() : CreatureScript("npc_bloodscale_enchantress") {}

    struct npc_bloodscale_enchantressAI : public QuantumBasicAI
    {
        npc_bloodscale_enchantressAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IceBarrierTimer;
		uint32 ArcaneMissilesTimer;
		uint32 FreezingCircleTimer;

        void Reset()
        {
			IceBarrierTimer = 1*IN_MILLISECONDS;
			ArcaneMissilesTimer = 3*IN_MILLISECONDS;
			FreezingCircleTimer = 5*IN_MILLISECONDS;

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
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BE_ARCANE_MISSILES);
					ArcaneMissilesTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ArcaneMissilesTimer -= diff;

			if (FreezingCircleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BE_FREEZING_CIRCLE, true);
					FreezingCircleTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else FreezingCircleTimer -= diff;

			if (IceBarrierTimer <= diff)
			{
				DoCast(me, SPELL_BE_ICE_BARRIER);
				IceBarrierTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else IceBarrierTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodscale_enchantressAI(creature);
    }
};

enum BloodscaleSlavedriver
{
	SPELL_BS_BATTLE_SHOUT = 32064,
};

class npc_bloodscale_slavedriver : public CreatureScript
{
public:
    npc_bloodscale_slavedriver() : CreatureScript("npc_bloodscale_slavedriver") {}

    struct npc_bloodscale_slavedriverAI : public QuantumBasicAI
    {
        npc_bloodscale_slavedriverAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BattleShoutTimer;

        void Reset()
        {
			BattleShoutTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_BS_BATTLE_SHOUT);
				BattleShoutTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else BattleShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodscale_slavedriverAI(creature);
    }
};

enum BloodscaleSentry
{
	SPELL_COSMETIC_STEAM_AURA = 36151,
	SPELL_FROSTBOLT_VOLLEY    = 36741,
};

class npc_bloodscale_sentry : public CreatureScript
{
public:
    npc_bloodscale_sentry() : CreatureScript("npc_bloodscale_sentry") {}

    struct npc_bloodscale_sentryAI : public QuantumBasicAI
    {
        npc_bloodscale_sentryAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltVolleyTimer;

        void Reset()
        {
			FrostboltVolleyTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_COSMETIC_STEAM_AURA);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT_VOLLEY);
					FrostboltVolleyTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FrostboltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodscale_sentryAI(creature);
    }
};

enum WrektSlave
{
	SPELL_WS_KICK         = 11978,
	SPELL_WS_SHARED_BONDS = 34789,
};

class npc_wrekt_slave : public CreatureScript
{
public:
    npc_wrekt_slave() : CreatureScript("npc_wrekt_slave") {}

    struct npc_wrekt_slaveAI : public QuantumBasicAI
    {
        npc_wrekt_slaveAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KickTimer;
		uint32 SharedBondsTimer;

        void Reset()
        {
			KickTimer = 2*IN_MILLISECONDS;
			SharedBondsTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_WS_KICK);
				KickTimer = 4*IN_MILLISECONDS;
			}
			else KickTimer -= diff;

			if (SharedBondsTimer <= diff)
			{
				DoCast(me, SPELL_WS_SHARED_BONDS, true);
				SharedBondsTimer = 6*IN_MILLISECONDS;
			}
			else SharedBondsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrekt_slaveAI(creature);
    }
};

enum SteamPumpOverseer 
{
	SPELL_TOUGHEN = 33962,
};

class npc_steam_pump_overseer : public CreatureScript
{
public:
    npc_steam_pump_overseer() : CreatureScript("npc_steam_pump_overseer") {}

    struct npc_steam_pump_overseerAI : public QuantumBasicAI
    {
        npc_steam_pump_overseerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ToughenTimer;

        void Reset()
        {
			ToughenTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ToughenTimer <= diff)
			{
				DoCast(me, SPELL_TOUGHEN);
				ToughenTimer = 3*IN_MILLISECONDS;
			}
			else ToughenTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steam_pump_overseerAI(creature);
    }
};

enum CoilfangEmissary
{
	SPELL_CE_FROSTBOLT        = 36990,
	SPELL_CE_ARCANE_EXPLOSION = 46457,
	SPELL_CE_FROST_NOVA       = 11831,
	SPELL_CE_WATER_SPOUT      = 39207,
};

class npc_coilfang_emissary : public CreatureScript
{
public:
    npc_coilfang_emissary() : CreatureScript("npc_coilfang_emissary") {}

    struct npc_coilfang_emissaryAI : public QuantumBasicAI
    {
        npc_coilfang_emissaryAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 ArcaneExplosionTimer;
		uint32 WaterSpoutTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FrostboltTimer = 1*IN_MILLISECONDS;
			ArcaneExplosionTimer = 3*IN_MILLISECONDS;
			WaterSpoutTimer = 4*IN_MILLISECONDS;
			FrostNovaTimer = 6*IN_MILLISECONDS;

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
					DoCast(target, SPELL_CE_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_CE_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ArcaneExplosionTimer -= diff;

			if (WaterSpoutTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CE_WATER_SPOUT);
					WaterSpoutTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else WaterSpoutTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_CE_FROST_NOVA);
				FrostNovaTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_emissaryAI(creature);
    }
};

enum Terrorguard
{
	SPELL_CRIPPLE      = 11443,
	SPELL_TG_WAR_STOMP = 11876,
};

class npc_terrorguard : public CreatureScript
{
public:
    npc_terrorguard() : CreatureScript("npc_terrorguard") {}

    struct npc_terrorguardAI : public QuantumBasicAI
    {
        npc_terrorguardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrippleTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			CrippleTimer = 1*IN_MILLISECONDS;
			WarStompTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrippleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRIPPLE, true);
					CrippleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else CrippleTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_TG_WAR_STOMP);
				WarStompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_terrorguardAI(creature);
    }
};

enum Voidspawn
{
	SPELL_PAIN_SPIKE = 32026,
	SPELL_MIND_SEAR  = 36447,
};

class npc_voidspawn : public CreatureScript
{
public:
    npc_voidspawn() : CreatureScript("npc_voidspawn") {}

    struct npc_voidspawnAI : public QuantumBasicAI
    {
        npc_voidspawnAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PainSpikeTimer;
		uint32 MindSearTimer;

        void Reset()
        {
			PainSpikeTimer = 1*IN_MILLISECONDS;
			MindSearTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PainSpikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PAIN_SPIKE);
					PainSpikeTimer = 5*IN_MILLISECONDS;
				}
			}
			else PainSpikeTimer -= diff;

			if (MindSearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_SEAR);
					MindSearTimer = 7*IN_MILLISECONDS;
				}
			}
			else MindSearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_voidspawnAI(creature);
    }
};

enum UmbraglowStinger
{
	SPELL_GLOWING_BLOOD = 35325,
};

class npc_umbraglow_stinger : public CreatureScript
{
public:
    npc_umbraglow_stinger() : CreatureScript("npc_umbraglow_stinger") {}

    struct npc_umbraglow_stingerAI : public QuantumBasicAI
    {
        npc_umbraglow_stingerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GlowingBloodTimer;

        void Reset()
        {
			GlowingBloodTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GlowingBloodTimer <= diff)
			{
				DoCastVictim(SPELL_GLOWING_BLOOD);
				GlowingBloodTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else GlowingBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_umbraglow_stingerAI(creature);
    }
};

enum FenStrider
{
	SPELL_FS_LIGHTNING_TETHER = 33628,
};

class npc_fen_strider : public CreatureScript
{
public:
    npc_fen_strider() : CreatureScript("npc_fen_strider") {}

    struct npc_fen_striderAI : public QuantumBasicAI
    {
        npc_fen_striderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningTetherTimer;

        void Reset()
        {
			LightningTetherTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningTetherTimer <= diff)
			{
				DoCastVictim(SPELL_FS_LIGHTNING_TETHER);
				LightningTetherTimer = 4*IN_MILLISECONDS;
			}
			else LightningTetherTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fen_striderAI(creature);
    }
};

enum YoungSporebat
{
	SPELL_ENERGIZING_SPORES = 35336,
};

class npc_young_sporebat : public CreatureScript
{
public:
    npc_young_sporebat() : CreatureScript("npc_young_sporebat") {}

    struct npc_young_sporebatAI : public QuantumBasicAI
    {
        npc_young_sporebatAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			killer->CastSpell(killer, SPELL_ENERGIZING_SPORES, true);
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
        return new npc_young_sporebatAI(creature);
    }
};

enum SarineiWhitestar
{
	SPELL_SW_SHOOT = 15547,
	SAY_SW_TRADE   = -1300000,
};

class npc_sarinei_whitestar : public CreatureScript
{
public:
    npc_sarinei_whitestar() : CreatureScript("npc_sarinei_whitestar") {}

    struct npc_sarinei_whitestarAI : public QuantumBasicAI
    {
        npc_sarinei_whitestarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 SayTradeTimer;

        void Reset()
        {
			ShootTimer = 1*IN_MILLISECONDS;
			SayTradeTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SW_SHOOT);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SayTradeTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(SAY_SW_TRADE, me);
				SayTradeTimer = 90000;
			}
			else SayTradeTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				DoCastVictim(SPELL_SW_SHOOT);
				ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else ShootTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sarinei_whitestarAI(creature);
    }
};

enum Sporebat
{
	SPELL_S_ENERGIZING_SPORES = 35336,
	SPELL_S_SPORE_CLOUD       = 35394,
};

class npc_sporebat : public CreatureScript
{
public:
    npc_sporebat() : CreatureScript("npc_sporebat") {}

    struct npc_sporebatAI : public QuantumBasicAI
    {
        npc_sporebatAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SporeCloudTimer;

        void Reset()
        {
			SporeCloudTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			killer->CastSpell(killer, SPELL_S_ENERGIZING_SPORES, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SporeCloudTimer <= diff)
			{
				DoCastVictim(SPELL_S_SPORE_CLOUD);
				SporeCloudTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SporeCloudTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sporebatAI(creature);
    }
};

enum BloodthirstyMarshfang
{
	SPELL_DRAIN_MANA   = 17008,
	SPELL_NETHER_LEECH = 35335,
};

class npc_bloodthirst_marshfang : public CreatureScript
{
public:
    npc_bloodthirst_marshfang() : CreatureScript("npc_bloodthirst_marshfang") {}

    struct npc_bloodthirst_marshfangAI : public QuantumBasicAI
    {
        npc_bloodthirst_marshfangAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DrainManaTimer;
		uint32 NetherLeechTimer;

        void Reset()
        {
			DrainManaTimer = 2*IN_MILLISECONDS;
			NetherLeechTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DrainManaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_DRAIN_MANA);
						DrainManaTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else DrainManaTimer -= diff;

			if (NetherLeechTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NETHER_LEECH);
					NetherLeechTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else NetherLeechTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodthirst_marshfangAI(creature);
    }
};

enum FeralfenHunter
{
	SPELL_FH_SHOOT         = 6660,
	SPELL_FH_NET           = 12024,
	SPELL_FH_SERPENT_STING = 31975,
	SPELL_SUMMON_SPOREBAT  = 32060,

	NPC_TAMED_SPOREBAT     = 18201,
};

class npc_feralfen_hunter : public CreatureScript
{
public:
    npc_feralfen_hunter() : CreatureScript("npc_feralfen_hunter") {}

    struct npc_feralfen_hunterAI : public QuantumBasicAI
    {
		npc_feralfen_hunterAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShootTimer;
		uint32 SerpentStingTimer;
		uint32 NetTimer;

		SummonList Summons;

        void Reset()
        {
			ShootTimer = 2*IN_MILLISECONDS;
			SerpentStingTimer = 4*IN_MILLISECONDS;
			NetTimer = 6*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_SPOREBAT, true);
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
			if (summon->GetEntry() == NPC_TAMED_SPOREBAT)
				Summons.Summon(summon);
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

			if (SerpentStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_FH_SERPENT_STING);
						SerpentStingTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else SerpentStingTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (NetTimer <= diff)
				{
					DoCastVictim(SPELL_FH_NET);
					NetTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
				else NetTimer -= diff;

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
						DoCast(target, SPELL_FH_SHOOT);
						ShootTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_feralfen_hunterAI(creature);
    }
};

enum EnragedCrusher
{
	SPELL_EC_ENRAGE = 8599,
	SPELL_KNOCKDOWN = 32015,
};

class npc_enraged_crusher : public CreatureScript
{
public:
    npc_enraged_crusher() : CreatureScript("npc_enraged_crusher") {}

    struct npc_enraged_crusherAI : public QuantumBasicAI
    {
        npc_enraged_crusherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockdownTimer;

        void Reset()
        {
			KnockdownTimer = 1*IN_MILLISECONDS;

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
				KnockdownTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else KnockdownTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_EC_ENRAGE, 0))
				{
					DoCast(me, SPELL_EC_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enraged_crusherAI(creature);
    }
};

enum StormRager
{
	SPELL_CALL_LIGHTNING = 32018,
};

class npc_storm_rager : public CreatureScript
{
public:
    npc_storm_rager() : CreatureScript("npc_storm_rager") {}

    struct npc_storm_ragerAI : public QuantumBasicAI
    {
        npc_storm_ragerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CallLightningTimer;

        void Reset()
        {
			CallLightningTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					DoCast(target, SPELL_CALL_LIGHTNING);
					CallLightningTimer = 3*IN_MILLISECONDS;
				}
			}
			else CallLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_storm_ragerAI(creature);
    }
};

enum LakeSpirit
{
	SPELL_LS_WATER_BOLT = 34425,
};

class npc_lake_spirit : public CreatureScript
{
public:
    npc_lake_spirit() : CreatureScript("npc_lake_spirit") {}

    struct npc_lake_spiritAI : public QuantumBasicAI
    {
        npc_lake_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

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
					DoCast(target, SPELL_LS_WATER_BOLT);
					WaterBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else WaterBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lake_spiritAI(creature);
    }
};

enum VoidConduit
{
	SPELL_DARKSTALKER_BIRTH = 35242,
};

class npc_void_conduit : public CreatureScript
{
public:
    npc_void_conduit() : CreatureScript("npc_void_conduit") {}

    struct npc_void_conduitAI : public QuantumBasicAI
    {
        npc_void_conduitAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
        {
			DoCast(me, SPELL_DARKSTALKER_BIRTH);
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
        return new npc_void_conduitAI(creature);
    }
};

enum ShivanAssassin
{
	// Aspect of the Shadow
	SPELL_ASPECT_OF_THE_SHADOW = 41595,
	SPELL_SIPHON_LIFE          = 41597,
	SPELL_DEATH_BLAST          = 40736,
	SPELL_SHADOW_NOVA          = 40737,
	// Aspect of the Flame
	SPELL_ASPECT_OF_THE_FLAME  = 41593,
	SPELL_PYROBLAST            = 41578,
	SPELL_CLEANSING_FLAME      = 41589,
	SPELL_FLAME_BUFFET         = 41596,
	// Aspect of the Ice
	SPELL_ASPECT_OF_THE_ICE    = 41594,
	SPELL_ICEBLAST             = 41579,
	SPELL_ICY_LEAP             = 40727,
	SPELL_ICE_BLOCK            = 41590,
	// Other
	SPELL_DUAL_WIELD           = 30798,
	SPELL_CHAOS_STRIKE         = 41964,
	//Phases
	PHASE_SHADOW               = 1,
	PHASE_FIRE                 = 2,
	PHASE_FROST                = 3,
	// Events Phase Shadow
	EVENT_SIPHON_LIFE          = 1,
	EVENT_DEATH_BLAST          = 2,
	EVENT_SHADOW_NOVA          = 3,
	// Events Phase Fire
	EVENT_PYROBLAST            = 4,
	EVENT_FLAME_BUFFET         = 5,
	// Events Phase Frost
	EVENT_ICEBLAST             = 6,
	EVENT_ICY_LEAP             = 7,
	EVENT_ICE_BLOCK            = 8,
	// Last Event after Ice Block
	EVENT_CHAOS_STRIKE         = 9,
};

class npc_shivan_assassin : public CreatureScript
{
public:
    npc_shivan_assassin() : CreatureScript("npc_shivan_assassin") {}

    struct npc_shivan_assassinAI : public QuantumBasicAI
    {
        npc_shivan_assassinAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
		{
			events.Reset();

			DoCast(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_SIPHON_LIFE, 1*IN_MILLISECONDS);
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
				switch(eventId)
				{
					case EVENT_SIPHON_LIFE:
						events.SetPhase(PHASE_SHADOW);
						me->RemoveAurasDueToSpell(SPELL_ASPECT_OF_THE_ICE);
						DoCast(me, SPELL_ASPECT_OF_THE_SHADOW, true);
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_SIPHON_LIFE);
						}
						events.ScheduleEvent(EVENT_DEATH_BLAST, 4*IN_MILLISECONDS, PHASE_SHADOW);
						break;
					case EVENT_DEATH_BLAST:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_DEATH_BLAST);
						}
						events.ScheduleEvent(EVENT_SHADOW_NOVA, 4*IN_MILLISECONDS, PHASE_SHADOW);
						break;
					case EVENT_SHADOW_NOVA:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_SHADOW_NOVA);
						}
						events.ScheduleEvent(EVENT_PYROBLAST, 4*IN_MILLISECONDS, PHASE_SHADOW);
						break;
					case EVENT_PYROBLAST:
						events.SetPhase(PHASE_FIRE);
						me->RemoveAurasDueToSpell(SPELL_ASPECT_OF_THE_SHADOW);
						DoCast(me, SPELL_ASPECT_OF_THE_FLAME, true);
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_PYROBLAST);
						}
						events.ScheduleEvent(EVENT_FLAME_BUFFET, 4*IN_MILLISECONDS, PHASE_FIRE);
						break;
					case EVENT_FLAME_BUFFET:
						DoCast(me, SPELL_CLEANSING_FLAME, true);
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_FLAME_BUFFET);
						}
						events.ScheduleEvent(EVENT_ICEBLAST, 4*IN_MILLISECONDS, PHASE_FIRE);
						break;
					case EVENT_ICEBLAST:
						events.SetPhase(PHASE_FROST);
						me->RemoveAurasDueToSpell(SPELL_ASPECT_OF_THE_FLAME);
						DoCast(me, SPELL_ASPECT_OF_THE_ICE, true);
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_ICEBLAST);
						}
						events.ScheduleEvent(EVENT_ICY_LEAP, 4*IN_MILLISECONDS, PHASE_FROST);
						break;
					case EVENT_ICY_LEAP:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_ICY_LEAP);
						}
						events.ScheduleEvent(EVENT_ICE_BLOCK, 4*IN_MILLISECONDS, PHASE_FROST);
						break;
					case EVENT_ICE_BLOCK:
						DoCast(me, SPELL_ICE_BLOCK);
						events.ScheduleEvent(EVENT_CHAOS_STRIKE, 6*IN_MILLISECONDS, PHASE_FROST);
						break;
					case EVENT_CHAOS_STRIKE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_CHAOS_STRIKE, true);
						}
						events.ScheduleEvent(EVENT_SIPHON_LIFE, 8*IN_MILLISECONDS, PHASE_FROST);
						break;
					default:
						break;
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shivan_assassinAI(creature);
    }
};

enum Shartuul
{
	SPELL_SH_SHADOW_BOLT   = 41957, // SPELL_SHADOW_RESONANCE
	SPELL_SH_IMMOLATE      = 41958,
	SPELL_SH_MAGNETIC_PULL = 41959, // SPELL_CORRUPTED_LIGHT
	SPELL_SH_INCINERATE    = 41960,
	SPELL_SH_CHAOS_STRIKE  = 41964,
	SPELL_SH_SHADOW_FORM   = 37816,
	SPELL_SHADOW_RESONANCE = 41961,
	SPELL_CORRUPTED_LIGHT  = 41965,
	SPELL_SHARTUUL_CREDIT  = 54992,
	// Eye Spells
	SPELL_FELL_FIREBALL    = 40633,
	SPELL_DISRUPTION_RAY   = 41550,
	SPELL_DARK_GLARE       = 41936,
	SPELL_STASIS_FIELD     = 57887,

	NPC_EYE_OF_SHARTUUL    = 23228,
	NPC_STUN_TRAP          = 23312,
	MODEL_ID_EYE           = 21586,

	EVENT_SHADOW_BOLT      = 1,
	EVENT_IMMOLATE         = 2,
	EVENT_MAGNETIC_PULL    = 3,
	EVENT_INCINERATE       = 4,
};

const Position PosShartuulEyes[7] =
{
	{2635.03f, 7152.65f, 364.978f, 3.41836f},
	{2641.56f, 7137.63f, 366.63f, 4.24774f},
	{2660.65f, 7128.62f, 365.415f, 4.84857f},
	{2673.14f, 7141.53f, 366.19f, 6.09267f},
	{2674.12f, 7160.9f, 365.019f, 0.880036f},
	{2660.07f, 7170.68f, 364.625f, 1.40225f},
	{2638.9f, 7169.41f, 364.973f, 2.61726f},
};

class npc_shartuul : public CreatureScript
{
public:
    npc_shartuul() : CreatureScript("npc_shartuul") {}

    struct npc_shartuulAI : public QuantumBasicAI
    {
        npc_shartuulAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		EventMap events;
		SummonList Summons;

        void Reset()
		{
			events.Reset();

			DoCast(me, SPELL_SH_SHADOW_FORM, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			// Summon Eyes
			for (uint8 i = 0; i < 7; i++)
				me->SummonCreature(NPC_EYE_OF_SHARTUUL, PosShartuulEyes[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_SHADOW_BOLT, 1*IN_MILLISECONDS);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_SHARTUUL_CREDIT, true);

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
			    case NPC_EYE_OF_SHARTUUL:
				case NPC_STUN_TRAP:
					Summons.Summon(summon);
					break;
				default:
					break;
			}
		}

		void JustReachedHome()
		{
			Summons.DespawnAll();
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
				switch(eventId)
				{
					case EVENT_SHADOW_BOLT:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_SHADOW_RESONANCE, true);
							DoCast(target, SPELL_SH_SHADOW_BOLT);
						}
						events.ScheduleEvent(EVENT_IMMOLATE, 4*IN_MILLISECONDS);
						break;
					case EVENT_IMMOLATE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_SH_IMMOLATE);
						}
						events.ScheduleEvent(EVENT_MAGNETIC_PULL, 4*IN_MILLISECONDS);
						break;
					case EVENT_MAGNETIC_PULL:
						DoCastVictim(SPELL_CORRUPTED_LIGHT, true);
						DoCastAOE(SPELL_SH_MAGNETIC_PULL);
						events.ScheduleEvent(EVENT_INCINERATE, 6*IN_MILLISECONDS);
						break;
					case EVENT_INCINERATE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_SH_INCINERATE);
						}
						events.ScheduleEvent(EVENT_SHADOW_BOLT, 6*IN_MILLISECONDS);
						break;
					default:
						break;
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shartuulAI(creature);
    }
};

class npc_eye_of_shartuul : public CreatureScript
{
public:
    npc_eye_of_shartuul() : CreatureScript("npc_eye_of_shartuul") {}

    struct npc_eye_of_shartuulAI : public QuantumBasicAI
    {
        npc_eye_of_shartuulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 FellFireballTimer;
		uint32 DisruptionRayTimer;
		uint32 DarkGlareTimer;

        void Reset()
        {
			FellFireballTimer = 0.5*IN_MILLISECONDS;
			DisruptionRayTimer = 3*IN_MILLISECONDS;
			DarkGlareTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FellFireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FELL_FIREBALL);
					FellFireballTimer = 6*IN_MILLISECONDS;
				}
			}
			else FellFireballTimer -= diff;

			if (DisruptionRayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DISRUPTION_RAY);
					DisruptionRayTimer = 9*IN_MILLISECONDS;
				}
			}
			else DisruptionRayTimer -= diff;

			if (DarkGlareTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DARK_GLARE);
					DarkGlareTimer = 12*IN_MILLISECONDS;
				}
			}
			else DarkGlareTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_20))
				me->SetDisplayId(MODEL_ID_EYE);

			// Eyes not use melee attacks
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eye_of_shartuulAI(creature);
    }
};

class npc_eye_of_shartuul_transform : public CreatureScript
{
public:
    npc_eye_of_shartuul_transform() : CreatureScript("npc_eye_of_shartuul_transform") {}

    struct npc_eye_of_shartuul_transformAI : public QuantumBasicAI
    {
        npc_eye_of_shartuul_transformAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FellFireballTimer;
		uint32 DisruptionRayTimer;
		uint32 DarkGlareTimer;

        void Reset()
        {
			FellFireballTimer = 0.5*IN_MILLISECONDS;
			DisruptionRayTimer = 3*IN_MILLISECONDS;
			DarkGlareTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FellFireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FELL_FIREBALL);
					FellFireballTimer = 6*IN_MILLISECONDS;
				}
			}
			else FellFireballTimer -= diff;

			if (DisruptionRayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DISRUPTION_RAY);
					DisruptionRayTimer = 9*IN_MILLISECONDS;
				}
			}
			else DisruptionRayTimer -= diff;

			if (DarkGlareTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DARK_GLARE);
					DarkGlareTimer = 12*IN_MILLISECONDS;
				}
			}
			else DarkGlareTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eye_of_shartuul_transformAI(creature);
    }
};

class npc_legion_ring_stun_field : public CreatureScript
{
public:
    npc_legion_ring_stun_field() : CreatureScript("npc_legion_ring_stun_field") {}

    struct npc_legion_ring_stun_fieldAI : public QuantumBasicAI
    {
        npc_legion_ring_stun_fieldAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_STASIS_FIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_legion_ring_stun_fieldAI(creature);
    }
};

enum Dreadmaw
{
	SPELL_RAMPAGING_CHARGE = 41939,
	SPELL_GROWTH           = 41953,
};

class npc_dreadmaw : public CreatureScript
{
public:
    npc_dreadmaw() : CreatureScript("npc_dreadmaw") {}

    struct npc_dreadmawAI : public QuantumBasicAI
    {
        npc_dreadmawAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RampagingChargeTimer;
		uint32 GrowthTimer;

        void Reset()
        {
			RampagingChargeTimer = 0.5*IN_MILLISECONDS;
			GrowthTimer = 1500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RampagingChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAMPAGING_CHARGE);
					RampagingChargeTimer = 4*IN_MILLISECONDS;
				}
			}
			else RampagingChargeTimer -= diff;

			if (GrowthTimer <= diff)
			{
				DoCast(me, SPELL_GROWTH);
				GrowthTimer = 8*IN_MILLISECONDS;
			}
			else GrowthTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dreadmawAI(creature);
    }
};

enum LesserNetherDrake
{
	SPELL_INTANGIBLE_PRESENCE = 36513,
};

class npc_lesser_nether_drake : public CreatureScript
{
public:
    npc_lesser_nether_drake() : CreatureScript("npc_lesser_nether_drake") {}

    struct npc_lesser_nether_drakeAI : public QuantumBasicAI
    {
        npc_lesser_nether_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IntangiblePresence;

        void Reset()
        {
			IntangiblePresence = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IntangiblePresence <= diff)
			{
				DoCast(SPELL_INTANGIBLE_PRESENCE);
				IntangiblePresence = 8*IN_MILLISECONDS;
			}
			else IntangiblePresence -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lesser_nether_drakeAI(creature);
    }
};

enum MaraudingCrustBurster
{
	SPELL_STAY_SUBMERGED      = 46981,
	SPELL_TUNNEL_BORE_PASSIVE = 29147,
	SPELL_STAND               = 37752,
	SPELL_SF_POISON           = 31747,
};

class npc_marauding_crust_burster : public CreatureScript
{
public:
    npc_marauding_crust_burster() : CreatureScript("npc_marauding_crust_burster") {}

    struct npc_marauding_crust_bursterAI : public QuantumBasicAI
    {
        npc_marauding_crust_bursterAI(Creature* creature) : QuantumBasicAI(creature){}

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
        return new npc_marauding_crust_bursterAI(creature);
    }
};

enum ViciousTeromoth
{
	SPELL_DAZZLING_DUST  = 32913,
	SPELL_WING_BUFFET    = 32914,
	SPELL_BLUE_RADIATION = 32423,
};

class npc_vicious_teromoth : public CreatureScript
{
public:
    npc_vicious_teromoth() : CreatureScript("npc_vicious_teromoth") {}

    struct npc_vicious_teromothAI : public QuantumBasicAI
    {
        npc_vicious_teromothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DazzlingDustTimer;
		uint32 WingBuffetTimer;

        void Reset()
        {
			DazzlingDustTimer = 1*IN_MILLISECONDS;
			WingBuffetTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_BLUE_RADIATION);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DazzlingDustTimer <= diff)
			{
				DoCastVictim(SPELL_DAZZLING_DUST);
				DazzlingDustTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else DazzlingDustTimer -= diff;

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
        return new npc_vicious_teromothAI(creature);
    }
};

enum DragonmawSkybreaker
{
	SPELL_SHOOT         = 38858,
	SPELL_SERPENT_STING = 38859,
	SPELL_AIMED_SHOT    = 33861,

	MOUNT_ID_SKYBREAKER = 20684,
};

class npc_dragonmaw_skybreaker : public CreatureScript
{
public:
    npc_dragonmaw_skybreaker() : CreatureScript("npc_dragonmaw_skybreaker") {}

    struct npc_dragonmaw_skybreakerAI : public QuantumBasicAI
    {
        npc_dragonmaw_skybreakerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 SerpentStingTimer;
		uint32 AimedShotTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			SerpentStingTimer = 1*IN_MILLISECONDS;
			AimedShotTimer = 3*IN_MILLISECONDS;

			me->Mount(MOUNT_ID_SKYBREAKER);
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
					DoCast(target, SPELL_SHOOT);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (SerpentStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SERPENT_STING);
					SerpentStingTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else SerpentStingTimer -= diff;

			if (AimedShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AIMED_SHOT, true);
					AimedShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else AimedShotTimer -= diff;

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
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonmaw_skybreakerAI(creature);
    }
};

enum SunhawkReclaimer
{
	SPELL_EXPLOIT_WEAKNESS    = 6595,
	SPELL_SR_FIREBALL         = 19816,
	SPELL_FROST_ARMOR         = 12544,
	SPELL_MARK_OF_THE_SUNHAWK = 31734,
};

class npc_sunhawk_reclaimer : public CreatureScript
{
public:
    npc_sunhawk_reclaimer() : CreatureScript("npc_sunhawk_reclaimer") {}

    struct npc_sunhawk_reclaimerAI : public QuantumBasicAI
    {
        npc_sunhawk_reclaimerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FireballTimer;
		uint32 ExploitWeaknessTimer;
		uint32 MarkOfTheSunhawkTimer;

        void Reset()
        {
			FrostArmorTimer = 0.1*IN_MILLISECONDS;
			FireballTimer = 0.5*IN_MILLISECONDS;
			ExploitWeaknessTimer = 3*IN_MILLISECONDS;
			MarkOfTheSunhawkTimer = 4*IN_MILLISECONDS;

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
					DoCast(target, SPELL_SR_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (ExploitWeaknessTimer <= diff)
			{
				DoCastVictim(SPELL_EXPLOIT_WEAKNESS);
				ExploitWeaknessTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ExploitWeaknessTimer -= diff;

			if (MarkOfTheSunhawkTimer <= diff)
			{
				DoCastVictim(SPELL_MARK_OF_THE_SUNHAWK);
				MarkOfTheSunhawkTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else MarkOfTheSunhawkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunhawk_reclaimerAI(creature);
    }
};

enum TherokkarForestWoods
{
	SPELL_SUMMON_WOOD_MITES   = 39130,
	SPELL_SUMMON_LOTS_OF_WOOD = 39134,
	SPELL_IR_REGROWTH         = 39000,
	SPELL_RT_THUNDERCLAP      = 15548,
};

class npc_infested_root_walker : public CreatureScript
{
public:
    npc_infested_root_walker() : CreatureScript("npc_infested_root_walker") { }

    struct npc_infested_root_walkerAI : public QuantumBasicAI
    {
        npc_infested_root_walkerAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 RegrowthTimer;

        void Reset()
		{
			RegrowthTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_SUMMON_WOOD_MITES, true);
		}

		void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RegrowthTimer <= diff)
			{
				DoCast(me, SPELL_IR_REGROWTH, true);
				RegrowthTimer = 4*IN_MILLISECONDS;
			}
			else RegrowthTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infested_root_walkerAI(creature);
    }
};

class npc_rotting_forest_rager : public CreatureScript
{
public:
    npc_rotting_forest_rager() : CreatureScript("npc_rotting_forest_rager") { }

    struct npc_rotting_forest_ragerAI : public QuantumBasicAI
    {
        npc_rotting_forest_ragerAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 ThunderclapTimer;

        void Reset()
		{
			ThunderclapTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_SUMMON_LOTS_OF_WOOD, true);
		}

		void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_RT_THUNDERCLAP);
				ThunderclapTimer = 6*IN_MILLISECONDS;
			}
			else ThunderclapTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rotting_forest_ragerAI(creature);
    }
};

enum Tunneler
{
	SPELL_T_STAY_SUBMERGED      = 46981,
	SPELL_T_TUNNEL_BORE_PASSIVE = 29147,
	SPELL_T_STAND               = 37752,
	SPELL_T_POISON              = 31747,
	SPELL_T_ENRAGE              = 32714,
	SPELL_T_BORE                = 32738,
};

class npc_tunneler : public CreatureScript
{
public:
    npc_tunneler() : CreatureScript("npc_tunneler") {}

    struct npc_tunnelerAI : public QuantumBasicAI
    {
        npc_tunnelerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonTimer;
		uint32 BoreTimer;

        void Reset()
        {
			PoisonTimer = 0.5*IN_MILLISECONDS;
			BoreTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_T_STAY_SUBMERGED, true);
			DoCast(me, SPELL_T_TUNNEL_BORE_PASSIVE, true);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

			me->RemoveAurasDueToSpell(SPELL_T_STAY_SUBMERGED);
			me->RemoveAurasDueToSpell(SPELL_T_TUNNEL_BORE_PASSIVE);

			DoCast(me, SPELL_T_STAND);
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
					DoCast(target, SPELL_T_POISON);
					PoisonTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else PoisonTimer -= diff;

			if (BoreTimer <= diff)
			{
				DoCastVictim(SPELL_T_BORE);
				BoreTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else BoreTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_T_ENRAGE, 0))
				{
					DoCast(me, SPELL_T_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tunnelerAI(creature);
    }
};

enum MagisterIdonis
{
	SPELL_MI_FROST_ARMOR    = 12544,
	SPELL_MI_FIREBALL       = 20793,

	SAY_MAGISTER_IDONIS_OOC = -1420084,
};

class npc_magister_idonis : public CreatureScript
{
public:
    npc_magister_idonis() : CreatureScript("npc_magister_idonis") {}

    struct npc_magister_idonisAI : public QuantumBasicAI
    {
        npc_magister_idonisAI(Creature* creature) : QuantumBasicAI(creature){}

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
				DoSendQuantumText(SAY_MAGISTER_IDONIS_OOC, me);
				DoCast(me, SPELL_MI_FROST_ARMOR);
				FrostArmorTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else FrostArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_MI_FROST_ARMOR))
				DoCast(me, SPELL_MI_FROST_ARMOR, true);

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MI_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magister_idonisAI(creature);
    }
};

enum ManaWyrm
{
	SPELL_FAERIE_FIRE       = 25602,
	SPELL_ARCANE_TORRENT_1  = 25046,
	SPELL_ARCANE_TORRENT_2  = 28730,
	SPELL_ARCANE_TORRENT_3  = 50613,
	SPELL_QUEST_CREDIT_8346 = 61314,
};

class npc_mana_wyrm : public CreatureScript
{
public:
    npc_mana_wyrm() : CreatureScript("npc_mana_wyrm") {}

    struct npc_mana_wyrmAI : public QuantumBasicAI
    {
        npc_mana_wyrmAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FaerieFireTimer;

        void Reset()
		{
			FaerieFireTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
			switch (spell->Id)
			{
			    case SPELL_ARCANE_TORRENT_1:
				case SPELL_ARCANE_TORRENT_2:
				case SPELL_ARCANE_TORRENT_3:
					if (Player* player = caster->ToPlayer())
						DoCastAOE(SPELL_QUEST_CREDIT_8346);
					break;
			}
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
					FaerieFireTimer = 6*IN_MILLISECONDS;
				}
			}
			else FaerieFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_wyrmAI(creature);
    }
};

enum SunfuryBowman
{
	SPELL_SB_SHOOT            = 6660,
	SPELL_SB_NET              = 12024,
	SPELL_SB_IMMOLATION_ARROW = 37847,
};

class npc_sunfury_bowman : public CreatureScript
{
public:
    npc_sunfury_bowman() : CreatureScript("npc_sunfury_bowman") {}

    struct npc_sunfury_bowmanAI : public QuantumBasicAI
    {
        npc_sunfury_bowmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 ImmolationArrowTimer;
		uint32 HookedNetTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			ImmolationArrowTimer = 1*IN_MILLISECONDS;
			HookedNetTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_BOW);
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
					DoCast(target, SPELL_SB_SHOOT);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (ImmolationArrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SB_IMMOLATION_ARROW, true);
					ImmolationArrowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ImmolationArrowTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (HookedNetTimer <= diff)
				{
					DoCastVictim(SPELL_SB_NET);
					HookedNetTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
				else HookedNetTimer -= diff;

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
						DoCast(target, SPELL_SB_SHOOT);
						ShootTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunfury_bowmanAI(creature);
    }
};

enum SunfuryBloodKnight
{
	SPELL_BLOOD_HEAL   = 36476,
	SPELL_SPELLBREAKER = 35871,
	SPELL_SB_ENRAGE    = 8599,
};

class npc_sunfury_blood_knight : public CreatureScript
{
public:
    npc_sunfury_blood_knight() : CreatureScript("npc_sunfury_blood_knight") {}

    struct npc_sunfury_blood_knightAI : public QuantumBasicAI
    {
        npc_sunfury_blood_knightAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SpellbreakerTimer;
		uint32 BloodHealTimer;

        void Reset()
        {
			SpellbreakerTimer = 2*IN_MILLISECONDS;
			BloodHealTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SpellbreakerTimer <= diff)
			{
				DoCastVictim(SPELL_SPELLBREAKER);
				SpellbreakerTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SpellbreakerTimer -= diff;

			if (BloodHealTimer <= diff)
			{
				DoCast(me, SPELL_BLOOD_HEAL);
				BloodHealTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BloodHealTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_SB_ENRAGE, 0))
				{
					DoCast(me, SPELL_SB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunfury_blood_knightAI(creature);
    }
};

class npc_sunfury_centurion : public CreatureScript
{
public:
    npc_sunfury_centurion() : CreatureScript("npc_sunfury_centurion") { }

    struct npc_sunfury_centurionAI : public QuantumBasicAI
    {
        npc_sunfury_centurionAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 SpellBreakerTimer;

        void Reset()
        {
            SpellBreakerTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SpellBreakerTimer <= diff)
            {
				DoCastVictim(SPELL_SPELLBREAKER);
				SpellBreakerTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
            }
			else SpellBreakerTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_SB_ENRAGE, 0))
				{
					DoCast(me, SPELL_SB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunfury_centurionAI(creature);
    }
};

enum SunfuryNethermancer
{
	SPELL_SHADOW_BOLT        = 9613,
	SPELL_DRAIN_LIFE         = 17173,
	SPELL_BLOODCRYSTAL_SURGE = 35778,
	SPELL_SUMMON_MANA_BEAST  = 36477,

	NPC_MANA_BEAST           = 21267,
};

class npc_sunfury_nethermancer : public CreatureScript
{
public:
    npc_sunfury_nethermancer() : CreatureScript("npc_sunfury_nethermancer") {}

    struct npc_sunfury_nethermancerAI : public QuantumBasicAI
    {
        npc_sunfury_nethermancerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShadowBoltTimer;
		uint32 DrainLifeTimer;

		SummonList Summons;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			DrainLifeTimer = 3*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_BLOODCRYSTAL_SURGE, true);
			DoCast(me, SPELL_SUMMON_MANA_BEAST, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_MANA_BEAST)
				Summons.Summon(summon);
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

			if (DrainLifeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DRAIN_LIFE);
					DrainLifeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else DrainLifeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunfury_nethermancerAI(creature);
    }
};

enum FarahlonLasher
{
	SPELL_ENERGY_FIELD_MODULATOR = 35772,
	SPELL_FLANKING_GROWTH        = 36604,
	SPELL_FL_ENTANGLING_ROOTS    = 12747,
	SPELL_FRENZY                 = 3019,

	NPC_THORNY_GROWTH            = 21331,
	NPC_MUTATED_FARAHLON_LASHER  = 20983,
};

class npc_farahlon_lasher : public CreatureScript
{
public:
    npc_farahlon_lasher() : CreatureScript("npc_farahlon_lasher") {}

    struct npc_farahlon_lasherAI : public QuantumBasicAI
    {
        npc_farahlon_lasherAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 FlankingGrowthTimer;
		uint32 EntanglingRootsTimer;

		SummonList Summons;

        void Reset()
        {
			FlankingGrowthTimer = 0.1*IN_MILLISECONDS;
			EntanglingRootsTimer = 2*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_THORNY_GROWTH)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 65.0f, true))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_ENERGY_FIELD_MODULATOR)
				me->UpdateEntry(NPC_MUTATED_FARAHLON_LASHER);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlankingGrowthTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLANKING_GROWTH);
					FlankingGrowthTimer = 10*IN_MILLISECONDS;
				}
			}
			else FlankingGrowthTimer -= diff;

			if (EntanglingRootsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FL_ENTANGLING_ROOTS);
					EntanglingRootsTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else EntanglingRootsTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_farahlon_lasherAI(creature);
    }
};

enum GlacialTemplar
{
	SPELL_GT_FROST_NOVA  = 14907,
	SPELL_GT_FROST_SHOCK = 15089,
};

class npc_glacial_templar : public CreatureScript
{
public:
    npc_glacial_templar() : CreatureScript("npc_glacial_templar") {}

    struct npc_glacial_templarAI : public QuantumBasicAI
    {
        npc_glacial_templarAI(Creature* creature) : QuantumBasicAI(creature){}

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
					DoCast(target, SPELL_GT_FROST_SHOCK);
					FrostShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostShockTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_GT_FROST_NOVA);
				FrostNovaTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_glacial_templarAI(creature);
    }
};

enum TwilightFrostblade
{
	SPELL_TF_CLEAVE            = 15284,
	SPELL_TF_STRIKE            = 12057,
	SPELL_TF_FRIGHTENING_SHOUT = 19134,
};

class npc_twilight_frostblade : public CreatureScript
{
public:
    npc_twilight_frostblade() : CreatureScript("npc_twilight_frostblade") {}

    struct npc_twilight_frostbladeAI : public QuantumBasicAI
    {
        npc_twilight_frostbladeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 CleaveTimer;
		uint32 FrighteningShoutTimer;

        void Reset()
		{
			StrikeTimer = 1*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;
			FrighteningShoutTimer = 3*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_TF_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_TF_CLEAVE);
				CleaveTimer = 6*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastAOE(SPELL_TF_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = 8*IN_MILLISECONDS;
			}
			else FrighteningShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_frostbladeAI(creature);
    }
};

enum TwilightCryomancer
{
	SPELL_TC_FROSTBOLT   = 12675,
	SPELL_TC_FROST_ARMOR = 18100,
	SPELL_TC_ICE_BLOCK   = 27619,
};

class npc_twilight_cryomancer : public CreatureScript
{
public:
    npc_twilight_cryomancer() : CreatureScript("npc_twilight_cryomancer") {}

    struct npc_twilight_cryomancerAI : public QuantumBasicAI
    {
        npc_twilight_cryomancerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;

		bool IceBlocked;

        void Reset()
		{
			FrostboltTimer = 0.5*IN_MILLISECONDS;

			IceBlocked = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat
			if (!me->HasAura(SPELL_TC_FROST_ARMOR) && !me->IsInCombatActive())
				DoCast(me, SPELL_TC_FROST_ARMOR);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_TC_FROST_ARMOR))
				DoCast(me, SPELL_TC_FROST_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TC_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_30) && !IceBlocked)
			{
				if (!me->HasAuraEffect(SPELL_TC_ICE_BLOCK, 0))
				{
					IceBlocked = true;
					DoCast(me, SPELL_TC_ICE_BLOCK, true);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_cryomancerAI(creature);
    }
};

enum VelanBrightoak
{
	SPELL_VB_FIREBALL = 20811,
};

class npc_velan_brightoak : public CreatureScript
{
public:
    npc_velan_brightoak() : CreatureScript("npc_velan_brightoak") {}

    struct npc_velan_brightoakAI : public QuantumBasicAI
    {
        npc_velan_brightoakAI(Creature* creature) : QuantumBasicAI(creature){}

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
					DoCast(target, SPELL_VB_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_velan_brightoakAI(creature);
    }
};

enum RangerJaela
{
	SPELL_RJ_FIREBALL = 20811,
};

class npc_ranger_jaela : public CreatureScript
{
public:
    npc_ranger_jaela() : CreatureScript("npc_ranger_jaela") {}

    struct npc_ranger_jaelaAI : public QuantumBasicAI
    {
        npc_ranger_jaelaAI(Creature* creature) : QuantumBasicAI(creature){}

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
					DoCast(target, SPELL_RJ_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ranger_jaelaAI(creature);
    }
};

enum MagisterJaronis
{
	SPELL_MJ_FIREBALL = 20811,
};

class npc_magister_jaronis : public CreatureScript
{
public:
    npc_magister_jaronis() : CreatureScript("npc_magister_jaronis") {}

    struct npc_magister_jaronisAI : public QuantumBasicAI
    {
        npc_magister_jaronisAI(Creature* creature) : QuantumBasicAI(creature){}

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
					DoCast(target, SPELL_MJ_FIREBALL);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magister_jaronisAI(creature);
    }
};

enum TwilightMasterXarvos
{
	SPELL_TM_MORTAL_STRIKE   = 16856,
	SPELL_TM_CONCUSSION_BLOW = 22427,
};

class npc_twilight_master_xarvos : public CreatureScript
{
public:
    npc_twilight_master_xarvos() : CreatureScript("npc_twilight_master_xarvos") {}

    struct npc_twilight_master_xarvosAI : public QuantumBasicAI
    {
        npc_twilight_master_xarvosAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 ConcussionBlowTimer;

        void Reset()
		{
			MortalStrikeTimer = 1*IN_MILLISECONDS;
			ConcussionBlowTimer = 2*IN_MILLISECONDS;

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

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TM_MORTAL_STRIKE);
				MortalStrikeTimer = 4*IN_MILLISECONDS;
			}
			else MortalStrikeTimer -= diff;

			if (ConcussionBlowTimer <= diff)
			{
				DoCastVictim(SPELL_TM_CONCUSSION_BLOW);
				ConcussionBlowTimer = 6*IN_MILLISECONDS;
			}
			else ConcussionBlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_master_xarvosAI(creature);
    }
};

enum TwilightSpyViktor
{
	SPELL_TS_SHADOW_BOLT    = 9613,
	SPELL_TS_CURSE_OF_AGONY = 14868,
	SPELL_TS_SHADOWGUARD    = 32861,
	SPELL_SHADOW_CHANNELING = 43897,
};

class npc_twilight_spy_viktor : public CreatureScript
{
public:
    npc_twilight_spy_viktor() : CreatureScript("npc_twilight_spy_viktor") {}

    struct npc_twilight_spy_viktorAI : public QuantumBasicAI
    {
        npc_twilight_spy_viktorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfAgonyTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			CurseOfAgonyTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_SHADOW_CHANNELING);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_SHADOW_CHANNELING);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Check
			if (!me->HasAura(SPELL_TS_SHADOWGUARD) && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_TS_SHADOWGUARD, true);
				DoCast(me, SPELL_SHADOW_CHANNELING, true);
			}

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_TS_SHADOWGUARD))
				DoCast(me, SPELL_TS_SHADOWGUARD, true);

			if (CurseOfAgonyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TS_CURSE_OF_AGONY);
					CurseOfAgonyTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfAgonyTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TS_SHADOW_BOLT);
					ShadowBoltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_spy_viktorAI(creature);
    }
};

enum BloodscaleWavecaller
{
	SPELL_WATER_SHIELD  = 34827,
	SPELL_BW_WATER_BOLT = 32011,
};

class npc_bloodscale_wavecaller : public CreatureScript
{
public:
    npc_bloodscale_wavecaller() : CreatureScript("npc_bloodscale_wavecaller") {}

    struct npc_bloodscale_wavecallerAI : public QuantumBasicAI
    {
        npc_bloodscale_wavecallerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WaterBoltTimer;

        void Reset()
        {
			WaterBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Check
			if (!me->HasAura(SPELL_WATER_SHIELD) && !me->IsInCombatActive())
				DoCast(me, SPELL_WATER_SHIELD);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_WATER_SHIELD))
				DoCast(me, SPELL_WATER_SHIELD, true);

			if (WaterBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BW_WATER_BOLT);
					WaterBoltTimer = 2500;
				}
			}
			else WaterBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodscale_wavecallerAI(creature);
    }
};

enum BloodscaleOverseer
{
	SPELL_BO_HAMSTRING      = 9080,
	SPELL_CURSE_OF_NAZJATAR = 34812,
};

class npc_bloodscale_overseer : public CreatureScript
{
public:
    npc_bloodscale_overseer() : CreatureScript("npc_bloodscale_overseer") {}

    struct npc_bloodscale_overseerAI : public QuantumBasicAI
    {
        npc_bloodscale_overseerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfNazjatarTimer;
		uint32 HamstringTimer;

        void Reset()
        {
			CurseOfNazjatarTimer = 0.5*IN_MILLISECONDS;
			HamstringTimer = 2*IN_MILLISECONDS;

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

			if (CurseOfNazjatarTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_NAZJATAR);
					CurseOfNazjatarTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfNazjatarTimer -= diff;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_BO_HAMSTRING);
				HamstringTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodscale_overseerAI(creature);
    }
};

enum BogflareNeedler
{
	SPELL_LAMBENT_BLOOD = 35328,
};

class npc_bogflare_needler : public CreatureScript
{
public:
    npc_bogflare_needler() : CreatureScript("npc_bogflare_needler") {}

    struct npc_bogflare_needlerAI : public QuantumBasicAI
    {
        npc_bogflare_needlerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LambentBloodTimer;

        void Reset()
        {
			LambentBloodTimer = 0.5*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LambentBloodTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LAMBENT_BLOOD);
					LambentBloodTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LambentBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bogflare_needlerAI(creature);
    }
};

enum WhirligigWafflefry
{
	EVENT_WHIRLIGIG_SAY_1 = 1,
	EVENT_WHIRLIGIG_SAY_2 = 2,
	EVENT_WHIRLIGIG_SAY_3 = 3,
	EVENT_WHIRLIGIG_SAY_4 = 4,
	EVENT_WHIRLIGIG_SAY_5 = 5,

	SAY_WHIRLIGIG_OOC_1   = -1420085,
	SAY_WHIRLIGIG_OOC_2   = -1420086,
	SAY_WHIRLIGIG_OOC_3   = -1420087,
	SAY_WHIRLIGIG_OOC_4   = -1420088,
	SAY_WHIRLIGIG_OOC_5   = -1420089,
	SAY_HARBINGER_OOC     = -1420090,

	NPC_HARBINGER_HARONEM = 19475,
};

class npc_whirligig_wafflefry : public CreatureScript
{
public:
    npc_whirligig_wafflefry() : CreatureScript("npc_whirligig_wafflefry") {}

    struct npc_whirligig_wafflefryAI : public QuantumBasicAI
    {
        npc_whirligig_wafflefryAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
        {
			events.Reset();

			events.ScheduleEvent(EVENT_WHIRLIGIG_SAY_1, 5*IN_MILLISECONDS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
        }

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_WHIRLIGIG_SAY_1:
						DoSendQuantumText(SAY_WHIRLIGIG_OOC_1, me);
						events.ScheduleEvent(EVENT_WHIRLIGIG_SAY_2, 10*IN_MILLISECONDS);
						break;
					case EVENT_WHIRLIGIG_SAY_2:
						DoSendQuantumText(SAY_WHIRLIGIG_OOC_2, me);
						if (Creature* harbinger = me->FindCreatureWithDistance(NPC_HARBINGER_HARONEM, 75.0f))
							DoSendQuantumText(SAY_HARBINGER_OOC, harbinger);
						events.ScheduleEvent(EVENT_WHIRLIGIG_SAY_3, 10*IN_MILLISECONDS);
						break;
					case EVENT_WHIRLIGIG_SAY_3:
						DoSendQuantumText(SAY_WHIRLIGIG_OOC_3, me);
						events.ScheduleEvent(EVENT_WHIRLIGIG_SAY_4, 10*IN_MILLISECONDS);
						break;
					case EVENT_WHIRLIGIG_SAY_4:
						DoSendQuantumText(SAY_WHIRLIGIG_OOC_4, me);
						events.ScheduleEvent(EVENT_WHIRLIGIG_SAY_5, 10*IN_MILLISECONDS);
						break;
					case EVENT_WHIRLIGIG_SAY_5:
						DoSendQuantumText(SAY_WHIRLIGIG_OOC_5, me);
						events.ScheduleEvent(EVENT_WHIRLIGIG_SAY_1, 1.5*MINUTE*IN_MILLISECONDS);
						break;
					default:
						break;
				}
			}	

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_whirligig_wafflefryAI(creature);
    }
};

enum HellfireWarder
{
	SPELL_HELLFIRE_CHANNEL    = 33827,
	SPELL_SHADOW_BOLT_VOLLEY  = 39175,
	SPELL_RAIN_OF_FIRE        = 34435,
	SPELL_SHADOW_BURST        = 34436,
	SPELL_DEATH_COIL          = 34437,
	SPELL_UNSTABLE_AFFLICTION = 34439,
	SPELL_SHADOW_WORD_PAIN    = 34441,
};

class npc_hellfire_warder : public CreatureScript
{
public:
    npc_hellfire_warder() : CreatureScript("npc_hellfire_warder") {}

    struct npc_hellfire_warderAI : public QuantumBasicAI
    {
        npc_hellfire_warderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowWordPainTimer;
		uint32 ShadowBoltVolleyTimer;
		uint32 ShadowBurstTimer;
		uint32 UnstableAfflictionTimer;
		uint32 DeathCoilTimer;
		uint32 RainOfFireTimer;

        void Reset()
        {
			ShadowWordPainTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltVolleyTimer = 1*IN_MILLISECONDS;
			ShadowBurstTimer = 2*IN_MILLISECONDS;
			UnstableAfflictionTimer = 4*IN_MILLISECONDS;
			DeathCoilTimer = 6*IN_MILLISECONDS;
			RainOfFireTimer = 8*IN_MILLISECONDS;

			DoCast(me, SPELL_HELLFIRE_CHANNEL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_HELLFIRE_CHANNEL);
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

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_WORD_PAIN);
					ShadowWordPainTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (ShadowBoltVolleyTimer <= diff)
			{
				DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY, true);
				ShadowBoltVolleyTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ShadowBoltVolleyTimer -= diff;

			if (ShadowBurstTimer <= diff)
			{
				DoCastAOE(SPELL_SHADOW_BURST);
				ShadowBurstTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShadowBurstTimer -= diff;

			if (UnstableAfflictionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UNSTABLE_AFFLICTION);
					UnstableAfflictionTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else UnstableAfflictionTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEATH_COIL);
					DeathCoilTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
				}
			}
			else DeathCoilTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAIN_OF_FIRE, true);
					RainOfFireTimer = urand(13*IN_MILLISECONDS, 14*IN_MILLISECONDS);
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hellfire_warderAI(creature);
    }
};

enum NerubisCenturion
{
	SPELL_DEADLY_POISON = 3583,
};

class npc_nerubis_centurion : public CreatureScript
{
public:
    npc_nerubis_centurion() : CreatureScript("npc_nerubis_centurion") {}

    struct npc_nerubis_centurionAI : public QuantumBasicAI
    {
        npc_nerubis_centurionAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DeadlyPoisonTimer;

        void Reset()
        {
			DeadlyPoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeadlyPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_DEADLY_POISON);
				DeadlyPoisonTimer = 5*IN_MILLISECONDS;
			}
			else DeadlyPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nerubis_centurionAI(creature);
    }
};

enum ShimmerwingMoth
{
	SPELL_SM_SHIMMERWING_DUST = 36592,
	SPELL_SM_WING_BUFFET      = 32914,
};

class npc_shimmerwing_moth : public CreatureScript
{
public:
    npc_shimmerwing_moth() : CreatureScript("npc_shimmerwing_moth") {}

    struct npc_shimmerwing_mothAI : public QuantumBasicAI
    {
        npc_shimmerwing_mothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WingBuffetTimer;
		uint32 ShimmerwingDustTimer;

        void Reset()
        {
			WingBuffetTimer = 2*IN_MILLISECONDS;
			ShimmerwingDustTimer = 4*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WingBuffetTimer <= diff)
			{
				DoCast(SPELL_SM_WING_BUFFET);
				WingBuffetTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else WingBuffetTimer -= diff;

			if (ShimmerwingDustTimer <= diff)
			{
				DoCastAOE(SPELL_SM_SHIMMERWING_DUST);
				ShimmerwingDustTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ShimmerwingDustTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shimmerwing_mothAI(creature);
    }
};

enum GryphoneerLeafbeard
{
	SPELL_TAXI_SHATTER_POINT = 35066,

	GOSSIP_MENU_LEAFBEARD    = 8100,
};

class npc_gryphoneer_leafbeard : public CreatureScript
{
public:
    npc_gryphoneer_leafbeard() : CreatureScript("npc_gryphoneer_leafbeard") {}

    struct npc_gryphoneer_leafbeardAI : public QuantumBasicAI
    {
        npc_gryphoneer_leafbeardAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_LEAFBEARD && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_TAXI_SHATTER_POINT, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
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
        return new npc_gryphoneer_leafbeardAI(creature);
    }
};

void AddSC_outland_npc_scripts()
{
	new npc_warp_chaser();
	new npc_talbuk_stag();
	new npc_clefthoof();
	new npc_clefthoof_bull();
	new npc_talbuk_patriarch();
	new npc_talbuk_thorngrazer();
	new npc_injured_talbuk();
	new npc_wild_elekk();
	new npc_windroc();
	new npc_dust_howler();
	new npc_boulderfist_crusher();
	new npc_boulderfist_mystic();
	new npc_windyreed_scavenger();
	new npc_windyreed_wretch();
	new npc_distraction_of_akama();
	new npc_shadowlord_deathwail();
	new npc_felfire_summoner();
	new npc_ganarg_warp_tinker();
	new npc_moarg_warp_master();
	new npc_mana_beast();
	new npc_lashhan_windwalker();
	new npc_arch_mage_xintor();
	new npc_bladespire_shaman();
	new npc_bladespire_cook();
	new npc_bladespire_champion();
	new npc_bladespire_sober_defender();
	new npc_lakaan();
	new npc_demon_hunter_supplicant();
	new npc_megzeg_nukklebust();
	new npc_arcanist_torseldori();
	new npc_bloodmage();
	new npc_sergeant_shatterskull();
	new npc_bonechewer_riding_wolf();
	new npc_coilskar_myrmidon();
	new npc_coilskar_sorceress();
	new npc_coilskar_cobra();
	new npc_vilewing_chimaera();
	new npc_ashtongue_warrior();
	new npc_ashtongue_shaman();
	new npc_ashtongue_worker();
	new npc_illidari_overseer();
	new npc_elekk_demolisher();
	new npc_redeemed_spirit();
	new npc_painmistress_gabrissa();
	new npc_unyielding_footman();
	new npc_unyielding_sorcerer();
	new npc_unyielding_knight();
	new npc_sporeggar_spawn();
	new npc_starving_bog_lord();
	new npc_starving_fungal_giant();
	new npc_marsh_walker();
	new npc_marshlight_bleeder();
	new npc_greater_sporebat();
	new npc_bloodscale_enchantress();
	new npc_bloodscale_slavedriver();
	new npc_bloodscale_sentry();
	new npc_wrekt_slave();
	new npc_steam_pump_overseer();
	new npc_coilfang_emissary();
	new npc_terrorguard();
	new npc_voidspawn();
	new npc_umbraglow_stinger();
	new npc_fen_strider();
	new npc_young_sporebat();
	new npc_sarinei_whitestar();
	new npc_sporebat();
	new npc_bloodthirst_marshfang();
	new npc_feralfen_hunter();
	new npc_enraged_crusher();
	new npc_storm_rager();
	new npc_lake_spirit();
	new npc_void_conduit();
	new npc_shivan_assassin();
	new npc_shartuul();
	new npc_eye_of_shartuul();
	new npc_eye_of_shartuul_transform();
	new npc_legion_ring_stun_field();
	new npc_dreadmaw();
	new npc_lesser_nether_drake();
	new npc_marauding_crust_burster();
	new npc_vicious_teromoth();
	new npc_dragonmaw_skybreaker();
	new npc_sunhawk_reclaimer();
	new npc_infested_root_walker();
	new npc_rotting_forest_rager();
	new npc_tunneler();
	new npc_magister_idonis();
	new npc_mana_wyrm();
	new npc_sunfury_bowman();
	new npc_sunfury_blood_knight();
	new npc_sunfury_centurion();
	new npc_sunfury_nethermancer();
	new npc_farahlon_lasher();
	new npc_glacial_templar();
	new npc_twilight_frostblade();
	new npc_twilight_cryomancer();
	new npc_velan_brightoak();
	new npc_ranger_jaela();
	new npc_magister_jaronis();
	new npc_twilight_master_xarvos();
	new npc_twilight_spy_viktor();
	new npc_bloodscale_wavecaller();
	new npc_bloodscale_overseer();
	new npc_bogflare_needler();
	new npc_whirligig_wafflefry();
	new npc_hellfire_warder();
	new npc_nerubis_centurion();
	new npc_shimmerwing_moth();
	new npc_gryphoneer_leafbeard();
}