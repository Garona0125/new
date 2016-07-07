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
#include "../../../scripts/Outland/HellfireCitadel/BloodFurnace/blood_furnace.h"

enum Texts
{
	SAY_BLOOD_FURNACE_AGGRO_TEXT_1 = -1420022,
	SAY_BLOOD_FURNACE_AGGRO_TEXT_2 = -1420023,
	SAY_BLOOD_FURNACE_AGGRO_TEXT_3 = -1420024,
	SAY_BLOOD_FURNACE_AGGRO_TEXT_4 = -1420025,
	SAY_BLOOD_FURNACE_AGGRO_TEXT_5 = -1420026,
	SAY_BLOOD_FURNACE_AGGRO_TEXT_6 = -1420027,
	SAY_BLOOD_FURNACE_AGGRO_TEXT_7 = -1420028,
};

enum BloodFurnaceSpells
{
	SPELL_HELLFIRE_CHANNELING     = 31059,
	SPELL_SUMMON_VISUAL           = 35766,
	SPELL_STRIKE                  = 14516,
	SPELL_SHIELD_SLAM             = 15655,
	SPELL_KICK                    = 11978,
	SPELL_THRASH                  = 3417,
	SPELL_FIREBALL_5N             = 15242,
	SPELL_FIREBALL_5H             = 17290,
	SPELL_FLAMESTRIKE_5N          = 18399,
	SPELL_FLAMESTRIKE_5H          = 16102,
	SPELL_FIRE_BLAST              = 16144,
	SPELL_STEALTH                 = 30991,
	SPELL_SLICE_AND_DICE          = 6434,
	SPELL_KIDNEY_SHOT             = 30832,
	SPELL_POISON                  = 34969,
	SPELL_INTERCEPT               = 30151,
	SPELL_ENRAGE                  = 8599,
	SPELL_UPPERCUT                = 10966,
	SPELL_SHADOWMOON_WARLOCK      = 38442,
	SPELL_CORRUPTION_5N           = 32197,
	SPELL_CORRUPTION_5H           = 37113,
	SPELL_SHADOW_BOLT_5N          = 12739,
	SPELL_SHADOW_BOLT_5H          = 15472,
	SPELL_CURSE_OF_TONGUES        = 13338,
	SPELL_FEL_POWER               = 33111,
	SPELL_SEED_OF_CORRUPTION      = 32863,
	SPELL_SUMMON_FELHOUND         = 30851,
	SPELL_SUMMON_SEDUCTRESS       = 30853,
	SPELL_FRENZY                  = 8269,
	SPELL_CHARGE                  = 22120,
	SPELL_HF_FIREBALL_5N          = 11921,
	SPELL_HF_FIREBALL_5H          = 14034,
	SPELL_HF_FIRE_BLAST           = 13341,
	SPELL_MANA_BURN               = 13321,
	SPELL_SPELL_LOCK              = 30849,
	SPELL_SEDUCTION               = 31865,
	SPELL_LASH_OF_PAIN            = 32202,
	SPELL_SILENCE                 = 6726,
	SPELL_THROW_DYNAMITE_5N       = 40062,
	SPELL_THROW_DYNAMITE_5H       = 40064,
	SPELL_THROW_PROXIMITY_BOMB_5N = 30846,
	SPELL_THROW_PROXIMITY_BOMB_5H = 32784,
	SPELL_CONCUSSION_BLOW         = 22427,
	SPELL_STOMP                   = 31900,
	SPELL_BATTLE_SHOUT            = 30833,
	SPELL_PUMMEL                  = 15615,
	SPELL_FG_UPPERCUT             = 18072,
};

class npc_laughing_skull_enforcer : public CreatureScript
{
public:
    npc_laughing_skull_enforcer() : CreatureScript("npc_laughing_skull_enforcer") {}

    struct npc_laughing_skull_enforcerAI : public QuantumBasicAI
    {
        npc_laughing_skull_enforcerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;
		uint32 ShieldSlamTimer;

        void Reset()
		{
			StrikeTimer = 2*IN_MILLISECONDS;
			ShieldSlamTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
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

			if (ShieldSlamTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_SLAM);
				ShieldSlamTimer = 6*IN_MILLISECONDS;
			}
			else ShieldSlamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laughing_skull_enforcerAI(creature);
    }
};

class npc_laughing_skull_rogue : public CreatureScript
{
public:
    npc_laughing_skull_rogue() : CreatureScript("npc_laughing_skull_rogue") {}

    struct npc_laughing_skull_rogueAI : public QuantumBasicAI
    {
        npc_laughing_skull_rogueAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SliceAndDiceTimer;
		uint32 PoisonTimer;
		uint32 KidneyShotTimer;

        void Reset()
		{
			SliceAndDiceTimer = 1*IN_MILLISECONDS;
			PoisonTimer = 2*IN_MILLISECONDS;
			KidneyShotTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_STEALTH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4,
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SliceAndDiceTimer <= diff)
			{
				DoCast(me, SPELL_SLICE_AND_DICE);
				SliceAndDiceTimer = 4*IN_MILLISECONDS;
			}
			else SliceAndDiceTimer -= diff;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_POISON);
				PoisonTimer = 5*IN_MILLISECONDS;
			}
			else PoisonTimer -= diff;

			if (KidneyShotTimer <= diff)
			{
				DoCastVictim(SPELL_KIDNEY_SHOT);
				KidneyShotTimer = 6*IN_MILLISECONDS;
			}
			else KidneyShotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laughing_skull_rogueAI(creature);
    }
};

class npc_laughing_skull_legionnaire : public CreatureScript
{
public:
    npc_laughing_skull_legionnaire() : CreatureScript("npc_laughing_skull_legionnaire") {}

    struct npc_laughing_skull_legionnaireAI : public QuantumBasicAI
    {
        npc_laughing_skull_legionnaireAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 UppercutTimer;

        void Reset()
		{
			UppercutTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCastVictim(SPELL_INTERCEPT, true);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = 4*IN_MILLISECONDS;
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
        return new npc_laughing_skull_legionnaireAI(creature);
    }
};

class npc_laughing_skull_warden : public CreatureScript
{
public:
    npc_laughing_skull_warden() : CreatureScript("npc_laughing_skull_warden") {}

    struct npc_laughing_skull_wardenAI : public QuantumBasicAI
    {
        npc_laughing_skull_wardenAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BattleShoutTimer;

        void Reset()
		{
			BattleShoutTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCastVictim(SPELL_INTERCEPT, true);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
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
        return new npc_laughing_skull_wardenAI(creature);
    }
};

class npc_shadowmoon_adept : public CreatureScript
{
public:
    npc_shadowmoon_adept() : CreatureScript("npc_shadowmoon_adept") {}

    struct npc_shadowmoon_adeptAI : public QuantumBasicAI
    {
        npc_shadowmoon_adeptAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 KickTimer;

        void Reset()
		{
			KickTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_HELLFIRE_CHANNELING, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);

			DoCast(me, SPELL_THRASH);
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

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = 5*IN_MILLISECONDS;
			}
			else KickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_adeptAI(creature);
    }
};

class npc_shadowmoon_warlock : public CreatureScript
{
public:
    npc_shadowmoon_warlock() : CreatureScript("npc_shadowmoon_warlock") {}

    struct npc_shadowmoon_warlockAI : public QuantumBasicAI
    {
        npc_shadowmoon_warlockAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CorruptionTimer;
		uint32 ShadowBoltTimer;
		uint32 CurseOfTonguesTimer;
		uint32 FelPowerTimer;
		uint32 SeedOfCorruptionTimer;

        void Reset()
		{
			CorruptionTimer = 0.5*IN_MILLISECONDS;
			ShadowBoltTimer = 1*IN_MILLISECONDS;
			CurseOfTonguesTimer = 2*IN_MILLISECONDS;
			FelPowerTimer = 4*IN_MILLISECONDS;
			SeedOfCorruptionTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_SHADOWMOON_WARLOCK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CORRUPTION_5N, SPELL_CORRUPTION_5H));
					CorruptionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CorruptionTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H), true);
					ShadowBoltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (CurseOfTonguesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_CURSE_OF_TONGUES);
						CurseOfTonguesTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
					}
				}
			}
			else CurseOfTonguesTimer -= diff;

			if (FelPowerTimer <= diff)
			{
				DoCast(me, SPELL_FEL_POWER, true);
				FelPowerTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else FelPowerTimer -= diff;

			if (SeedOfCorruptionTimer < diff && IsHeroic())
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SEED_OF_CORRUPTION);
					SeedOfCorruptionTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
				}
            }
			else SeedOfCorruptionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_warlockAI(creature);
    }
};

class npc_shadowmoon_summoner : public CreatureScript
{
public:
    npc_shadowmoon_summoner() : CreatureScript("npc_shadowmoon_summoner") {}

    struct npc_shadowmoon_summonerAI : public QuantumBasicAI
    {
        npc_shadowmoon_summonerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireballTimer;
		uint32 FlamestrikeTimer;

		SummonList Summons;

        void Reset()
		{
			FireballTimer = 0.5*IN_MILLISECONDS;
			FlamestrikeTimer = 2.5*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_HELLFIRE_CHANNELING, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(me, SPELL_SUMMON_FELHOUND, true);
			DoCast(me, SPELL_SUMMON_SEDUCTRESS, true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_FELHOUND_MANASTALKER:
				case NPC_SEDUCTRESS:
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
					break;
				default:
					break;
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

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (FlamestrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FLAMESTRIKE_5N, SPELL_FLAMESTRIKE_5H));
					FlamestrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FlamestrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_summonerAI(creature);
    }
};

class npc_shadowmoon_technician : public CreatureScript
{
public:
    npc_shadowmoon_technician() : CreatureScript("npc_shadowmoon_technician") {}

    struct npc_shadowmoon_technicianAI : public QuantumBasicAI
    {
        npc_shadowmoon_technicianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ThrowDynamiteTimer;
		uint32 ThrowProximityBombTimer;
		uint32 SilenceTimer;

        void Reset()
		{
			ThrowDynamiteTimer = 0.5*IN_MILLISECONDS;
			ThrowProximityBombTimer = 2*IN_MILLISECONDS;
			SilenceTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
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
					DoCast(target, DUNGEON_MODE(SPELL_THROW_DYNAMITE_5N, SPELL_THROW_DYNAMITE_5H));
					ThrowDynamiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ThrowDynamiteTimer -= diff;

			if (ThrowProximityBombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_THROW_PROXIMITY_BOMB_5N, SPELL_THROW_PROXIMITY_BOMB_5H));
					ThrowProximityBombTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ThrowProximityBombTimer -= diff;

			if (SilenceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SILENCE);
					SilenceTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else SilenceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_technicianAI(creature);
    }
};

class npc_hellfire_imp : public CreatureScript
{
public:
    npc_hellfire_imp() : CreatureScript("npc_hellfire_imp") {}

    struct npc_hellfire_impAI : public QuantumBasicAI
    {
        npc_hellfire_impAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireBlastTimer;
		uint32 FireballTimer;

        void Reset()
		{
			FireBlastTimer = 0.5*IN_MILLISECONDS;
			FireballTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_SUMMON_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_SUMMON_VISUAL);
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
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = 3*IN_MILLISECONDS;
				}
			}
			else FireballTimer -= diff;

			if (FireBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIRE_BLAST);
					FireBlastTimer = 5*IN_MILLISECONDS;
				}
			}
			else FireBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hellfire_impAI(creature);
    }
};

class npc_hellfire_familiar : public CreatureScript
{
public:
    npc_hellfire_familiar() : CreatureScript("npc_hellfire_familiar") {}

    struct npc_hellfire_familiarAI : public QuantumBasicAI
    {
        npc_hellfire_familiarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireBlastTimer;
		uint32 FireballTimer;

        void Reset()
		{
			FireBlastTimer = 0.5*IN_MILLISECONDS;
			FireballTimer = 2*IN_MILLISECONDS;

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
					DoCast(target, DUNGEON_MODE(SPELL_HF_FIREBALL_5N, SPELL_HF_FIREBALL_5H));
					FireballTimer = 3*IN_MILLISECONDS;
				}
			}
			else FireballTimer -= diff;

			if (FireBlastTimer <= diff && IsHeroic()) // Works Only Heroic Mode
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HF_FIRE_BLAST);
					FireBlastTimer = 5*IN_MILLISECONDS;
				}
			}
			else FireBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hellfire_familiarAI(creature);
    }
};

class npc_fel_orc_neophyte : public CreatureScript
{
public:
    npc_fel_orc_neophyte() : CreatureScript("npc_fel_orc_neophyte") {}

    struct npc_fel_orc_neophyteAI : public QuantumBasicAI
    {
        npc_fel_orc_neophyteAI(Creature* creature) : QuantumBasicAI(creature)
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

		void EnterToBattle(Unit* who)
		{
			DoCastVictim(SPELL_CHARGE);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
		}

		void JustReachedHome()
		{
			me->RemoveAurasDueToSpell(SPELL_FRENZY);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_30))
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
        return new npc_fel_orc_neophyteAI(creature);
    }
};

class npc_felhound_manastalker : public CreatureScript
{
public:
    npc_felhound_manastalker() : CreatureScript("npc_felhound_manastalker") {}

    struct npc_felhound_manastalkerAI : public QuantumBasicAI
    {
        npc_felhound_manastalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ManaBurnTimer;
		uint32 SpellLockTimer;

        void Reset()
		{
			ManaBurnTimer = 0.5*IN_MILLISECONDS;
			SpellLockTimer = 2.5*IN_MILLISECONDS;

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

			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
					}
				}
			}
			else ManaBurnTimer -= diff;

			if (SpellLockTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA && target->HasUnitState(UNIT_STATE_CASTING))
					{
						DoCast(target, SPELL_SPELL_LOCK);
						SpellLockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
					}
				}
            }
			else SpellLockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felhound_manastalkerAI(creature);
    }
};

class npc_seductress : public CreatureScript
{
public:
    npc_seductress() : CreatureScript("npc_seductress") {}

    struct npc_seductressAI : public QuantumBasicAI
    {
        npc_seductressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 LashOfPainTimer;
		uint32 SeductionTimer;

        void Reset()
		{
			LashOfPainTimer = 2*IN_MILLISECONDS;
			SeductionTimer = 3*IN_MILLISECONDS;

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
				DoCastVictim(SPELL_LASH_OF_PAIN);
				LashOfPainTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else LashOfPainTimer -= diff;

			if (SeductionTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SEDUCTION);
					SeductionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else SeductionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_seductressAI(creature);
    }
};

class npc_nascent_fel_orc : public CreatureScript
{
public:
    npc_nascent_fel_orc() : CreatureScript("npc_nascent_fel_orc") {}

    struct npc_nascent_fel_orcAI : public QuantumBasicAI
    {
        npc_nascent_fel_orcAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ConcussionBlowTimer;
		uint32 StompTimer;

        void Reset()
		{
			ConcussionBlowTimer = 2*IN_MILLISECONDS;
			StompTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BLOOD_FURNACE_AGGRO_TEXT_1, SAY_BLOOD_FURNACE_AGGRO_TEXT_2, SAY_BLOOD_FURNACE_AGGRO_TEXT_3, SAY_BLOOD_FURNACE_AGGRO_TEXT_4, 
				SAY_BLOOD_FURNACE_AGGRO_TEXT_5, SAY_BLOOD_FURNACE_AGGRO_TEXT_6, SAY_BLOOD_FURNACE_AGGRO_TEXT_7), me);
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
				ConcussionBlowTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ConcussionBlowTimer -= diff;

			if (StompTimer < diff)
            {
				DoCastAOE(SPELL_STOMP);
				StompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            }
			else StompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nascent_fel_orcAI(creature);
    }
};

class npc_felguard_brute : public CreatureScript
{
public:
    npc_felguard_brute() : CreatureScript("npc_felguard_brute") {}

    struct npc_felguard_bruteAI : public QuantumBasicAI
    {
        npc_felguard_bruteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 UppercutTimer;
		uint32 PummelTimer;

        void Reset()
		{
			UppercutTimer = 2*IN_MILLISECONDS;
			PummelTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_INTERCEPT, true);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_FG_UPPERCUT);
				UppercutTimer = 4*IN_MILLISECONDS;
			}
			else UppercutTimer -= diff;

			if (PummelTimer <= diff)
			{
				DoCastVictim(SPELL_PUMMEL);
				PummelTimer = 6*IN_MILLISECONDS;
			}
			else PummelTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felguard_bruteAI(creature);
    }
};

class npc_felguard_annihilator : public CreatureScript
{
public:
    npc_felguard_annihilator() : CreatureScript("npc_felguard_annihilator") {}

    struct npc_felguard_annihilatorAI : public QuantumBasicAI
    {
        npc_felguard_annihilatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 UppercutTimer;
		uint32 PummelTimer;

        void Reset()
		{
			UppercutTimer = 2*IN_MILLISECONDS;
			PummelTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_INTERCEPT, true);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_FG_UPPERCUT);
				UppercutTimer = 4*IN_MILLISECONDS;
			}
			else UppercutTimer -= diff;

			if (PummelTimer <= diff)
			{
				DoCastVictim(SPELL_PUMMEL);
				PummelTimer = 6*IN_MILLISECONDS;
			}
			else PummelTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felguard_annihilatorAI(creature);
    }
};

void AddSC_blood_furnace_trash()
{
	new npc_laughing_skull_enforcer();
	new npc_laughing_skull_rogue();
	new npc_laughing_skull_legionnaire();
	new npc_laughing_skull_warden();
	new npc_shadowmoon_adept();
	new npc_shadowmoon_warlock();
	new npc_shadowmoon_summoner();
	new npc_shadowmoon_technician();
	new npc_hellfire_imp();
	new npc_hellfire_familiar();
	new npc_fel_orc_neophyte();
	new npc_felhound_manastalker();
	new npc_seductress();
	new npc_nascent_fel_orc();
	new npc_felguard_brute();
	new npc_felguard_annihilator();
}