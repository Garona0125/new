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
#include "the_black_morass.h"

enum Texts
{
	SAY_INTRO           = -1269000,
	SAY_AGGRO           = -1269001,
	SAY_DESPAWN_MEDIVH  = -1269002,
	SAY_SLAY_1          = -1269003,
	SAY_SLAY_2          = -1269004,
	SAY_DEATH           = -1269005,
};

enum Spells
{
	SPELL_HASTE           = 31458,
	SPELL_MORTAL_WOUND    = 31464,
	SPELL_WING_BUFFET_5N  = 31475,
	SPELL_WING_BUFFET_5H  = 38593,
	SPELL_REFLECT         = 38592,
};

class boss_temporus : public CreatureScript
{
public:
    boss_temporus() : CreatureScript("boss_temporus") {}

    struct boss_temporusAI : public QuantumBasicAI
    {
        boss_temporusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
        }

        InstanceScript* instance;

		bool Intro;

		uint32 MortalWoundTimer;
		uint32 WingBuffetTimer;
		uint32 HasteTimer;
        uint32 SpellReflectionTimer;

        void Reset()
        {
			MortalWoundTimer = 2000;
			WingBuffetTimer = 4000;
            HasteTimer = 6000;
            SpellReflectionTimer = 8000;

			Intro = true;

            if (instance)
                if (IsHeroic())
					instance->SetData(DATA_TEMPORUS_DEATH, 0);
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (me->IsWithinDistInMap(who, 1500.0f))
			{
				if (who->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (Intro)
					{
						DoSendQuantumText(SAY_INTRO, me);
						Intro = false;
					}
				}
			}
		}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);
        }

        void KilledUnit(Unit* /*who*/)
        {
			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(DATA_TEMPORUS_DEATH, 1);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalWoundTimer < diff)
            {
                DoCastVictim(SPELL_MORTAL_WOUND);
                MortalWoundTimer = urand(3000, 4000);
            }
			else MortalWoundTimer -= diff;

			if (WingBuffetTimer < diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_WING_BUFFET_5N, SPELL_WING_BUFFET_5H));
                WingBuffetTimer = urand(5000, 6000);
            }
			else WingBuffetTimer -= diff;

            if (HasteTimer < diff)
            {
                DoCast(me, SPELL_HASTE);
                HasteTimer = urand(7000, 8000);
            }
			else HasteTimer -= diff;

            if (SpellReflectionTimer < diff && IsHeroic())
            {
                DoCast(me, SPELL_REFLECT);
                SpellReflectionTimer = urand(14000, 16000);
            }
			else SpellReflectionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_temporusAI(creature);
    }
};

void AddSC_boss_temporus()
{
    new boss_temporus();
}