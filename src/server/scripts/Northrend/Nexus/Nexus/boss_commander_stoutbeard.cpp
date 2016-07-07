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
#include "nexus.h"

enum Texts
{
	SAY_AGGRO = -1576028,
	SAY_SLAY  = -1576029,
	SAY_DEATH = -1576030,
};

enum CommanderStoutbeard
{
	SPELL_FROZEN_PRISON        = 47543,
	SPELL_BATTLE_SHOUT         = 31403,
	SPELL_CHARGE               = 60067,
	SPELL_FRIGHTENING_SHOUT_5N = 19134,
	SPELL_FRIGHTENING_SHOUT_5H = 29544,
	SPELL_WHIRLWIND_5N         = 38618,
	SPELL_WHIRLWIND_5H         = 38619,
};

class boss_commander_stoutbeard : public CreatureScript
{
public:
    boss_commander_stoutbeard() : CreatureScript("boss_commander_stoutbeard") {}

    struct boss_commander_stoutbeardAI : public QuantumBasicAI
    {
        boss_commander_stoutbeardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BattleShoutTimer;
		uint32 FrighteningShoutTimer;
		uint32 WhirlwindTimer;

        void Reset()
        {
			BattleShoutTimer = 500;
			FrighteningShoutTimer = 2000;
			WhirlwindTimer = 4000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustReachedHome()
		{
			Reset();
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
			DoSendQuantumText(SAY_AGGRO, me);
			DoCastVictim(SPELL_CHARGE);
		}

		void KilledUnit(Unit* victim)
		{
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoSendQuantumText(SAY_DEATH, me);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_BATTLE_SHOUT);
				BattleShoutTimer = urand(5000, 6000);
			}
			else BattleShoutTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_WHIRLWIND_5N, SPELL_WHIRLWIND_5H));
				WhirlwindTimer = urand(7000, 8000);
			}
			else WhirlwindTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FRIGHTENING_SHOUT_5N, SPELL_FRIGHTENING_SHOUT_5H));
				FrighteningShoutTimer = urand(10000, 11000);
			}
			else FrighteningShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_commander_stoutbeardAI(creature);
    }
};

void AddSC_boss_commander_stoutbeard()
{
	new boss_commander_stoutbeard();
}