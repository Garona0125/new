/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
#include "uldaman.h"

#define SAY_AGGRO     "Who dares awaken Archaedas? Who dares the wrath of the makers!"
#define SOUND_AGGRO   5855
#define SAY_SUMMON    "Awake ye servants, defend the discs!"
#define SOUND_SUMMON  5856
#define SAY_SUMMON2   "To my side, brothers. For the makers!"
#define SOUND_SUMMON2 5857
#define SAY_KILL      "Reckless mortal."
#define SOUND_KILL    5858

enum Spells
{
    SPELL_GROUND_TREMOR           = 6524,
    SPELL_ARCHAEDAS_AWAKEN        = 10347,
    SPELL_BOSS_OBJECT_VISUAL      = 11206,
    SPELL_BOSS_AGGRO              = 10340,
    SPELL_SUB_BOSS_AGGRO          = 11568,
    SPELL_AWAKEN_VAULT_WALKER     = 10258,
    SPELL_AWAKEN_EARTHEN_GUARDIAN = 10252,
	SPELL_SELF_DESTRUCT           = 9874,
};

class boss_archaedas : public CreatureScript
{
    public:
        boss_archaedas() : CreatureScript("boss_archaedas"){}

        struct boss_archaedasAI : public QuantumBasicAI
        {
            boss_archaedasAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            uint32 TremorTimer;
            int32 AwakenTimer;
            uint32 WallMinionTimer;

            bool WakingUp;
            bool GuardiansAwake;
            bool VaultWalkersAwake;

            InstanceScript* instance;

            void Reset()
            {
                TremorTimer = 60000;
                AwakenTimer = 0;
                WallMinionTimer = 10000;

                WakingUp = false;
                GuardiansAwake = false;
                VaultWalkersAwake = false;

				instance->SetData(0, 5);    // respawn any dead minions

                me->SetCurrentFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void ActivateMinion(uint64 Guid, bool flag)
            {
                Unit* minion = Unit::GetUnit(*me, Guid);

                if (minion && minion->IsAlive())
                {
					DoCast(minion, SPELL_AWAKEN_VAULT_WALKER, flag);
					minion->CastSpell(minion, SPELL_ARCHAEDAS_AWAKEN, true);
					minion->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					minion->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE);
					minion->SetCurrentFaction(16);
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetCurrentFaction(16);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
            {
                // Being woken up from the altar, start the awaken sequence
                if (spell->Id == SPELL_ARCHAEDAS_AWAKEN)
                {
                    me->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, SOUND_AGGRO);
                    AwakenTimer = 4000;
                    WakingUp = true;
                }
            }

            void KilledUnit(Unit* /*victim*/)
            {
                me->MonsterYell(SAY_KILL, LANG_UNIVERSAL, 0);
                DoPlaySoundToSet(me, SOUND_KILL);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
                    return;

                if (WakingUp && AwakenTimer >= 0)
                {
                    AwakenTimer -= diff;
                    return;
                }
				else if (WakingUp && AwakenTimer <= 0)
                {
                    WakingUp = false;
                    AttackStart(Unit::GetUnit(*me, instance->GetData64(0)));
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (WallMinionTimer <= diff)
                {
                    instance->SetData(DATA_MINIONS, IN_PROGRESS);
                    WallMinionTimer = 10000;
                }
				else WallMinionTimer -= diff;

                if (!GuardiansAwake && !HealthAbovePct(66))
                {
                    ActivateMinion(instance->GetData64(5), true);   // EarthenGuardian1
                    ActivateMinion(instance->GetData64(6), true);   // EarthenGuardian2
                    ActivateMinion(instance->GetData64(7), true);   // EarthenGuardian3
                    ActivateMinion(instance->GetData64(8), true);   // EarthenGuardian4
                    ActivateMinion(instance->GetData64(9), true);   // EarthenGuardian5
                    ActivateMinion(instance->GetData64(10), false); // EarthenGuardian6
                    me->MonsterYell(SAY_SUMMON, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, SOUND_SUMMON);
                    GuardiansAwake = true;
                }

                if (!VaultWalkersAwake && !HealthAbovePct(33))
                {
                    ActivateMinion(instance->GetData64(1), true);    // VaultWalker1
                    ActivateMinion(instance->GetData64(2), true);    // VaultWalker2
                    ActivateMinion(instance->GetData64(3), true);    // VaultWalker3
                    ActivateMinion(instance->GetData64(4), false);    // VaultWalker4
                    me->MonsterYell(SAY_SUMMON2, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, SOUND_SUMMON2);
                    VaultWalkersAwake = true;
                }

                if (TremorTimer <= diff)
                {
                    DoCastVictim(SPELL_GROUND_TREMOR);
                    TremorTimer  = 45000;
                }
				else TremorTimer  -= diff;

                DoMeleeAttackIfReady();
            }

            void JustDied (Unit* /*killer*/)
            {
				instance->SetData(DATA_ANCIENT_DOOR, DONE);
				instance->SetData(DATA_MINIONS, SPECIAL);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_archaedasAI(creature);
        }
};

class mob_archaedas_minions : public CreatureScript
{
    public:
		mob_archaedas_minions() : CreatureScript("mob_archaedas_minions"){}

        struct mob_archaedas_minionsAI : public QuantumBasicAI
        {
            mob_archaedas_minionsAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            uint32 ArcingTimer;
            int32 AwakenTimer;
            bool WakingUp;

            bool bAmIAwake;
            InstanceScript* instance;

            void Reset()
            {
                ArcingTimer = 3000;
                AwakenTimer = 0;

                WakingUp = false;
                bAmIAwake = false;

                me->SetCurrentFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->RemoveAllAuras();
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetCurrentFaction(16);
                me->RemoveAllAuras();
                me->RemoveFlag (UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag (UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                bAmIAwake = true;
            }

            void SpellHit (Unit* /*caster*/, const SpellInfo* spell) {
                // time to wake up, start animation
                if (spell == sSpellMgr->GetSpellInfo(SPELL_ARCHAEDAS_AWAKEN))
                {
                    AwakenTimer = 5000;
                    WakingUp = true;
                }
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (bAmIAwake)
                    QuantumBasicAI::MoveInLineOfSight(who);
            }

            void UpdateAI(const uint32 diff)
            {
                // we're still in the awaken animation
                if (WakingUp && AwakenTimer >= 0)
                {
                    AwakenTimer -= diff;
                    return;        // dont do anything until we are done
                }
				else if (WakingUp && AwakenTimer <= 0)
                {
                    WakingUp = false;
                    bAmIAwake = true;
                    // AttackStart(Unit::GetUnit(*me, instance->GetData64(0))); // whoWokeArchaedasGUID
                    return;     // dont want to continue until we finish the AttackStart method
                }

                //Return since we have no target
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_archaedas_minionsAI(creature);
        }
};

class mob_stonekeepers : public CreatureScript
{
    public:
        mob_stonekeepers() : CreatureScript("mob_stonekeepers"){}

        struct mob_stonekeepersAI : public QuantumBasicAI
        {
            mob_stonekeepersAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset()
            {
                me->SetCurrentFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->RemoveAllAuras();
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetCurrentFaction(16);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void UpdateAI(const uint32 /*diff*/)
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*attacker*/)
            {
                DoCast (me, SPELL_SELF_DESTRUCT, true);
                if (instance)
                    instance->SetData(DATA_STONE_KEEPERS, IN_PROGRESS);    // activate next stonekeeper
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_stonekeepersAI(creature);
        }
};

class go_altar_of_archaedas : public GameObjectScript
{
    public:
        go_altar_of_archaedas() : GameObjectScript("go_altar_of_archaedas"){}

        bool OnGossipHello(Player* player, GameObject* /*go*/)
        {
			InstanceScript* instance = player->GetInstanceScript();
            if (!instance)
                return false;

            player->CastSpell (player, SPELL_BOSS_OBJECT_VISUAL, false);
            instance->SetData64(0, player->GetGUID());     // activate archaedas
            return false;
		}
};

class go_altar_of_the_keepers : public GameObjectScript
{
    public:
        go_altar_of_the_keepers() : GameObjectScript("go_altar_of_the_keepers"){}

        bool OnGossipHello(Player* player, GameObject* /*go*/)
        {
            InstanceScript* instance = player->GetInstanceScript();
            if (!instance)
                return false;

            player->CastSpell (player, SPELL_BOSS_OBJECT_VISUAL, false);
            instance->SetData(DATA_STONE_KEEPERS, IN_PROGRESS); // activate the Stone Keepers
            return false;
        }
};

void AddSC_boss_archaedas()
{
    new boss_archaedas();
    new mob_archaedas_minions();
    new mob_stonekeepers();
    new go_altar_of_archaedas();
    new go_altar_of_the_keepers();
}