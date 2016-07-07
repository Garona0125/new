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
    SPELL_SHADOWBOLT      = 17228,
    SPELL_CURSEOFTONGUES  = 15470,
    SPELL_CURSEOFWEAKNESS = 17227,
    SPELL_DEMONARMOR      = 11735,
    SPELL_ENVELOPINGWEB   = 15471,
};

class boss_anubshiah : public CreatureScript
{
public:
    boss_anubshiah() : CreatureScript("boss_anubshiah") { }

    struct boss_anubshiahAI : public QuantumBasicAI
    {
        boss_anubshiahAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowBoltTimer;
        uint32 CurseOfTonguesTimer;
        uint32 CurseOfWeaknessTimer;
        uint32 DemonArmorTimer;
        uint32 EnvelopingWebTimer;

        void Reset()
        {
            ShadowBoltTimer = 7000;
            CurseOfTonguesTimer = 24000;
            CurseOfWeaknessTimer = 12000;
            DemonArmorTimer = 3000;
            EnvelopingWebTimer = 16000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {

            if (!UpdateVictim())
                return;

            if (ShadowBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOWBOLT);
					ShadowBoltTimer = 7000;
				}
            }
			else ShadowBoltTimer -= diff;

            if (CurseOfTonguesTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_CURSEOFTONGUES);
					CurseOfTonguesTimer = 18000;
				}
            }
			else CurseOfTonguesTimer -= diff;

            if (CurseOfWeaknessTimer <= diff)
            {
                DoCastVictim(SPELL_CURSEOFWEAKNESS);
                CurseOfWeaknessTimer = 45000;
            }
			else CurseOfWeaknessTimer -= diff;

            if (DemonArmorTimer <= diff)
            {
                DoCast(me, SPELL_DEMONARMOR);
                DemonArmorTimer = 300000;
            }
			else DemonArmorTimer -= diff;

            if (EnvelopingWebTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_ENVELOPINGWEB);
					EnvelopingWebTimer = 12000;
				}
            }
			else EnvelopingWebTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_anubshiahAI(creature);
    }
};

void AddSC_boss_anubshiah()
{
    new boss_anubshiah();
}