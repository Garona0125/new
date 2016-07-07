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
#include "SpellScript.h"
#include "vault_of_archavon.h"

enum BossEmotes
{
    EMOTE_OVERCHARGE     = -1590000,
    EMOTE_MINION_RESPAWN = -1590001,
    EMOTE_BERSERK        = -1590002,
};

enum Spells
{
    SPELL_OVERCHARGE          = 64218,
    SPELL_BERSERK             = 26662,
    SPELL_SHOCK               = 64363,
    SPELL_OVERCHARGED         = 64217,
    SPELL_OVERCHARGED_BLAST   = 64219,
    SPELL_WARDER_OVERCHARGE   = 64379,
	SPELL_CHAIN_LIGHTNING_10N = 64213,
	SPELL_CHAIN_LIGHTNING_25N = 64215,
	SPELL_LIGHTNING_NOVA_10N  = 64216,
	SPELL_LIGHTNING_NOVA_25N  = 65279,
};

enum Events
{
    EVENT_CHAIN_LIGHTNING   = 1,
    EVENT_LIGHTNING_NOVA    = 2,
    EVENT_OVERCHARGE        = 3,
    EVENT_BERSERK           = 4,
    EVENT_SHOCK             = 5,
};

#define MAX_TEMPEST_MINIONS         4

struct Position TempestMinions[MAX_TEMPEST_MINIONS] =
{
    {-203.980103f, -281.287720f, 91.650223f, 1.598807f},
    {-233.489410f, -281.139282f, 91.652412f, 1.598807f},
    {-233.267578f, -297.104645f, 91.681915f, 1.598807f},
    {-203.842529f, -297.097015f, 91.745163f, 1.598807f}
};

class boss_emalon_the_storm_watcher : public CreatureScript
{
    public:
        boss_emalon_the_storm_watcher() : CreatureScript("boss_emalon_the_storm_watcher") { }

        struct boss_emalon_the_storm_watcherAI : public BossAI
        {
            boss_emalon_the_storm_watcherAI(Creature* creature) : BossAI(creature, DATA_EMALON){}

            void Reset()
            {
                _Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                for (uint8 i = 0; i < MAX_TEMPEST_MINIONS; ++i)
                    me->SummonCreature(NPC_TEMPEST_WARDER, TempestMinions[i], TEMPSUMMON_CORPSE_DESPAWN, 0);
            }

            void JustSummoned(Creature* summoned)
            {
                BossAI::JustSummoned(summoned);

                if (summoned->AI())
                    summoned->AI()->AttackStart(me->GetVictim());
            }

            void EnterToBattle(Unit* who)
            {
				_EnterToBattle();

                if (!summons.empty())
                {
                    for (std::list<uint64>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    {
                        Creature* minion = Unit::GetCreature(*me, *itr);
                        if (minion && minion->IsAlive() && !minion->GetVictim() && minion->AI())
                            minion->AI()->AttackStart(who);
                    }
                }

                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 5000);
                events.ScheduleEvent(EVENT_LIGHTNING_NOVA, 40000);
                events.ScheduleEvent(EVENT_BERSERK, 360000);
                events.ScheduleEvent(EVENT_OVERCHARGE, 45000);
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHAIN_LIGHTNING:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_CHAIN_LIGHTNING_10N, SPELL_CHAIN_LIGHTNING_25N));

                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(10000, 12000));
                            break;
                        case EVENT_LIGHTNING_NOVA:
							DoCastAOE(RAID_MODE(SPELL_LIGHTNING_NOVA_10N, SPELL_LIGHTNING_NOVA_25N));
                            events.ScheduleEvent(EVENT_LIGHTNING_NOVA, 40000);
                            break;
                        case EVENT_OVERCHARGE:
                            DoCast(SPELL_OVERCHARGE);
                            DoSendQuantumText(EMOTE_OVERCHARGE, me);
                            events.ScheduleEvent(EVENT_OVERCHARGE, 45000);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            DoSendQuantumText(EMOTE_BERSERK, me);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_emalon_the_storm_watcherAI(creature);
        }
};

class npc_tempest_warder : public CreatureScript
{
    public:
        npc_tempest_warder() : CreatureScript("npc_tempest_warder") { }

        struct npc_tempest_warderAI : public QuantumBasicAI
        {
            npc_tempest_warderAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
                events.Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
            }

            void JustDied(Unit* /*Killer*/)
            {
                if (!me->IsSummon())
                    return;

                if (Creature* emalon = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EMALON) : 0))
                {
                    if (emalon->IsAlive())
                    {
                        emalon->SummonCreature(NPC_TEMPEST_WARDER, *emalon, TEMPSUMMON_CORPSE_DESPAWN, 0);
                        DoSendQuantumText(EMOTE_MINION_RESPAWN, me);
                    }
                }
            }

            void EnterToBattle(Unit* who)
            {
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_SHOCK, urand(10000, 20000));

                if (!me->IsSummon())
                    return;

                if (Creature* emalon = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EMALON) : 0))
                {
                    if (!emalon->GetVictim() && emalon->AI())
                        emalon->AI()->AttackStart(who);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!me->IsSummon() && HealthBelowPct(40) && !me->HasAura(SPELL_WARDER_OVERCHARGE))
                    DoCast(me, SPELL_WARDER_OVERCHARGE, true);

                if (me->GetAuraCount(SPELL_OVERCHARGED) >= 10)
                {
                    DoCast(me, SPELL_OVERCHARGED_BLAST);
                    me->DealDamage(me, me->GetHealth());
                }

                if (events.ExecuteEvent() == EVENT_SHOCK)
                {
                    DoCastVictim(SPELL_SHOCK);
                    events.ScheduleEvent(EVENT_SHOCK, urand(17500, 22500));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tempest_warderAI(creature);
        }
};

class spell_overcharge_targeting : public SpellScriptLoader
{
    public:
        spell_overcharge_targeting() : SpellScriptLoader("spell_overcharge_targeting") { }

        class spell_overcharge_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_overcharge_targeting_SpellScript);

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
                if (unitList.empty())
                    return;

                std::list<Unit*>::iterator itr = unitList.begin();
                std::advance(itr, urand(0, unitList.size() - 1));
                Unit* target = *itr;
                unitList.clear();
                unitList.push_back(target);
                _target = target;
            }

            void FilterTargetsSubsequent(std::list<Unit*>& unitList)
            {
                unitList.clear();
                if (_target)
                    unitList.push_back(_target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_overcharge_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_overcharge_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_overcharge_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_overcharge_targeting_SpellScript();
        }
};

class spell_emalon_lightning_nova : public SpellScriptLoader
{
    public:
        spell_emalon_lightning_nova() : SpellScriptLoader("spell_emalon_lightning_nova") { }

        class spell_emalon_lightning_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_emalon_lightning_nova_SpellScript);

            void CalcDamage()
            {
                if (!GetHitUnit() || !GetCaster())
                    return;

                float distance = GetHitUnit()->GetExactDist2d(GetCaster());
                if (distance < 10.0f)
                    return;

                SetHitDamage(int32(GetHitDamage() * 10 / distance));
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_emalon_lightning_nova_SpellScript::CalcDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_emalon_lightning_nova_SpellScript();
        }
};

void AddSC_boss_emalon_the_storm_watcher()
{
    new boss_emalon_the_storm_watcher();
    new npc_tempest_warder();
    new spell_overcharge_targeting();
    new spell_emalon_lightning_nova();
}