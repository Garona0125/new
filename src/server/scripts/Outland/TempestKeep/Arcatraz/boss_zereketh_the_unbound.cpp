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
#include "arcatraz.h"

enum Texts
{
	SAY_AGGRO                   = -1562006,
	SAY_KILL_1                  = -1562007,
	SAY_KILL_2                  = -1562008,
	SAY_SHADOW_NOVA_1           = -1562009,
	SAY_SHADOW_NOVA_2           = -1562010,
	SAY_DEATH                   = -1562011,
};

enum Spells
{
	SPELL_SEED_OF_CORRUPTION_5N = 36123,
	SPELL_SEED_OF_CORRUPTION_5H = 39367,
	SPELL_SHADOW_NOVA_5N        = 36127,
	SPELL_SHADOW_NOVA_5H        = 39005,
	SPELL_VOID_ZONE             = 36119,
	SPELL_CONSUMPTION           = 36120,
};

class boss_zereketh_the_unbound : public CreatureScript
{
public:
    boss_zereketh_the_unbound() : CreatureScript("boss_zereketh_the_unbound") {}

    struct boss_zereketh_the_unboundAI : public QuantumBasicAI
    {
        boss_zereketh_the_unboundAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 SeedOfCorruptionTimer;
		uint32 ShadowNovaTimer;
		uint32 VoidZoneTimer;

		SummonList Summons;

        void Reset()
        {
			SeedOfCorruptionTimer = 2000;
			ShadowNovaTimer = 4000;
			VoidZoneTimer = 6000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_AGGRO, me);
		}

		void KilledUnit(Unit* victim)
		{
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoSendQuantumText(SAY_DEATH, me);

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_VOID_ZONE)
				Summons.Summon(summon);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SeedOfCorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAND(SPELL_SEED_OF_CORRUPTION_5N, SPELL_SEED_OF_CORRUPTION_5H));
					SeedOfCorruptionTimer = urand(3000, 4000);
				}
			}
			else SeedOfCorruptionTimer -= diff;

			if (ShadowNovaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoSendQuantumText(RAND(SAY_SHADOW_NOVA_1, SAY_SHADOW_NOVA_2), me);
					DoCast(target, RAND(SPELL_SHADOW_NOVA_5N, SPELL_SHADOW_NOVA_5H));
					ShadowNovaTimer = urand(6000, 7000);
				}
			}
			else ShadowNovaTimer -= diff;

			if (VoidZoneTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VOID_ZONE);
					VoidZoneTimer = urand(9000, 10000);
				}
			}
			else VoidZoneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_zereketh_the_unboundAI(creature);
    }
};

class npc_unbound_void_zone : public CreatureScript
{
public:
	npc_unbound_void_zone() : CreatureScript("npc_unbound_void_zone") {}

	struct npc_unbound_void_zoneAI : public QuantumBasicAI
	{
		npc_unbound_void_zoneAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		void Reset()
		{
			DoCastAOE(SPELL_CONSUMPTION);
		}

		void EnterToBattle(Unit* /*who*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_unbound_void_zoneAI(creature);
	}
};

void AddSC_boss_zereketh_the_unbound()
{
	new boss_zereketh_the_unbound();
	new npc_unbound_void_zone();
}