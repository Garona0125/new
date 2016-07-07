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
#include "vault_of_archavon.h"

enum Spells
{
    SPELL_FREEZING_GROUND_10 = 72090,
	SPELL_FREEZING_GROUND_25 = 72104,
    SPELL_FROZEN_ORB_10      = 72091,
	SPELL_FROZEN_ORB_25      = 72095,
    SPELL_WHITEOUT_10        = 72034,
	SPELL_WHITEOUT_25        = 72096,
    SPELL_FROZEN_MALLET_10   = 71993,
	SPELL_FROZEN_MALLET_25   = 72122,
    SPELL_FROST_BLAST_10     = 72123,
	SPELL_FROST_BLAST_25     = 72124,

    SPELL_FROZEN_ORB_DMG    = 72081,

    SPELL_FROZEN_ORB_AURA   = 72067,
    SPELL_FROZEN_ORB_SUMMON = 72093,
};

enum Events
{
    EVENT_FREEZING_GROUND   = 1,
    EVENT_FROZEN_ORB        = 2,
    EVENT_WHITEOUT          = 3,
    EVENT_FROST_BLAST       = 4,
};

class boss_toravon_the_ice_watcher : public CreatureScript
{
    public:
        boss_toravon_the_ice_watcher() : CreatureScript("boss_toravon_the_ice_watcher") { }

        struct boss_toravon_the_ice_watcherAI : public BossAI
        {
            boss_toravon_the_ice_watcherAI(Creature* creature) : BossAI(creature, DATA_TORAVON){}

			void Reset()
			{
				_Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				DoCast(me, RAID_MODE(SPELL_FROZEN_MALLET_10, SPELL_FROZEN_MALLET_25), true);
			}

            void EnterToBattle(Unit* /*who*/)
            {
				_EnterToBattle();

                events.ScheduleEvent(EVENT_FROZEN_ORB, 11000);
                events.ScheduleEvent(EVENT_WHITEOUT, 13000);
                events.ScheduleEvent(EVENT_FREEZING_GROUND, 15000);
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();
			}

            void UpdateAI(const uint32 diff)
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
                        case EVENT_FROZEN_ORB:
                            me->CastCustomSpell(RAID_MODE(SPELL_FROZEN_ORB_10, SPELL_FROZEN_ORB_25), SPELLVALUE_MAX_TARGETS, 1, me);
                            events.ScheduleEvent(EVENT_FROZEN_ORB, 38000);
                            break;
                        case EVENT_WHITEOUT:
                            DoCast(me, RAID_MODE(SPELL_WHITEOUT_10, SPELL_WHITEOUT_25));
                            events.ScheduleEvent(EVENT_WHITEOUT, 38000);
                            break;
                        case EVENT_FREEZING_GROUND:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
							{
                                DoCast(target, RAID_MODE(SPELL_FREEZING_GROUND_10, SPELL_FREEZING_GROUND_25));
							}
                            events.ScheduleEvent(EVENT_FREEZING_GROUND, 20000);
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
            return new boss_toravon_the_ice_watcherAI(creature);
        }
};

class npc_frost_warder : public CreatureScript
{
    public:
        npc_frost_warder() : CreatureScript("npc_frost_warder") { }

        struct npc_frost_warderAI : public QuantumBasicAI
        {
            npc_frost_warderAI(Creature* creature) : QuantumBasicAI(creature) {}

			EventMap events;

            void Reset()
            {
                events.Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				DoCast(me, RAID_MODE(SPELL_FROZEN_MALLET_10, SPELL_FROZEN_MALLET_25), true);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoZoneInCombat();

                events.ScheduleEvent(EVENT_FROST_BLAST, 500);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == EVENT_FROST_BLAST)
                {
                    DoCastVictim(RAID_MODE(SPELL_FROST_BLAST_10, SPELL_FROST_BLAST_25));
                    events.ScheduleEvent(EVENT_FROST_BLAST, 4000);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_frost_warderAI(creature);
        }
};

class npc_frozen_orb : public CreatureScript
{
public:
    npc_frozen_orb() : CreatureScript("npc_frozen_orb") { }

    struct npc_frozen_orbAI : public QuantumBasicAI
    {
        npc_frozen_orbAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KillTimer;

        bool done;

        void Reset()
        {
            done = false;
            KillTimer = 1*MINUTE*IN_MILLISECONDS;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!done)
            {
                DoCast(me, SPELL_FROZEN_ORB_AURA, true);
                DoCast(me, SPELL_FROZEN_ORB_DMG, true);

                done = true;
            }

            if (KillTimer <= diff)
            {
                if (!UpdateVictim())
				{
                    me->DespawnAfterAction();

					KillTimer = 10*IN_MILLISECONDS;
				}
            }
            else KillTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frozen_orbAI(creature);
    }
};

class npc_toravon_frozen_orb_stalker : public CreatureScript
{
    public:
        npc_toravon_frozen_orb_stalker() : CreatureScript("npc_toravon_frozen_orb_stalker") { }

        struct npc_toravon_frozen_orb_stalkerAI : public QuantumBasicAI
        {
            npc_toravon_frozen_orb_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
            {
                creature->SetVisible(false);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                creature->SetReactState(REACT_PASSIVE);
                instance = creature->GetInstanceScript();
                spawned = false;
				SetCombatMovement(false);
            }

			InstanceScript* instance;
            bool spawned;

            void UpdateAI(const uint32 /*diff*/)
            {
                if (spawned)
                    return;

                spawned = true;
                Unit* toravon = me->GetCreature(*me, instance ? instance->GetData64(DATA_TORAVON) : 0);
                if (!toravon)
                    return;

                uint8 num_orbs = RAID_MODE(1, 3);
                for (uint8 i = 0; i < num_orbs; ++i)
                {
                    Position pos;
                    me->GetNearPoint(toravon, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 0.0f, 10.0f, 0.0f);
                    me->SetPosition(pos);
                    DoCast(me, SPELL_FROZEN_ORB_SUMMON);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_toravon_frozen_orb_stalkerAI(creature);
        }
};

void AddSC_boss_toravon_the_ice_watcher()
{
    new boss_toravon_the_ice_watcher();
    new npc_frost_warder();
    new npc_frozen_orb();
    new npc_toravon_frozen_orb_stalker();
}