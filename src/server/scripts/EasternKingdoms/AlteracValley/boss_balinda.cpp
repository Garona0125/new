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

enum Spells
{
    SPELL_ARCANE_EXPLOSION = 46608,
    SPELL_CONE_OF_COLD     = 38384,
    SPELL_FIREBALL         = 46988,
    SPELL_FROSTBOLT        = 46987
};

enum Yells
{
	YELL_AGGRO             = 0,
	YELL_EVADE             = 1,
	YELL_SALVATION         = 2,
};

enum Creatures
{
    NPC_GREATER_WATER_ELEMENTAL = 25040,
};

enum WaterElementalSpells
{
    SPELL_WATERBOLT = 46983,
};

class npc_greater_water_elemental : public CreatureScript
{
public:
    npc_greater_water_elemental() : CreatureScript("npc_greater_water_elemental") { }

    struct npc_greater_water_elementalAI : public QuantumBasicAI
    {
        npc_greater_water_elementalAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WaterBoltTimer;
        uint64 BalindaGUID;
        uint32 ResetTimer;

        void Reset()
        {
            WaterBoltTimer = 3*IN_MILLISECONDS;
            ResetTimer = 5*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WaterBoltTimer <= diff)
            {
                DoCastVictim(SPELL_WATERBOLT);
                WaterBoltTimer = 5*IN_MILLISECONDS;
            }
			else WaterBoltTimer -= diff;

            // check if creature is not outside of building
            if (ResetTimer <= diff)
            {
                if (Creature* balinda = Unit::GetCreature(*me, BalindaGUID))
                    if (me->GetDistance2d(balinda->GetHomePosition().GetPositionX(), balinda->GetHomePosition().GetPositionY()) > 50)
                        EnterEvadeMode();
                    ResetTimer = 5*IN_MILLISECONDS;
            }
			else ResetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_water_elementalAI(creature);
    }
};

class boss_balinda : public CreatureScript
{
public:
    boss_balinda() : CreatureScript("boss_balinda") { }

    struct boss_balindaAI : public QuantumBasicAI
    {
        boss_balindaAI(Creature* creature) : QuantumBasicAI(creature), summons(me) { }

        uint32 ArcaneExplosionTimer;
        uint32 ConeOfColdTimer;
        uint32 FireBoltTimer;
        uint32 FrostboltTimer;
        uint32 ResetTimer;
        uint32 WaterElementalTimer;

        SummonList summons;

        void Reset()
        {
			ArcaneExplosionTimer = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            ConeOfColdTimer = 8*IN_MILLISECONDS;
            FireBoltTimer = 1*IN_MILLISECONDS;
            FrostboltTimer = 4*IN_MILLISECONDS;
            ResetTimer = 5*IN_MILLISECONDS;
            WaterElementalTimer = 0;
            summons.DespawnAll();
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(YELL_AGGRO);
        }

        void JustRespawned()
        {
            Reset();
        }

        void JustSummoned(Creature* summoned)
        {
            CAST_AI(npc_greater_water_elemental::npc_greater_water_elementalAI, summoned->AI())->BalindaGUID = me->GetGUID();
            summoned->AI()->AttackStart(SelectTarget(TARGET_RANDOM, 0, 50, true));
            summoned->SetCurrentFaction(me->GetFaction());
            summons.Summon(summoned);
        }

        void JustDied(Unit* /*killer*/)
        {
            summons.DespawnAll();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WaterElementalTimer <= diff)
            {
                if (summons.empty())
                    me->SummonCreature(NPC_GREATER_WATER_ELEMENTAL, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45*IN_MILLISECONDS);
                WaterElementalTimer = 50*IN_MILLISECONDS;
            }
			else WaterElementalTimer -= diff;

            if (ArcaneExplosionTimer <= diff)
            {
                DoCastVictim(SPELL_ARCANE_EXPLOSION);
                ArcaneExplosionTimer =  urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
			else ArcaneExplosionTimer -= diff;

            if (ConeOfColdTimer <= diff)
            {
                DoCastVictim(SPELL_CONE_OF_COLD);
                ConeOfColdTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            }
			else ConeOfColdTimer -= diff;

            if (FireBoltTimer <= diff)
            {
                DoCastVictim(SPELL_FIREBALL);
                FireBoltTimer = urand(5*IN_MILLISECONDS, 9*IN_MILLISECONDS);
            }
			else FireBoltTimer -= diff;

            if (FrostboltTimer <= diff)
            {
                DoCastVictim(SPELL_FROSTBOLT);
                FrostboltTimer = urand(4*IN_MILLISECONDS, 12*IN_MILLISECONDS);
            }
			else FrostboltTimer -= diff;

            if (ResetTimer <= diff)
            {
                if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
                {
                    EnterEvadeMode();
					Talk(YELL_EVADE);
                }
                ResetTimer = 5*IN_MILLISECONDS;
            }
			else ResetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_balindaAI(creature);
    }
};

void AddSC_boss_balinda()
{
    new boss_balinda();
    new npc_greater_water_elemental();
}