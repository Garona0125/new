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

enum AhnkahetSpells
{
	SPELL_COMBINED_TOXINS      = 56583,
	SPELL_GLUTINOUS_POISON_5N  = 56580,
	SPELL_GLUTINOUS_POISON_5H  = 59108,
	SPELL_FATAL_STING_5N       = 56581,
	SPELL_FATAL_STING_5H       = 59109,
	SPELL_CONTAGION_OF_ROT     = 56707,
	SPELL_AURA_OF_LOST_HOPE_5N = 56709,
	SPELL_AURA_OF_LOST_HOPE_5H = 61459,
	SPELL_WEB_GRAB_5N          = 56640,
	SPELL_WEB_GRAB_5H          = 59106,
	SPELL_TANGLED_WEBS         = 56632,
	SPELL_SHADOW_SICKLE_5N     = 56701,
	SPELL_SHADOW_SICKLE_5H     = 59104,
	SPELL_SHADOW_BLAST_5N      = 56698,
	SPELL_SHADOW_BLAST_5H      = 59102,
	SPELL_CLEAVE               = 42746,
	SPELL_TRIPLE_SLASH         = 56643,
	SPELL_AS_ENRAGE_5N         = 56646,
	SPELL_AS_ENRAGE_5H         = 32714,
	SPELL_SPRINT               = 56354,
	SPELL_ICY_WINDS            = 56716,
	SPELL_FROSTBOLT_5N         = 57825,
	SPELL_FROSTBOLT_5H         = 61461,
	SPELL_FROST_NOVA_5N        = 15063,
	SPELL_FROST_NOVA_5H        = 61462,
	SPELL_PLUNDER_HEALTH_5N    = 56715,
	SPELL_PLUNDER_HEALTH_5H    = 59114,
	SPELL_SHADOW_SHOCK_5N      = 17439,
	SPELL_SHADOW_SHOCK_5H      = 17289,
	SPELL_DARK_COUNTERSPELL_5N = 56730,
	SPELL_DARK_COUNTERSPELL_5H = 59111,
	SPELL_EYES_IN_THE_DARK     = 56728,
	SPELL_SHADOWFURY_5N        = 56733,
	SPELL_SHADOWFURY_5H        = 61463,
	SPELL_ENRAGE               = 8599,
	SPELL_FRIGHTENING_SHOUT    = 19134,
	SPELL_TRAMPLE_5N           = 56736,
	SPELL_TRAMPLE_5H           = 59107,
	SPELL_STOMP                = 56737,
	SPELL_UPPERCUT             = 30471,
	SPELL_SCB_ENRAGE           = 48193,
	SPELL_POISON_CLOUD_5N      = 56867,
	SPELL_POISON_CLOUD_5H      = 59116,
	SPELL_HEALING_WAVE         = 11986,
	SPELL_BLOODLUST            = 28902,
	SPELL_LIGHTNING_SHIELD_5N  = 12550,
	SPELL_LIGHTNING_SHIELD_5H  = 61570,
	SPELL_CURSE_OF_TONGUES     = 13338,
	SPELL_SHADOW_BOLT_5N       = 12739,
	SPELL_SHADOW_BOLT_5H       = 61562,
	SPELL_CORRUPTION_5N        = 56898,
	SPELL_CORRUPTION_5H        = 61563,
	SPELL_FIREBALL_5N          = 17290,
	SPELL_FIREBALL_5H          = 61567,
	SPELL_FLAMESTRIKE_5N       = 56858,
	SPELL_FLAMESTRIKE_5H       = 61568,
	SPELL_WATER_BOLT           = 56862,
	SPELL_WATER_SPOUT_5N       = 39207,
	SPELL_WATER_SPOUT_5H       = 35735,
	SPELL_LIGHTNING_BOLT_5N    = 22414,
	SPELL_LIGHTNING_BOLT_5H    = 59169,
	SPELL_FORKED_LIGHTNING_5N  = 46150,
	SPELL_FORKED_LIGHTNING_5H  = 59152,
	SPELL_FIRE_NOVA            = 12470,
	SPELL_SCORCH               = 17195,
	SPELL_PSYCHIC_SCREAM       = 34322,
	SPELL_SHADOW_CRASH_5N      = 60833,
	SPELL_SHADOW_CRASH_5H      = 60848,
	SPELL_SHADOW_NOVA_5N       = 60845,
	SPELL_SHADOW_NOVA_5H       = 60851,
};

class npc_deep_crawler : public CreatureScript
{
public:
    npc_deep_crawler() : CreatureScript("npc_deep_crawler") { }

    struct npc_deep_crawlerAI : public QuantumBasicAI
    {
        npc_deep_crawlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 GlutinousPoisonTimer;
        uint32 FatalStingTimer;

        void Reset()
        {
            GlutinousPoisonTimer = 500;
            FatalStingTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_COMBINED_TOXINS);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GlutinousPoisonTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_GLUTINOUS_POISON_5N, SPELL_GLUTINOUS_POISON_5H));
				GlutinousPoisonTimer = 4000;
            }
			else GlutinousPoisonTimer -= diff;

            if (FatalStingTimer <= diff && me->GetVictim()->HealthBelowPct(HEALTH_PERCENT_20))
            {
				DoCastVictim(DUNGEON_MODE(SPELL_FATAL_STING_5N, SPELL_FATAL_STING_5H));
				FatalStingTimer = 6000;
            }
			else FatalStingTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deep_crawlerAI(creature);
    }
};

class npc_plague_walker : public CreatureScript
{
public:
    npc_plague_walker() : CreatureScript("npc_plague_walker") { }

    struct npc_plague_walkerAI : public QuantumBasicAI
    {
        npc_plague_walkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ContagionOfRotTimer;

        void Reset()
        {
            ContagionOfRotTimer = 1500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, DUNGEON_MODE(SPELL_AURA_OF_LOST_HOPE_5N, SPELL_AURA_OF_LOST_HOPE_5H));
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ContagionOfRotTimer <= diff)
            {
				DoCastVictim(SPELL_CONTAGION_OF_ROT);
				ContagionOfRotTimer = urand(3000, 4000);
            }
			else ContagionOfRotTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plague_walkerAI(creature);
    }
};

class npc_ahnkahar_web_winder : public CreatureScript
{
public:
    npc_ahnkahar_web_winder() : CreatureScript("npc_ahnkahar_web_winder") { }

    struct npc_ahnkahar_web_winderAI : public QuantumBasicAI
    {
        npc_ahnkahar_web_winderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 WebGrabTimer;
        uint32 TangledWebsTimer;

        void Reset()
        {
            WebGrabTimer = 500;
            TangledWebsTimer = 1500;

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

			if (WebGrabTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WEB_GRAB_5N, SPELL_WEB_GRAB_5H));
					WebGrabTimer = 4000;
				}
            }
			else WebGrabTimer -= diff;

            if (TangledWebsTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TANGLED_WEBS);
					TangledWebsTimer = 6000;
				}
            }
			else TangledWebsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ahnkahar_web_winderAI(creature);
    }
};

class npc_ahnkahar_spell_flinger : public CreatureScript
{
public:
    npc_ahnkahar_spell_flinger() : CreatureScript("npc_ahnkahar_spell_flinger") { }

    struct npc_ahnkahar_spell_flingerAI : public QuantumBasicAI
    {
        npc_ahnkahar_spell_flingerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowSickleTimer;
        uint32 ShadowBlastTimer;

        void Reset()
        {
			ShadowSickleTimer = 500;
            ShadowBlastTimer = 1500;

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

			if (ShadowSickleTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_SICKLE_5N, SPELL_SHADOW_SICKLE_5H));
					ShadowSickleTimer = urand(2000, 3000);
				}
            }
			else ShadowSickleTimer -= diff;

			if (ShadowBlastTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BLAST_5N, SPELL_SHADOW_BLAST_5H));
					ShadowBlastTimer = urand(4000, 5000);
				}
            }
			else ShadowBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ahnkahar_spell_flingerAI(creature);
    }
};

class npc_ahnkahar_slasher : public CreatureScript
{
public:
    npc_ahnkahar_slasher() : CreatureScript("npc_ahnkahar_slasher") { }

    struct npc_ahnkahar_slasherAI : public QuantumBasicAI
    {
        npc_ahnkahar_slasherAI(Creature* creature) : QuantumBasicAI(creature)
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
        uint32 TripleSlashTimer;
		uint32 EnrageTimer;

        void Reset()
        {
            CleaveTimer = 1500;
            TripleSlashTimer = 2500;
			EnrageTimer = 3000;

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

			if (CleaveTimer <= diff)
            {
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 3000;
            }
			else CleaveTimer -= diff;

            if (TripleSlashTimer <= diff)
            {
				DoCastVictim(SPELL_TRIPLE_SLASH);
				TripleSlashTimer = 4500;
            }
			else TripleSlashTimer -= diff;

			if (EnrageTimer <= diff)
            {
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				DoCast(me, DUNGEON_MODE(SPELL_AS_ENRAGE_5N, SPELL_AS_ENRAGE_5H));
				EnrageTimer = 12000;
            }
			else EnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ahnkahar_slasherAI(creature);
    }
};

class npc_ahnkahar_swarmer : public CreatureScript
{
public:
	npc_ahnkahar_swarmer() : CreatureScript("npc_ahnkahar_swarmer") { }

    struct npc_ahnkahar_swarmerAI : public QuantumBasicAI
    {
		npc_ahnkahar_swarmerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 SprintTimer;

        void Reset()
        {
            SprintTimer = 500;

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

			if (SprintTimer <= diff)
            {
				DoCast(me, SPELL_SPRINT, true);
				SprintTimer = 6000;
            }
			else SprintTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ahnkahar_swarmerAI(creature);
    }
};

class npc_frostbringer : public CreatureScript
{
public:
    npc_frostbringer() : CreatureScript("npc_frostbringer") { }

    struct npc_frostbringerAI : public QuantumBasicAI
    {
        npc_frostbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FrostboltTimer;
        uint32 FrostNovaTimer;

        void Reset()
        {
            FrostboltTimer = 500;
			FrostNovaTimer = 2000;

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

			if (!me->HasAura(SPELL_ICY_WINDS))
				DoCast(me, SPELL_ICY_WINDS, true);

			if (FrostboltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(3000, 4000);
				}
            }
			else FrostboltTimer -= diff;

            if (FrostNovaTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_FROST_NOVA_5N, SPELL_FROST_NOVA_5H));
				FrostNovaTimer = urand(5000, 6000);
            }
			else FrostNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostbringerAI(creature);
    }
};

class npc_plundering_geist : public CreatureScript
{
public:
    npc_plundering_geist() : CreatureScript("npc_plundering_geist") { }

    struct npc_plundering_geistAI : public QuantumBasicAI
    {
        npc_plundering_geistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PlunderHealthTimer;

        void Reset()
        {
            PlunderHealthTimer = 1000;

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

            if (PlunderHealthTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_PLUNDER_HEALTH_5N, SPELL_PLUNDER_HEALTH_5H));
				PlunderHealthTimer = urand(4000, 5000);
            }
			else PlunderHealthTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plundering_geistAI(creature);
    }
};

class npc_eye_of_taldaram : public CreatureScript
{
public:
    npc_eye_of_taldaram() : CreatureScript("npc_eye_of_taldaram") { }

    struct npc_eye_of_taldaramAI : public QuantumBasicAI
    {
        npc_eye_of_taldaramAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowShockTimer;
        uint32 DarkCounterspellTimer;
		uint32 ShadowfuryTimer;

        void Reset()
        {
            ShadowShockTimer = 500;
			DarkCounterspellTimer = 1500;
			ShadowfuryTimer = 3000;

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

			if (ShadowShockTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_SHOCK_5N, SPELL_SHADOW_SHOCK_5H));
					ShadowShockTimer = urand(3000, 4000);
				}
            }
			else ShadowShockTimer -= diff;

            if (DarkCounterspellTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_EYES_IN_THE_DARK, true);
					DoCast(target, DUNGEON_MODE(SPELL_DARK_COUNTERSPELL_5N, SPELL_DARK_COUNTERSPELL_5H));
					DarkCounterspellTimer = urand(5000, 6000);
				}
            }
			else DarkCounterspellTimer -= diff;

			if (ShadowfuryTimer <= diff)
            {
				DoCast(me, DUNGEON_MODE(SPELL_SHADOWFURY_5N, SPELL_SHADOWFURY_5H));
				ShadowfuryTimer = urand(7000, 8000);
            }
			else ShadowfuryTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eye_of_taldaramAI(creature);
    }
};

class npc_bonegrinder : public CreatureScript
{
public:
    npc_bonegrinder() : CreatureScript("npc_bonegrinder") { }

    struct npc_bonegrinderAI : public QuantumBasicAI
    {
        npc_bonegrinderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 TrampleTimer;
        uint32 StompTimer;
		uint32 FrighteningShoutTimer;

        void Reset()
        {
            TrampleTimer = 2000;
            StompTimer = 4000;
			FrighteningShoutTimer = 6000;

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

			if (TrampleTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_TRAMPLE_5N, SPELL_TRAMPLE_5H));
				TrampleTimer = 3000;
            }
			else TrampleTimer -= diff;

            if (StompTimer <= diff)
            {
				DoCastAOE(SPELL_STOMP);
				StompTimer = 5000;
            }
			else StompTimer -= diff;

			if (FrighteningShoutTimer <= diff)
            {
				DoCastAOE(SPELL_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = 7000;
            }
			else FrighteningShoutTimer -= diff;

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
        return new npc_bonegrinderAI(creature);
    }
};

class npc_savage_cave_beast : public CreatureScript
{
public:
    npc_savage_cave_beast() : CreatureScript("npc_savage_cave_beast") { }

    struct npc_savage_cave_beastAI : public QuantumBasicAI
    {
        npc_savage_cave_beastAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 UppercutTimer;
        uint32 PoisonCloudTimer;
		uint32 EnrageTimer;

        void Reset()
        {
            UppercutTimer = 1500;
            PoisonCloudTimer = 2000;
			EnrageTimer = 4000;

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

			if (UppercutTimer <= diff)
            {
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = 3000;
            }
			else UppercutTimer -= diff;

            if (PoisonCloudTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_POISON_CLOUD_5N, SPELL_POISON_CLOUD_5H));
				PoisonCloudTimer = 4500;
            }
			else PoisonCloudTimer -= diff;

			if (EnrageTimer <= diff)
            {
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				DoCast(me, SPELL_SCB_ENRAGE);
				EnrageTimer = 12000;
            }
			else EnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_savage_cave_beastAI(creature);
    }
};

class npc_twilight_apostle : public CreatureScript
{
public:
    npc_twilight_apostle() : CreatureScript("npc_twilight_apostle") { }

    struct npc_twilight_apostleAI : public QuantumBasicAI
    {
        npc_twilight_apostleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 LightningShieldTimer;
        uint32 HealingWaveTimer;
		uint32 BloodlustTimer;

        void Reset()
        {
            LightningShieldTimer = 500;
            HealingWaveTimer = 2000;
			BloodlustTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (LightningShieldTimer <= diff && !me->IsInCombatActive())
            {
				DoCast(me, DUNGEON_MODE(SPELL_LIGHTNING_SHIELD_5N, SPELL_LIGHTNING_SHIELD_5H));
				LightningShieldTimer = 120000; // 2 Minutes
            }
			else LightningShieldTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Lightning Shield Buff Check in Combat
			if (!me->HasAura(DUNGEON_MODE(SPELL_LIGHTNING_SHIELD_5N, SPELL_LIGHTNING_SHIELD_5H)))
				DoCast(me, DUNGEON_MODE(SPELL_LIGHTNING_SHIELD_5N, SPELL_LIGHTNING_SHIELD_5H));

            if (HealingWaveTimer <= diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_HEALING_WAVE);
					HealingWaveTimer = urand(3000, 4000);
				}
            }
			else HealingWaveTimer -= diff;

			if (BloodlustTimer <= diff)
            {
				DoCast(me, SPELL_BLOODLUST);
				BloodlustTimer = 12000;
            }
			else BloodlustTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_apostleAI(creature);
    }
};

class npc_twilight_darkcaster : public CreatureScript
{
public:
    npc_twilight_darkcaster() : CreatureScript("npc_twilight_darkcaster") {}

    struct npc_twilight_darkcasterAI : public QuantumBasicAI
    {
        npc_twilight_darkcasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CorruptionTimer;
		uint32 ShadowBoltTimer;
		uint32 CurseOfTonguesTimer;

        void Reset()
		{
			CorruptionTimer = 500;
			ShadowBoltTimer = 1000;
			CurseOfTonguesTimer = 3000;

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

			if (CorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CORRUPTION_5N, SPELL_CORRUPTION_5H));
					CorruptionTimer = urand(3000, 4000);
				}
			}
			else CorruptionTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(5000, 6000);
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
						CurseOfTonguesTimer = urand(7000, 8000);
					}
				}
			}
			else CurseOfTonguesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_darkcasterAI(creature);
    }
};

class npc_twilight_worshipper : public CreatureScript
{
public:
    npc_twilight_worshipper() : CreatureScript("npc_twilight_worshipper") {}

    struct npc_twilight_worshipperAI : public QuantumBasicAI
    {
        npc_twilight_worshipperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireballTimer;
		uint32 FlamestrikeTimer;

        void Reset()
		{
			FireballTimer = 500;
			FlamestrikeTimer = 3000;

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

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = urand(3000, 4000);
				}
			}
			else FireballTimer -= diff;

			if (FlamestrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FLAMESTRIKE_5N, SPELL_FLAMESTRIKE_5H));
					FlamestrikeTimer = urand(5000, 6000);
				}
			}
			else FlamestrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_worshipperAI(creature);
    }
};

class npc_bound_water_elemental : public CreatureScript
{
public:
    npc_bound_water_elemental() : CreatureScript("npc_bound_water_elemental") {}

    struct npc_bound_water_elementalAI : public QuantumBasicAI
    {
        npc_bound_water_elementalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 WaterBoltTimer;
		uint32 WaterSpoutTimer;

        void Reset()
		{
			WaterBoltTimer = 500;
			WaterSpoutTimer = 3000;

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

			if (WaterBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WATER_BOLT);
					WaterBoltTimer = urand(3000, 4000);
				}
			}
			else WaterBoltTimer -= diff;

			if (WaterSpoutTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WATER_SPOUT_5N, SPELL_WATER_SPOUT_5H));
					WaterSpoutTimer = urand(5000, 6000);
				}
			}
			else WaterSpoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bound_water_elementalAI(creature);
    }
};

class npc_bound_air_elemental : public CreatureScript
{
public:
    npc_bound_air_elemental() : CreatureScript("npc_bound_air_elemental") {}

    struct npc_bound_air_elementalAI : public QuantumBasicAI
    {
        npc_bound_air_elementalAI(Creature* creature) : QuantumBasicAI(creature)
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
		uint32 ForkedLightningTimer;

        void Reset()
		{
			LightningBoltTimer = 500;
			ForkedLightningTimer = 3000;

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

			if (ForkedLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCastAOE(DUNGEON_MODE(SPELL_FORKED_LIGHTNING_5N, SPELL_FORKED_LIGHTNING_5H));
					ForkedLightningTimer = urand(5000, 6000);
				}
			}
			else ForkedLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bound_air_elementalAI(creature);
    }
};

class npc_bound_fire_elemental : public CreatureScript
{
public:
    npc_bound_fire_elemental() : CreatureScript("npc_bound_fire_elemental") {}

    struct npc_bound_fire_elementalAI : public QuantumBasicAI
    {
        npc_bound_fire_elementalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ScorchTimer;
		uint32 FireNovaTimer;

        void Reset()
		{
			ScorchTimer = 500;
			FireNovaTimer = 3000;

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

			if (ScorchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SCORCH);
					ScorchTimer = urand(3000, 4000);
				}
			}
			else ScorchTimer -= diff;

			if (FireNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FIRE_NOVA);
				FireNovaTimer = urand(5000, 6000);
			}
			else FireNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bound_fire_elementalAI(creature);
    }
};

class npc_forgotten_one : public CreatureScript
{
public:
    npc_forgotten_one() : CreatureScript("npc_forgotten_one") {}

    struct npc_forgotten_oneAI : public QuantumBasicAI
    {
        npc_forgotten_oneAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowCrashTimer;
		uint32 ShadowNovaTimer;
		uint32 PsychicScreamTimer;

        void Reset()
		{
			ShadowCrashTimer = 500;
			ShadowNovaTimer = 3000;
			PsychicScreamTimer = 5000;

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

			if (ShadowCrashTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_CRASH_5N, SPELL_SHADOW_CRASH_5H));
					ShadowCrashTimer = urand(3000, 4000);
				}
			}
			else ShadowCrashTimer -= diff;

			if (ShadowNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_SHADOW_NOVA_5N, SPELL_SHADOW_NOVA_5H));
				ShadowNovaTimer = urand(5000, 6000);
			}
			else ShadowNovaTimer -= diff;

			if (PsychicScreamTimer <= diff)
			{
				DoCastAOE(SPELL_PSYCHIC_SCREAM);
				PsychicScreamTimer = urand(7000, 8000);
			}
			else PsychicScreamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_oneAI(creature);
    }
};

void AddSC_ahnkahet_the_old_kingdom_trash()
{
	new npc_deep_crawler();
	new npc_plague_walker();
	new npc_ahnkahar_web_winder();
	new npc_ahnkahar_spell_flinger();
	new npc_ahnkahar_slasher();
	new npc_ahnkahar_swarmer();
	new npc_frostbringer();
	new npc_plundering_geist();
	new npc_eye_of_taldaram();
	new npc_bonegrinder();
	new npc_savage_cave_beast();
	new npc_twilight_apostle();
	new npc_twilight_darkcaster();
	new npc_twilight_worshipper();
	new npc_bound_water_elemental();
	new npc_bound_air_elemental();
	new npc_bound_fire_elemental();
	new npc_forgotten_one();
}