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

enum RubySanctumSpells
{
	SPELL_FLAME_WAVE     = 75413,
    SPELL_SCORCH_10      = 75412,
    SPELL_SCORCH_25      = 75419,
	SPELL_SKULL_CRACK    = 15621,
	SPELL_MORTAL_STRIKE  = 13737,
    SPELL_RALLYING_SHOUT = 75414,
	SPELL_CLEAVE         = 15284,
    SPELL_SHOCKWAVE_10   = 75417,
    SPELL_SHOCKWAVE_25   = 75418,
};

class npc_charscale_assaulter : public CreatureScript
{
public:
    npc_charscale_assaulter() : CreatureScript("npc_charscale_assaulter") { }

    struct npc_charscale_assaulterAI : public QuantumBasicAI
    {
        npc_charscale_assaulterAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        uint32 CleaveTimer;
        uint32 ShockwaveTimer;

        void Reset()
        {
            CleaveTimer = 2000;
            ShockwaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 4000;
            }
			else CleaveTimer -= diff;

            if (ShockwaveTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_SHOCKWAVE_10, SPELL_SHOCKWAVE_25, SPELL_SHOCKWAVE_10, SPELL_SHOCKWAVE_25));
				ShockwaveTimer = 6000;
            }
			else ShockwaveTimer -= diff;
			
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_charscale_assaulterAI(creature);
    }
};

class npc_charscale_invoker : public CreatureScript
{
public:
    npc_charscale_invoker() : CreatureScript("npc_charscale_invoker") { }

    struct npc_charscale_invokerAI : public QuantumBasicAI
    {
        npc_charscale_invokerAI(Creature* creature) : QuantumBasicAI(creature)
        {
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }
		
		uint32 FlameWaveTimer;
        uint32 ScorchTimer;

        void Reset()
        {
			FlameWaveTimer = 2000;
            ScorchTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
                return;

            if (FlameWaveTimer <= diff)
            {
                DoCastAOE(SPELL_FLAME_WAVE);
                FlameWaveTimer = 4000;
            }
            else FlameWaveTimer -= diff;

            if (ScorchTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, RAID_MODE(SPELL_SCORCH_10, SPELL_SCORCH_25, SPELL_SCORCH_10, SPELL_SCORCH_25));
					ScorchTimer = 6000;
                }
            }
			else ScorchTimer -= diff;
			
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_charscale_invokerAI(creature);
    }
};

class npc_charscale_elite : public CreatureScript
{
public:
    npc_charscale_elite() : CreatureScript("npc_charscale_elite") { }

    struct npc_charscale_eliteAI : public QuantumBasicAI
    {
        npc_charscale_eliteAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        uint32 SkullCrackTimer;

        void Reset()
        {
            SkullCrackTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (SkullCrackTimer <= diff)
            {
                DoCast(SPELL_SKULL_CRACK);
                SkullCrackTimer = 4000;
            }
			else SkullCrackTimer -= diff;
			
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_charscale_eliteAI(creature);
    }
};

class npc_charscale_commander : public CreatureScript
{
public:
    npc_charscale_commander() : CreatureScript("npc_charscale_commander") { }

    struct npc_charscale_commanderAI : public QuantumBasicAI
    {
        npc_charscale_commanderAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        uint32 MortalStrikeTimer;
        uint32 RallyingShoutTimer;

        void Reset()
        {
            MortalStrikeTimer = 2000;
            RallyingShoutTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = 4000;
            }
			else MortalStrikeTimer -= diff;

            if (RallyingShoutTimer <= diff)
            {
                me->AddAura(SPELL_RALLYING_SHOUT, me);
                RallyingShoutTimer = 6000;
            }
			else RallyingShoutTimer -=diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_charscale_commanderAI(creature);
    }
};

void AddSC_ruby_sanctum_trash()
{
	new npc_charscale_assaulter();
	new npc_charscale_invoker();
	new npc_charscale_elite();
	new npc_charscale_commander();
}