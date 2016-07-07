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

enum HallsOfReflectionSpells
{
	SPELL_CIRCLE_OF_DESTRUCTION_5N  = 72320,
	SPELL_CIRCLE_OF_DESTRUCTION_5H  = 72323,
    SPELL_SHADOW_WORD_PAIN_5N       = 72318,
	SPELL_SHADOW_WORD_PAIN_5H       = 72319,
    SPELL_DARK_MENDING_5N           = 72322,
	SPELL_DARK_MENDING_5H           = 72324,
	SPELL_COWER_IN_FEAR             = 72321,
	SPELL_FIREBALL_5N               = 72163,
	SPELL_FIREBALL_5H               = 72164,
	SPELL_FROSTBOLT_5N              = 72166,
	SPELL_FROSTBOLT_5H              = 72167,
    SPELL_FLAMESTRIKE_5N            = 72169,
	SPELL_FLAMESTRIKE_5H            = 72170,
    SPELL_CHAINS_OF_ICE             = 72121,
    SPELL_HALLUCINATION             = 72342,
	SPELL_HALLUCINATION_2           = 72344,
	SPELL_SHADOW_STEP_5N            = 72326,
	SPELL_SHADOW_STEP_5H            = 72327,
    SPELL_DEADLY_POISON_5N          = 72329,
	SPELL_DEADLY_POISON_5H          = 72330,
    SPELL_ENVENOMED_DAGGER_THROW_5N = 72333,
	SPELL_ENVENOMED_DAGGER_THROW_5H = 72334,
    SPELL_KIDNEY_SHOT               = 72335,
    SPELL_SPECTRAL_STRIKE_5N        = 72198,
	SPELL_SPECTRAL_STRIKE_5H        = 72688,
    SPELL_SHIELD_BASH_5N            = 72194,
	SPELL_SHIELD_BASH_5H            = 72196,
    SPELL_TORTURED_ENRAGE           = 72203,
    SPELL_SHOOT_5N                  = 72208,
	SPELL_SHOOT_5H                  = 72211,
	SPELL_ICE_SHOT_5N               = 72268,
	SPELL_ICE_SHOT_5H               = 72269,
    SPELL_CURSED_ARROW              = 72222,
    SPELL_FROST_TRAP                = 72215,
	SPELL_BALEFUL_STRIKE_5N         = 69933,
	SPELL_BALEFUL_STRIKE_5H         = 70400,
	SPELL_SPIRIT_BURST_5N           = 69900,
	SPELL_SPIRIT_BURST_5H           = 73046,
};

enum GhostlyPriest
{
	EVENT_SHADOW_WORD_PAIN         = 1,
    EVENT_CIRCLE_OF_DESTRUCTION    = 2,
    EVENT_COWER_IN_FEAR            = 3,
    EVENT_DARK_MENDING             = 4,
};

class npc_ghostly_priest : public CreatureScript
{
public:
    npc_ghostly_priest() : CreatureScript("npc_ghostly_priest") { }

    struct npc_ghostly_priestAI: public QuantumBasicAI
    {
		npc_ghostly_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, 8000);
            events.ScheduleEvent(EVENT_CIRCLE_OF_DESTRUCTION, 12000);
            events.ScheduleEvent(EVENT_COWER_IN_FEAR, 10000);
            events.ScheduleEvent(EVENT_DARK_MENDING, 20000);
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
                    case EVENT_SHADOW_WORD_PAIN:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H));
						}
                        events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, 8000);
                        return;
                    case EVENT_CIRCLE_OF_DESTRUCTION:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_CIRCLE_OF_DESTRUCTION_5N, SPELL_CIRCLE_OF_DESTRUCTION_5H));
						}
                        events.ScheduleEvent(EVENT_CIRCLE_OF_DESTRUCTION, 12000);
                        return;
                    case EVENT_COWER_IN_FEAR:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_COWER_IN_FEAR);
						}
                        events.ScheduleEvent(EVENT_COWER_IN_FEAR, 10000);
                        return;
                    case EVENT_DARK_MENDING:
					{
                        if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
                        {
                            DoCast(target, DUNGEON_MODE(SPELL_DARK_MENDING_5N, SPELL_DARK_MENDING_5H));
                            events.ScheduleEvent(EVENT_DARK_MENDING, 20000);
                        }
                        else
                        {
                            events.ScheduleEvent(EVENT_DARK_MENDING, 5000);
                        }
                        return;
					}
                }
            }
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_ghostly_priestAI(creature);
	}
};

enum PhantomMage
{
	EVENT_FIREBALL      = 1,
    EVENT_FLAMESTRIKE   = 2,
    EVENT_FROSTBOLT     = 3,
    EVENT_CHAINS_OF_ICE = 4,
    EVENT_HALLUCINATION = 5,
};

class npc_phantom_mage : public CreatureScript
{
public:
    npc_phantom_mage() : CreatureScript("npc_phantom_mage") { }

    struct npc_phantom_mageAI: public QuantumBasicAI
    {
        npc_phantom_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_FIREBALL, 3000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_FLAMESTRIKE, 6000);
            events.ScheduleEvent(EVENT_FROSTBOLT, 9000);
            events.ScheduleEvent(EVENT_CHAINS_OF_ICE, 12000);
            events.ScheduleEvent(EVENT_HALLUCINATION, 40000);
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
                    case EVENT_FIREBALL:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
						}
                        events.ScheduleEvent(EVENT_FIREBALL, 15000);
                        return;
                    case EVENT_FLAMESTRIKE:
						DoCast(DUNGEON_MODE(SPELL_FLAMESTRIKE_5N, SPELL_FLAMESTRIKE_5H));
                        events.ScheduleEvent(EVENT_FLAMESTRIKE, 15000);
                        return;
                    case EVENT_FROSTBOLT:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
						}
                        events.ScheduleEvent(EVENT_FROSTBOLT, 15000);
                        return;
                    case EVENT_CHAINS_OF_ICE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_CHAINS_OF_ICE);
						}
                        events.ScheduleEvent(EVENT_CHAINS_OF_ICE, 15000);
                        return;
                    case EVENT_HALLUCINATION:
                        DoCast(SPELL_HALLUCINATION);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_mageAI(creature);
    }
};

class npc_phantom_hallucination : public CreatureScript
{
public:
    npc_phantom_hallucination() : CreatureScript("npc_phantom_hallucination") { }

    struct npc_phantom_hallucinationAI : public npc_phantom_mage::npc_phantom_mageAI
    {
        npc_phantom_hallucinationAI(Creature* creature) : npc_phantom_mage::npc_phantom_mageAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void JustDied(Unit* /*who*/)
        {
			DoCastAOE(SPELL_HALLUCINATION_2, true);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_hallucinationAI(creature);
    }
};

enum ShadowyMercenary
{
	EVENT_SHADOW_STEP            = 1,
    EVENT_DEADLY_POISON          = 2,
    EVENT_ENVENOMED_DAGGER_THROW = 3,
    EVENT_KIDNEY_SHOT            = 4,
};

class npc_shadowy_mercenary : public CreatureScript
{
public:
    npc_shadowy_mercenary() : CreatureScript("npc_shadowy_mercenary") { }

    struct npc_shadowy_mercenaryAI: public QuantumBasicAI
    {
        npc_shadowy_mercenaryAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHADOW_STEP, 8000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_DEADLY_POISON, 5000);
            events.ScheduleEvent(EVENT_ENVENOMED_DAGGER_THROW, 10000);
            events.ScheduleEvent(EVENT_KIDNEY_SHOT, 12000);
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
                    case EVENT_SHADOW_STEP:
						DoCast(DUNGEON_MODE(SPELL_SHADOW_STEP_5N, SPELL_SHADOW_STEP_5H));
                        events.ScheduleEvent(EVENT_SHADOW_STEP, 8000);
                        return;
                    case EVENT_DEADLY_POISON:
						DoCastVictim(DUNGEON_MODE(SPELL_DEADLY_POISON_5N, SPELL_DEADLY_POISON_5H));
                        events.ScheduleEvent(EVENT_DEADLY_POISON, 10000);
                        return;
                    case EVENT_ENVENOMED_DAGGER_THROW:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, DUNGEON_MODE(SPELL_ENVENOMED_DAGGER_THROW_5N, SPELL_ENVENOMED_DAGGER_THROW_5H));
						}
                        events.ScheduleEvent(EVENT_ENVENOMED_DAGGER_THROW, 10000);
                        return;
                    case EVENT_KIDNEY_SHOT:
                        DoCastVictim(SPELL_KIDNEY_SHOT);
                        events.ScheduleEvent(EVENT_KIDNEY_SHOT, 10000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowy_mercenaryAI(creature);
    }
};

enum SpectralFootman
{
	EVENT_SPECTRAL_STRIKE   = 1,
    EVENT_SHIELD_BASH       = 2,
    EVENT_TORTURED_ENRAGE   = 3,
};

class npc_spectral_footman : public CreatureScript
{
public:
    npc_spectral_footman() : CreatureScript("npc_spectral_footman") { }

    struct npc_spectral_footmanAI: public QuantumBasicAI
    {
        npc_spectral_footmanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SPECTRAL_STRIKE, 5000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_SHIELD_BASH, 10000);
            events.ScheduleEvent(EVENT_TORTURED_ENRAGE, 15000);
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
                    case EVENT_SPECTRAL_STRIKE:
						DoCastVictim(DUNGEON_MODE(SPELL_SPECTRAL_STRIKE_5N, SPELL_SPECTRAL_STRIKE_5H));
                        events.ScheduleEvent(EVENT_SPECTRAL_STRIKE, 5000);
                        return;
                    case EVENT_SHIELD_BASH:
						DoCastVictim(DUNGEON_MODE(SPELL_SHIELD_BASH_5N, SPELL_SHIELD_BASH_5H));
                        events.ScheduleEvent(EVENT_SHIELD_BASH, 5000);
                        return;
                    case EVENT_TORTURED_ENRAGE:
                        DoCast(SPELL_TORTURED_ENRAGE);
                        events.ScheduleEvent(EVENT_TORTURED_ENRAGE, 15000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_footmanAI(creature);
    }
};

enum TorturedRifleman
{
	EVENT_SHOOT        = 1,
    EVENT_CURSED_ARROW = 2,
    EVENT_FROST_TRAP   = 3,
    EVENT_ICE_SHOT     = 4,
};

class npc_tortured_rifleman : public CreatureScript
{
public:
    npc_tortured_rifleman() : CreatureScript("npc_tortured_rifleman") { }

    struct npc_tortured_riflemanAI  : public QuantumBasicAI
    {
        npc_tortured_riflemanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHOOT, 2000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_CURSED_ARROW, 10000);
            events.ScheduleEvent(EVENT_FROST_TRAP, 1000);
            events.ScheduleEvent(EVENT_ICE_SHOT, 15000);
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
                    case EVENT_SHOOT:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						}
                        events.ScheduleEvent(EVENT_SHOOT, 2000);
                        return;
                    case EVENT_CURSED_ARROW:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_CURSED_ARROW);
						}
                        events.ScheduleEvent(EVENT_CURSED_ARROW, 10000);
                        return;
                    case EVENT_FROST_TRAP:
                        DoCast(SPELL_FROST_TRAP);
                        events.ScheduleEvent(EVENT_FROST_TRAP, 30000);
                        return;
                    case EVENT_ICE_SHOT:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, DUNGEON_MODE(SPELL_ICE_SHOT_5N, SPELL_ICE_SHOT_5H));
						}
                        events.ScheduleEvent(EVENT_ICE_SHOT, 15000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tortured_riflemanAI(creature);
    }
};

class npc_spiritual_reflection : public CreatureScript
{
public:
    npc_spiritual_reflection() : CreatureScript("npc_spiritual_reflection") { }

    struct npc_spiritual_reflectionAI : public QuantumBasicAI
    {
        npc_spiritual_reflectionAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        }

        uint32 BalefulStrikeTimer;

        void Reset()
        {
            BalefulStrikeTimer = urand(1000,3000);    
        }

        void JustDied(Unit* killer)
        {
			DoCast(killer, (DUNGEON_MODE(SPELL_SPIRIT_BURST_5N, SPELL_SPIRIT_BURST_5H)));
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (BalefulStrikeTimer < diff)
            {
                if (Unit *target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BALEFUL_STRIKE_5N, SPELL_BALEFUL_STRIKE_5H));
				}
                BalefulStrikeTimer = urand(3000, 8000);
            }
            else BalefulStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spiritual_reflectionAI(creature);
    }
};

void AddSC_halls_of_reflection_trash()
{
	new npc_ghostly_priest();
    new npc_phantom_mage();
    new npc_phantom_hallucination();
    new npc_shadowy_mercenary();
    new npc_spectral_footman();
    new npc_tortured_rifleman();
	new npc_spiritual_reflection();
}