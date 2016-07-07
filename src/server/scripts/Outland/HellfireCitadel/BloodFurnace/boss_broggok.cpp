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
#include "blood_furnace.h"

enum Texts
{
	SAY_AGGRO                   = -1542008,
};

enum Spells
{
	SPELL_SLIME_SPRAY_5N        = 30913,
	SPELL_SLIME_SPRAY_5H        = 38458,
    SPELL_POISON_BOLT_VOLLEY_5N = 30917,
	SPELL_POISON_BOLT_VOLLEY_5H = 38459,
	SPELL_POISON_CLOUD_PASSIVE  = 30914,
    SPELL_SUMMON_POISON_CLOUD   = 30916,
};

class boss_broggok : public CreatureScript
{
    public:
		boss_broggok() : CreatureScript("boss_broggok") { }

        struct boss_broggokAI : public QuantumBasicAI
        {
            boss_broggokAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
			SummonList Summons;

            uint32 AcidSprayTimer;
			uint32 PoisonBoltVolleyTimer;
            uint32 PoisonCloudSummonTimer;

            void Reset()
            {
                PoisonCloudSummonTimer = 2000;
                PoisonBoltVolleyTimer = 4000;
				AcidSprayTimer = 6000;

				Summons.DespawnAll();

				instance->SetData(TYPE_BROGGOK_EVENT, NOT_STARTED);
				instance->HandleGameObject(instance->GetData64(DATA_DOOR_4), true);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);

				instance->SetData(TYPE_BROGGOK_EVENT, IN_PROGRESS);
				instance->HandleGameObject(instance->GetData64(DATA_DOOR_4), false);
				instance->HandleGameObject(instance->GetData64(DATA_DOOR_5), false);
            }

            void JustSummoned(Creature* summon)
            {
				if (summon->GetEntry() == NPC_BROGGOK_POISON_CLOUD)
					Summons.Summon(summon);
            }

			void JustDied(Unit* /*who*/)
            {
				instance->HandleGameObject(instance->GetData64(DATA_DOOR_4), true);
				instance->HandleGameObject(instance->GetData64(DATA_DOOR_5), true);
				instance->SetData(TYPE_BROGGOK_EVENT, DONE);

				Summons.DespawnAll();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (AcidSprayTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SLIME_SPRAY_5N, SPELL_SLIME_SPRAY_5H));
						AcidSprayTimer = urand(4000, 5000);
					}
                }
                else AcidSprayTimer -=diff;

                if (PoisonBoltVolleyTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_POISON_BOLT_VOLLEY_5N, SPELL_POISON_BOLT_VOLLEY_5H));
						PoisonBoltVolleyTimer = urand(6000, 7000);
					}
                }
                else PoisonBoltVolleyTimer -=diff;

                if (PoisonCloudSummonTimer <= diff)
                {
                    DoCast(me, SPELL_SUMMON_POISON_CLOUD, true);
                    PoisonCloudSummonTimer = urand(8000, 9000);
                }
                else PoisonCloudSummonTimer -=diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_broggokAI(creature);
        }
};

class npc_broggok_poison_cloud : public CreatureScript
{
public:
	npc_broggok_poison_cloud() : CreatureScript("npc_broggok_poison_cloud") { }

	struct npc_broggok_poison_cloudAI : public QuantumBasicAI
	{
		npc_broggok_poison_cloudAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
			me->SetReactState(REACT_PASSIVE);
		}

		void Reset()
		{
			DoCast(me, SPELL_POISON_CLOUD_PASSIVE, true);
		}

		void UpdateAI(uint32 const /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_broggok_poison_cloudAI(creature);
	}
};

class go_broggok_lever : public GameObjectScript
{
public:
    go_broggok_lever() : GameObjectScript("go_broggok_lever") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        if (InstanceScript* instance = go->GetInstanceScript())
        {
            go->SummonCreature(NPC_LAUGHING_SKULL_WARDEN, 464.566986f, 54.492401f, 9.614540f, 1.621839f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            go->SummonCreature(NPC_LAUGHING_SKULL_WARDEN, 447.079010f, 52.09660f, 9.617910f, 1.127570f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            go->SummonCreature(NPC_FEL_ORC_NEOPHYTE, 449.192993f, 52.041500f, 9.620020f, 1.257160f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            go->SummonCreature(NPC_FEL_ORC_NEOPHYTE, 461.744995f, 51.934700f, 9.620450f, 1.736250f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            instance->HandleGameObject(instance->GetData64(DATA_DOOR_5), true);
        }
		return true;
    }
};

void AddSC_boss_broggok()
{
    new boss_broggok();
	new npc_broggok_poison_cloud();
	new go_broggok_lever();
}