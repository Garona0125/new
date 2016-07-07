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
#include "GameEventMgr.h"

//front, left
#define ADD_1X 3553.851807f
#define ADD_1Y -2945.885986f
#define ADD_1Z 125.001015f
#define ADD_1O 0.592007f
//front, right
#define ADD_2X 3559.206299f
#define ADD_2Y -2952.929932f
#define ADD_2Z 125.001015f
#define ADD_2O 0.592007f
//mid, left
#define ADD_3X 3552.417480f
#define ADD_3Y -2948.667236f
#define ADD_3Z 125.001015f
#define ADD_3O 0.592007f
//mid, right
#define ADD_4X 3555.651855f
#define ADD_4Y -2953.519043f
#define ADD_4Z 125.001015f
#define ADD_4O 0.592007f
//back, left
#define ADD_5X 3547.927246f
#define ADD_5Y -2950.977295f
#define ADD_5Z 125.001015f
#define ADD_5O 0.592007f
//back, mid
#define ADD_6X 3553.094697f
#define ADD_6Y -2952.123291f
#define ADD_6Z 125.001015f
#define ADD_6O 0.592007f
//back, right
#define ADD_7X 3552.727539f
#define ADD_7Y -2957.776123f
#define ADD_7Z 125.001015f
#define ADD_7O 0.592007f
//behind, left
#define ADD_8X 3547.156250f
#define ADD_8Y -2953.162354f
#define ADD_8Z 125.001015f
#define ADD_8O 0.592007f
//behind, right
#define ADD_9X 3550.202148f
#define ADD_9Y -2957.437744f
#define ADD_9Z 125.001015f
#define ADD_9O 0.592007f

enum Spells
{
	SPELL_KNOCK_AWAY      = 10101,
	SPELL_PUMMEL          = 15615,
	SPELL_SHOOT           = 16496,
	SPELL_SUMMON_RIFLEMAN = 17279,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_cannon_master_willey : public CreatureScript
{
public:
    boss_cannon_master_willey() : CreatureScript("boss_cannon_master_willey") { }

    struct boss_cannon_master_willeyAI : public QuantumBasicAI
    {
        boss_cannon_master_willeyAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 KnockAwayTimer;
        uint32 PummelTimer;
        uint32 ShootTimer;
        uint32 SummonRiflemanTimer;

        void Reset()
        {
            ShootTimer = 1000;
            PummelTimer = 7000;
            KnockAwayTimer = 11000;
            SummonRiflemanTimer = 15000;

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
				me->SetDisplayId(MODEL_CHRISTMAS_MASTER_WILLEY);
        }

        void JustDied(Unit* /*Victim*/)
        {
            me->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 240000);
            me->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 240000);
            me->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 240000);
            me->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 240000);
            me->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 240000);
            me->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 240000);
            me->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 240000);
        }

		void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (PummelTimer <= diff)
            {
                if (rand()%100 < 90)
                    DoCastVictim(SPELL_PUMMEL);

                PummelTimer = 12000;
            }
			else PummelTimer -= diff;

            if (KnockAwayTimer <= diff)
            {
                if (rand()%100 < 80)
                    DoCastVictim(SPELL_KNOCK_AWAY);

                KnockAwayTimer = 14000;
            }
			else KnockAwayTimer -= diff;

            if (ShootTimer <= diff)
            {
                DoCastVictim(SPELL_SHOOT);
                ShootTimer = 1000;
            }
			else ShootTimer -= diff;

            if (SummonRiflemanTimer <= diff)
            {
                switch (rand()%9)
                {
                case 0:
                    me->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 1:
                    me->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 2:
                    me->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_6X, ADD_6Y, ADD_6Z, ADD_6O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 3:
                    me->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 4:
                    me->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_6X, ADD_6Y, ADD_6Z, ADD_6O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_8X, ADD_8Y, ADD_8Z, ADD_8O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 5:
                    me->SummonCreature(11054, ADD_6X, ADD_6Y, ADD_6Z, ADD_6O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 6:
                    me->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_8X, ADD_8Y, ADD_8Z, ADD_8O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 7:
                    me->SummonCreature(11054, ADD_8X, ADD_8Y, ADD_8Z, ADD_8O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                case 8:
                    me->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    me->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 240000);
                    break;
                }
                SummonRiflemanTimer = 30000;
            }
			else SummonRiflemanTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_cannon_master_willeyAI(creature);
    }
};

void AddSC_boss_cannon_master_willey()
{
    new boss_cannon_master_willey();
}