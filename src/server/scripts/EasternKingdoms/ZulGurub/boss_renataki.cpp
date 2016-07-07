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
#include "zulgurub.h"

enum Renataki
{
	SPELL_AMBUSH           = 24337,
	SPELL_THOUSANDBLADES   = 24649,
	EQUIP_ID_MAIN_HAND     = 0,
};

class boss_renataki : public CreatureScript
{
    public:
        boss_renataki() : CreatureScript("boss_renataki") {}

        struct boss_renatakiAI : public QuantumBasicAI
        {
            boss_renatakiAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 InvisibleTimer;
            uint32 AmbushTimer;
            uint32 VisibleTimer;
            uint32 AggroTimer;
            uint32 ThousandBladesTimer;

            bool Invisible;
            bool Ambushed;

            void Reset()
            {
                InvisibleTimer = urand(8000, 18000);
                AmbushTimer = 3000;
                VisibleTimer = 4000;
                AggroTimer = urand(15000, 25000);
                ThousandBladesTimer = urand(4000, 8000);

                Invisible = false;
                Ambushed = false;
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (InvisibleTimer <= diff)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);

                    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                    me->SetDisplayId(MODEL_ID_INVISIBLE);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Invisible = true;

                    InvisibleTimer = urand(15000, 30000);
                }
				else InvisibleTimer -= diff;

                if (Invisible)
                {
                    if (AmbushTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        {
                            DoTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                            DoCast(target, SPELL_AMBUSH);
                        }
                        Ambushed = true;
                        AmbushTimer = 3000;
                    }
					else AmbushTimer -= diff;
                }

                if (Ambushed)
                {
                    if (VisibleTimer <= diff)
                    {
                        me->InterruptSpell(CURRENT_GENERIC_SPELL);

                        me->SetDisplayId(15268);
                        SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Invisible = false;

                        VisibleTimer = 4000;
                    }
					else VisibleTimer -= diff;
                }

                if (!Invisible)
                {
                    if (AggroTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
						{
							if (DoGetThreat(me->GetVictim()))
								DoModifyThreatPercent(me->GetVictim(), -50);

							AttackStart(target);
							AggroTimer = urand(7000, 20000);
						}
                    }
					else AggroTimer -= diff;

                    if (ThousandBladesTimer <= diff)
                    {
                        DoCastVictim(SPELL_THOUSANDBLADES);
                        ThousandBladesTimer = urand(7000, 12000);
                    }
					else ThousandBladesTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_renatakiAI(creature);
        }
};

void AddSC_boss_renataki()
{
    new boss_renataki();
}