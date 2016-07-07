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

enum AzjolNerubSpells
{
	SPELL_ACID_SPLASH_5N        = 52446,
	SPELL_ACID_SPLASH_5H        = 59363,
	SPELL_STRIKE                = 52532,
    SPELL_CLEAVE                = 49806,
	SPELL_CHARGE                = 16979,
	SPELL_BACKSTAB              = 52540,
	SPELL_SHADOW_BOLT_5N        = 52534,
	SPELL_SHADOW_BOLT_5H        = 59357,
    SPELL_SHADOW_NOVA_5N        = 52535,
    SPELL_SHADOW_NOVA_5H        = 59358,
	SPELL_WEB_WRAP              = 52086,
	SPELL_WEB_SHOT_5N           = 54290,
	SPELL_WEB_SHOT_5H           = 59362,
	SPELL_PUMMEL_5N             = 53394,
	SPELL_PUMMEL_5H             = 59344,
	SPELL_REND_5N               = 53317,
	SPELL_REND_5H               = 59343,
	SPELL_INFECTED_WOUND_5N     = 53330,
	SPELL_INFECTED_WOUND_5H     = 59348,
	SPELL_CRUSHING_WEBS_5N      = 53322,
	SPELL_CRUSHING_WEBS_5H      = 59347,
	SPELL_SMASH_5N              = 53318,
	SPELL_SMASH_5H              = 59346,
	SPELL_FRENZY                = 53801,
	SPELL_MARK_OF_DARKNESS_5N   = 54309,
	SPELL_MARK_OF_DARKNESS_5H   = 59352,
	SPELL_DRAIN_POWER_5N        = 54314,
	SPELL_DRAIN_POWER_5H        = 59354,
	SPELL_DART_5N               = 53602,
	SPELL_DART_5H               = 59349,
	SPELL_SUNDER_ARMOR_5N       = 53618,
	SPELL_SUNDER_ARMOR_5H       = 59350,
	SPELL_POISON_BOLT_5N        = 53617,
	SPELL_POISON_BOLT_5H        = 59359,
	SPELL_POISON_BOLT_VOLLEY_5N = 53616,
	SPELL_POISON_BOLT_VOLLEY_5H = 59360,
};

class npc_skittering_infector : public CreatureScript
{
public:
    npc_skittering_infector() : CreatureScript("npc_skittering_infector") { }

    struct npc_skittering_infectorAI : public QuantumBasicAI
    {
        npc_skittering_infectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            DoCastVictim(DUNGEON_MODE(SPELL_ACID_SPLASH_5N, SPELL_ACID_SPLASH_5H), true);
        }

		void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skittering_infectorAI(creature);
    }
};

class npc_anubar_warrior : public CreatureScript
{
public:
    npc_anubar_warrior() : CreatureScript("npc_anubar_warrior") { }

    struct npc_anubar_warriorAI : public QuantumBasicAI
    {
        npc_anubar_warriorAI(Creature* creature) : QuantumBasicAI(creature)
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
        uint32 StrikeTimer;

        void Reset()
        {
            CleaveTimer = 2000;
            StrikeTimer = 4000;

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

            if (StrikeTimer <= diff)
            {
                DoCastVictim(SPELL_STRIKE);
                StrikeTimer = urand(4000, 5000);
            }
			else StrikeTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(6000, 7000);
            }
			else CleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_warriorAI(creature);
    }
};

class npc_anubar_skirmisher : public CreatureScript
{
public:
    npc_anubar_skirmisher() : CreatureScript("npc_anubar_skirmisher") { }

    struct npc_anubar_skirmisherAI : public QuantumBasicAI
    {
        npc_anubar_skirmisherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ChargeTimer;
        uint32 BackstabTimer;

        void Reset()
        {
            ChargeTimer = 500;
            BackstabTimer = 1500;

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

            if (ChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoResetThreat();
                    me->AddThreat(target, 1.0f);
                    DoCast(target, SPELL_CHARGE, true);
					ChargeTimer = urand(4000, 5000);
				}
            }
			else ChargeTimer -= diff;

            if (BackstabTimer <= diff)
            {
                DoCastVictim(SPELL_BACKSTAB);
                BackstabTimer = urand(6000, 7000);
            }
			else BackstabTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_skirmisherAI(creature);
    }
};

class npc_anubar_shadowcaster : public CreatureScript
{
public:
    npc_anubar_shadowcaster() : CreatureScript("npc_anubar_shadowcaster") { }

    struct npc_anubar_shadowcasterAI : public QuantumBasicAI
    {
        npc_anubar_shadowcasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowBoltTimer;
        uint32 ShadowNovaTimer;

        void Reset()
        {
            ShadowBoltTimer = 500;
            ShadowNovaTimer = 2000;

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

            if (ShadowBoltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                     DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					 ShadowBoltTimer = urand(3000, 4000);
				}
            }
			else ShadowBoltTimer -= diff;

            if (ShadowNovaTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_NOVA_5N, SPELL_SHADOW_NOVA_5H));
					ShadowNovaTimer = urand(5000, 6000);
				}
            }
			else ShadowNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_shadowcasterAI(creature);
    }
};

class npc_anubar_webspinner : public CreatureScript
{
public:
    npc_anubar_webspinner() : CreatureScript("npc_anubar_webspinner") { }

    struct npc_anubar_webspinnerAI : public QuantumBasicAI
    {
        npc_anubar_webspinnerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 WebWrapTimer;
        uint32 WebShotTimer;

        void Reset()
        {
            WebWrapTimer = 500;
            WebShotTimer = 3500;

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

            if (WebWrapTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                     DoCast(target, SPELL_WEB_WRAP, true);
					 WebWrapTimer = 4000;
				}
            }
			else WebWrapTimer -= diff;

            if (WebShotTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WEB_SHOT_5N, SPELL_WEB_SHOT_5H));
					WebShotTimer = 7000;
				}
            }
			else WebShotTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_webspinnerAI(creature);
    }
};

class npc_anubar_champion : public CreatureScript
{
public:
    npc_anubar_champion() : CreatureScript("npc_anubar_champion") { }

    struct npc_anubar_championAI : public QuantumBasicAI
    {
        npc_anubar_championAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 RendTimer;
        uint32 PummelTimer;

        void Reset()
        {
            RendTimer = 1500;
            PummelTimer = 2500;

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
				DoCastVictim(DUNGEON_MODE(SPELL_REND_5N, SPELL_REND_5H));
				RendTimer = 4000;
            }
			else RendTimer -= diff;

            if (PummelTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_PUMMEL_5N, SPELL_PUMMEL_5H));
				PummelTimer = 6000;
            }
			else PummelTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_championAI(creature);
    }
};

class npc_anubar_crypt_fiend : public CreatureScript
{
public:
    npc_anubar_crypt_fiend() : CreatureScript("npc_anubar_crypt_fiend") { }

    struct npc_anubar_crypt_fiendAI : public QuantumBasicAI
    {
        npc_anubar_crypt_fiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CrushingWebsTimer;
        uint32 InfectedWoundTimer;

        void Reset()
        {
            CrushingWebsTimer = 500;
            InfectedWoundTimer = 2000;

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

            if (CrushingWebsTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CRUSHING_WEBS_5N, SPELL_CRUSHING_WEBS_5H));
					CrushingWebsTimer = urand(3000, 4000);
				}
            }
			else CrushingWebsTimer -= diff;

            if (InfectedWoundTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_INFECTED_WOUND_5N, SPELL_INFECTED_WOUND_5H));
				InfectedWoundTimer = urand(5000, 6000);
            }
			else InfectedWoundTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_crypt_fiendAI(creature);
    }
};

class npc_anubar_crusher : public CreatureScript
{
public:
    npc_anubar_crusher() : CreatureScript("npc_anubar_crusher") { }

    struct npc_anubar_crusherAI : public QuantumBasicAI
    {
        npc_anubar_crusherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 SmashTimer;

        void Reset()
        {
            SmashTimer = 1500;

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

            if (SmashTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_SMASH_5N, SPELL_SMASH_5H));
				SmashTimer = urand(5000, 6000);
            }
			else SmashTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
					me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
				}
			}

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_crusherAI(creature);
    }
};

class npc_anubar_prime_guard : public CreatureScript
{
public:
    npc_anubar_prime_guard() : CreatureScript("npc_anubar_prime_guard") { }

    struct npc_anubar_prime_guardAI : public QuantumBasicAI
    {
        npc_anubar_prime_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 MarkOfDarknessTimer;
        uint32 DrainPowerTimer;

        void Reset()
        {
            MarkOfDarknessTimer = 500;
            DrainPowerTimer = 1500;

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

            if (MarkOfDarknessTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                     DoCast(target, DUNGEON_MODE(SPELL_MARK_OF_DARKNESS_5N, SPELL_MARK_OF_DARKNESS_5H));
					 MarkOfDarknessTimer = 3000;
				}
            }
			else MarkOfDarknessTimer -= diff;

            if (DrainPowerTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_DRAIN_POWER_5N, SPELL_DRAIN_POWER_5H));
					DrainPowerTimer = 4500;
				}
            }
			else DrainPowerTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_prime_guardAI(creature);
    }
};

class npc_anubar_darter : public CreatureScript
{
public:
    npc_anubar_darter() : CreatureScript("npc_anubar_darter") { }

    struct npc_anubar_darterAI : public QuantumBasicAI
    {
        npc_anubar_darterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DartTimer;

        void Reset()
        {
            DartTimer = 2000;

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

            if (DartTimer <= diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_DART_5N, SPELL_DART_5H));
                DartTimer = urand(3000, 4000);
            }
			else DartTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_darterAI(creature);
    }
};

class npc_anubar_assassin : public CreatureScript
{
public:
    npc_anubar_assassin() : CreatureScript("npc_anubar_assassin") { }

    struct npc_anubar_assassinAI : public QuantumBasicAI
    {
        npc_anubar_assassinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BackstabTimer;

        void Reset()
        {
            BackstabTimer = 2000;

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

            if (BackstabTimer <= diff)
            {
                DoCastVictim(SPELL_BACKSTAB);
                BackstabTimer = 7000;
            }
			else BackstabTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_assassinAI(creature);
    }
};

class npc_anubar_guardian : public CreatureScript
{
public:
    npc_anubar_guardian() : CreatureScript("npc_anubar_guardian") { }

    struct npc_anubar_guardianAI : public QuantumBasicAI
    {
        npc_anubar_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 StrikeTimer;
		uint32 SunderArmorTimer;

        void Reset()
        {
            StrikeTimer = 2000;
            SunderArmorTimer = 4000;

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

            if (StrikeTimer <= diff)
            {
                DoCastVictim(SPELL_STRIKE);
                StrikeTimer = urand(3000, 4000);
            }
			else StrikeTimer -= diff;

            if (SunderArmorTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_SUNDER_ARMOR_5N, SPELL_SUNDER_ARMOR_5H));
                SunderArmorTimer = urand(5000, 6000);
            }
			else SunderArmorTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_guardianAI(creature);
    }
};

class npc_anubar_venomancer : public CreatureScript
{
public:
    npc_anubar_venomancer() : CreatureScript("npc_anubar_venomancer") { }

    struct npc_anubar_venomancerAI : public QuantumBasicAI
    {
        npc_anubar_venomancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 PoisonBoltTimer;
        uint32 PoisonBoltVolleyTimer;

        void Reset()
        {
            PoisonBoltTimer = 500;
            PoisonBoltVolleyTimer = 1500;

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

            if (PoisonBoltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                     DoCast(target, DUNGEON_MODE(SPELL_POISON_BOLT_5N, SPELL_POISON_BOLT_5H));
					 PoisonBoltTimer = urand(3000, 4000);
				}
            }
			else PoisonBoltTimer -= diff;

            if (PoisonBoltVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POISON_BOLT_VOLLEY_5N, SPELL_POISON_BOLT_VOLLEY_5H));
					PoisonBoltVolleyTimer = urand(5000, 6000);
				}
            }
			else PoisonBoltVolleyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_venomancerAI(creature);
    }
};

void AddSC_azjol_nerub_trash()
{
	new npc_skittering_infector();
	new npc_anubar_warrior();
	new npc_anubar_skirmisher();
    new npc_anubar_shadowcaster();
	new npc_anubar_webspinner();
	new npc_anubar_champion();
	new npc_anubar_crypt_fiend();
	new npc_anubar_crusher();
	new npc_anubar_prime_guard();
	new npc_anubar_darter();
	new npc_anubar_assassin();
	new npc_anubar_guardian();
	new npc_anubar_venomancer();
}