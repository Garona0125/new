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
#include "the_slave_pens.h"

enum Says
{
	SAY_AGGRO = 0,
	SAY_KILL  = 1,
	SAY_DEATH = 2,
};

enum Spells
{
	SPELL_TAINTED_EARTHGRAB_TOTEM = 31981,
	SPELL_TAINTED_STONESKIN_TOTEM = 31985,
	SPELL_CORRUPTED_NOVA_TOTEM    = 31991,
	SPELL_LIGHTNING_BOLT_5N       = 35010,
	SPELL_LIGHTNING_BOLT_5H       = 38465,
	SPELL_MENNUS_HEALING_WARD     = 34980,
	SPELL_EARTHGRAB               = 31983,
	SPELL_STONESKIN               = 31986,
	SPELL_EXPLODE                 = 30687,
};

class boss_mennu_the_betrayer : public CreatureScript
{
public:
	boss_mennu_the_betrayer() : CreatureScript("boss_mennu_the_betrayer") {}

    struct boss_mennu_the_betrayerAI : public QuantumBasicAI
    {
		boss_mennu_the_betrayerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 TotemPlaceTimer;
		uint32 LightningBoltTimer;
		uint32 HealingWardTimer;

		SummonList Summons;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			TotemPlaceTimer = 2000;
			LightningBoltTimer = 3000;
			HealingWardTimer = 8000;

			Summons.DespawnAll();
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_AGGRO);
		}

		void JustDied(Unit* /*killer*/)
		{
			Talk(SAY_DEATH);

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
				case NPC_EARTHGRAB_TOTEM:
				case NPC_STONESKIN_TOTEM:
				case NPC_CORRUPTED_NOVA_TOTEM:
				case NPC_HEALING_WARD:
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
					break;
				default:
					break;
			}
		}

		void KilledUnit(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				Talk(SAY_KILL);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TotemPlaceTimer <= diff)
			{
				Summons.DespawnAll();

				DoCast(me, SPELL_TAINTED_EARTHGRAB_TOTEM, true);
				DoCast(me, SPELL_TAINTED_STONESKIN_TOTEM, true);
				DoCast(me, SPELL_CORRUPTED_NOVA_TOTEM, true);

				TotemPlaceTimer = 25000;
			}
			else TotemPlaceTimer -= diff;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (HealingWardTimer <= diff)
			{
				DoCast(me, SPELL_MENNUS_HEALING_WARD, true);
				HealingWardTimer = 30000;
			}
			else HealingWardTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_mennu_the_betrayerAI(creature);
    }
};

class npc_tainted_earthgrab_totem : public CreatureScript
{
public:
    npc_tainted_earthgrab_totem() : CreatureScript("npc_tainted_earthgrab_totem") {}

    struct npc_tainted_earthgrab_totemAI : public QuantumBasicAI
    {
        npc_tainted_earthgrab_totemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 EarthgrabTimer;

        void Reset(){}

		void EnterToBattle(Unit* /*who*/)
		{
			EarthgrabTimer = 1000;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (EarthgrabTimer <= diff)
			{
				DoCastAOE(SPELL_EARTHGRAB);
				EarthgrabTimer = 3000;
			}
			else EarthgrabTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tainted_earthgrab_totemAI(creature);
    }
};

class npc_tainted_stoneskin_totem : public CreatureScript
{
public:
    npc_tainted_stoneskin_totem() : CreatureScript("npc_tainted_stoneskin_totem") {}

    struct npc_tainted_stoneskin_totemAI : public QuantumBasicAI
    {
        npc_tainted_stoneskin_totemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 StoneskinAuraTimer;

        void Reset(){}

		void EnterToBattle(Unit* /*who*/)
		{
			StoneskinAuraTimer = 1000;
		}

		void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (StoneskinAuraTimer <= diff)
			{
				DoCastAOE(SPELL_STONESKIN);
				StoneskinAuraTimer = 3000;
			}
			else StoneskinAuraTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tainted_stoneskin_totemAI(creature);
    }
};

class npc_corrupted_nova_totem : public CreatureScript
{
public:
    npc_corrupted_nova_totem() : CreatureScript("npc_corrupted_nova_totem") {}

    struct npc_corrupted_nova_totemAI : public QuantumBasicAI
    {
        npc_corrupted_nova_totemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 ExplodeTimer;

        void Reset(){}

		void EnterToBattle(Unit* /*who*/)
		{
			ExplodeTimer = 1000;
		}

		void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ExplodeTimer <= diff)
			{
				DoCastVictim(SPELL_EXPLODE);
				ExplodeTimer = 10000;
			}
			else ExplodeTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_corrupted_nova_totemAI(creature);
    }
};

void AddSC_boss_mennu_the_betrayer()
{
	new boss_mennu_the_betrayer();
	new npc_tainted_earthgrab_totem();
	new npc_tainted_stoneskin_totem();
	new npc_corrupted_nova_totem();
}