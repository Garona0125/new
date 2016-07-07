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
#include "../../../scripts/Outland/TempestKeep/Botanica/the_botanica.h"

enum Texts
{
	SAY_BLOODWARDER_AGGRO = -1420074,
	SAY_BLOODWARDER_DEATH = -1420075,
};

enum BotanicaSpells
{
	SPELL_CRYSTAL_STRIKE          = 29765,
	SPELL_SLOW                    = 31589,
	SPELL_INTERVENE               = 34784,
	SPELL_SPELL_REFLECTION        = 35399,
	SPELL_HOLY_FURY               = 34809,
	SPELL_MIND_BLAST_5N           = 17194,
	SPELL_MIND_BLAST_5H           = 17287,
	SPELL_GREATER_HEAL            = 35096,
	SPELL_IMPENDING_COMA          = 34800,
	SPELL_GREENKEEPERS_FURY_5N    = 34798,
	SPELL_GREENKEEPERS_FURY_5H    = 39121,
	SPELL_NATURE_SHOCK_5N         = 34797,
	SPELL_NATURE_SHOCK_5H         = 39120,
	SPELL_CALL_OF_THE_FALCON      = 34852,
	SPELL_DETERRENCE              = 31567,
	SPELL_MULTI_SHOT              = 34879,
	SPELL_WING_CLIP               = 32908,
	SPELL_CHARGE                  = 32323,
	SPELL_BLOODBURN               = 34856,
	SPELL_SWOOP                   = 18144,
	SPELL_ARCANE_FLURRY           = 34821,
	SPELL_MIND_SHOCK              = 35352,
	SPELL_FROST_SHOCK             = 34353,
	SPELL_FLAME_SHOCK             = 34354,
	SPELL_POISON_SHIELD           = 34355,
	SPELL_FIRE_BREATH_POTION_5N   = 34359,
	SPELL_FIRE_BREATH_POTION_5H   = 39128,
	SPELL_VIAL_OF_POISON_5N       = 34358,
	SPELL_VIAL_OF_POISON_5H       = 39127,
	SPELL_ENTANGLING_ROOTS        = 22127,
	SPELL_REGROWTH_5N             = 34361,
	SPELL_REGROWTH_5H             = 39125,
	SPELL_REJUVENATE_PLANT_5N     = 34254,
	SPELL_REJUVENATE_PLANT_5H     = 39126,
	SPELL_NATURES_RAGE            = 34350,
	SPELL_ARCANE_EXPLOSION        = 34791,
	SPELL_CRYSTAL_CHANNEL         = 32958,
	SPELL_SUNSEEKER_AURA          = 34634,
	SPELL_SOUL_CHANNEL            = 34637,
	SPELL_SUNSEEKER_BLESSING_5N   = 34173,
	SPELL_SUNSEEKER_BLESSING_5H   = 34187,
	SPELL_SUNSEEKER_BLESSING_HEAL = 34222,
	SPELL_POLYMORPH               = 34639,
	SPELL_WILTING_TOUCH           = 34640,
	SPELL_DEATH_AND_DECAY_5N      = 34642,
	SPELL_DEATH_AND_DECAY_5H      = 39347,
	SPELL_ARCANE_BLAST            = 34793,
	SPELL_ARCANE_VOLLEY_5N        = 34785,
	SPELL_ARCANE_VOLLEY_5H        = 38633,
	SPELL_LASH_5N                 = 34644,
	SPELL_LASH_5H                 = 39122,
	SPELL_FROST_FORM              = 34202,
	SPELL_FIRE_FORM               = 34203,
	SPELL_ARCANE_FORM             = 34204,
	SPELL_SHADOW_FORM             = 34205,
	SPELL_KIDNEY_SHOT             = 30621,
	SPELL_MIND_NUMBING_POISON     = 34615,
	SPELL_DEADLY_POISON           = 34616,
	SPELL_CLEAVE                  = 15284,
	SPELL_STEALTH                 = 30991,
	SPELL_BACKSTAB                = 34614,
	SPELL_VICIOUS_BITE            = 34351,
	SPELL_CORRODE_ARMOR           = 34643,
	SPELL_FEAR                    = 30584,
	SPELL_ARCANE_STRIKE           = 34820,
	SPELL_MIND_FLAY               = 35507,
	SPELL_FREEZING_TOUCH          = 34752,
	SPELL_TOXIC_VOLLEY            = 25812,
	SPELL_BIND_FEET               = 34782,
	SPELL_FIRE_BLAST              = 36339,
	SPELL_SHOOT_THORNS            = 34745,
};

class npc_bloodwarder_protector : public CreatureScript
{
public:
    npc_bloodwarder_protector() : CreatureScript("npc_bloodwarder_protector") {}

    struct npc_bloodwarder_protectorAI : public QuantumBasicAI
    {
        npc_bloodwarder_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SlowTimer;
		uint32 CrystalStrikeTimer;
		uint32 InterveneTimer;
		uint32 SpellReflectionTimer;

        void Reset()
		{
			SlowTimer = 0.5*IN_MILLISECONDS;
			CrystalStrikeTimer = 2*IN_MILLISECONDS;
			InterveneTimer = 4*IN_MILLISECONDS;
			SpellReflectionTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_BLOODWARDER_AGGRO, me);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoSendQuantumText(SAY_BLOODWARDER_DEATH, me);
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
					DoCast(target, SPELL_SLOW, true);
					SlowTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SlowTimer -= diff;

			if (CrystalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_CRYSTAL_STRIKE);
				CrystalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CrystalStrikeTimer -= diff;

			if (InterveneTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					DoCast(target, SPELL_INTERVENE);
					InterveneTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else InterveneTimer -= diff;

			if (SpellReflectionTimer <= diff)
			{
				DoCast(me, SPELL_SPELL_REFLECTION, true);
				SpellReflectionTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else SpellReflectionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_protectorAI(creature);
    }
};

class npc_bloodwarder_mender : public CreatureScript
{
public:
    npc_bloodwarder_mender() : CreatureScript("npc_bloodwarder_mender") {}

    struct npc_bloodwarder_menderAI : public QuantumBasicAI
    {
        npc_bloodwarder_menderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 HolyFuryTimer;
		uint32 MindBlastTimer;
		uint32 GreaterHealTimer;

        void Reset()
		{
			HolyFuryTimer = 0.5*IN_MILLISECONDS;
			MindBlastTimer = 1*IN_MILLISECONDS;
			GreaterHealTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (HolyFuryTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_HOLY_FURY);
				HolyFuryTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else HolyFuryTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Holy Fury Buff Check in Combat
			if (!me->HasAura(SPELL_HOLY_FURY))
				DoCast(me, SPELL_HOLY_FURY, true);

			if (MindBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_BLAST_5N, SPELL_MIND_BLAST_5H));
					MindBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindBlastTimer -= diff;

			if (GreaterHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_GREATER_HEAL);
					GreaterHealTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else GreaterHealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_menderAI(creature);
    }
};

class npc_bloodwarder_greenkeeper : public CreatureScript
{
public:
    npc_bloodwarder_greenkeeper() : CreatureScript("npc_bloodwarder_greenkeeper") {}

    struct npc_bloodwarder_greenkeeperAI : public QuantumBasicAI
    {
        npc_bloodwarder_greenkeeperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ImpendingComaTimer;
		uint32 NatureShockTimer;
		uint32 GreenkeepersFuryTimer;

        void Reset()
		{
			ImpendingComaTimer = 1*IN_MILLISECONDS;
			NatureShockTimer = 2*IN_MILLISECONDS;
			GreenkeepersFuryTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImpendingComaTimer <= diff)
			{
				DoCastVictim(SPELL_IMPENDING_COMA);
				ImpendingComaTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ImpendingComaTimer -= diff;

			if (NatureShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_NATURE_SHOCK_5N, SPELL_NATURE_SHOCK_5H));
					NatureShockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else NatureShockTimer -= diff;

			if (GreenkeepersFuryTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_GREENKEEPERS_FURY_5N, SPELL_GREENKEEPERS_FURY_5H));
					GreenkeepersFuryTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else GreenkeepersFuryTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_greenkeeperAI(creature);
    }
};

class npc_bloodwarder_falconer : public CreatureScript
{
public:
    npc_bloodwarder_falconer() : CreatureScript("npc_bloodwarder_falconer") {}

    struct npc_bloodwarder_falconerAI : public QuantumBasicAI
    {
        npc_bloodwarder_falconerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MultiShotTimer;
		uint32 CallOfTheFalconTimer;
		uint32 DeterrenceTimer;
		uint32 WingClipTimer;

        void Reset()
		{
			MultiShotTimer = 0.5*IN_MILLISECONDS;
			CallOfTheFalconTimer = 2*IN_MILLISECONDS;
			DeterrenceTimer = 3*IN_MILLISECONDS;
			WingClipTimer = 5*IN_MILLISECONDS;

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

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MULTI_SHOT);
					MultiShotTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else MultiShotTimer -= diff;

			if (CallOfTheFalconTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CALL_OF_THE_FALCON);
					CallOfTheFalconTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else CallOfTheFalconTimer -= diff;

			if (DeterrenceTimer <= diff)
			{
				DoCast(me, SPELL_DETERRENCE);
				DeterrenceTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DeterrenceTimer -= diff;

			if (WingClipTimer <= diff)
			{
				DoCastVictim(SPELL_WING_CLIP);
				WingClipTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else WingClipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_falconerAI(creature);
    }
};

class npc_bloodwarder_steward : public CreatureScript
{
public:
    npc_bloodwarder_steward() : CreatureScript("npc_bloodwarder_steward") {}

    struct npc_bloodwarder_stewardAI : public QuantumBasicAI
    {
        npc_bloodwarder_stewardAI(Creature* creature) : QuantumBasicAI(creature)
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

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_ARCANE_FLURRY);
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
        return new npc_bloodwarder_stewardAI(creature);
    }
};

class npc_sunseeker_researcher : public CreatureScript
{
public:
    npc_sunseeker_researcher() : CreatureScript("npc_sunseeker_researcher") {}

    struct npc_sunseeker_researcherAI : public QuantumBasicAI
    {
        npc_sunseeker_researcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PoisionShieldTimer;
		uint32 MindShockTimer;
		uint32 FrostShockTimer;
		uint32 FlameShockTimer;

        void Reset()
		{
			PoisionShieldTimer = 0.5*IN_MILLISECONDS;
			MindShockTimer = 1*IN_MILLISECONDS;
			FrostShockTimer = 2*IN_MILLISECONDS;
			FlameShockTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING_NO_SHEATHE);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PoisionShieldTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_POISON_SHIELD);
				PoisionShieldTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else PoisionShieldTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Poison Shield Buff Check in Combat
			if (!me->HasAura(SPELL_POISON_SHIELD))
				DoCast(me, SPELL_POISON_SHIELD, true);

			if (MindShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_SHOCK);
					MindShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindShockTimer -= diff;

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_SHOCK);
					FrostShockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FrostShockTimer -= diff;

			if (FlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_SHOCK);
					FlameShockTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else FlameShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_researcherAI(creature);
    }
};

class npc_sunseeker_chemist : public CreatureScript
{
public:
    npc_sunseeker_chemist() : CreatureScript("npc_sunseeker_chemist") {}

    struct npc_sunseeker_chemistAI : public QuantumBasicAI
    {
        npc_sunseeker_chemistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireBreathPotionTimer;
		uint32 VialOfPoisonTimer;

        void Reset()
		{
			FireBreathPotionTimer = 2*IN_MILLISECONDS;
			VialOfPoisonTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING_NO_SHEATHE);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireBreathPotionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					me->SetFacingToObject(target);
					DoCast(target, DUNGEON_MODE(SPELL_FIRE_BREATH_POTION_5N, SPELL_FIRE_BREATH_POTION_5H));
					FireBreathPotionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireBreathPotionTimer -= diff;

			if (VialOfPoisonTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_VIAL_OF_POISON_5N, SPELL_VIAL_OF_POISON_5H));
				VialOfPoisonTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else VialOfPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_chemistAI(creature);
    }
};

class npc_sunseeker_herbalist : public CreatureScript
{
public:
    npc_sunseeker_herbalist() : CreatureScript("npc_sunseeker_herbalist") {}

    struct npc_sunseeker_herbalistAI : public QuantumBasicAI
    {
        npc_sunseeker_herbalistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 EntanglingRootsTimer;

        void Reset()
		{
			EntanglingRootsTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING_NO_SHEATHE);
        }

		void EnterToBattle(Unit* /*who*/){}

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

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_herbalistAI(creature);
    }
};

class npc_sunseeker_botanist : public CreatureScript
{
public:
    npc_sunseeker_botanist() : CreatureScript("npc_sunseeker_botanist") {}

    struct npc_sunseeker_botanistAI : public QuantumBasicAI
    {
        npc_sunseeker_botanistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 NaturesRageTimer;
		uint32 RegrowthTimer;
		uint32 RejuvenationPlantTimer;

        void Reset()
		{
			NaturesRageTimer = 0.1*IN_MILLISECONDS;
			RegrowthTimer = 0.5*IN_MILLISECONDS;
			RejuvenationPlantTimer = 1*IN_MILLISECONDS;

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

			if (NaturesRageTimer <= diff)
			{
				DoCast(me, SPELL_NATURES_RAGE, true);
				NaturesRageTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else NaturesRageTimer -= diff;

			if (RegrowthTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_REGROWTH_5N, SPELL_REGROWTH_5H));
					RegrowthTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else RegrowthTimer -= diff;

			if (RejuvenationPlantTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_REJUVENATE_PLANT_5N, SPELL_REJUVENATE_PLANT_5H));
					RejuvenationPlantTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else RejuvenationPlantTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_botanistAI(creature);
    }
};

class npc_sunseeker_geomancer : public CreatureScript
{
public:
    npc_sunseeker_geomancer() : CreatureScript("npc_sunseeker_geomancer") {}

    struct npc_sunseeker_geomancerAI : public QuantumBasicAI
    {
        npc_sunseeker_geomancerAI(Creature* creature) : QuantumBasicAI(creature)
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
			ArcaneExplosionTimer = 0.5*IN_MILLISECONDS;

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

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_EXPLOSION, true);
				ArcaneExplosionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_geomancerAI(creature);
    }
};

class npc_sunseeker_channeler : public CreatureScript
{
public:
    npc_sunseeker_channeler() : CreatureScript("npc_sunseeker_channeler") {}

    struct npc_sunseeker_channelerAI : public QuantumBasicAI
    {
        npc_sunseeker_channelerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CrystalChannelTimer;
		uint32 SunseekerBlessingBuffTimer;
		uint32 SoulChannelTimer;
		uint32 SunseekerBlessingTimer;

        void Reset()
		{
			CrystalChannelTimer = 0.1*IN_MILLISECONDS;
			SunseekerBlessingBuffTimer = 0.5*IN_MILLISECONDS;
			SoulChannelTimer = 2*IN_MILLISECONDS;
			SunseekerBlessingTimer = 8*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
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
			if (CrystalChannelTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 15.0f))
				{
					me->SetFacingToObject(trigger);
					DoCast(trigger, SPELL_CRYSTAL_CHANNEL, true);
					CrystalChannelTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else CrystalChannelTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Sunseeker Aura Check in Combat
			if (!me->HasAura(SPELL_SUNSEEKER_AURA))
				DoCast(me, SPELL_SUNSEEKER_AURA, true);

			if (SunseekerBlessingBuffTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SUNSEEKER_BLESSING_5N, SPELL_SUNSEEKER_BLESSING_5H), true);
					SunseekerBlessingBuffTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else SunseekerBlessingBuffTimer -= diff;

			if (SoulChannelTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SOUL_CHANNEL, true);
					SoulChannelTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else SoulChannelTimer -= diff;

			if (SunseekerBlessingTimer <= diff)
			{
				DoCastAOE(SPELL_SUNSEEKER_BLESSING_HEAL);
				SunseekerBlessingTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else SunseekerBlessingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_channelerAI(creature);
    }
};

class npc_sunseeker_harvester : public CreatureScript
{
public:
    npc_sunseeker_harvester() : CreatureScript("npc_sunseeker_harvester") {}

    struct npc_sunseeker_harvesterAI : public QuantumBasicAI
    {
        npc_sunseeker_harvesterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PolymorphTimer;
		uint32 WiltingTouchTimer;

        void Reset()
		{
			PolymorphTimer = 0.5*IN_MILLISECONDS;
			WiltingTouchTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING_NO_SHEATHE);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PolymorphTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POLYMORPH);
					PolymorphTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PolymorphTimer -= diff;

			if (WiltingTouchTimer <= diff)
			{
				DoCastVictim(SPELL_WILTING_TOUCH);
				WiltingTouchTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else WiltingTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_harvesterAI(creature);
    }
};

class npc_sunseeker_gene_splicer : public CreatureScript
{
public:
    npc_sunseeker_gene_splicer() : CreatureScript("npc_sunseeker_gene_splicer") {}

    struct npc_sunseeker_gene_splicerAI : public QuantumBasicAI
    {
        npc_sunseeker_gene_splicerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DeathAndDecayTimer;

        void Reset()
		{
			DeathAndDecayTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING_NO_SHEATHE);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeathAndDecayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_DEATH_AND_DECAY_5N, SPELL_DEATH_AND_DECAY_5H));
					DeathAndDecayTimer = 4*IN_MILLISECONDS;
				}
			}
			else DeathAndDecayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_gene_splicerAI(creature);
    }
};

class npc_nethervine_inciter : public CreatureScript
{
public:
    npc_nethervine_inciter() : CreatureScript("npc_nethervine_inciter") {}

    struct npc_nethervine_inciterAI : public QuantumBasicAI
    {
        npc_nethervine_inciterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MindNumbingPoisonTimer;
		uint32 DeadlyPoisonTimer;
		uint32 KidneyShotTimer;

        void Reset()
		{
			MindNumbingPoisonTimer = 1*IN_MILLISECONDS;
			DeadlyPoisonTimer = 2*IN_MILLISECONDS;
			KidneyShotTimer = 3*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (MindNumbingPoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_NUMBING_POISON);
					MindNumbingPoisonTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else MindNumbingPoisonTimer -= diff;

			if (DeadlyPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_DEADLY_POISON);
				DeadlyPoisonTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DeadlyPoisonTimer -= diff;

			if (KidneyShotTimer <= diff)
			{
				DoCastVictim(SPELL_KIDNEY_SHOT);
				KidneyShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else KidneyShotTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAura(DUNGEON_MODE(SPELL_SUNSEEKER_BLESSING_5N, SPELL_SUNSEEKER_BLESSING_5H)))
					DoCast(me, DUNGEON_MODE(SPELL_SUNSEEKER_BLESSING_5N, SPELL_SUNSEEKER_BLESSING_5H), true);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nethervine_inciterAI(creature);
    }
};

class npc_nethervine_reaper : public CreatureScript
{
public:
    npc_nethervine_reaper() : CreatureScript("npc_nethervine_reaper") {}

    struct npc_nethervine_reaperAI : public QuantumBasicAI
    {
        npc_nethervine_reaperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BlessingTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			BlessingTimer = 0.5*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;

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

			if (BlessingTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_SUNSEEKER_BLESSING_5N, SPELL_SUNSEEKER_BLESSING_5H), true);
				BlessingTimer = 25*IN_MILLISECONDS;
			}
			else BlessingTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nethervine_reaperAI(creature);
    }
};

class npc_nethervine_trickster : public CreatureScript
{
public:
    npc_nethervine_trickster() : CreatureScript("npc_nethervine_trickster") {}

    struct npc_nethervine_tricksterAI : public QuantumBasicAI
    {
        npc_nethervine_tricksterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BackstabTimer;

        void Reset()
		{
			BackstabTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_STEALTH);
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

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_BACKSTAB);
				BackstabTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nethervine_tricksterAI(creature);
    }
};

class npc_bloodfalcon : public CreatureScript
{
public:
    npc_bloodfalcon() : CreatureScript("npc_bloodfalcon") {}

    struct npc_bloodfalconAI : public QuantumBasicAI
    {
        npc_bloodfalconAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ChargeTimer;
		uint32 BloodburnTimer;
		uint32 SwoopTimer;

        void Reset()
		{
			ChargeTimer = 0.5*IN_MILLISECONDS;
			BloodburnTimer = 2*IN_MILLISECONDS;
			SwoopTimer = 4*IN_MILLISECONDS;

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

			if (ChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CHARGE, true);
					ChargeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ChargeTimer -= diff;

			if (BloodburnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 20.0f, true))
				{
					DoCast(target, SPELL_BLOODBURN);
					BloodburnTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else BloodburnTimer -= diff;

			if (SwoopTimer <= diff)
			{
				DoCastVictim(SPELL_SWOOP);
				SwoopTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else SwoopTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodfalconAI(creature);
    }
};

class npc_tempest_forge_peacekeeper : public CreatureScript
{
public:
    npc_tempest_forge_peacekeeper() : CreatureScript("npc_tempest_forge_peacekeeper") {}

    struct npc_tempest_forge_peacekeeperAI : public QuantumBasicAI
    {
        npc_tempest_forge_peacekeeperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ArcaneVolleyTimer;
		uint32 ArcaneBlastTimer;
		uint32 ArcaneExplosionTimer;

        void Reset()
		{
			ArcaneVolleyTimer = 0.5*IN_MILLISECONDS;
			ArcaneBlastTimer = 2*IN_MILLISECONDS;
			ArcaneExplosionTimer = 3*IN_MILLISECONDS;

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

			if (ArcaneVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_VOLLEY_5N, SPELL_ARCANE_VOLLEY_5H));
					ArcaneVolleyTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ArcaneVolleyTimer -= diff;

			if (ArcaneBlastTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_BLAST);
				ArcaneBlastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ArcaneBlastTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_EXPLOSION, true);
				ArcaneExplosionTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_forge_peacekeeperAI(creature);
    }
};

class npc_greater_frayer : public CreatureScript
{
public:
    npc_greater_frayer() : CreatureScript("npc_greater_frayer") {}

    struct npc_greater_frayerAI : public QuantumBasicAI
    {
        npc_greater_frayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 LashTimer;

        void Reset()
		{
			LashTimer = 1*IN_MILLISECONDS;

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

			if (LashTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_LASH_5N, SPELL_LASH_5H));
				LashTimer = 4*IN_MILLISECONDS;
			}
			else LashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_frayerAI(creature);
    }
};

class npc_frayer : public CreatureScript
{
public:
    npc_frayer() : CreatureScript("npc_frayer") {}

    struct npc_frayerAI : public QuantumBasicAI
    {
        npc_frayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 OutCombatResistanceTimer;
		uint32 InCombatResistanceTimer;

        void Reset()
		{
			OutCombatResistanceTimer = 0.1*IN_MILLISECONDS;
			InCombatResistanceTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (OutCombatResistanceTimer <= diff)
			{
				DoCast(me, RAND(SPELL_FROST_FORM, SPELL_FIRE_FORM, SPELL_ARCANE_FORM, SPELL_SHADOW_FORM));
				OutCombatResistanceTimer = 13.5*IN_MILLISECONDS;
			}
			else OutCombatResistanceTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InCombatResistanceTimer <= diff)
			{
				DoCast(me, RAND(SPELL_FROST_FORM, SPELL_FIRE_FORM, SPELL_ARCANE_FORM, SPELL_SHADOW_FORM));
				InCombatResistanceTimer = 13.5*IN_MILLISECONDS;
			}
			else InCombatResistanceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frayerAI(creature);
    }
};

class npc_thorn_flayer : public CreatureScript
{
public:
    npc_thorn_flayer() : CreatureScript("npc_thorn_flayer") {}

    struct npc_thorn_flayerAI : public QuantumBasicAI
    {
        npc_thorn_flayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MindFlayTimer;

        void Reset()
		{
			MindFlayTimer = 0.5*IN_MILLISECONDS;

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

			if (MindFlayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_FLAY);
					MindFlayTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thorn_flayerAI(creature);
    }
};

class npc_thorn_lasher : public CreatureScript
{
public:
	npc_thorn_lasher() : CreatureScript("npc_thorn_lasher") {}

    struct npc_thorn_lasherAI : public QuantumBasicAI
    {
        npc_thorn_lasherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 LashTimer;

        void Reset()
		{
			LashTimer = 1*IN_MILLISECONDS;

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

			if (LashTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_LASH_5N, SPELL_LASH_5H));
				LashTimer = 4*IN_MILLISECONDS;
			}
			else LashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thorn_lasherAI(creature);
    }
};

class npc_frayer_wildling : public CreatureScript
{
public:
    npc_frayer_wildling() : CreatureScript("npc_frayer_wildling") {}

    struct npc_frayer_wildlingAI : public QuantumBasicAI
    {
        npc_frayer_wildlingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 LashTimer;

        void Reset()
		{
			LashTimer = 1*IN_MILLISECONDS;

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

			if (LashTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_LASH_5N, SPELL_LASH_5H));
				LashTimer = 4*IN_MILLISECONDS;
			}
			else LashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frayer_wildlingAI(creature);
    }
};

class npc_frayer_protector : public CreatureScript
{
public:
    npc_frayer_protector() : CreatureScript("npc_frayer_protector") {}

    struct npc_frayer_protectorAI : public QuantumBasicAI
    {
        npc_frayer_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootThornsTimer;

        void Reset()
		{
			ShootThornsTimer = 1*IN_MILLISECONDS;

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

			if (ShootThornsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHOOT_THORNS);
					ShootThornsTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShootThornsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frayer_protectorAI(creature);
    }
};

class npc_mutate_fleshlasher : public CreatureScript
{
public:
    npc_mutate_fleshlasher() : CreatureScript("npc_mutate_fleshlasher") {}

    struct npc_mutate_fleshlasherAI : public QuantumBasicAI
    {
        npc_mutate_fleshlasherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ViciousBiteTimer;

        void Reset()
		{
			ViciousBiteTimer = 1*IN_MILLISECONDS;

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

			if (ViciousBiteTimer <= diff)
			{
				DoCastVictim(SPELL_VICIOUS_BITE);
				ViciousBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ViciousBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mutate_fleshlasherAI(creature);
    }
};

class npc_mutate_horror : public CreatureScript
{
public:
    npc_mutate_horror() : CreatureScript("npc_mutate_horror") {}

    struct npc_mutate_horrorAI : public QuantumBasicAI
    {
        npc_mutate_horrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CorrodeArmorTimer;

        void Reset()
		{
			CorrodeArmorTimer = 1*IN_MILLISECONDS;

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

			if (CorrodeArmorTimer <= diff)
			{
				DoCastVictim(SPELL_CORRODE_ARMOR);
				CorrodeArmorTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CorrodeArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mutate_horrorAI(creature);
    }
};

class npc_mutate_fear_shrieker : public CreatureScript
{
public:
    npc_mutate_fear_shrieker() : CreatureScript("npc_mutate_fear_shrieker") {}

    struct npc_mutate_fear_shriekerAI : public QuantumBasicAI
    {
        npc_mutate_fear_shriekerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FearTimer;

        void Reset()
		{
			FearTimer = 1*IN_MILLISECONDS;

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

			if (FearTimer <= diff)
			{
				DoCastAOE(SPELL_FEAR);
				FearTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mutate_fear_shriekerAI(creature);
    }
};

class npc_summoned_bloodwarder_reservist : public CreatureScript
{
public:
    npc_summoned_bloodwarder_reservist() : CreatureScript("npc_summoned_bloodwarder_reservist") {}

    struct npc_summoned_bloodwarder_reservistAI : public QuantumBasicAI
    {
        npc_summoned_bloodwarder_reservistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ArcaneStrikeTimer;

        void Reset()
		{
			ArcaneStrikeTimer = 1*IN_MILLISECONDS;

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

			if (ArcaneStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_STRIKE);
				ArcaneStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ArcaneStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_summoned_bloodwarder_reservistAI(creature);
    }
};

class npc_summoned_bloodwarder_mender : public CreatureScript
{
public:
    npc_summoned_bloodwarder_mender() : CreatureScript("npc_summoned_bloodwarder_mender") {}

    struct npc_summoned_bloodwarder_menderAI : public QuantumBasicAI
    {
        npc_summoned_bloodwarder_menderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MindBlastTimer;
		uint32 GreaterHealTimer;

        void Reset()
		{
			MindBlastTimer = 0.5*IN_MILLISECONDS;
			GreaterHealTimer = 3*IN_MILLISECONDS;

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

			// Holy Fury Buff Check in Combat
			if (!me->HasAura(SPELL_HOLY_FURY))
				DoCast(me, SPELL_HOLY_FURY, true);

			if (MindBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_BLAST_5N, SPELL_MIND_BLAST_5H));
					MindBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindBlastTimer -= diff;

			if (GreaterHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_GREATER_HEAL);
					GreaterHealTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else GreaterHealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_summoned_bloodwarder_menderAI(creature);
    }
};

class npc_white_seedling : public CreatureScript
{
public:
    npc_white_seedling() : CreatureScript("npc_white_seedling") {}

    struct npc_white_seedlingAI : public QuantumBasicAI
    {
        npc_white_seedlingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FreezingTouchTimer;

        void Reset()
		{
			FreezingTouchTimer = 1*IN_MILLISECONDS;

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

			if (FreezingTouchTimer <= diff)
			{
				DoCastVictim(SPELL_FREEZING_TOUCH);
				FreezingTouchTimer = 5*IN_MILLISECONDS;
			}
			else FreezingTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_white_seedlingAI(creature);
    }
};

class npc_green_seedling : public CreatureScript
{
public:
    npc_green_seedling() : CreatureScript("npc_green_seedling") {}

    struct npc_green_seedlingAI : public QuantumBasicAI
    {
        npc_green_seedlingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ToxicVolleyTimer;

        void Reset()
		{
			ToxicVolleyTimer = 0.5*IN_MILLISECONDS;

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

			if (ToxicVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TOXIC_VOLLEY);
					ToxicVolleyTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ToxicVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_green_seedlingAI(creature);
    }
};

class npc_blue_seedling : public CreatureScript
{
public:
    npc_blue_seedling() : CreatureScript("npc_blue_seedling") {}

    struct npc_blue_seedlingAI : public QuantumBasicAI
    {
        npc_blue_seedlingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BindFeetTimer;

        void Reset()
		{
			BindFeetTimer = 0.5*IN_MILLISECONDS;

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

			if (BindFeetTimer <= diff)
			{
				DoCastAOE(SPELL_BIND_FEET);
				BindFeetTimer = 4*IN_MILLISECONDS;
			}
			else BindFeetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blue_seedlingAI(creature);
    }
};

class npc_red_seedling : public CreatureScript
{
public:
    npc_red_seedling() : CreatureScript("npc_red_seedling") {}

    struct npc_red_seedlingAI : public QuantumBasicAI
    {
        npc_red_seedlingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FirebalstTimer;

        void Reset()
		{
			FirebalstTimer = 0.5*IN_MILLISECONDS;

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

			if (FirebalstTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIRE_BLAST);
					FirebalstTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FirebalstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_red_seedlingAI(creature);
    }
};

void AddSC_botanica_trash()
{
	new npc_bloodwarder_protector();
	new npc_bloodwarder_mender();
	new npc_bloodwarder_greenkeeper();
	new npc_bloodwarder_falconer();
	new npc_bloodwarder_steward();
	new npc_sunseeker_researcher();
	new npc_sunseeker_chemist();
	new npc_sunseeker_herbalist();
	new npc_sunseeker_botanist();
	new npc_sunseeker_geomancer();
	new npc_sunseeker_channeler();
	new npc_sunseeker_harvester();
	new npc_sunseeker_gene_splicer();
	new npc_nethervine_inciter();
	new npc_nethervine_reaper();
	new npc_nethervine_trickster();
	new npc_bloodfalcon();
	new npc_tempest_forge_peacekeeper();
	new npc_greater_frayer();
	new npc_frayer();
	new npc_thorn_flayer();
	new npc_thorn_lasher();
	new npc_frayer_wildling();
	new npc_frayer_protector();
	new npc_mutate_fleshlasher();
	new npc_mutate_horror();
	new npc_mutate_fear_shrieker();
	new npc_summoned_bloodwarder_reservist();
	new npc_summoned_bloodwarder_mender();
	new npc_white_seedling();
	new npc_green_seedling();
	new npc_blue_seedling();
	new npc_red_seedling();
}