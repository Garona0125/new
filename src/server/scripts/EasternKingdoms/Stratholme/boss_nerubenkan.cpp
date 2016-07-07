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
#include "stratholme.h"

enum Spells
{
	SPELL_ENCASING_WEBS       = 4962,
	SPELL_PIERCE_ARMOR        = 6016,
	SPELL_CRYPT_SCARABS       = 31602,
	SPELL_RAISE_UNDEAD_SCARAB = 17235,
};

class boss_nerubenkan : public CreatureScript
{
public:
    boss_nerubenkan() : CreatureScript("boss_nerubenkan") { }

    struct boss_nerubenkanAI : public QuantumBasicAI
    {
        boss_nerubenkanAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 EncasingWebsTimer;
        uint32 PierceArmorTimer;
        uint32 CryptScarabsTimer;
        uint32 RaiseUndeadScarabTimer;

        void Reset()
        {
            CryptScarabsTimer = 3000;
            EncasingWebsTimer = 7000;
            PierceArmorTimer = 19000;
            RaiseUndeadScarabTimer = 3000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(TYPE_NERUB, IN_PROGRESS);
        }

        void RaiseUndeadScarab(Unit* victim)
        {
            if (Creature* pUndeadScarab = DoSpawnCreature(10876, float(irand(-9, 9)), float(irand(-9, 9)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 180000))
			{
                if (pUndeadScarab->AI())
                    pUndeadScarab->AI()->AttackStart(victim);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (EncasingWebsTimer <= diff)
            {
                DoCastVictim(SPELL_ENCASING_WEBS);
                EncasingWebsTimer = 30000;
            }
			else EncasingWebsTimer -= diff;

            if (PierceArmorTimer <= diff)
            {
                if (urand(0, 3) < 2)
                    DoCastVictim(SPELL_PIERCE_ARMOR);
                PierceArmorTimer = 35000;
            }
			else PierceArmorTimer -= diff;

            if (CryptScarabsTimer <= diff)
            {
                DoCastVictim(SPELL_CRYPT_SCARABS);
                CryptScarabsTimer = 20000;
            }
			else CryptScarabsTimer -= diff;

            if (RaiseUndeadScarabTimer <= diff)
            {
                RaiseUndeadScarab(me->GetVictim());
                RaiseUndeadScarabTimer = 16000;
            }
			else RaiseUndeadScarabTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_nerubenkanAI(creature);
    }
};

void AddSC_boss_nerubenkan()
{
    new boss_nerubenkan();
}