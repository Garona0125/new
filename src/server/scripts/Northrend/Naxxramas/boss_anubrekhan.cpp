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

const Position GuardSummonPos = {3333.72f, -3476.30f, 287.1f, 6.2801f};

#define EMOTE_LOCUST_SWARM  "AnubRekhan begins to unleash an insect swarm!"
#define EMOTE_CRYPT_GUARD   "A Crypt Guard joins the fight!"
#define EMOTE_CORPSE_SCARAB "Corpse Scarabs appears from a Crypt Guards corpse!"

enum Texts
{
	SAY_GREET_1   = -1533000,
	SAY_AGGRO_1   = -1533001,
	SAY_AGGRO_2   = -1533002,
	SAY_AGGRO_3   = -1533003,
	SAY_GREET_2   = -1533004,
	SAY_GREET_3   = -1533005,
	SAY_GREET_4   = -1533006,
	SAY_GREET_5   = -1533007,
	SAY_SLAY      = -1533008,
};

enum Events
{
    EVENT_NONE,
    EVENT_IMPALE,
    EVENT_LOCUST,
    EVENT_SPAWN_GUARDIAN_NORMAL,
    EVENT_BERSERK,
};

enum Spells
{
    SPELL_IMPALE_10                 = 28783,
    SPELL_IMPALE_25                 = 56090,
    SPELL_LOCUST_SWARM_10           = 28785,
    SPELL_LOCUST_SWARM_25           = 54021,
    SPELL_SUMMON_CORPSE_SCARABS_PLR = 29105,
    SPELL_SUMMON_CORPSE_SCARABS_MOB = 28864,
    SPELL_BERSERK                   = 27680,
};

class boss_anubrekhan : public CreatureScript
{
public:
    boss_anubrekhan() : CreatureScript("boss_anubrekhan") { }

    struct boss_anubrekhanAI : public BossAI
    {
        boss_anubrekhanAI(Creature* creature) : BossAI(creature, BOSS_ANUBREKHAN) {}

        bool hasTaunted;

        void Reset()
        {
            _Reset();

            hasTaunted = false;

            Position pos;

            pos = me->GetHomePosition();
            pos.m_positionY -= 10.0f;
            me->SummonCreature(NPC_CRYPT_GUARD, pos, TEMPSUMMON_CORPSE_DESPAWN);

            if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
            {
                pos = me->GetHomePosition();
                pos.m_positionY += 10.0f;
                me->SummonCreature(NPC_CRYPT_GUARD, pos, TEMPSUMMON_CORPSE_DESPAWN);
            }

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
			{
				victim->CastSpell(victim, SPELL_SUMMON_CORPSE_SCARABS_PLR, true, NULL, NULL, me->GetGUID());
				DoSendQuantumText(SAY_SLAY, me);
			}
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_ARACHNOPHOBIA_START_EVENT);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

			DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

            events.ScheduleEvent(EVENT_IMPALE, urand(10000, 20000));
            events.ScheduleEvent(EVENT_LOCUST, urand(80000, 120000));
            events.ScheduleEvent(EVENT_BERSERK, 600000);
            events.ScheduleEvent(EVENT_SPAWN_GUARDIAN_NORMAL, urand(15000, 20000));
        }

        void MoveInLineOfSight(Unit *who)
        {
            if (!hasTaunted && me->IsWithinDistInMap(who, 60.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
            {
				DoSendQuantumText(RAND(SAY_GREET_1, SAY_GREET_2, SAY_GREET_3, SAY_GREET_4, SAY_GREET_5), me);
                hasTaunted = true;
            }
            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void SummonedCreatureDespawn(Creature *summon)
        {
            BossAI::SummonedCreatureDespawn(summon);

            if (!me->IsAlive() || summon->IsAlive() || summon->GetEntry() != NPC_CRYPT_GUARD)
                return;

			me->MonsterTextEmote(EMOTE_CORPSE_SCARAB, 0, true);
            summon->CastSpell(summon, SPELL_SUMMON_CORPSE_SCARABS_MOB, true, NULL, NULL, me->GetGUID());
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || !CheckInRoom())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_IMPALE:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
							if (!me->HasAura(RAID_MODE(SPELL_LOCUST_SWARM_10, SPELL_LOCUST_SWARM_25)))
							{
								if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									DoCast(target, RAID_MODE(SPELL_IMPALE_10, SPELL_IMPALE_25));
							}
							events.ScheduleEvent(EVENT_IMPALE, urand(10000, 20000));
						}
                        break;
                    case EVENT_LOCUST:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
							me->MonsterTextEmote(EMOTE_LOCUST_SWARM, 0, true);
                            DoCast(me, RAID_MODE(SPELL_LOCUST_SWARM_10,SPELL_LOCUST_SWARM_25));
                            DoSummon(NPC_CRYPT_GUARD, GuardSummonPos, 0, TEMPSUMMON_CORPSE_DESPAWN);
                            events.ScheduleEvent(EVENT_LOCUST, urand(85000,95000));
                        }
                        break;
                    case EVENT_SPAWN_GUARDIAN_NORMAL:
						me->MonsterTextEmote(EMOTE_CRYPT_GUARD, 0, true);
                        DoSummon(NPC_CRYPT_GUARD, GuardSummonPos, 0, TEMPSUMMON_CORPSE_DESPAWN);
                        break;
                    case EVENT_BERSERK:
                        if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
						{
                            DoCast(me, SPELL_BERSERK, true);
						}
                        events.ScheduleEvent(EVENT_BERSERK, 600000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_anubrekhanAI(creature);
    }
};

void AddSC_boss_anubrekhan()
{
    new boss_anubrekhan();
}