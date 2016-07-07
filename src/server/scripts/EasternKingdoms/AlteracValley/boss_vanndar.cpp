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

enum Yells
{
	YELL_AGGRO                 = 0,
	YELL_EVADE                 = 1,
	YELL_RANDOM                = 2,
	YELL_SPELL                 = 3,
};

enum Spells
{
    SPELL_AVATAR               = 19135,
	SPELL_STORMBOLT            = 19136,
	SPELL_THUNDERCLAP          = 15588,
};

class boss_vanndar : public CreatureScript
{
public:
    boss_vanndar() : CreatureScript("boss_vanndar") { }

    struct boss_vanndarAI : public QuantumBasicAI
    {
        boss_vanndarAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 AvatarTimer;
        uint32 ThunderclapTimer;
        uint32 StormboltTimer;
        uint32 ResetTimer;
        uint32 YellTimer;

        void Reset()
        {
            AvatarTimer = 3*IN_MILLISECONDS;
            ThunderclapTimer = 4*IN_MILLISECONDS;
            StormboltTimer = 6*IN_MILLISECONDS;
            ResetTimer = 5*IN_MILLISECONDS;
            YellTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(YELL_AGGRO);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (AvatarTimer <= diff)
            {
                DoCastVictim(SPELL_AVATAR);
                AvatarTimer =  urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            }
			else AvatarTimer -= diff;

            if (ThunderclapTimer <= diff)
            {
                DoCastVictim(SPELL_THUNDERCLAP);
                ThunderclapTimer = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
			else ThunderclapTimer -= diff;

            if (StormboltTimer <= diff)
            {
                DoCastVictim(SPELL_STORMBOLT);
                StormboltTimer = urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS);
            }
			else StormboltTimer -= diff;

            if (YellTimer <= diff)
            {
				Talk(YELL_RANDOM);
                YellTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS); //20 to 30 seconds
            }
			else YellTimer -= diff;

            // check if creature is not outside of building
            if (ResetTimer <= diff)
            {
                if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
                {
                    EnterEvadeMode();
					Talk(YELL_EVADE);
                }
                ResetTimer = 5*IN_MILLISECONDS;
            }
			else ResetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vanndarAI(creature);
    }
};

void AddSC_boss_vanndar()
{
    new boss_vanndar();
}