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
#include "SpellAuraEffects.h"
#include "naxxramas.h"

#define EMOTE_SPRAY "Grobbulus sprays slime across the room!"
#define EMOTE_INJECTION "Grobbulus injects you with a mutagen!!"

enum Spells
{
	SPELL_BOMBARD_SLIME         = 28280,
	SPELL_POISON_CLOUD          = 28240,
	SPELL_MUTATING_INJECTION    = 28169,
	SPELL_SLIME_SPRAY_10        = 28157,
	SPELL_SLIME_SPRAY_25        = 54364,
	SPELL_BERSERK               = 26662,
	SPELL_POISON_CLOUD_ADD      = 28158,
	SPELL_SLIME_STREAM          = 28137,
};

enum EventsAndNpcs
{
	EVENT_BERSERK     = 1,
	EVENT_CLOUD       = 2,
	EVENT_INJECT      = 3,
	EVENT_SPRAY       = 4,
};

class boss_grobbulus : public CreatureScript
{
public:
    boss_grobbulus() : CreatureScript("boss_grobbulus") {}

    struct boss_grobbulusAI : public BossAI
    {
        boss_grobbulusAI(Creature* creature) : BossAI(creature, BOSS_GROBBULUS)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_POISON_CLOUD_ADD, true);
        }

        uint32 SlimeStreamTimer;

        void Reset()
        {
            _Reset();

            SlimeStreamTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            events.ScheduleEvent(EVENT_CLOUD, 15000);
            events.ScheduleEvent(EVENT_INJECT, 20000);
            events.ScheduleEvent(EVENT_SPRAY, 15000+rand()%15000);
            events.ScheduleEvent(EVENT_BERSERK, RAID_MODE(12*60000,9*60000));
        }

        void EnterEvadeMode()
        {
            _EnterEvadeMode();

            Reset();
        }

		void JustDied(Unit* /*killer*/)
		{
			_JustDied();
		}

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == RAID_MODE(SPELL_SLIME_SPRAY_10, SPELL_SLIME_SPRAY_25))
            {
                if (TempSummon* slime = me->SummonCreature(NPC_FALLOUT_SLIME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
                    DoZoneInCombat(slime);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (SlimeStreamTimer <= diff)
                {
                    DoCast(SPELL_SLIME_STREAM);
                    SlimeStreamTimer = 3*IN_MILLISECONDS;
                }
                else SlimeStreamTimer -= diff;
            }
            else SlimeStreamTimer = 3*IN_MILLISECONDS;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLOUD:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCastAOE(SPELL_POISON_CLOUD);
                            events.ScheduleEvent(EVENT_CLOUD, 15000);
                        }
                        return;
                    case EVENT_BERSERK:
                        if (!me->IsNonMeleeSpellCasted(false))
                            DoCastAOE(SPELL_BERSERK);
                        return;
                    case EVENT_SPRAY:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
							me->MonsterTextEmote(EMOTE_SPRAY, 0, true);
							DoCastAOE(RAID_MODE(SPELL_SLIME_SPRAY_10, SPELL_SLIME_SPRAY_25));
                            events.ScheduleEvent(EVENT_SPRAY, 15000+rand()%15000);
                        }
                        return;
                    case EVENT_INJECT:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 200, true))
							{
                                if (!target->HasAura(SPELL_MUTATING_INJECTION))
                                {
									me->MonsterTextEmote(EMOTE_INJECTION, 0, true);
                                    DoCast(target, SPELL_MUTATING_INJECTION);
                                    events.ScheduleEvent(EVENT_INJECT, 8000 + uint32(120 * me->GetHealthPct()));
                                }
                        }
                        return;
					}
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grobbulusAI(creature);
    }
};

class npc_grobbulus_cloud : public CreatureScript
{
public:
    npc_grobbulus_cloud() : CreatureScript("npc_grobbulus_cloud") { }

    struct npc_grobbulus_cloudAI : public QuantumBasicAI
    {
        npc_grobbulus_cloudAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
			Reset();
        }

        uint32 CloudTimer;

        bool Clouded;

        void Reset()
        {
            CloudTimer = 1000;
            Clouded = false;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetCurrentFaction(16);
        }

        void UpdateAI(const uint32 diff)
        {
            if (CloudTimer <= diff)
            {
                if (!me->HasAura(SPELL_POISON_CLOUD_ADD))
				{
                    if (!Clouded)
                    {
                        DoCast(me, SPELL_POISON_CLOUD_ADD);
                        Clouded = true;
                    }
                    else me->DealDamage(me, me->GetHealth());
				}
                CloudTimer = 10000;
            }
			else CloudTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grobbulus_cloudAI(creature);
    }
};

void AddSC_boss_grobbulus()
{
    new boss_grobbulus();
    new npc_grobbulus_cloud();
}