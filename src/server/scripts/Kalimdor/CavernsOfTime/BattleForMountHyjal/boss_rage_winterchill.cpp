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
#include "hyjal.h"
#include "hyjal_trash.h"

enum Texts
{
	SAY_DEATH           = 0,
	SAY_SLAY            = 1,
	SAY_DEATH_AND_DECAY = 2,
	SAY_FROST_NOVA      = 3,
	SAY_AGGRO           = 4,
};

enum Spells
{
	SPELL_FROST_ARMOR     = 31256,
	SPELL_DEATH_AND_DECAY = 31258,
	SPELL_FROST_NOVA      = 31250,
	SPELL_ICEBOLT         = 31249,
};

class boss_rage_winterchill : public CreatureScript
{
public:
    boss_rage_winterchill() : CreatureScript("boss_rage_winterchill") { }

    struct boss_rage_winterchillAI : public hyjal_trashAI
    {
        boss_rage_winterchillAI(Creature* creature) : hyjal_trashAI(creature)
        {
            instance = creature->GetInstanceScript();

            go = false;
        }

		uint32 IceboltTimer;
        uint32 FrostArmorTimer;
        uint32 DeathAndDecayTimer;
        uint32 FrostNovaTimer;

        bool go;

        void Reset()
        {
            damageTaken = 0;

			IceboltTimer = 0.5*IN_MILLISECONDS;
            FrostArmorTimer = 3*IN_MILLISECONDS;
            DeathAndDecayTimer = 5*IN_MILLISECONDS;
            FrostNovaTimer = 7*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if (instance && IsEvent)
                instance->SetData(DATA_RAGE_WINTERCHILL_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (instance && IsEvent)
                instance->SetData(DATA_RAGE_WINTERCHILL_EVENT, IN_PROGRESS);

			Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_SLAY);
        }

        void WaypointReached(uint32 waypointId)
        {
			if (waypointId == 7 && instance)
            {
                Unit* target = Unit::GetUnit(*me, instance->GetData64(DATA_JAINAPROUDMOORE));

                if (target && target->IsAlive())
                    me->AddThreat(target, 0.0f);
            }
        }

        void JustDied(Unit* victim)
        {
            hyjal_trashAI::JustDied(victim);

            if (instance && IsEvent)
                instance->SetData(DATA_RAGE_WINTERCHILL_EVENT, DONE);

			Talk(SAY_DEATH);
        }

        void UpdateAI(const uint32 diff)
        {
            if (IsEvent)
            {
                //Must update npc_escortAI
                npc_escortAI::UpdateAI(diff);
                if (!go)
                {
                    go = true;
                    if (instance)
                    {
                        AddWaypoint(0, 4896.08f, -1576.35f, 1333.65f);
                        AddWaypoint(1, 4898.68f, -1615.02f, 1329.48f);
                        AddWaypoint(2, 4907.12f, -1667.08f, 1321.00f);
                        AddWaypoint(3, 4963.18f, -1699.35f, 1340.51f);
                        AddWaypoint(4, 4989.16f, -1716.67f, 1335.74f);
                        AddWaypoint(5, 5026.27f, -1736.89f, 1323.02f);
                        AddWaypoint(6, 5037.77f, -1770.56f, 1324.36f);
                        AddWaypoint(7, 5067.23f, -1789.95f, 1321.17f);
                        Start(false, true);
                        SetDespawnAtEnd(false);
                    }
                }
            }

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IceboltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
				{
					DoCast(target, SPELL_ICEBOLT);
					IceboltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else IceboltTimer -= diff;

            if (FrostArmorTimer <= diff)
            {
                DoCast(me, SPELL_FROST_ARMOR);
                FrostArmorTimer = urand(12*IN_MILLISECONDS, 13*IN_MILLISECONDS);
            }
			else FrostArmorTimer -= diff;

			if (DeathAndDecayTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					Talk(SAY_DEATH_AND_DECAY);

					DoCast(target, SPELL_DEATH_AND_DECAY);
					DeathAndDecayTimer = urand(15*IN_MILLISECONDS, 16*IN_MILLISECONDS);
				}
            }
			else DeathAndDecayTimer -= diff;

            if (FrostNovaTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					Talk(SAY_FROST_NOVA);

					DoCast(target, SPELL_FROST_NOVA);
					FrostNovaTimer = urand(18*IN_MILLISECONDS, 19*IN_MILLISECONDS);
				}
            }
			else FrostNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_rage_winterchillAI(creature);
    }
};

void AddSC_boss_rage_winterchill()
{
    new boss_rage_winterchill();
}