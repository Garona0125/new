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

enum ManaTombsSpells
{
	SPELL_ENRAGE                 = 8599,
	SPELL_CHARGE                 = 22911,
	SPELL_BATTLE_SHOUT           = 31403,
	SPELL_SOUL_STRIKE            = 32315,
	SPELL_SINGE                  = 33865,
	SPELL_STRIKE_5N              = 15580,
	SPELL_STRIKE_5H              = 34920,
	SPELL_SHIELD_BASH            = 33871,
	SPELL_SLOW                   = 25603,
	SPELL_ARCANE_MISSILES_5N     = 15790,
	SPELL_ARCANE_MISSILES_5H     = 22272,
	SPELL_POWER_WORD_SHIELD_5N   = 17139,
	SPELL_POWER_WORD_SHIELD_5H   = 36052,
	SPELL_HEAL_5N                = 34945,
	SPELL_HEAL_5H                = 39378,
	SPELL_HOLY_NOVA_5N           = 34944,
	SPELL_HOLY_NOVA_5H           = 37669,
	SPELL_SHADOWFORM             = 16592,
	SPELL_SHADOW_WORD_PAIN       = 34942,
	SPELL_MANA_BURN_5N           = 34931,
	SPELL_MANA_BURN_5H           = 34930,
	SPELL_IMMOLATE_5N            = 17883,
	SPELL_IMMOLATE_5H            = 37668,
	SPELL_COUNTERSPELL           = 37470,
	SPELL_SUMMON_ETHEREAL_WRAITH = 32316,
	SPELL_SHADOW_BOLT_VOLLEY     = 34934,
	SPELL_BLAST_WAVE_5N          = 17145,
	SPELL_BLAST_WAVE_5H          = 38064,
	SPELL_POLYMORPH              = 13323,
	SPELL_CURSE_OF_IMPOTENCE     = 34925,
	SPELL_SHADOWS_EMBRACE        = 34922,
	SPELL_PSYCHIC_SCREAM         = 34322,
	SPELL_DEATH_COIL             = 38065,
	SPELL_GOUGE                  = 34940,
	SPELL_PHANTOM_STRIKE_5N      = 33925,
	SPELL_PHANTOM_STRIKE_5H      = 39332,
	SPELL_MANA_BURN              = 15785,
	SPELL_FAERIE_FIRE            = 25602,
	SPELL_ARCANE_EXPLOSION       = 34933,
};

enum Creatures
{
	NPC_ETHEREAL_WRAITH = 18394,
};

enum Texts
{
	SAY_ETHEREAL_AGGRO_1 = -1420058,
	SAY_ETHEREAL_AGGRO_2 = -1420059,
	SAY_ETHEREAL_AGGRO_3 = -1420060,
	SAY_ETHEREAL_AGGRO_4 = -1420061,
};

class npc_ethereal_crypt_raider : public CreatureScript
{
public:
    npc_ethereal_crypt_raider() : CreatureScript("npc_ethereal_crypt_raider") {}

    struct npc_ethereal_crypt_raiderAI : public QuantumBasicAI
    {
        npc_ethereal_crypt_raiderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BattleShoutTimer;
		uint32 SoulStrikeTimer;

        void Reset()
		{
			BattleShoutTimer = 1000;
			SoulStrikeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCastVictim(SPELL_CHARGE, true);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
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
				BattleShoutTimer = 4000;
			}
			else BattleShoutTimer -= diff;

			if (SoulStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SOUL_STRIKE);
				SoulStrikeTimer = 5000;
			}
			else SoulStrikeTimer -= diff;

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
        return new npc_ethereal_crypt_raiderAI(creature);
    }
};

class npc_ethereal_scavenger : public CreatureScript
{
public:
    npc_ethereal_scavenger() : CreatureScript("npc_ethereal_scavenger") {}

    struct npc_ethereal_scavengerAI : public QuantumBasicAI
    {
        npc_ethereal_scavengerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SingeTimer;
		uint32 StrikeTimer;
		uint32 ShieldBashTimer;

        void Reset()
		{
			SingeTimer = 2000;
			StrikeTimer = 4000;
			ShieldBashTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SingeTimer <= diff)
			{
				DoCastVictim(SPELL_SINGE);
				SingeTimer = 8000;
			}
			else SingeTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_STRIKE_5N, SPELL_STRIKE_5H));
				StrikeTimer = 4000;
			}
			else StrikeTimer -= diff;

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
        return new npc_ethereal_scavengerAI(creature);
    }
};

class npc_ethereal_sorcerer : public CreatureScript
{
public:
    npc_ethereal_sorcerer() : CreatureScript("npc_ethereal_sorcerer") {}

    struct npc_ethereal_sorcererAI : public QuantumBasicAI
    {
        npc_ethereal_sorcererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SlowTimer;
		uint32 ArcaneMissilesTimer;

        void Reset()
		{
			SlowTimer = 500;
			ArcaneMissilesTimer = 1500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
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
					SlowTimer = urand(3000, 4000);
				}
			}
			else SlowTimer -= diff;

			if (ArcaneMissilesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_MISSILES_5N, SPELL_ARCANE_MISSILES_5H));
					ArcaneMissilesTimer = urand(5000, 6000);
				}
			}
			else ArcaneMissilesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_sorcererAI(creature);
    }
};

class npc_ethereal_priest : public CreatureScript
{
public:
    npc_ethereal_priest() : CreatureScript("npc_ethereal_priest") {}

    struct npc_ethereal_priestAI : public QuantumBasicAI
    {
        npc_ethereal_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PowerWordShieldTimer;
		uint32 HolyNovaTimer;
		uint32 HealTimer;

        void Reset()
		{
			PowerWordShieldTimer = 1000;
			HolyNovaTimer = 2000;
			HealTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PowerWordShieldTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H));
				PowerWordShieldTimer = urand(4000, 5000);
			}
			else PowerWordShieldTimer -= diff;

			if (HolyNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_HOLY_NOVA_5N, SPELL_HOLY_NOVA_5H));
				HolyNovaTimer = urand(6000, 7000);
			}
			else HolyNovaTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H), true);
					HealTimer = urand(8000, 9000);
				}
			}
			else HealTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_priestAI(creature);
    }
};

class npc_ethereal_darkcaster : public CreatureScript
{
public:
    npc_ethereal_darkcaster() : CreatureScript("npc_ethereal_darkcaster") {}

    struct npc_ethereal_darkcasterAI : public QuantumBasicAI
    {
        npc_ethereal_darkcasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowWordPainTimer;
		uint32 ManaBurnTimer;

        void Reset()
		{
			ShadowWordPainTimer = 1000;
			ManaBurnTimer = 2000;

			DoCast(me, SPELL_SHADOWFORM);
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
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
					ShadowWordPainTimer = urand(4000, 5000);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, DUNGEON_MODE(SPELL_MANA_BURN_5N, SPELL_MANA_BURN_5H));
						ManaBurnTimer = urand(6000, 7000);
					}
				}
			}
			else ManaBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_darkcasterAI(creature);
    }
};

class npc_ethereal_spellbinder : public CreatureScript
{
public:
    npc_ethereal_spellbinder() : CreatureScript("npc_ethereal_spellbinder") {}

    struct npc_ethereal_spellbinderAI : public QuantumBasicAI
    {
        npc_ethereal_spellbinderAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ImmolateTimer;
		uint32 CounterspellTimer;
		uint32 SummonEtherealWraithTimer;

		SummonList Summons;

        void Reset()
		{
			ImmolateTimer = 1000;
			CounterspellTimer = 3000;
			SummonEtherealWraithTimer = 6000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(me, SPELL_SUMMON_ETHEREAL_WRAITH, true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_ETHEREAL_WRAITH)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
			}
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

			if (CounterspellTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_COUNTERSPELL);
						CounterspellTimer = urand(5000, 6000);
					}
				}
			}
			else CounterspellTimer -= diff;

			if (SummonEtherealWraithTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_ETHEREAL_WRAITH, true);
				DoCast(me, SPELL_SUMMON_ETHEREAL_WRAITH, true);
				SummonEtherealWraithTimer = urand(8000, 9000);
			}
			else SummonEtherealWraithTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_spellbinderAI(creature);
    }
};

class npc_ethereal_wraith : public CreatureScript
{
public:
    npc_ethereal_wraith() : CreatureScript("npc_ethereal_wraith") {}

    struct npc_ethereal_wraithAI : public QuantumBasicAI
    {
        npc_ethereal_wraithAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltVolleyTimer;

        void Reset()
		{
			ShadowBoltVolleyTimer = 1000;

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

			if (ShadowBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
					ShadowBoltVolleyTimer = 5000;
				}
			}
			else ShadowBoltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_wraithAI(creature);
    }
};

class npc_ethereal_theurgist : public CreatureScript
{
public:
    npc_ethereal_theurgist() : CreatureScript("npc_ethereal_theurgist") {}

    struct npc_ethereal_theurgistAI : public QuantumBasicAI
    {
        npc_ethereal_theurgistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;
		uint32 PolymorphTimer;
		uint32 BlastWaveTimer;

        void Reset()
		{
			StrikeTimer = 1000;
			PolymorphTimer = 2000;
			BlastWaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_STRIKE_5N, SPELL_STRIKE_5H));
				StrikeTimer = urand(3000, 4000);
			}
			else StrikeTimer -= diff;

			if (PolymorphTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POLYMORPH);
					PolymorphTimer = urand(5000, 6000);
				}
			}
			else PolymorphTimer -= diff;

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BLAST_WAVE_5N, SPELL_BLAST_WAVE_5H));
				BlastWaveTimer = urand(7000, 8000);
			}
			else BlastWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_theurgistAI(creature);
    }
};

class npc_nexus_terror : public CreatureScript
{
public:
    npc_nexus_terror() : CreatureScript("npc_nexus_terror") {}

    struct npc_nexus_terrorAI : public QuantumBasicAI
    {
        npc_nexus_terrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CurseOfImpotenceTimer;
		uint32 ShadowsEmbraceTimer;
		uint32 DeathCoilTimer;
		uint32 PsychicScreamTimer;

        void Reset()
		{
			CurseOfImpotenceTimer = 1000;
			ShadowsEmbraceTimer = 2000;
			DeathCoilTimer = 4000;
			PsychicScreamTimer = 6000;

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

			if (CurseOfImpotenceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_IMPOTENCE);
					CurseOfImpotenceTimer = urand(4000, 5000);
				}
			}
			else CurseOfImpotenceTimer -= diff;

			if (ShadowsEmbraceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOWS_EMBRACE);
					ShadowsEmbraceTimer = urand(6000, 7000);
				}
			}
			else ShadowsEmbraceTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEATH_COIL);
					DeathCoilTimer = urand(8000, 9000);
				}
			}
			else DeathCoilTimer -= diff;

			if (PsychicScreamTimer <= diff)
			{
				DoCastAOE(SPELL_PSYCHIC_SCREAM);
				PsychicScreamTimer = urand(10000, 11000);
			}
			else PsychicScreamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nexus_terrorAI(creature);
    }
};

class npc_nexus_stalker : public CreatureScript
{
public:
    npc_nexus_stalker() : CreatureScript("npc_nexus_stalker") {}

    struct npc_nexus_stalkerAI : public QuantumBasicAI
    {
        npc_nexus_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PhantomStrikeTimer;
		uint32 GougeTimer;

        void Reset()
		{
			PhantomStrikeTimer = 2000;
			GougeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ETHEREAL_AGGRO_1, SAY_ETHEREAL_AGGRO_2, SAY_ETHEREAL_AGGRO_3, SAY_ETHEREAL_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PhantomStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_PHANTOM_STRIKE_5N, SPELL_PHANTOM_STRIKE_5H));
				PhantomStrikeTimer = 4000;
			}
			else PhantomStrikeTimer -= diff;

			if (GougeTimer <= diff)
			{
				DoCastVictim(SPELL_GOUGE);
				GougeTimer = 6000;
			}
			else GougeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nexus_stalkerAI(creature);
    }
};

class npc_mana_leech : public CreatureScript
{
public:
    npc_mana_leech() : CreatureScript("npc_mana_leech") {}

    struct npc_mana_leechAI : public QuantumBasicAI
    {
        npc_mana_leechAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ManaBurnTimer;
		uint32 FaerieFireTimer;

        void Reset()
		{
			ManaBurnTimer = 1000;
			FaerieFireTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_ARCANE_EXPLOSION, true);
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
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = urand(4000, 5000);
					}
				}
			}
			else ManaBurnTimer -= diff;

			if (FaerieFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FAERIE_FIRE);
					FaerieFireTimer = urand(6000, 7000);
				}
			}
			else FaerieFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_leechAI(creature);
    }
};

void AddSC_mana_tombs_trash()
{
	new npc_ethereal_crypt_raider();
	new npc_ethereal_scavenger();
	new npc_ethereal_sorcerer();
	new npc_ethereal_priest();
	new npc_ethereal_darkcaster();
	new npc_ethereal_spellbinder();
	new npc_ethereal_wraith();
	new npc_ethereal_theurgist();
	new npc_nexus_terror();
	new npc_nexus_stalker();
	new npc_mana_leech();
}