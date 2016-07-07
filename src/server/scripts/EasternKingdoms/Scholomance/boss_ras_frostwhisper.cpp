/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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
SDName: Boss_Ras_Frostwhisper
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum Spells
{
    SPELL_FROSTBOLT         = 21369,
    SPELL_ICEARMOR          = 18100, // This is actually a buff he gives himself
    SPELL_FREEZE            = 18763,
    SPELL_FEAR              = 26070,
    SPELL_CHILLNOVA         = 18099,
    SPELL_FROSTVOLLEY       = 8398
};

class boss_boss_ras_frostwhisper : public CreatureScript
{
public:
    boss_boss_ras_frostwhisper() : CreatureScript("boss_boss_ras_frostwhisper") { }

    struct boss_rasfrostAI : public QuantumBasicAI
    {
        boss_rasfrostAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 IceArmorTimer;
        uint32 FrostboltTimer;
        uint32 FreezeTimer;
        uint32 FearTimer;
        uint32 ChillNovaTimer;
        uint32 FrostVolleyTimer;

        void Reset()
        {
            IceArmorTimer = 2000;
            FrostboltTimer = 8000;
            ChillNovaTimer = 12000;
            FreezeTimer = 18000;
            FrostVolleyTimer = 24000;
            FearTimer = 45000;

            DoCast(me, SPELL_ICEARMOR, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (IceArmorTimer <= diff)
            {
                DoCast(me, SPELL_ICEARMOR);
                IceArmorTimer = 180000;
            }
			else IceArmorTimer -= diff;

            if (FrostboltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = 8000;
				}
            }
			else FrostboltTimer -= diff;

            if (FreezeTimer <= diff)
            {
                DoCastVictim(SPELL_FREEZE);
                FreezeTimer = 24000;
            }
			else FreezeTimer -= diff;

            if (FearTimer <= diff)
            {
                DoCastVictim(SPELL_FEAR);
                FearTimer = 30000;
            }
			else FearTimer -= diff;

            //ChillNovaTimer
            if (ChillNovaTimer <= diff)
            {
                DoCastVictim(SPELL_CHILLNOVA);
                ChillNovaTimer = 14000;
            }
			else ChillNovaTimer -= diff;

            //FrostVolleyTimer
            if (FrostVolleyTimer <= diff)
            {
                DoCastVictim(SPELL_FROSTVOLLEY);
                FrostVolleyTimer = 15000;
            }
			else FrostVolleyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_rasfrostAI(creature);
    }
};

void AddSC_boss_rasfrost()
{
    new boss_boss_ras_frostwhisper();
}