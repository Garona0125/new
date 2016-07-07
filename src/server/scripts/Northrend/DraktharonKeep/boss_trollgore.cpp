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

/*
 * Comment: TODO: spawn troll waves
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "QuantumCreature.h"
#include "drak_tharon_keep.h"

enum Yells
{
    SAY_AGGRO    = -1600006,
    SAY_KILL     = -1600007,
    SAY_CONSUME  = -1600008,
    SAY_EXPLODE  = -1600009,
    SAY_DEATH    = -1600010,
};

enum Spells
{
    SPELL_INFECTED_WOUND    = 49637,
    SPELL_CRUSH             = 49639,
    SPELL_CORPSE_EXPLODE_5N = 49555,
	SPELL_CORPSE_EXPLODE_5H = 59807,
    SPELL_CONSUME_5N        = 49380,
	SPELL_CONSUME_5H        = 59803,
    SPELL_CONSUME_AURA_5N   = 49381,
	SPELL_CONSUME_AURA_5H   = 59805,
};

Position AddSpawnPoint = { -260.493011f, -622.968018f, 26.605301f, 3.036870f };

class boss_trollgore : public CreatureScript
{
public:
    boss_trollgore() : CreatureScript("boss_trollgore") { }

    struct boss_trollgoreAI : public QuantumBasicAI
    {
        boss_trollgoreAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ConsumeTimer;
        uint32 AuraCountTimer;
        uint32 CrushTimer;
        uint32 InfectedWoundTimer;
        uint32 ExplodeCorpseTimer;
        uint32 SpawnTimer;

        bool bAchiev;

        SummonList Summons;
        InstanceScript* instance;

        void Reset()
        {
            ConsumeTimer = 15*IN_MILLISECONDS;
            AuraCountTimer = 15500;
            CrushTimer = urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS);
            InfectedWoundTimer = urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            ExplodeCorpseTimer = 3*IN_MILLISECONDS;
            SpawnTimer = urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS);

            bAchiev = IsHeroic();

            Summons.DespawnAll();

            me->RemoveAura(DUNGEON_MODE(SPELL_CONSUME_AURA_5N, SPELL_CONSUME_AURA_5H));

            if (instance)
                instance->SetData(DATA_TROLLGORE_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SpawnTimer <= diff)
            {
                uint32 spawnNumber = urand(2, DUNGEON_MODE(3, 5));
                for (uint8 i = 0; i < spawnNumber; ++i)
                    DoSummon(RAND(NPC_DRAKKARI_INVADER_1, NPC_DRAKKARI_INVADER_2), AddSpawnPoint, 0, TEMPSUMMON_DEAD_DESPAWN);
                SpawnTimer = urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS);
            }
			else SpawnTimer -= diff;

            if (ConsumeTimer <= diff)
            {
                DoSendQuantumText(SAY_CONSUME, me);
                DoCastAOE(DUNGEON_MODE(SPELL_CONSUME_5N, SPELL_CONSUME_5H));
                ConsumeTimer = 15*IN_MILLISECONDS;
            }
			else ConsumeTimer -= diff;

            if (bAchiev)
            {
                Aura* ConsumeAura = me->GetAura(DUNGEON_MODE(SPELL_CONSUME_AURA_5N, SPELL_CONSUME_AURA_5H));
                if (ConsumeAura && ConsumeAura->GetStackAmount() > 9)
                    bAchiev = false;
            }

            if (CrushTimer <= diff)
            {
                DoCastVictim(SPELL_CRUSH);
                CrushTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
			else CrushTimer -= diff;

            if (InfectedWoundTimer <= diff)
            {
                DoCastVictim(SPELL_INFECTED_WOUND);
                InfectedWoundTimer = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS);
            }
			else InfectedWoundTimer -= diff;

            if (ExplodeCorpseTimer <= diff)
            {
                DoCast(DUNGEON_MODE(SPELL_CORPSE_EXPLODE_5N, SPELL_CORPSE_EXPLODE_5H));
                DoSendQuantumText(SAY_EXPLODE, me);
                ExplodeCorpseTimer = urand(15*IN_MILLISECONDS, 19*IN_MILLISECONDS);
            }
			else ExplodeCorpseTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            Summons.DespawnAll();

            if (instance)
            {
                if (bAchiev)
                    instance->DoCompleteAchievement(ACHIEVEMENT_CONSUMPTION_JUNCTION);
                instance->SetData(DATA_TROLLGORE_EVENT, DONE);
            }
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(SAY_KILL, me);
        }

        void JustSummoned(Creature* summon)
        {
            Summons.push_back(summon->GetGUID());
            if (summon->AI())
                summon->AI()->AttackStart(me);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_trollgoreAI(creature);
    }
};

void AddSC_boss_trollgore()
{
    new boss_trollgore();
}