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

enum StillbladeData
{
    SAY_HEAL                 = 0,
    QUEST_REDEEMING_THE_DEAD = 9685,
    SPELL_SHIMMERING_VESSEL  = 31225,
    SPELL_REVIVE_SELF        = 32343,
};

class npc_blood_knight_stillblade : public CreatureScript
{
public:
    npc_blood_knight_stillblade() : CreatureScript("npc_blood_knight_stillblade") { }

    struct npc_blood_knight_stillbladeAI : public QuantumBasicAI
    {
        npc_blood_knight_stillbladeAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 LifeTimer;

        bool Casted;

        void Reset()
        {
            LifeTimer = 120000;

            me->SetStandState(UNIT_STAND_STATE_DEAD);
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SHOW_HEALTH);

            Casted = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (me->IsStandState())
            {
                if (LifeTimer <= diff)
				{
                    me->AI()->EnterEvadeMode();
				}
                else LifeTimer -= diff;
            }
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if ((spell->Id == SPELL_SHIMMERING_VESSEL) && !Casted && (caster->GetTypeId() == TYPE_ID_PLAYER) && (CAST_PLR(caster)->IsActiveQuest(QUEST_REDEEMING_THE_DEAD)))
            {
                CAST_PLR(caster)->AreaExploredOrEventHappens(QUEST_REDEEMING_THE_DEAD);
                DoCast(me, SPELL_REVIVE_SELF);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
                //me->RemoveAllAuras();
				Talk(SAY_HEAL);
                Casted = true;
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blood_knight_stillbladeAI(creature);
    }
};

void AddSC_silvermoon_city()
{
    new npc_blood_knight_stillblade();
}