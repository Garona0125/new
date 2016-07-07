/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "the_underbog.h"

enum Spells
{
	SPELL_FOUL_SPORES = 31673,
	SPELL_ACID_GEYSER = 38739,
};

class boss_hungarfen : public CreatureScript
{
public:
    boss_hungarfen() : CreatureScript("boss_hungarfen") { }

    struct boss_hungarfenAI : public QuantumBasicAI
    {
        boss_hungarfenAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Root;
        uint32 MushroomTimer;
        uint32 AcidGeyserTimer;

        void Reset()
        {
            Root = false;
            MushroomTimer = 5000;
            AcidGeyserTimer = 10000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!HealthAbovePct(20))
            {
                if (!Root)
                {
                    DoCast(me, SPELL_FOUL_SPORES);
                    Root = true;
                }
            }

            if (MushroomTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    me->SummonCreature(17990, target->GetPositionX()+(rand()%8), target->GetPositionY()+(rand()%8), target->GetPositionZ(), float(rand()%5), TEMPSUMMON_TIMED_DESPAWN, 22000);
                else
                    me->SummonCreature(17990, me->GetPositionX()+(rand()%8), me->GetPositionY()+(rand()%8), me->GetPositionZ(), float(rand()%5), TEMPSUMMON_TIMED_DESPAWN, 22000);

                MushroomTimer = 10000;
            }
			else MushroomTimer -= diff;

            if (AcidGeyserTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_ACID_GEYSER);
					AcidGeyserTimer = 10000+rand()%7500;
				}
            }
			else AcidGeyserTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_hungarfenAI(creature);
    }
};

enum MushroomSpells
{
	SPELL_SPORE_CLOUD     = 34168,
	SPELL_PUTRID_MUSHROOM = 31690,
	SPELL_GROW            = 31698,
};

class npc_underbog_mushroom : public CreatureScript
{
public:
    npc_underbog_mushroom() : CreatureScript("npc_underbog_mushroom") { }

    struct npc_underbog_mushroomAI : public QuantumBasicAI
    {
        npc_underbog_mushroomAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Stop;
        uint32 GrowTimer;
        uint32 ShrinkTimer;

        void Reset()
        {
            Stop = false;
            GrowTimer = 0;
            ShrinkTimer = 20000;

            DoCast(me, SPELL_PUTRID_MUSHROOM, true);
            DoCast(me, SPELL_SPORE_CLOUD, true);
        }

        void MoveInLineOfSight(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (Stop)
                return;

            if (GrowTimer <= diff)
            {
                DoCast(me, SPELL_GROW);
                GrowTimer = 3000;
            }
			else GrowTimer -= diff;

            if (ShrinkTimer <= diff)
            {
                me->RemoveAurasDueToSpell(SPELL_GROW);
                Stop = true;
            }
			else ShrinkTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_underbog_mushroomAI(creature);
    }
};

void AddSC_boss_hungarfen()
{
    new boss_hungarfen();
    new npc_underbog_mushroom();
}