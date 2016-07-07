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

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum Spells
{
    SPELL_HEAL             = 10917,
    SPELL_RENEW            = 10929,
    SPELL_SHIELD           = 10901,
    SPELL_MINDBLAST        = 10947,
    SPELL_SHADOW_WORD_PAIN = 10894,
    SPELL_SMITE            = 10934,
};

class boss_moira_bronzebeard : public CreatureScript
{
public:
    boss_moira_bronzebeard() : CreatureScript("boss_moira_bronzebeard") { }

    struct boss_moira_bronzebeardAI : public QuantumBasicAI
    {
        boss_moira_bronzebeardAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 HealTimer;
        uint32 MindBlastTimer;
        uint32 ShadowWordPainTimer;
        uint32 SmiteTimer;

        void Reset()
        {
            HealTimer = 12000;                 // These times are probably wrong
            MindBlastTimer = 16000;
            ShadowWordPainTimer = 2000;
            SmiteTimer = 8000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MindBlastTimer <= diff)
            {
                DoCastVictim(SPELL_MINDBLAST);
                MindBlastTimer = 14000;
            }
			else MindBlastTimer -= diff;

            if (ShadowWordPainTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOW_WORD_PAIN);
                ShadowWordPainTimer = 18000;
            }
			else ShadowWordPainTimer -= diff;

            if (SmiteTimer <= diff)
            {
                DoCastVictim(SPELL_SMITE);
                SmiteTimer = 10000;
            }
			else SmiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_moira_bronzebeardAI(creature);
    }
};

void AddSC_boss_moira_bronzebeard()
{
    new boss_moira_bronzebeard();
}