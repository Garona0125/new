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
	SAY_AGGRO               = 0,
	SAY_SUMMON60            = 1,
	SAY_SUMMON30            = 2,
	SAY_HP                  = 3,
	SAY_KILL                = 4,
};

enum Spells
{
	SPELL_AMNENNARS_WRATH = 13009,
	SPELL_FROSTBOLT       = 15530,
	SPELL_FROST_NOVA      = 15531,
	SPELL_FROST_SPECTRES  = 12642,
};

class boss_amnennar_the_coldbringer : public CreatureScript
{
public:
    boss_amnennar_the_coldbringer() : CreatureScript("boss_amnennar_the_coldbringer") { }

    struct boss_amnennar_the_coldbringerAI : public QuantumBasicAI
    {
        boss_amnennar_the_coldbringerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 AmnenarsWrathTimer;
        uint32 FrostBoltTimer;
        uint32 FrostNovaTimer;

        bool Spectrals60;
        bool Spectrals30;
        bool Hp;

        void Reset()
        {
            AmnenarsWrathTimer = 8000;
            FrostBoltTimer = 1000;
            FrostNovaTimer = urand(10000, 15000);

            Spectrals30 = false;
            Spectrals60 = false;
            Hp = false;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            Talk(SAY_KILL);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (AmnenarsWrathTimer <= diff)
            {
                DoCastVictim(SPELL_AMNENNARS_WRATH);
                AmnenarsWrathTimer = 12000;
            }
			else AmnenarsWrathTimer -= diff;

            if (FrostBoltTimer <= diff)
            {
                DoCastVictim(SPELL_FROSTBOLT);
                FrostBoltTimer = 8000;
            }
			else FrostBoltTimer -= diff;

            if (FrostNovaTimer <= diff)
            {
                DoCast(me, SPELL_FROST_NOVA);
                FrostNovaTimer = 15000;
            }
			else FrostNovaTimer -= diff;

            if (!Spectrals60 && HealthBelowPct(HEALTH_PERCENT_60))
            {
                Talk(SAY_SUMMON60);
                DoCastVictim(SPELL_FROST_SPECTRES);
                Spectrals60 = true;
            }

			if (!Hp && HealthBelowPct(HEALTH_PERCENT_50))
            {
                Talk(SAY_HP);
                Hp = true;
            }

			if (!Spectrals30 && HealthBelowPct(HEALTH_PERCENT_30))
            {
                Talk(SAY_SUMMON30);
                DoCastVictim(SPELL_FROST_SPECTRES);
                Spectrals30 = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_amnennar_the_coldbringerAI(creature);
    }
};

void AddSC_boss_amnennar_the_coldbringer()
{
    new boss_amnennar_the_coldbringer();
}