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
#include "../../../scripts/Outland/HellfireCitadel/HellfireRamparts/hellfire_ramparts.h"

enum Texts
{
	SAY_BONECHEWER_AGGRO_1 = -1420022,
	SAY_BONECHEWER_AGGRO_2 = -1420023,
	SAY_BONECHEWER_AGGRO_3 = -1420024,
	SAY_BONECHEWER_AGGRO_4 = -1420025,
	SAY_BONECHEWER_AGGRO_5 = -1420026,
	SAY_BONECHEWER_AGGRO_6 = -1420027,
	SAY_BONECHEWER_AGGRO_7 = -1420028,
	SAY_BONECHEWER_OOC_1   = -1420076,
	SAY_BONECHEWER_OOC_2   = -1420077,
	SAY_BONECHEWER_RIPPER  = -1420078,
	SAY_BEASTMASTER_AGGRO  = -1420079,
	SAY_BEASTMASTER_SUMMON = -1420080,
};

enum HellfirePampartsSpells
{
	SPELL_DEMORALIZING_SHOUT   = 16244,
	SPELL_DISARM               = 6713,
	SPELL_STRIKE               = 14516,
	SPELL_KIDNEY_SHOT          = 30621,
	SPELL_KNOCK_AWAY           = 10101,
	SPELL_MORTAL_STRIKE        = 16856,
	SPELL_SCORCH_5N            = 15241,
	SPELL_SCORCH_5H            = 36807,
	SPELL_RAIN_OF_FIRE_5N      = 31598,
	SPELL_RAIN_OF_FIRE_5H      = 36808,
	SPELL_AIMED_SHOT           = 30614,
	SPELL_MULTI_SHOT_5N        = 30990,
	SPELL_MULTI_SHOT_5H        = 34879,
	SPELL_SHADOW_CHANNELING    = 43897,
	SPELL_FEL_INFUSION         = 30659,
	SPELL_FEAR                 = 30615,
	SPELL_SHADOW_BOLT_5N       = 12471,
	SPELL_SHADOW_BOLT_5H       = 15232,
	SPELL_ENRAGE               = 18501,
	SPELL_BATTLE_SHOUT         = 30635,
	SPELL_UPPERCUT             = 10966,
	SPELL_RENEW                = 8362,
	SPELL_HEAL_5N              = 12039,
	SPELL_HEAL_5H              = 30643,
	SPELL_POWER_WORD_SHIELD_5N = 17139,
	SPELL_POWER_WORD_SHIELD_5H = 36052,
	SPELL_SHADOW_WORD_PAIN_5N  = 14032,
	SPELL_SHADOW_WORD_PAIN_5H  = 34441,
	SPELL_FURIOUS_HOWL_5N      = 30636,
	SPELL_FURIOUS_HOWL_5H      = 35942,
	SPELL_CARNIVOROUS_BITE_5N  = 30639,
	SPELL_CARNIVOROUS_BITE_5H  = 32901,
};

class npc_bonechewer_hungerer : public CreatureScript
{
public:
    npc_bonechewer_hungerer() : CreatureScript("npc_bonechewer_hungerer") {}

    struct npc_bonechewer_hungererAI : public QuantumBasicAI
    {
        npc_bonechewer_hungererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;
		uint32 DemoralizingShoutTimer;
		uint32 DisarmTimer;

        void Reset()
		{
			StrikeTimer = 2000;
			DemoralizingShoutTimer = 3000;
			DisarmTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
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
				StrikeTimer = 4000;
			}
			else StrikeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = 6000;
			}
			else DemoralizingShoutTimer -= diff;

			if (DisarmTimer <= diff)
			{
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = 8000;
			}
			else DisarmTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonechewer_hungererAI(creature);
    }
};

class npc_bonechewer_ravener : public CreatureScript
{
public:
    npc_bonechewer_ravener() : CreatureScript("npc_bonechewer_ravener") {}

    struct npc_bonechewer_ravenerAI : public QuantumBasicAI
    {
        npc_bonechewer_ravenerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SayTimer;
		uint32 KidneyShotTimer;

        void Reset()
		{
			SayTimer = 500;
			KidneyShotTimer = 2000;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(SAY_BONECHEWER_OOC_1, me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KidneyShotTimer <= diff)
			{
				DoCastVictim(SPELL_KIDNEY_SHOT);
				KidneyShotTimer = 6000;
			}
			else KidneyShotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonechewer_ravenerAI(creature);
    }
};

class npc_bonechewer_destroyer : public CreatureScript
{
public:
    npc_bonechewer_destroyer() : CreatureScript("npc_bonechewer_destroyer") {}

    struct npc_bonechewer_destroyerAI : public QuantumBasicAI
    {
        npc_bonechewer_destroyerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SayTimer;
		uint32 MortalStrikeTimer;
		uint32 KnockAwayTimer;

        void Reset()
		{
			SayTimer = 1000;
			MortalStrikeTimer = 2000;
			KnockAwayTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(SAY_BONECHEWER_OOC_2, me);
				SayTimer = 61000; // 61 second
			}
			else SayTimer -= diff;

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

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCK_AWAY);
				KnockAwayTimer = 6000;
			}
			else KnockAwayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonechewer_destroyerAI(creature);
    }
};

class npc_bonechewer_ripper : public CreatureScript
{
public:
    npc_bonechewer_ripper() : CreatureScript("npc_bonechewer_ripper") {}

    struct npc_bonechewer_ripperAI : public QuantumBasicAI
    {
        npc_bonechewer_ripperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 EnrageTimer;

        void Reset()
		{
			EnrageTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_BONECHEWER_RIPPER, me);
		}

        void UpdateAI(uint32 const diff)
        {
			if (me->HasAura(SPELL_FEL_INFUSION) && !me->IsInCombatActive())
				me->RemoveAurasDueToSpell(SPELL_FEL_INFUSION);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EnrageTimer <= diff)
			{
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				DoCast(me, SPELL_ENRAGE);
				EnrageTimer = 6000;
			}
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonechewer_ripperAI(creature);
    }
};

class npc_bonechewer_beastmaster : public CreatureScript
{
public:
    npc_bonechewer_beastmaster() : CreatureScript("npc_bonechewer_beastmaster") {}

    struct npc_bonechewer_beastmasterAI : public QuantumBasicAI
    {
        npc_bonechewer_beastmasterAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BattleShoutTimer;
		uint32 UppercutTimer;
		uint32 SummonTimer;

		SummonList Summons;

        void Reset()
		{
			BattleShoutTimer = 500;
			UppercutTimer = 2000;
			SummonTimer = 3000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_BEASTMASTER_AGGRO, me);
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_WARHOUND)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
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

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_BATTLE_SHOUT);
				BattleShoutTimer = 4000;
			}
			else BattleShoutTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = 6000;
			}
			else UppercutTimer -= diff;

			if (SummonTimer <= diff)
			{
				DoSendQuantumText(SAY_BEASTMASTER_SUMMON, me);
				me->SummonCreature(NPC_WARHOUND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
				me->SummonCreature(NPC_WARHOUND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
				me->SummonCreature(NPC_WARHOUND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
				SummonTimer = 8000;
			}
			else SummonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonechewer_beastmasterAI(creature);
    }
};

class npc_bleeding_hollow_darkcaster : public CreatureScript
{
public:
    npc_bleeding_hollow_darkcaster() : CreatureScript("npc_bleeding_hollow_darkcaster") {}

    struct npc_bleeding_hollow_darkcasterAI : public QuantumBasicAI
    {
        npc_bleeding_hollow_darkcasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ScorchTimer;
		uint32 RainOfFireTimer;

        void Reset()
		{
			ScorchTimer = 500;
			RainOfFireTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ScorchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SCORCH_5N, SPELL_SCORCH_5H));
					ScorchTimer = urand(2000, 3000);
				}
			}
			else ScorchTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RAIN_OF_FIRE_5N, SPELL_RAIN_OF_FIRE_5H), true);
					RainOfFireTimer = urand(5000, 6000);
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bleeding_hollow_darkcasterAI(creature);
    }
};

class npc_bleeding_hollow_archer : public CreatureScript
{
public:
    npc_bleeding_hollow_archer() : CreatureScript("npc_bleeding_hollow_archer") {}

    struct npc_bleeding_hollow_archerAI : public QuantumBasicAI
    {
        npc_bleeding_hollow_archerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 AimedShotTimer;
		uint32 MultiShotTimer;

        void Reset()
        {
			AimedShotTimer = 500;
			MultiShotTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
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

			if (AimedShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_AIMED_SHOT, true);
						AimedShotTimer = urand(2000, 3000);
					}
				}
			}
			else AimedShotTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_MULTI_SHOT_5N, SPELL_MULTI_SHOT_5H));
						MultiShotTimer = urand(4000, 5000);
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

				if (AimedShotTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_AIMED_SHOT, true);
						AimedShotTimer = urand(6000, 7000);
					}
				}
				else AimedShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bleeding_hollow_archerAI(creature);
    }
};

class npc_bleeding_hollow_scryer : public CreatureScript
{
public:
    npc_bleeding_hollow_scryer() : CreatureScript("npc_bleeding_hollow_scryer") {}

    struct npc_bleeding_hollow_scryerAI : public QuantumBasicAI
    {
        npc_bleeding_hollow_scryerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltTimer;
		uint32 FelInfusionTimer;
		uint32 FearTimer;

        void Reset()
		{
			ShadowBoltTimer = 500;
			FelInfusionTimer = 3500;
			FearTimer = 4500;

			DoCast(me, SPELL_SHADOW_CHANNELING, true);
			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
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
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (FelInfusionTimer <= diff)
			{
				DoCastAOE(SPELL_FEL_INFUSION, true);
				FelInfusionTimer = urand(5000, 6000);
			}
			else FelInfusionTimer -= diff;

			if (FearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = urand(7000, 8000);
				}
			}
			else FearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bleeding_hollow_scryerAI(creature);
    }
};

class npc_hellfire_watcher : public CreatureScript
{
public:
    npc_hellfire_watcher() : CreatureScript("npc_hellfire_watcher") {}

    struct npc_hellfire_watcherAI : public QuantumBasicAI
    {
        npc_hellfire_watcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowWordPainTimer;
		uint32 PowerWordShieldTimer;
		uint32 RenewTimer;
		uint32 HealTimer;

        void Reset()
		{
			ShadowWordPainTimer = 500;
			RenewTimer = 1000;
			HealTimer = 2000;
			PowerWordShieldTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BONECHEWER_AGGRO_1, SAY_BONECHEWER_AGGRO_2, SAY_BONECHEWER_AGGRO_3, SAY_BONECHEWER_AGGRO_4, 
				SAY_BONECHEWER_AGGRO_5, SAY_BONECHEWER_AGGRO_6, SAY_BONECHEWER_AGGRO_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
			if (!me->HasAura(DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H)))
				DoCast(me, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H), true);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H), true);
					ShadowWordPainTimer = urand(3000, 4000);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (RenewTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(me, SPELL_RENEW, true);
					DoCast(target, SPELL_RENEW, true);
					RenewTimer = urand(5000, 6000);
				}
			}
			else RenewTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));
					HealTimer = urand(7000, 8000);
				}
			}
			else HealTimer -= diff;

			if (PowerWordShieldTimer <= diff)
			{
				std::list<Creature*> FriendlyList = DoFindFriendlyMissingBuff(35.0f, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H));
				if (!FriendlyList.empty())
				{
					if (Unit* target = *(FriendlyList.begin()))
					{
						if (target->IsInCombatActive())
						{
							DoCast(target, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H), true);
							PowerWordShieldTimer = urand(9000, 10000);
						}
					}
                }
			}
			else PowerWordShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hellfire_watcherAI(creature);
    }
};

class npc_shattered_hand_warhound : public CreatureScript
{
public:
    npc_shattered_hand_warhound() : CreatureScript("npc_shattered_hand_warhound") {}

    struct npc_shattered_hand_warhoundAI : public QuantumBasicAI
    {
        npc_shattered_hand_warhoundAI(Creature* creature) : QuantumBasicAI(creature)
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
			FuriousHowlTimer = 500;
			CarnivorousBiteTimer = 1000;

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
        return new npc_shattered_hand_warhoundAI(creature);
    }
};

void AddSC_hellfire_ramparts_trash()
{
	new npc_bonechewer_hungerer();
	new npc_bonechewer_ravener();
	new npc_bonechewer_destroyer();
	new npc_bonechewer_ripper();
	new npc_bonechewer_beastmaster();
	new npc_bleeding_hollow_darkcaster();
	new npc_bleeding_hollow_archer();
	new npc_bleeding_hollow_scryer();
	new npc_hellfire_watcher();
	new npc_shattered_hand_warhound();
}