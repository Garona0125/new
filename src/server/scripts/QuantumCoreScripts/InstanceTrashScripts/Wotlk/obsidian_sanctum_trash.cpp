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

enum ObsidianSanctumSpells
{
	SPELL_CONJURE_FLAME_ORB   = 57753,
    SPELL_FLAME_SHOCK_10      = 39529,
    SPELL_FLAME_SHOCK_25      = 58940,
    SPELL_RAIN_OF_FIRE_10     = 57757,
    SPELL_RAIN_OF_FIRE_25     = 58936,
	SPELL_AVENGING_FURY       = 57742,
    SPELL_DEVOTION_AURA_10    = 57740,
    SPELL_DEVOTION_AURA_25    = 58944,
    SPELL_DRACONIC_RAGE_10    = 57733,
    SPELL_DRACONIC_RAGE_25    = 58942,
    SPELL_MORTAL_STRIKE       = 13737,
    SPELL_HAMMER_DROP         = 57759,
    SPELL_PUMMEL              = 58953,
    SPELL_CURSE_OF_MENDING_10 = 39647,
    SPELL_CURSE_OF_MENDING_25 = 58948,
    SPELL_FRENZY              = 53801,
    SPELL_SHOCKWAVE_10        = 57728,
    SPELL_SHOCKWAVE_25        = 58947,
};

class npc_onyx_blaze_mistress : public CreatureScript
{
public:
    npc_onyx_blaze_mistress() : CreatureScript("npc_onyx_blaze_mistress") { }

    struct npc_onyx_blaze_mistressAI: public QuantumBasicAI
    {
        npc_onyx_blaze_mistressAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FlameShockTimer;
        uint32 RainOfFireTimer;
        uint32 FlameOrbTimer;

        void Reset()
        {
            FlameShockTimer = 1000;
            RainOfFireTimer = 3000;
            FlameOrbTimer = 5000;

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

            if (FlameShockTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_FLAME_SHOCK_10, SPELL_FLAME_SHOCK_25));
					FlameShockTimer = urand(3000, 4000);
				}
            }
			else FlameShockTimer -= diff;

            if (RainOfFireTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_RAIN_OF_FIRE_10, SPELL_RAIN_OF_FIRE_25));
					RainOfFireTimer = urand(6000, 7000);
				}
            }
			else RainOfFireTimer -= diff;

            if (FlameOrbTimer <= diff)
            {
				DoCastAOE(SPELL_CONJURE_FLAME_ORB);
				FlameOrbTimer = urand(9000, 10000);
            }
			else FlameOrbTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onyx_blaze_mistressAI(creature);
    }
};

class npc_onyx_brood_general : public CreatureScript
{
public:
    npc_onyx_brood_general() : CreatureScript("npc_onyx_brood_general") { }

    struct npc_onyx_brood_generalAI: public QuantumBasicAI
    {
        npc_onyx_brood_generalAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MortalStrikeTimer;
        uint32 AvengingFuryTimer;
        uint32 DraconicRageTimer;

        void Reset()
        {
            MortalStrikeTimer = 2000;
            AvengingFuryTimer = 4000;
            DraconicRageTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, RAID_MODE(SPELL_DEVOTION_AURA_10, SPELL_DEVOTION_AURA_25));
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (MortalStrikeTimer <= diff)
            {
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3000, 4000);
            }
			else MortalStrikeTimer -= diff;

			if (AvengingFuryTimer <= diff)
            {
				DoCastAOE(SPELL_AVENGING_FURY);
				AvengingFuryTimer = urand(6000, 7000);
            }
			else AvengingFuryTimer -= diff;

            if (DraconicRageTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_DRACONIC_RAGE_10, SPELL_DRACONIC_RAGE_25));
				DraconicRageTimer = urand(9000, 10000);
            }
			else DraconicRageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onyx_brood_generalAI(creature);
    }
};

class npc_onyx_flight_captain : public CreatureScript
{
public:
    npc_onyx_flight_captain() : CreatureScript("npc_onyx_flight_captain") { }

    struct npc_onyx_flight_captainAI: public QuantumBasicAI
    {
        npc_onyx_flight_captainAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 HammerDropTimer;
        uint32 PummelTimer;

        void Reset()
        {
            HammerDropTimer = 2000;
            PummelTimer = 4000;

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

            if (HammerDropTimer <= diff)
            {
				DoCastVictim(SPELL_HAMMER_DROP);
				HammerDropTimer = 4000;
            }
			else HammerDropTimer -= diff;

            if (PummelTimer <= diff)
            {
				DoCastVictim(SPELL_PUMMEL);
				PummelTimer = 6000;
            }
			else PummelTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onyx_flight_captainAI(creature);
    }
};

class npc_onyx_sanctum_guardian : public CreatureScript
{
public:
    npc_onyx_sanctum_guardian() : CreatureScript("npc_onyx_sanctum_guardian") { }

    struct npc_onyx_sanctum_guardianAI: public QuantumBasicAI
    {
        npc_onyx_sanctum_guardianAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShockwaveTimer;

        void Reset()
        {
            ShockwaveTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCastAOE(RAID_MODE(SPELL_CURSE_OF_MENDING_10, SPELL_CURSE_OF_MENDING_25));
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShockwaveTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_SHOCKWAVE_10, SPELL_SHOCKWAVE_25));
				ShockwaveTimer = urand(5000, 6000);
            }
			else ShockwaveTimer -= diff;

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
        return new npc_onyx_sanctum_guardianAI(creature);
    }
};

void AddSC_obsidian_sanctum_trash()
{
	new npc_onyx_blaze_mistress();
	new npc_onyx_brood_general();
	new npc_onyx_flight_captain();
	new npc_onyx_sanctum_guardian();
}