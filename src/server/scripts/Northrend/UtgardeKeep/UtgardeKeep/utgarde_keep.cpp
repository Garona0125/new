/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "utgarde_keep.h"

enum DarkRangerMarrah
{
	SPELL_RANGER_STEALTH = 34189,
};

class npc_dark_ranger_marrah : public CreatureScript
{
public:
    npc_dark_ranger_marrah() : CreatureScript("npc_dark_ranger_marrah") { }

    struct npc_dark_ranger_marrahAI : public QuantumBasicAI
    {
        npc_dark_ranger_marrahAI(Creature* creature) : QuantumBasicAI(creature) { }

        void Reset()
        {
            DoCast(me, SPELL_RANGER_STEALTH, true);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (who->ToPlayer()->GetTeamId() == TEAM_HORDE)
            {
                if (who->GetDistance2d(me) <= 5.0f)
                    me->RemoveAurasDueToSpell(SPELL_RANGER_STEALTH);
            }
			else
                me->SetVisible(false);

            return;
        }

		void EnterToBattle(Unit* /*who*/) { }

        void AttackStart(Unit* /*who*/)
		{
			return;
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_ranger_marrahAI(creature);
    }
};

void AddSC_utgarde_keep()
{
    new npc_dark_ranger_marrah();
}