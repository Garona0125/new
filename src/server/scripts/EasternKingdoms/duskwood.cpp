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

enum TwilightCorrupter
{
	SAY_CORRUPTOR_SPAWN         = 0,
    SAY_CORRUPTOR_AGGRO         = 1,
    SAY_CORRUPTOR_SLAY          = 2,

	SPELL_SOUL_CORRUPTION       = 25805,
	SPELL_CREATURE_OF_NIGHTMARE = 25806,
	SPELL_LEVEL_UP              = 24312,

	NPC_TWILIGHT_CORRUPTOR      = 15625,

	ITEM_FRAGMENT               = 21149,
};

class boss_twilight_corrupter : public CreatureScript
{
public:
    boss_twilight_corrupter() : CreatureScript("boss_twilight_corrupter") { }

    struct boss_twilight_corrupterAI : public QuantumBasicAI
    {
        boss_twilight_corrupterAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SoulCorruptionTimer;
        uint32 CreatureOfNightmareTimer;
        uint8 KillCount;

        void Reset()
        {
            SoulCorruptionTimer = 15000;
            CreatureOfNightmareTimer = 30000;
            KillCount = 0;
        }
        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_CORRUPTOR_AGGRO);
        }

        void KilledUnit(Unit* who)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                ++KillCount;
				Talk(SAY_CORRUPTOR_SLAY, who->GetGUID());

                if (KillCount == 3)
                {
                    DoCast(me, SPELL_LEVEL_UP, true);
                    KillCount = 0;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SoulCorruptionTimer <= diff)
            {
                DoCastVictim(SPELL_SOUL_CORRUPTION);
                SoulCorruptionTimer = rand()%4000+15000;
            }
			else SoulCorruptionTimer -= diff;

            if (CreatureOfNightmareTimer <= diff)
            {
                DoCastVictim(SPELL_CREATURE_OF_NIGHTMARE);
                CreatureOfNightmareTimer = 45000;
            }
			else CreatureOfNightmareTimer -= diff;

			DoMeleeAttackIfReady();
        };
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_twilight_corrupterAI(creature);
    }
};

class at_twilight_grove : public AreaTriggerScript
{
public:
    at_twilight_grove() : AreaTriggerScript("at_twilight_grove") { }

    bool OnTrigger(Player* player, const AreaTriggerEntry* /*triggerId*/)
    {
        if (player->HasQuestForItem(ITEM_FRAGMENT) && !player->FindCreatureWithDistance(NPC_TWILIGHT_CORRUPTOR, 500.0f))
        {
            if (Creature* corrupter = player->SummonCreature(NPC_TWILIGHT_CORRUPTOR, -10328.16f, -489.57f, 49.95f, 0, TEMPSUMMON_MANUAL_DESPAWN, 60000))
            {
                corrupter->SetCurrentFaction(16);
                corrupter->SetMaxHealth(832750);
            }

            if (Creature* speaker = player->SummonCreature(1, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()-1, 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
            {
                speaker->SetName("Twilight Corrupter");
                speaker->SetVisible(true);
                speaker->AI()->Talk(SAY_CORRUPTOR_SPAWN, player->GetGUID());
            }
        }
        return false;
    };
};

enum MorbentFel
{
	SPELL_TOUCH_OF_DEATH     = 3108,
	SPELL_PRESENCE_OF_DEATH  = 3109,
	SPELL_UNHOLY_SHIELD      = 8909,
	SPELL_SACRED_CLEANSING   = 8913,

	NPC_WEAKENED_MORBENT_FEL = 24782,
};

class npc_morbent_fel : public CreatureScript
{
public:
    npc_morbent_fel() : CreatureScript("npc_morbent_fel") {}

    struct npc_morbent_felAI : public QuantumBasicAI
    {
        npc_morbent_felAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 UnholyShieldTimer;
		uint32 TouchOfDeathTimer;
		uint32 PresenceOfDeathTimer;

        void Reset()
		{
			UnholyShieldTimer = 100;
			TouchOfDeathTimer = 500;
			PresenceOfDeathTimer = 2500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void DamageTaken(Unit* DoneBy, uint32 &damage)
        {
            if (damage > me->GetHealth() || me->HealthBelowPctDamaged(15.0f, damage))
            {
                damage = 0;

                if (DoneBy->GetTypeId() == TYPE_ID_PLAYER)
					me->RemoveAurasDueToSpell(SPELL_UNHOLY_SHIELD);
            }
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_SACRED_CLEANSING)
				me->UpdateEntry(NPC_WEAKENED_MORBENT_FEL);
        }

        void UpdateAI(const uint32 diff)
        {
			// Out of combat timer
			if (UnholyShieldTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_UNHOLY_SHIELD);
				UnholyShieldTimer = 120000; // 2 minutes
			}
			else UnholyShieldTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TouchOfDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TOUCH_OF_DEATH);
					TouchOfDeathTimer = urand(3000, 4000);
				}
			}
			else TouchOfDeathTimer -= diff;

			if (PresenceOfDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PRESENCE_OF_DEATH);
					PresenceOfDeathTimer = urand(5000, 6000);
				}
			}
			else PresenceOfDeathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_morbent_felAI(creature);
    }
};

void AddSC_duskwood()
{
    new boss_twilight_corrupter();
    new at_twilight_grove();
	new npc_morbent_fel();
}