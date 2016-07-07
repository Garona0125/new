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

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum Spells
{
	SPELL_CURSE_OF_BLOOD = 24673,
	SPELL_ILLUSION       = 17773,
	SPELL_CLEAVE         = 15584,
};

class boss_jandice_barov : public CreatureScript
{
public:
    boss_jandice_barov() : CreatureScript("boss_jandice_barov") { }

    struct boss_jandicebarovAI : public QuantumBasicAI
    {
        boss_jandicebarovAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CurseOfBloodTimer;
        uint32 IllusionTimer;
        //uint32 Illusioncounter;
        uint32 InvisibleTimer;
        bool Invisible;

        void Reset()
        {
            CurseOfBloodTimer = 15000;
            IllusionTimer = 30000;
            InvisibleTimer = 3000;
            Invisible = false;
        }

        void EnterToBattle(Unit* /*who*/){}

        void SummonIllusions(Unit* victim)
        {
            if (Creature* Illusion = DoSpawnCreature(11439, float(irand(-9, 9)), float(irand(-9, 9)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000))
                Illusion->AI()->AttackStart(victim);
        }

        void UpdateAI(uint32 const diff)
        {
            if (Invisible && InvisibleTimer <= diff)
            {
                me->SetCurrentFaction(16);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetDisplayId(11073);
                Invisible = false;
            }
			else if (Invisible)
            {
                InvisibleTimer -= diff;
                return;
            }

            if (!UpdateVictim())
                return;

            if (CurseOfBloodTimer <= diff)
            {
                DoCastVictim(SPELL_CURSE_OF_BLOOD);
                CurseOfBloodTimer = 30000;
            }
			else CurseOfBloodTimer -= diff;

            if (!Invisible && IllusionTimer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                me->SetCurrentFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetDisplayId(MODEL_ID_INVISIBLE);  // Invisible Model
                DoModifyThreatPercent(me->GetVictim(), -99);

                Unit* target = NULL;
                for (uint8 i = 0; i < 10; ++i)
                {
                    target = SelectTarget(TARGET_RANDOM, 0);
					SummonIllusions(target);
                }
                Invisible = true;
                InvisibleTimer = 3000;
                IllusionTimer = 25000;
            }
			else IllusionTimer -= diff;

            //            //IllusionTimer
            //            if (IllusionTimer <= diff)
            //            {
            //                  //Cast
            //                DoCastVictim(SPELL_ILLUSION);
            //
            //                  //3 Illusion will be summoned
            //                  if (Illusioncounter < 3)
            //                  {
            //                    IllusionTimer = 500;
            //                    ++Illusioncounter;
            //                  }
            //                  else {
            //                      //15 seconds until we should cast this again
            //                      IllusionTimer = 15000;
            //                      Illusioncounter = 0;
            //                  }
            //
            //            } else IllusionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_jandicebarovAI(creature);
    }
};

// Illusion of Jandice Barov Script

class mob_illusionofjandicebarov : public CreatureScript
{
public:
    mob_illusionofjandicebarov() : CreatureScript("mob_illusionofjandicebarov") { }

    struct mob_illusionofjandicebarovAI : public QuantumBasicAI
    {
        mob_illusionofjandicebarovAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;

        void Reset()
        {
            CleaveTimer = urand(2000, 8000);
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(5000, 8000);
            }
			else CleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_illusionofjandicebarovAI(creature);
    }
};

void AddSC_boss_jandicebarov()
{
    new boss_jandice_barov();
    new mob_illusionofjandicebarov();
}