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
#include "../../../scripts/Northrend/DraktharonKeep/drak_tharon_keep.h"

enum DrakTharonSpells
{
	SPELL_GHOST_STRIKE         = 36093,
	SPELL_CRUSH_ARMOR          = 33661,
	SPELL_HEAL                 = 47668,
	SPELL_REFLECT_AURA         = 36096,
	SPELL_HAMSTRING            = 9080,
	SPELL_SHIELD_BASH          = 11972,
	SPELL_CRIPPLE              = 50379,
	SPELL_UNHOLY_FRENZY        = 49805,
	SPELL_FROSTBOLT_5N         = 50378,
	SPELL_FROSTBOLT_5H         = 59017,
	SPELL_KNOCKDOWN            = 35011,
	SPELL_SHADOW_BLAST_5N      = 49696,
	SPELL_SHADOW_BLAST_5H      = 59013,
	SPELL_BLOOD_SIPHON_5N      = 49701,
	SPELL_BLOOD_SIPHON_5H      = 59015,
	SPELL_SHADOW_VOID_5N       = 55847,
	SPELL_SHADOW_VOID_5H       = 59014,
	SPELL_PIERCE_ARMOR         = 46202,
	SPELL_FRENZY               = 50933,
	SPELL_CLEAVE               = 51917,
	SPELL_FLESH_ROT_5N         = 49678,
	SPELL_FLESH_ROT_5H         = 59007,
	SPELL_DISEASE_CLOUD        = 16345,
	SPELL_WB_CLEAVE            = 40504,
	SPELL_BILE_VOMIT_5N        = 49703,
	SPELL_BILE_VOMIT_5H        = 59018,
	SPELL_ENCASING_WEBS        = 49704,
	SPELL_POISON_SPIT_5N       = 49708,
	SPELL_POISON_SPIT_5H       = 32330,
	SPELL_IMPALE               = 16001,
	SPELL_CURSE_OF_BLOOD_5N    = 50414,
	SPELL_CURSE_OF_BLOOD_5H    = 59009,
	SPELL_BACKHAND             = 6253,
	SPELL_HOOKED_NET           = 49711,
	SPELL_SHOOT_5N             = 49712,
	SPELL_SHOOT_5H             = 59001,
	SPELL_GUT_RIP              = 49710,
	SPELL_REND                 = 13738,
	SPELL_MORTAL_STRIKE        = 16856,
	SPELL_LIGHTNING_BOLT_5N    = 48895,
	SPELL_LIGHTNING_BOLT_5H    = 59006,
	SPELL_CHAIN_HEAL           = 48894,
	SPELL_FEAR                 = 51240,
	SPELL_DEAFENING_ROAR_5N    = 49721,
	SPELL_DEAFENING_ROAR_5H    = 59010,
	SPELL_ICY_TOUCH_5N         = 49723,
	SPELL_ICY_TOUCH_5H         = 59011,
	SPELL_DC_FRENZY            = 8269,
	SPELL_BATTLE_SHOUT         = 49724,
	SPELL_WHIRLWIND            = 13736,
	SPELL_FLASH_OF_DARKNESS_5N = 49668,
    SPELL_FLASH_OF_DARKNESS_5H = 59004,
};

enum Misc
{
	MOUNT_DRAKKARI_HANDLER   = 6469,
	MOUNT_DRAKKARI_BAT_RIDER = 26751,
};

class npc_risen_drakkari_warrior : public CreatureScript
{
public:
    npc_risen_drakkari_warrior() : CreatureScript("npc_risen_drakkari_warrior") {}

    struct npc_risen_drakkari_warriorAI : public QuantumBasicAI
    {
        npc_risen_drakkari_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 GhostStrikeTimer;
		uint32 CrushArmorTimer;
		uint32 HealTimer;
		uint32 SpellReflectAura;

        void Reset()
        {
			GhostStrikeTimer = 2000;
			CrushArmorTimer = 3000;
			HealTimer = 5000;
			SpellReflectAura = 7000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (GhostStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_GHOST_STRIKE);
				GhostStrikeTimer = urand(3000, 4000);
			}
			else GhostStrikeTimer -= diff;

			if (CrushArmorTimer <= diff)
			{
				DoCastVictim(SPELL_CRUSH_ARMOR);
				CrushArmorTimer = urand(5000, 6000);
			}
			else CrushArmorTimer -= diff;

			if (HealTimer <= diff)
			{
				DoCast(me, SPELL_HEAL);
				HealTimer = urand(7000, 8000);
			}
			else HealTimer -= diff;

			if (SpellReflectAura <= diff)
			{
				DoCast(me, SPELL_REFLECT_AURA);
				SpellReflectAura = urand(9000, 10000);
			}
			else SpellReflectAura -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_drakkari_warriorAI(creature);
    }
};

class npc_drakkari_guardian : public CreatureScript
{
public:
    npc_drakkari_guardian() : CreatureScript("npc_drakkari_guardian") {}

    struct npc_drakkari_guardianAI : public QuantumBasicAI
    {
        npc_drakkari_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HamstringTimer;
		uint32 ShieldBashTimer;
		uint32 HealTimer;

        void Reset()
        {
			HamstringTimer = 2000;
			ShieldBashTimer = 3000;
			HealTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = urand(3000, 4000);
			}
			else HamstringTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(5000, 6000);
			}
			else ShieldBashTimer -= diff;

			if (HealTimer <= diff)
			{
				DoCast(me, SPELL_HEAL);
				HealTimer = urand(7000, 8000);
			}
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_guardianAI(creature);
    }
};

class npc_scourge_reanimator : public CreatureScript
{
public:
    npc_scourge_reanimator() : CreatureScript("npc_scourge_reanimator") {}

    struct npc_scourge_reanimatorAI : public QuantumBasicAI
    {
        npc_scourge_reanimatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CrippleTimer;
		uint32 FrostboltTimer;
		uint32 UnholyFrenzyTimer;

        void Reset()
        {
			CrippleTimer = 500;
			FrostboltTimer = 2000;
			UnholyFrenzyTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrippleTimer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLE);
				CrippleTimer = urand(3000, 4000);
			}
			else CrippleTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(5000, 6000);
				}
			}
			else FrostboltTimer -= diff;

			if (UnholyFrenzyTimer <= diff)
			{
				DoCast(me, SPELL_UNHOLY_FRENZY);
				UnholyFrenzyTimer = urand(7000, 8000);
			}
			else UnholyFrenzyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_reanimatorAI(creature);
    }
};

class npc_risen_drakkari_soulmage : public CreatureScript
{
public:
    npc_risen_drakkari_soulmage() : CreatureScript("npc_risen_drakkari_soulmage") {}

    struct npc_risen_drakkari_soulmageAI : public QuantumBasicAI
    {
        npc_risen_drakkari_soulmageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 KnockdownTimer;
		uint32 ShadowBlastTimer;
		uint32 ShadowVoidTimer;
		uint32 BloodSiphonTimer;

        void Reset()
        {
			KnockdownTimer = 2000;
			ShadowBlastTimer = 4000;
			ShadowVoidTimer = 6000;
			BloodSiphonTimer = 8000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = urand(4000, 5000);
			}
			else KnockdownTimer -= diff;

			if (ShadowBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BLAST_5N, SPELL_SHADOW_BLAST_5H));
					ShadowBlastTimer = urand(4000, 5000);
				}
			}
			else ShadowBlastTimer -= diff;

			if (ShadowVoidTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_VOID_5N, SPELL_SHADOW_VOID_5H));
					ShadowVoidTimer = urand(7000, 9000);
				}
			}
			else ShadowVoidTimer -= diff;

			if (BloodSiphonTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_BLOOD_SIPHON_5N, SPELL_BLOOD_SIPHON_5H));
				BloodSiphonTimer = urand(11000, 12000);
			}
			else BloodSiphonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_drakkari_soulmageAI(creature);
    }
};

class npc_flesheating_ghoul : public CreatureScript
{
public:
    npc_flesheating_ghoul() : CreatureScript("npc_flesheating_ghoul") {}

    struct npc_flesheating_ghoulAI : public QuantumBasicAI
    {
        npc_flesheating_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PierceArmorTimer;

        void Reset()
        {
			PierceArmorTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCE_ARMOR);
				PierceArmorTimer = urand(4000, 5000);
			}
			else PierceArmorTimer -= diff;

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
        return new npc_flesheating_ghoulAI(creature);
    }
};

class npc_ghoul_tormentor : public CreatureScript
{
public:
    npc_ghoul_tormentor() : CreatureScript("npc_ghoul_tormentor") {}

    struct npc_ghoul_tormentorAI : public QuantumBasicAI
    {
        npc_ghoul_tormentorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 FleshRotTimer;

        void Reset()
        {
			CleaveTimer = 2000;
			FleshRotTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(4000, 5000);
			}
			else CleaveTimer -= diff;

			if (FleshRotTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FLESH_ROT_5N, SPELL_FLESH_ROT_5H));
				FleshRotTimer = urand(7000, 8000);
			}
			else FleshRotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghoul_tormentorAI(creature);
    }
};

class npc_wretched_belcher : public CreatureScript
{
public:
    npc_wretched_belcher() : CreatureScript("npc_wretched_belcher") {}

    struct npc_wretched_belcherAI : public QuantumBasicAI
    {
        npc_wretched_belcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 BileVomitTimer;

        void Reset()
        {
			CleaveTimer = 2000;
			BileVomitTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_DISEASE_CLOUD);
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
				DoCastVictim(SPELL_WB_CLEAVE);
				CleaveTimer = urand(4000, 5000);
			}
			else CleaveTimer -= diff;

			if (BileVomitTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_BILE_VOMIT_5N, SPELL_BILE_VOMIT_5H));
				BileVomitTimer = urand(7000, 8000);
			}
			else BileVomitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_belcherAI(creature);
    }
};

class npc_darkweb_recluse : public CreatureScript
{
public:
    npc_darkweb_recluse() : CreatureScript("npc_darkweb_recluse") {}

    struct npc_darkweb_recluseAI : public QuantumBasicAI
    {
        npc_darkweb_recluseAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EncasingWebsTimer;
		uint32 PoisonSpitTimer;

        void Reset()
        {
			EncasingWebsTimer = 1000;
			PoisonSpitTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EncasingWebsTimer <= diff)
			{
				DoCastVictim(SPELL_ENCASING_WEBS);
				EncasingWebsTimer = urand(3000, 4000);
			}
			else EncasingWebsTimer -= diff;

			if (PoisonSpitTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POISON_SPIT_5N, SPELL_POISON_SPIT_5H));
					PoisonSpitTimer = urand(5000, 6000);
				}
			}
			else PoisonSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkweb_recluseAI(creature);
    }
};

class npc_risen_drakkari_bat_rider : public CreatureScript
{
public:
    npc_risen_drakkari_bat_rider() : CreatureScript("npc_risen_drakkari_bat_rider") {}

    struct npc_risen_drakkari_bat_riderAI : public QuantumBasicAI
    {
        npc_risen_drakkari_bat_riderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ImpaleTimer;
		uint32 CurseOfBloodTimer;

        void Reset()
        {
			ImpaleTimer = 500;
			CurseOfBloodTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->Mount(MOUNT_DRAKKARI_BAT_RIDER);
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

			if (ImpaleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IMPALE);
					ImpaleTimer = urand(3000, 4000);
				}
			}
			else ImpaleTimer -= diff;

			if (CurseOfBloodTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CURSE_OF_BLOOD_5N, SPELL_CURSE_OF_BLOOD_5H));
				CurseOfBloodTimer = urand(5000, 6000);
			}
			else CurseOfBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_drakkari_bat_riderAI(creature);
    }
};

class npc_risen_drakkari_handler : public CreatureScript
{
public:
    npc_risen_drakkari_handler() : CreatureScript("npc_risen_drakkari_handler") {}

    struct npc_risen_drakkari_handlerAI : public QuantumBasicAI
    {
        npc_risen_drakkari_handlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BackhandTimer;
		uint32 HookedNetTimer;

        void Reset()
        {
			BackhandTimer = 2000;
			HookedNetTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->Mount(MOUNT_DRAKKARI_HANDLER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			DoCastVictim(DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BackhandTimer <= diff)
			{
				DoCastVictim(SPELL_BACKHAND);
				BackhandTimer = urand(4000, 5000);
			}
			else BackhandTimer -= diff;

			if (HookedNetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HOOKED_NET);
					HookedNetTimer = urand(6000, 7000);
				}
			}
			else HookedNetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_drakkari_handlerAI(creature);
    }
};

class npc_drakkari_gutripper : public CreatureScript
{
public:
	npc_drakkari_gutripper() : CreatureScript("npc_drakkari_gutripper") { }

	struct npc_drakkari_gutripperAI : public QuantumBasicAI
	{
		npc_drakkari_gutripperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = me->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		InstanceScript* instance;

		uint32 GutRipTimer;

		void Reset()
		{
			GutRipTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GutRipTimer <= diff)
			{
				DoCastVictim(SPELL_GUT_RIP);
				GutRipTimer = urand(4000, 5000);
			}
			else GutRipTimer -= diff;

			DoMeleeAttackIfReady();
		}

		void JustDied(Unit* /*killer*/)
		{
			if (Creature* dred = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DRED)))
				dred->AI()->DoAction(ACTION_RAPTOR_KILLED);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_drakkari_gutripperAI(creature);
	}
};

class npc_drakkari_scytheclaw : public CreatureScript
{
public:
	npc_drakkari_scytheclaw() : CreatureScript("npc_drakkari_scytheclaw") { }

	struct npc_drakkari_scytheclawAI : public QuantumBasicAI
	{
		npc_drakkari_scytheclawAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = me->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		InstanceScript* instance;

		uint32 RendTimer;

		void Reset()
		{
			RendTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_REND);
				RendTimer = urand(10000, 15000);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
		}

		void JustDied(Unit* /*who*/)
		{
			if (Creature* dred = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DRED)))
				dred->AI()->DoAction(ACTION_RAPTOR_KILLED);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_drakkari_scytheclawAI(creature);
	}
};

class npc_scourge_brute : public CreatureScript
{
public:
    npc_scourge_brute() : CreatureScript("npc_scourge_brute") {}

    struct npc_scourge_bruteAI : public QuantumBasicAI
    {
        npc_scourge_bruteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MortalStrikeTimer;
		uint32 KnockdownTimer;
		uint32 HealTimer;

        void Reset()
        {
			MortalStrikeTimer = 2000;
			KnockdownTimer = 4000;
			HealTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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
				DoCastVictim(SPELL_HAMSTRING);
				MortalStrikeTimer = urand(4000, 5000);
			}
			else MortalStrikeTimer -= diff;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = urand(6000, 7000);
			}
			else KnockdownTimer -= diff;

			if (HealTimer <= diff)
			{
				DoCast(me, SPELL_HEAL);
				HealTimer = urand(8000, 9000);
			}
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_bruteAI(creature);
    }
};

class npc_drakkari_shaman : public CreatureScript
{
public:
    npc_drakkari_shaman() : CreatureScript("npc_drakkari_shaman") {}

    struct npc_drakkari_shamanAI : public QuantumBasicAI
    {
        npc_drakkari_shamanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 ChainHealTimer;
		uint32 HealTimer;

        void Reset()
        {
			LightningBoltTimer = 500;
			ChainHealTimer = 2000;
			HealTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

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
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (ChainHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_CHAIN_HEAL);
					ChainHealTimer = urand(5000, 6000);
				}
			}
			else ChainHealTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_HEAL);
					HealTimer = urand(7000, 8000);
				}
			}
			else HealTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_HEAL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_shamanAI(creature);
    }
};

class npc_risen_drakkari_death_knight : public CreatureScript
{
public:
    npc_risen_drakkari_death_knight() : CreatureScript("npc_risen_drakkari_death_knight") {}

    struct npc_risen_drakkari_death_knightAI : public QuantumBasicAI
    {
        npc_risen_drakkari_death_knightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FearTimer;
		uint32 IcyTouchTimer;
		uint32 DeafeningRoarTimer;

        void Reset()
        {
			FearTimer = 500;
			IcyTouchTimer = 2000;
			DeafeningRoarTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = urand(3000, 4000);
				}
			}
			else FearTimer -= diff;

			if (IcyTouchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ICY_TOUCH_5N, SPELL_ICY_TOUCH_5H));
					IcyTouchTimer = urand(5000, 6000);
				}
			}
			else IcyTouchTimer -= diff;

			if (DeafeningRoarTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_DEAFENING_ROAR_5N, SPELL_DEAFENING_ROAR_5H));
				DeafeningRoarTimer = urand(7000, 8000);
			}
			else DeafeningRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_drakkari_death_knightAI(creature);
    }
};

class npc_drakkari_commander : public CreatureScript
{
public:
    npc_drakkari_commander() : CreatureScript("npc_drakkari_commander") {}

    struct npc_drakkari_commanderAI : public QuantumBasicAI
    {
        npc_drakkari_commanderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BattleShoutTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			BattleShoutTimer = 1000;
			WhirlwindTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

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

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = 6000;
			}
			else WhirlwindTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_DC_FRENZY, 0))
				{
					DoCast(me, SPELL_DC_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_commanderAI(creature);
    }
};

class npc_crystal_handler : public CreatureScript
{
public:
    npc_crystal_handler() : CreatureScript("npc_crystal_handler") { }

    struct npc_crystal_handlerAI : public QuantumBasicAI
    {
        npc_crystal_handlerAI(Creature* creature) : QuantumBasicAI(creature)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        }

        uint32 FlashOfDarknessTimer;

        void Reset()
        {
            FlashOfDarknessTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FlashOfDarknessTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_FLASH_OF_DARKNESS_5N, SPELL_FLASH_OF_DARKNESS_5H));
                FlashOfDarknessTimer = 6000;
            }
			else FlashOfDarknessTimer -= diff;

            DoMeleeAttackIfReady();
        }

		Unit* GetRandomTarget()
        {
            return SelectTarget(TARGET_RANDOM, 0, 100, true);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystal_handlerAI (creature);
    }
};

void AddSC_drak_tharon_keep_trash()
{
	new npc_risen_drakkari_warrior();
	new npc_drakkari_guardian();
	new npc_scourge_reanimator();
	new npc_risen_drakkari_soulmage();
	new npc_flesheating_ghoul();
	new npc_ghoul_tormentor();
	new npc_wretched_belcher();
	new npc_darkweb_recluse();
	new npc_risen_drakkari_bat_rider();
	new npc_risen_drakkari_handler();
	new npc_drakkari_gutripper();
	new npc_drakkari_scytheclaw();
	new npc_scourge_brute();
	new npc_drakkari_shaman();
	new npc_risen_drakkari_death_knight();
	new npc_drakkari_commander();
	new npc_crystal_handler();
}