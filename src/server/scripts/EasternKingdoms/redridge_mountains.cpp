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
#include "QuantumGossip.h"
#include "QuantumEscortAI.h"

enum CorporalKeeshan
{
    QUEST_MISSING_IN_ACTION = 219,

	SAY_CORPORAL_1  = 0,
	SAY_CORPORAL_2  = 1,
	SAY_CORPORAL_3  = 2,
	SAY_CORPORAL_4  = 3,
	SAY_CORPORAL_5  = 4,

    SPELL_MOCKING_BLOW = 21008,
    SPELL_SHIELD_BASH  = 11972,
};

class npc_corporal_keeshan : public CreatureScript
{
public:
    npc_corporal_keeshan() : CreatureScript("npc_corporal_keeshan") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_MISSING_IN_ACTION)
        {
            CAST_AI(npc_corporal_keeshan::npc_corporal_keeshanAI, creature->AI())->Start(true, false, player->GetGUID(), quest);
            creature->AI()->Talk(SAY_CORPORAL_1);
			player->SetCurrentFaction(250);
        }
        return false;
    }

    struct npc_corporal_keeshanAI : public npc_escortAI
    {
        npc_corporal_keeshanAI(Creature* creature) : npc_escortAI(creature) {}

        uint32 Phase;
        uint32 Timer;
        uint32 Timer1;
        uint32 ShieldBashTimer;

        void Reset()
        {
            Timer = 0;
            Phase = 0;
            Timer1 = 5000;
            ShieldBashTimer  = 8000;
        }

        void WaypointReached(uint32 uiI)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            if (uiI >= 65 && me->GetUnitMovementFlags() == MOVEMENTFLAG_WALKING)
                me->SetWalk(false);

            switch (uiI)
            {
                case 39:
                    SetEscortPaused(true);
                    Timer = 2000;
                    Phase = 1;
                    break;
                case 65:
                    me->SetWalk(false);
                    break;
                case 115:
					player->GroupEventHappens(QUEST_MISSING_IN_ACTION, me);
                    Timer = 2000;
                    Phase = 4;
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (HasEscortState(STATE_ESCORT_NONE))
                return;

            npc_escortAI::UpdateAI(diff);

            if (Phase)
            {
                if (Timer <= diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            me->SetStandState(UNIT_STAND_STATE_SIT);
                            Timer = 1000;
                            Phase = 2;
                            break;
                        case 2:
                            Talk(SAY_CORPORAL_2);
                            Timer = 15000;
                            Phase = 3;
                            break;
                        case 3:
                            Talk(SAY_CORPORAL_3);
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            SetEscortPaused(false);
                            Timer = 0;
                            Phase = 0;
                            break;
                        case 4:
                            Talk(SAY_CORPORAL_4);
                            Timer = 2500;
                            Phase = 5;
                        case 5:
                            Talk(SAY_CORPORAL_5);
                            Timer = 0;
                            Phase = 0;
                    }
                }
				else Timer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (Timer1 <= diff)
            {
                DoCastVictim(SPELL_MOCKING_BLOW);
                Timer1 = 5000;
            }
			else Timer1 -= diff;

            if (ShieldBashTimer <= diff)
            {
                DoCastVictim(SPELL_MOCKING_BLOW);
                ShieldBashTimer = 8000;
            }
			else ShieldBashTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_corporal_keeshanAI(creature);
    }
};

void AddSC_redridge_mountains()
{
    new npc_corporal_keeshan();
}