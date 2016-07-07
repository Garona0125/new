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

enum Texts
{
	SAY_RANDOM              = 0,
	SAY_RAISE_SKELETONS     = 1,
	SAY_SLAY                = 2,
	SAY_DEATH               = 3,

	SAY_MAGE                = 4,
	SAY_WARRIOR             = 5,
	SAY_DRUID               = 6,
	SAY_PRIEST              = 7,
	SAY_PALADIN             = 8,
	SAY_SHAMAN              = 9,
	SAY_WARLOCK             = 10,
	SAY_HUNTER              = 11,
	SAY_ROGUE               = 12,
};

enum Spells
{
    SPELL_SHADOWFLAME_INITIAL   = 22972,
    SPELL_SHADOWFLAME           = 22539,
    SPELL_BELLOWING_ROAR        = 22686,
    SPELL_VEIL_OF_SHADOW        = 7068,
    SPELL_CLEAVE                = 20691,
    SPELL_TAIL_LASH             = 23364,
    SPELL_BONE_CONTRUST         = 23363,     //23362, 23361
    SPELL_MAGE                  = 23410,     //wild magic
    SPELL_WARRIOR               = 23397,     //beserk
    SPELL_DRUID                 = 23398,     // cat form
    SPELL_PRIEST                = 23401,     // corrupted healing
    SPELL_PALADIN               = 23418,     //syphon blessing
    SPELL_SHAMAN                = 23425,     //totems
    SPELL_WARLOCK               = 23427,     //infernals
    SPELL_HUNTER                = 23436,     //bow broke
    SPELL_ROGUE                 = 23414,     //Paralise
};

class boss_nefarian : public CreatureScript
{
public:
    boss_nefarian() : CreatureScript("boss_nefarian") { }

    struct boss_nefarianAI : public QuantumBasicAI
    {
        boss_nefarianAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowFlameTimer;
        uint32 BellowingRoarTimer;
        uint32 VeilOfShadowTimer;
        uint32 CleaveTimer;
        uint32 TailLashTimer;
        uint32 ClassCallTimer;
		uint32 DespawnTimer;

        bool Phase3;

        void Reset()
        {
            ShadowFlameTimer = 12*IN_MILLISECONDS;
            BellowingRoarTimer = 30*IN_MILLISECONDS;
            VeilOfShadowTimer = 15*IN_MILLISECONDS;
            CleaveTimer = 7*IN_MILLISECONDS;
            TailLashTimer = 10*IN_MILLISECONDS;
            //ClassCallTimer = 35*IN_MILLISECONDS;  // off timer becuase dps bug
            Phase3 = false;
            DespawnTimer = 5*IN_MILLISECONDS;
        }

        void KilledUnit(Unit* who)
        {
            Talk(SAY_SLAY, who->GetGUID());
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);
        }

        void EnterToBattle(Unit* who)
        {
			Talk(SAY_RANDOM);

            DoCast(who, SPELL_SHADOWFLAME_INITIAL);
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer <= diff)
            {
                if (!UpdateVictim())
                    me->DespawnAfterAction();

                DespawnTimer = 5*IN_MILLISECONDS;
            }
			else DespawnTimer -= diff;

            if (!UpdateVictim())
                return;

            if (ShadowFlameTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOWFLAME);
                ShadowFlameTimer = 12*IN_MILLISECONDS;
            }
			else ShadowFlameTimer -= diff;

            if (BellowingRoarTimer <= diff)
            {
                DoCastVictim(SPELL_BELLOWING_ROAR);
                BellowingRoarTimer = 30*IN_MILLISECONDS;
            }
			else BellowingRoarTimer -= diff;

            if (VeilOfShadowTimer <= diff)
            {
                DoCastVictim(SPELL_VEIL_OF_SHADOW);
                VeilOfShadowTimer = 15*IN_MILLISECONDS;
            }
			else VeilOfShadowTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 7*IN_MILLISECONDS;
            }
			else CleaveTimer -= diff;

            if (TailLashTimer <= diff)
            {
                DoCast(SPELL_TAIL_LASH);
                TailLashTimer = 10*IN_MILLISECONDS;
            }
			else TailLashTimer -= diff;

            /*if (ClassCallTimer <= diff)
            {
                switch (urand(0, 8))
                {
                    case 0:
                        Talk(SAY_MAGE);
                        DoCast(me, SPELL_MAGE);
                        break;
                    case 1:
                        Talk(SAY_WARRIOR);
                        DoCast(me, SPELL_WARRIOR);
                        break;
                    case 2:
                        Talk(SAY_DRUID);
                        DoCast(me, SPELL_DRUID);
                        break;
                    case 3:
                        Talk(SAY_PRIEST);
                        DoCast(me, SPELL_PRIEST);
                        break;
                    case 4:
                        Talk(SAY_PALADIN);
                        DoCast(me, SPELL_PALADIN);
                        break;
                    case 5:
                        Talk(SAY_SHAMAN);
                        DoCast(me, SPELL_SHAMAN);
                        break;
                    case 6:
                        Talk(SAY_WARLOCK);
                        DoCast(me, SPELL_WARLOCK);
                        break;
                    case 7:
                        Talk(SAY_HUNTER);
                        DoCast(me, SPELL_HUNTER);
                        break;
                    case 8:
                        Talk(SAY_ROGUE);
                        DoCast(me, SPELL_ROGUE);
                        break;
                }
                ClassCallTimer = 35*IN_MILLISECONDS + (rand() % 5*IN_MILLISECONDS);
            }
			else ClassCallTimer -= diff;*/

            //Phase3 begins when we are below X health
            if (!Phase3 && HealthBelowPct(HEALTH_PERCENT_20))
            {
                Phase3 = true;
                Talk(SAY_RAISE_SKELETONS);
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_nefarianAI(creature);
    }
};

void AddSC_boss_nefarian()
{
    new boss_nefarian();
}