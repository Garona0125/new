/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Gahz'ranka
SD%Complete: 85
SDComment: Massive Geyser with knockback not working. Spell buggy.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum Spells
{
	SPELL_FROST_BREATH   = 16099,
	SPELL_MASSIVE_GEYSER = 22421,
	SPELL_SLAM           = 24326,
};

class boss_gahzranka : public CreatureScript
{
public:
	boss_gahzranka() : CreatureScript("boss_gahzranka") {}

	struct boss_gahzrankaAI : public QuantumBasicAI
	{
		boss_gahzrankaAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 FrostbreathTimer;
		uint32 MassiveGeyserTimer;
		uint32 SlamTimer;

            void Reset()
            {
                FrostbreathTimer = 8000;
                MassiveGeyserTimer = 25000;
                SlamTimer = 17000;
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
				//Return since we have no target
                if (!UpdateVictim())
                    return;

                //FrostbreathTimer
                if (FrostbreathTimer <= diff)
                {
                    DoCastVictim(SPELL_FROST_BREATH);
                    FrostbreathTimer = urand(7000, 11000);
                }
				else FrostbreathTimer -= diff;

                //MassiveGeyserTimer
                if (MassiveGeyserTimer <= diff)
                {
                    DoCastVictim(SPELL_MASSIVE_GEYSER);
                    DoResetThreat();

                    MassiveGeyserTimer = urand(22000, 32000);
                }
				else MassiveGeyserTimer -= diff;

                //SlamTimer
                if (SlamTimer <= diff)
                {
                    DoCastVictim(SPELL_SLAM);
                    SlamTimer = urand(12000, 20000);
                }
				else SlamTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_gahzrankaAI(creature);
        }
};

void AddSC_boss_gahzranka()
{
    new boss_gahzranka();
}