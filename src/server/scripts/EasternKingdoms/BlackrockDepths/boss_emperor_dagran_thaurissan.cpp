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
#include "blackrock_depths.h"

enum Yells
{
    SAY_AGGRO = 0,
    SAY_SLAY  = 1,
};

enum Spells
{
    SPELL_HAND_OF_THAURISSAN = 17492,
    SPELL_AVATAR_OF_FLAME    = 15636,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_emperor_dagran_thaurissan : public CreatureScript
{
public:
    boss_emperor_dagran_thaurissan() : CreatureScript("boss_emperor_dagran_thaurissan") { }

    struct boss_draganthaurissanAI : public QuantumBasicAI
    {
        boss_draganthaurissanAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 HandOfThaurissanTimer;
        uint32 AvatarOfFlameTimer;
        //uint32 Counter;

        void Reset()
        {
            HandOfThaurissanTimer = 4000;
            AvatarOfFlameTimer = 25000;
            //Counter = 0;

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
				me->SetDisplayId(MODEL_CHRISTMAS_DAGRAN_THAURISSAN);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_AGGRO);
            me->CallForHelp(VISIBLE_RANGE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_SLAY);
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (HandOfThaurissanTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_HAND_OF_THAURISSAN);
                    HandOfThaurissanTimer = 5000;
                    //Counter              = 0;
				}
            }
			else HandOfThaurissanTimer -= diff;

            if (AvatarOfFlameTimer <= diff)
            {
                DoCastVictim(SPELL_AVATAR_OF_FLAME);
                AvatarOfFlameTimer = 18000;
            }
			else AvatarOfFlameTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* moira = me->FindCreatureWithDistance(NPC_MOIRA_BRONZEBEARD, 100.0f, true))
            {
                moira->SetCurrentFaction(35);
                moira->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                moira->AI()->EnterEvadeMode();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_draganthaurissanAI(creature);
    }
};

void AddSC_boss_draganthaurissan()
{
    new boss_emperor_dagran_thaurissan();
}