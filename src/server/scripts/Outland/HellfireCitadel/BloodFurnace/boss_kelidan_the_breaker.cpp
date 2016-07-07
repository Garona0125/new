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
    SAY_WAKE                    = -1542000,
    SAY_ADD_AGGRO_1             = -1542001,
    SAY_ADD_AGGRO_2             = -1542002,
    SAY_ADD_AGGRO_3             = -1542003,
    SAY_KILL_1                  = -1542004,
    SAY_KILL_2                  = -1542005,
    SAY_NOVA                    = -1542006,
    SAY_DIE                     = -1542007,
};

enum Spells
{
    SPELL_CORRUPTION            = 30938,
    SPELL_EVOCATION             = 30935,
    SPELL_FIRE_NOVA_5N          = 33132,
    SPELL_FIRE_NOVA_5H          = 37371,
    SPELL_SHADOW_BOLT_VOLLEY_5N = 28599,
    SPELL_SHADOW_BOLT_VOLLEY_5H = 40070,
    SPELL_BURNING_NOVA          = 30940,
    SPELL_VORTEX                = 37370,
	SPELL_SHADOW_BOLT_5N        = 12739,
    SPELL_SHADOW_BOLT_5H        = 15472,
    SPELL_MARK_OF_SHADOW        = 30937,
    SPELL_CHANNELING            = 39123,
};

const float ShadowmoonChannelers[5][4] =
{
    {302.0f, -87.0f, -24.4f, 0.157f},
    {321.0f, -63.5f, -24.6f, 4.887f},
    {346.0f, -74.5f, -24.6f, 3.595f},
    {344.0f, -103.5f, -24.5f, 2.356f},
    {316.0f, -109.0f, -24.6f, 1.257f},
};

class boss_kelidan_the_breaker : public CreatureScript
{
    public:
        boss_kelidan_the_breaker(): CreatureScript("boss_kelidan_the_breaker") { }

        struct boss_kelidan_the_breakerAI : public QuantumBasicAI
        {
            boss_kelidan_the_breakerAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

                for (uint8 i=0; i<5; ++i)
					Channelers[i] = 0;
            }

            InstanceScript* instance;

            uint32 ShadowVolleyTimer;
            uint32 BurningNovaTimer;
            uint32 FirenovaTimer;
            uint32 CorruptionTimer;
            uint32 CheckTimer;

            bool Firenova;
            bool addYell;

            uint64 Channelers[5];

            void Reset()
            {
                ShadowVolleyTimer = 500;
				CorruptionTimer = 5000;
                BurningNovaTimer = 10000;

                CheckTimer = 0;
                Firenova = false;
                addYell = false;

                SummonChannelers();

				instance->SetData(TYPE_KELIDAN_THE_BREAKER_EVENT, NOT_STARTED);
            }

            void EnterToBattle(Unit* who)
            {
                DoSendQuantumText(SAY_WAKE, me);

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(true);

                DoStartMovement(who);

				instance->SetData(TYPE_KELIDAN_THE_BREAKER_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                if (rand()%2)
                    return;

                DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void ChannelerEngaged(Unit* who)
            {
                if (who && !addYell)
                {
                    addYell = true;
                    DoSendQuantumText(RAND(SAY_ADD_AGGRO_1, SAY_ADD_AGGRO_2, SAY_ADD_AGGRO_3), me);
                }
                for (uint8 i=0; i<5; ++i)
                {
                    Creature* channeler = Unit::GetCreature(*me, Channelers[i]);

                    if (who && channeler && !channeler->IsInCombatActive())
                        channeler->AI()->AttackStart(who);
                }
            }

            void ChannelerDied(Unit* killer)
            {
                for (uint8 i=0; i<5; ++i)
                {
                    Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
                    if (channeler && channeler->IsAlive())
                        return;
                }

                if (killer)
                    me->AI()->AttackStart(killer);
            }

            uint64 GetChanneled(Creature* channeler1)
            {
                SummonChannelers();
                if (!channeler1) return 0;
                uint8 i;
                for (i=0; i<5; ++i)
                {
                    Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
                    if (channeler && channeler->GetGUID() == channeler1->GetGUID())
                        break;
                }
                return Channelers[(i+2)%5];
            }

            void SummonChannelers()
            {
                for (uint8 i=0; i<5; ++i)
                {
                    Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
                    if (!channeler || channeler->IsDead())
                        channeler = me->SummonCreature(NPC_CHANNELER, ShadowmoonChannelers[i][0], ShadowmoonChannelers[i][1], ShadowmoonChannelers[i][2], ShadowmoonChannelers[i][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000);
                    if (channeler)
                        Channelers[i] = channeler->GetGUID();
                    else
                        Channelers[i] = 0;
                }
            }

            void JustDied(Unit* /*Killer*/)
            {
                DoSendQuantumText(SAY_DIE, me);

                instance->SetData(TYPE_KELIDAN_THE_BREAKER_EVENT, DONE);

                instance->HandleGameObject(instance->GetData64(DATA_DOOR_1), true);
                instance->HandleGameObject(instance->GetData64(DATA_DOOR_6), true);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                {
                    if (CheckTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                            DoCast(me, SPELL_EVOCATION);

                        CheckTimer = 5000;
                    }
                    else CheckTimer -= diff;
                    return;
                }

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (Firenova)
                {
                    if (FirenovaTimer <= diff)
                    {
                        DoCastAOE(DUNGEON_MODE(SPELL_FIRE_NOVA_5N, SPELL_FIRE_NOVA_5N), true);
                        Firenova = false;
                        ShadowVolleyTimer = 2000;
                    }
                    else FirenovaTimer -= diff;
                    return;
                }

                if (ShadowVolleyTimer <= diff)
                {
                    DoCastAOE(DUNGEON_MODE(SPELL_SHADOW_BOLT_VOLLEY_5N, SPELL_SHADOW_BOLT_VOLLEY_5H), true);
                    ShadowVolleyTimer = 5000+rand()%8000;
                }
                else ShadowVolleyTimer -= diff;

                if (CorruptionTimer <= diff)
                {
                    DoCastAOE(SPELL_CORRUPTION);
                    CorruptionTimer = 30000+rand()%20000;
                }
                else CorruptionTimer -= diff;

                if (BurningNovaTimer <= diff)
                {
                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(true);

                    DoSendQuantumText(SAY_NOVA, me);

                    if (SpellInfo const* nova = sSpellMgr->GetSpellInfo(SPELL_BURNING_NOVA))
                    {
                        if (Aura* aura = Aura::TryRefreshStackOrCreate(nova, MAX_EFFECT_MASK, me, me))
                            aura->ApplyForTargets();
                    }

                    if (IsHeroic())
                        DoTeleportAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                    BurningNovaTimer = 20000+rand()%8000;
                    FirenovaTimer= 5000;
                    Firenova = true;
                }
                else BurningNovaTimer -= diff;

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_kelidan_the_breakerAI(creature);
        }
};

class npc_shadowmoon_channeler : public CreatureScript
{
    public:
		npc_shadowmoon_channeler() : CreatureScript("npc_shadowmoon_channeler") { }

        struct npc_shadowmoon_channelerAI : public QuantumBasicAI
        {
            npc_shadowmoon_channelerAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 ShadowBoltTimer;
            uint32 MarkOfShadowTimer;
            uint32 CheckTimer;

            void Reset()
            {
                ShadowBoltTimer = 500;
                MarkOfShadowTimer = 4000;
                CheckTimer = 0;

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(true);
            }

            void EnterToBattle(Unit* who)
            {
                if (Creature* Kelidan = me->FindCreatureWithDistance(NPC_KELIDAN, 100.0f))
                    CAST_AI(boss_kelidan_the_breaker::boss_kelidan_the_breakerAI, Kelidan->AI())->ChannelerEngaged(who);

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(true);

                DoStartMovement(who);
            }

            void JustDied(Unit* Killer)
            {
               if (Creature* Kelidan = me->FindCreatureWithDistance(NPC_KELIDAN, 100.0f))
                   CAST_AI(boss_kelidan_the_breaker::boss_kelidan_the_breakerAI, Kelidan->AI())->ChannelerDied(Killer);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                {
                    if (CheckTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
						{
                            if (Creature* Kelidan = me->FindCreatureWithDistance(NPC_KELIDAN, 100.0f))
                            {
								uint64 channeler = CAST_AI(boss_kelidan_the_breaker::boss_kelidan_the_breakerAI, Kelidan->AI())->GetChanneled(me);
                                if (Unit* channeled = Unit::GetUnit(*me, channeler))
								{
                                    DoCast(channeled, SPELL_CHANNELING);
								}
                            }
							CheckTimer = 5000;
						}
                    }
                    else CheckTimer -= diff;
                    return;
                }

				if (ShadowBoltTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
						ShadowBoltTimer = urand(3000, 4000);
					}
                }
                else ShadowBoltTimer -= diff;

                if (MarkOfShadowTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_MARK_OF_SHADOW);
						MarkOfShadowTimer = urand(7000, 8000);
					}
                }
                else MarkOfShadowTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_shadowmoon_channelerAI(creature);
        }
};

void AddSC_boss_kelidan_the_breaker()
{
    new boss_kelidan_the_breaker();
    new npc_shadowmoon_channeler();
}