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
#include "../../../scripts/Northrend/FrozenHalls/ForgeOfSouls/forge_of_souls.h"

enum ForgeOfSoulsSpells
{
	SPELL_SOULGUARD_CHANNEL            = 68797,
	SPELL_SOULGUARD_CHANNEL_BEAM       = 68834,
	SPELL_SPITEFUL_APPARATION_VISUAL_1 = 69136,
	SPELL_SPITEFUL_APPARATION_VISUAL_2 = 69658,
	SPELL_SPITE_5N                     = 68895,
    SPELL_SPITE_5H                     = 70212,
	SPELL_SHADOWFORM                   = 69144,
	SPELL_VEIL_OF_SHADOWS              = 69633,
    SPELL_WAIL_OF_SOULS_5N             = 69148,
    SPELL_WAIL_OF_SOULS_5H             = 70210,
	SPELL_SHROUD_OF_RUNES              = 69056,
    SPELL_UNHOLY_RAGE                  = 69053,
	SPELL_FROST_NOVA_5N                = 69060,
    SPELL_FROST_NOVA_5H                = 70209,
    SPELL_SHADOW_LANCE                 = 69058,
	SPELL_BONE_VOLLEY_5N               = 69080,
    SPELL_BONE_VOLLEY_5H               = 70206,
    SPELL_RAISE_DEAD                   = 69562,
    SPELL_SHIELD_OF_BONES_5N           = 69069,
    SPELL_SHIELD_OF_BONES_5H           = 70207,
	SPELL_SA_RAISE_DEAD                = 69562,
	SPELL_SHADOW_BOLT_5N               = 69068,
    SPELL_SHADOW_BOLT_5H               = 70208,
    SPELL_SOUL_SICKNESS                = 69131,
    SPELL_SOUL_SIPHON                  = 69128,
	SPELL_SD_RAISE_DEAD                = 69562,
	SPELL_SA_SHADOW_BOLT_5N            = 69068,
    SPELL_SA_SHADOW_BOLT_5H            = 70208,
	SPELL_DRAIN_LIFE_5N                = 69066,
    SPELL_DRAIN_LIFE_5H                = 70213,
    SPELL_SHADOW_MEND_5N               = 69564,
    SPELL_SHADOW_MEND_5H               = 70205,
	SPELL_SOUL_HORROR_VISUAL_1         = 69105,
	SPELL_SOUL_HORROR_VISUAL_2         = 69663,
	SPELL_SOUL_STRIKE_5N               = 69088,
    SPELL_SOUL_STRIKE_5H               = 70211,
	SPELL_CLAW_SLASH                   = 54185,
};

enum SpitefulApparition
{
	EVENT_SPITE = 1,
};

class npc_spiteful_apparition : public CreatureScript
{
public:
    npc_spiteful_apparition() : CreatureScript("npc_spiteful_apparition") { }

    struct npc_spiteful_apparitionAI: public QuantumBasicAI
    {
        npc_spiteful_apparitionAI(Creature* creature) : QuantumBasicAI(creature){}

        EventMap events;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPITEFUL_APPARATION_VISUAL_1, true);
			DoCast(me, SPELL_SPITEFUL_APPARATION_VISUAL_2, true);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
        }

		void JustReachedHome()
		{
			Reset();
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (me->IsWithinDist(who, 15.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				me->AI()->AttackStart(who);
			}
		}

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SPITE, 1000);
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
                switch (eventId)
                {
                    case EVENT_SPITE:
                        DoCastAOE(DUNGEON_MODE(SPELL_SPITE_5N, SPELL_SPITE_5H), true);
                        events.RescheduleEvent(EVENT_SPITE, 4000);
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spiteful_apparitionAI(creature);
    }
};

enum SpectralWarden
{
	EVENT_VEIL_OF_SHADOWS = 1,
    EVENT_WAIL_OF_SOULS   = 2,
};

class npc_spectral_warden : public CreatureScript
{
public:
    npc_spectral_warden() : CreatureScript("npc_spectral_warden") { }

    struct npc_spectral_wardenAI: public QuantumBasicAI
    {
        npc_spectral_wardenAI(Creature* creature) : QuantumBasicAI(creature)
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
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SHADOWFORM, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            events.Reset();
        }

		void JustReachedHome()
		{
			Reset();
		}

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_VEIL_OF_SHADOWS, 1000);
            events.ScheduleEvent(EVENT_WAIL_OF_SOULS, 3000);
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
                switch (eventId)
                {
                    case EVENT_VEIL_OF_SHADOWS:
                        DoCastVictim(SPELL_VEIL_OF_SHADOWS);
                        events.RescheduleEvent(EVENT_VEIL_OF_SHADOWS, 10000);
                        return;
                    case EVENT_WAIL_OF_SOULS:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, DUNGEON_MODE(SPELL_WAIL_OF_SOULS_5N, SPELL_WAIL_OF_SOULS_5H));
						}
                        events.RescheduleEvent(EVENT_WAIL_OF_SOULS, 5000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_wardenAI(creature);
    }
};

enum SoulguardWatchman
{
	EVENT_SHROUD_OF_RUNES = 1,
    EVENT_UNHOLY_RAGE     = 2,
};

class npc_soulguard_watchman : public CreatureScript
{
public:
    npc_soulguard_watchman() : CreatureScript("npc_soulguard_watchman") { }

    struct npc_soulguard_watchmanAI: public QuantumBasicAI
    {
        npc_soulguard_watchmanAI(Creature* creature) : QuantumBasicAI(creature)
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

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHROUD_OF_RUNES, 1000);
            events.ScheduleEvent(EVENT_UNHOLY_RAGE, 2000);
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
                switch (eventId)
                {
                    case EVENT_SHROUD_OF_RUNES:
                        DoCast(me, SPELL_SHROUD_OF_RUNES);
                        events.RescheduleEvent(EVENT_SHROUD_OF_RUNES, 5000);
                        return;
                    case EVENT_UNHOLY_RAGE:
						DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
                        DoCast(me, SPELL_UNHOLY_RAGE);
                        events.RescheduleEvent(EVENT_UNHOLY_RAGE, 10000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soulguard_watchmanAI(creature);
    }
};

enum SoulGuardReaper
{
	EVENT_FROST_NOVA   = 1,
    EVENT_SHADOW_LANCE = 2,
};

class npc_soulguard_reaper : public CreatureScript
{
public:
    npc_soulguard_reaper() : CreatureScript("npc_soulguard_reaper") { }

    struct npc_soulguard_reaperAI: public QuantumBasicAI
    {
        npc_soulguard_reaperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SoulguardChannelBeamTimer;

        EventMap events;

        void Reset()
        {
			events.Reset();

			SoulguardChannelBeamTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SOULGUARD_CHANNEL, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
        {
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

            events.ScheduleEvent(EVENT_FROST_NOVA, 8000);
            events.ScheduleEvent(EVENT_SHADOW_LANCE, 5000);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SoulguardChannelBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* horror = me->FindCreatureWithDistance(NPC_SOUL_HORROR, 10.0f))
				{
					me->SetFacingToObject(horror);
					DoCast(horror, SPELL_SOULGUARD_CHANNEL_BEAM, true);
					SoulguardChannelBeamTimer = 120000; // 2 Minutes
				}
			}
			else SoulguardChannelBeamTimer -= diff;

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_FROST_NOVA:
                        DoCastAOE(DUNGEON_MODE(SPELL_FROST_NOVA_5N, SPELL_FROST_NOVA_5H));
                        events.RescheduleEvent(EVENT_FROST_NOVA, 9600);
                        return;
                    case EVENT_SHADOW_LANCE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_SHADOW_LANCE);
						}
                        events.RescheduleEvent(EVENT_SHADOW_LANCE, 8000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soulguard_reaperAI(creature);
    }
};

enum SoulguardBonecaster
{
	EVENT_BONE_VOLLEY       = 1,
    EVENT_RAISE_DEAD        = 2,
    EVENT_SHIELD_OF_BONES   = 3,
};

class npc_soulguard_bonecaster : public CreatureScript
{
public:
    npc_soulguard_bonecaster() : CreatureScript("npc_soulguard_bonecaster") { }

    struct npc_soulguard_bonecasterAI: public QuantumBasicAI
    {
        npc_soulguard_bonecasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SoulguardChannelBeamTimer;

        EventMap events;

        void Reset()
        {
			events.Reset();

			SoulguardChannelBeamTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SOULGUARD_CHANNEL, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
        {
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

            events.ScheduleEvent(EVENT_BONE_VOLLEY, 6000);
            events.ScheduleEvent(EVENT_RAISE_DEAD, 25000);
            events.ScheduleEvent(EVENT_SHIELD_OF_BONES, 6000);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SoulguardChannelBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* horror = me->FindCreatureWithDistance(NPC_SOUL_HORROR, 10.0f))
				{
					me->SetFacingToObject(horror);
					DoCast(horror, SPELL_SOULGUARD_CHANNEL_BEAM, true);
					SoulguardChannelBeamTimer = 120000; // 2 Minutes
				}
			}
			else SoulguardChannelBeamTimer -= diff;

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_BONE_VOLLEY:
						DoCastAOE(DUNGEON_MODE(SPELL_BONE_VOLLEY_5N, SPELL_BONE_VOLLEY_5H));
                        events.RescheduleEvent(EVENT_BONE_VOLLEY, 7000);
                        return;
                    case EVENT_RAISE_DEAD:
                        DoCast(me, SPELL_RAISE_DEAD);
                        events.RescheduleEvent(EVENT_RAISE_DEAD, 25000);
                        return;
                    case EVENT_SHIELD_OF_BONES:
						DoCast(me, DUNGEON_MODE(SPELL_SHIELD_OF_BONES_5N, SPELL_SHIELD_OF_BONES_5H));
                        events.RescheduleEvent(EVENT_SHIELD_OF_BONES, 8000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soulguard_bonecasterAI(creature);
    }
};

enum SoulguardAnimator
{
	EVENT_SA_RAISE_DEAD = 1,
	EVENT_SHADOW_BOLT   = 2,
    EVENT_SOUL_SICKNESS = 3,
    EVENT_SOUL_SIPHON   = 4,
};

class npc_soulguard_animator : public CreatureScript
{
public:
    npc_soulguard_animator() : CreatureScript("npc_soulguard_animator") { }

    struct npc_soulguard_animatorAI : public QuantumBasicAI
    {
        npc_soulguard_animatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SoulguardChannelBeamTimer;

        EventMap events;

        void Reset()
        {
			events.Reset();

			SoulguardChannelBeamTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SOULGUARD_CHANNEL, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
        {
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

            events.ScheduleEvent(EVENT_SA_RAISE_DEAD, 25000);
            events.ScheduleEvent(EVENT_SHADOW_BOLT, 5000);
            events.ScheduleEvent(EVENT_SOUL_SICKNESS, 8000);
            events.ScheduleEvent(EVENT_SOUL_SIPHON, 10000);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SoulguardChannelBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* horror = me->FindCreatureWithDistance(NPC_SOUL_HORROR, 10.0f))
				{
					me->SetFacingToObject(horror);
					DoCast(horror, SPELL_SOULGUARD_CHANNEL_BEAM, true);
					SoulguardChannelBeamTimer = 120000; // 2 Minutes
				}
			}
			else SoulguardChannelBeamTimer -= diff;

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SA_RAISE_DEAD:
                        DoCast(me, SPELL_SA_RAISE_DEAD);
                        events.RescheduleEvent(EVENT_SA_RAISE_DEAD, 25000);
                        return;
                    case EVENT_SHADOW_BOLT:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
						}
                        events.RescheduleEvent(EVENT_SHADOW_BOLT, 5000);
                        return;
                    case EVENT_SOUL_SICKNESS:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_SOUL_SICKNESS);
						}
                        events.RescheduleEvent(EVENT_SOUL_SICKNESS, 10000);
                        return;
                    case EVENT_SOUL_SIPHON:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_SOUL_SIPHON);
						}
                        events.RescheduleEvent(EVENT_SOUL_SIPHON, 8000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soulguard_animatorAI(creature);
    }
};

enum SoulguardAdept
{
	EVENT_SD_RAISE_DEAD    = 1,
	EVENT_SA_SHADOW_BOLT   = 2,
	EVENT_DRAIN_LIFE       = 3,
    EVENT_SHADOW_MEND      = 4,
};

class npc_soulguard_adept : public CreatureScript
{
public:
    npc_soulguard_adept() : CreatureScript("npc_soulguard_adept") { }

    struct npc_soulguard_adeptAI: public QuantumBasicAI
    {
        npc_soulguard_adeptAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SoulguardChannelBeamTimer;

        EventMap events;

        void Reset()
        {
			events.Reset();

			SoulguardChannelBeamTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SOULGUARD_CHANNEL, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

            events.ScheduleEvent(EVENT_SD_RAISE_DEAD, 25000);
            events.ScheduleEvent(EVENT_SA_SHADOW_BOLT, 8000);
            events.ScheduleEvent(EVENT_DRAIN_LIFE, 7000);
            events.ScheduleEvent(EVENT_SHADOW_MEND, 35000);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SoulguardChannelBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* horror = me->FindCreatureWithDistance(NPC_SOUL_HORROR, 10.0f))
				{
					me->SetFacingToObject(horror);
					DoCast(horror, SPELL_SOULGUARD_CHANNEL_BEAM, true);
					SoulguardChannelBeamTimer = 120000; // 2 Minutes
				}
			}
			else SoulguardChannelBeamTimer -= diff;

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SD_RAISE_DEAD:
                        DoCast(me, SPELL_SD_RAISE_DEAD);
                        events.RescheduleEvent(EVENT_SD_RAISE_DEAD, 25000);
                        return;
                    case EVENT_SA_SHADOW_BOLT:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_SA_SHADOW_BOLT_5N, SPELL_SA_SHADOW_BOLT_5H));
						}
                        events.RescheduleEvent(EVENT_SA_SHADOW_BOLT, 4000);
                        return;
                    case EVENT_DRAIN_LIFE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_DRAIN_LIFE_5N, SPELL_DRAIN_LIFE_5H));
						}
                        events.RescheduleEvent(EVENT_DRAIN_LIFE, 9000);
                        return;
                    case EVENT_SHADOW_MEND:
                        DoCast(me, DUNGEON_MODE(SPELL_SHADOW_MEND_5N, SPELL_SHADOW_MEND_5H));
                        events.RescheduleEvent(EVENT_SHADOW_MEND, 20000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soulguard_adeptAI(creature);
    }
};

enum SoulHorror
{
    EVENT_SOUL_STRIKE = 1,
};

class npc_soul_horror : public CreatureScript
{
public:
    npc_soul_horror() : CreatureScript("npc_soul_horror") { }

    struct npc_soul_horrorAI : public QuantumBasicAI
    {
        npc_soul_horrorAI(Creature* creature) : QuantumBasicAI(creature)
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

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SOUL_HORROR_VISUAL_1, true);
			DoCast(me, SPELL_SOUL_HORROR_VISUAL_2, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SOUL_STRIKE, 6000);
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
                switch (eventId)
                {
                    case EVENT_SOUL_STRIKE:
                        DoCastVictim(DUNGEON_MODE(SPELL_SOUL_STRIKE_5N, SPELL_SOUL_STRIKE_5H));
                        events.RescheduleEvent(EVENT_SOUL_STRIKE, 8000);
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soul_horrorAI(creature);
    }
};

class npc_ghoul_minion : public CreatureScript
{
public:
    npc_ghoul_minion() : CreatureScript("npc_ghoul_minion") { }

    struct npc_ghoul_minionAI : public QuantumBasicAI
    {
        npc_ghoul_minionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ClawSlashTimer;

        void Reset()
        {
            ClawSlashTimer = 1000;

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

            if (ClawSlashTimer <= diff)
            {
                DoCastVictim(SPELL_CLAW_SLASH);
                ClawSlashTimer = 4000;
            }
			else ClawSlashTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghoul_minionAI(creature);
    }
};

void AddSC_forge_of_souls_trash()
{
	new npc_spiteful_apparition();
	new npc_spectral_warden();
    new npc_soulguard_watchman();
    new npc_soulguard_reaper();
    new npc_soulguard_bonecaster();
    new npc_soulguard_animator();
    new npc_soulguard_adept();
	new npc_soul_horror();
	new npc_ghoul_minion();
}