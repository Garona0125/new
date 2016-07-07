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
#include "culling_of_stratholme.h"

enum Texts
{
    SAY_AGGRO = -1595045,
    SAY_FAIL  = -1595046,
    SAY_DEATH = -1595047,
};

enum Spells
{
    SPELL_CORRUPTING_BLIGHT = 60588,
    SPELL_VOID_STRIKE       = 60590,
	SPELL_TIME_RIFT_CHANNEL = 31387,
};

class boss_infinite_corruptor : public CreatureScript
{
public:
    boss_infinite_corruptor() : CreatureScript("boss_infinite_corruptor") { }

    struct boss_infinite_corruptorAI : public QuantumBasicAI
    {
        boss_infinite_corruptorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

		uint32 TimeRiftChannelTimer;
        uint32 BlightTimer;
        uint32 EscapeTimer;
        uint32 VoidStrikeTimer;

        bool bEscaped;
        bool bEscaping;

        void Reset()
        {
			TimeRiftChannelTimer = 500;
            EscapeTimer = 2000;
            BlightTimer = urand(7000, 9000);
            VoidStrikeTimer = urand(6000, 10000);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            me->SetReactState(REACT_AGGRESSIVE);

            bEscaped = false;
            bEscaping = false;

			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_INFINITE_EVENT, DONE);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);

			if (!bEscaped)
				instance->DoCompleteAchievement(ACHIEVEMENT_CULLING_OF_TIME);
        }

        void UpdateAI(const uint32 diff)
        {
			if (TimeRiftChannelTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 75.0f, true))
				{
					trigger->SetDisplayId(MODEL_ID_TIME_RIFT);
					trigger->SetObjectScale(0.5f);
					me->SetFacingToObject(trigger);
					DoCast(trigger, SPELL_TIME_RIFT_CHANNEL, true);
					TimeRiftChannelTimer = 120000; // 2 Minutes
				}
			}
			else TimeRiftChannelTimer -= diff;

            if (EscapeTimer <= diff)
            {
                if (!bEscaping && !instance->GetData(DATA_COUNTDOWN))
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(0, 2335.93f, 1278.89f, 132.89f);
                    bEscaping = true;
                }
                EscapeTimer = 2000;
            }
			else EscapeTimer -= diff;

            if (bEscaping)
			{
                if (me->GetDistance(2335.93f, 1278.89f, 132.89f) < 1.0f)
                {
					instance->SetData(DATA_INFINITE_EVENT, DONE);
					instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);

                    bEscaped = true;
                    me->DisappearAndDie();
				}
			}

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (BlightTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_CORRUPTING_BLIGHT, false);
					BlightTimer = urand(7000, 9000);
				}
            }
			else BlightTimer -= diff;

            if (VoidStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_VOID_STRIKE, false);
                VoidStrikeTimer = urand(6000, 10000);
            }
			else VoidStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_infinite_corruptorAI(creature);
    }
};

void AddSC_boss_infinite_corruptor()
{
    new boss_infinite_corruptor();
}