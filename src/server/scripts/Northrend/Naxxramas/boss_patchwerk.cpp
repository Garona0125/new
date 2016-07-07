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

enum Texts
{
    SAY_AGGRO_1        = -1533017,
    SAY_AGGRO_2        = -1533018,
    SAY_SLAY           = -1533019,
    SAY_DEATH          = -1533020,
    EMOTE_BERSERK      = -1533021,
    EMOTE_ENRAGE       = -1533022,
};

enum Spells
{
    SPELL_HATEFUL_STRIKE_10 = 28308,
    SPELL_HATEFUL_STRIKE_25 = 59192,
    SPELL_FRENZY            = 28131,
    SPELL_BERSERK           = 26662,
    SPELL_SLIME_BOLT        = 32309,
};

enum Events
{
    EVENT_NONE    = 0,
    EVENT_BERSERK = 1,
    EVENT_HATEFUL = 2,
    EVENT_SLIME   = 3,
};

enum HatefulThreatAmounts
{
    HATEFUL_THREAT_AMT = 1000,
};

#define MAX_ENCOUNTER_TIME  3*60*1000

class boss_patchwerk : public CreatureScript
{
public:
    boss_patchwerk() : CreatureScript("boss_patchwerk") { }

    struct boss_patchwerkAI : public BossAI
    {
        boss_patchwerkAI(Creature* creature) : BossAI(creature, BOSS_PATCHWERK) {}

        bool Enraged;

        uint32 EncounterTime;

        void Reset()
        {
            _Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_MAKE_QUICK_WERK_OF_HIM_START_EVENT);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            Enraged = false;

            EncounterTime = 0;

            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2), me);

            events.ScheduleEvent(EVENT_HATEFUL, 1*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BERSERK, 6*MINUTE*IN_MILLISECONDS);

			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_MAKE_QUICK_WERK_OF_HIM_START_EVENT);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

            DoSendQuantumText(SAY_DEATH, me);

            if (EncounterTime <= MAX_ENCOUNTER_TIME)
            {
                AchievementEntry const *AchievMakeQuickWerkOfHim = sAchievementStore.LookupEntry(RAID_MODE(ACHIEVEMENT_MAKE_QUICK_WERK_OF_HIM_10, ACHIEVEMENT_MAKE_QUICK_WERK_OF_HIM_25));
                if (AchievMakeQuickWerkOfHim)
                {
                    Map* map = me->GetMap();
                    if (map && map->IsDungeon())
                    {
                        Map::PlayerList const &players = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                            itr->getSource()->CompletedAchievement(AchievMakeQuickWerkOfHim);
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            EncounterTime += diff;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
					case EVENT_HATEFUL:
                    {
                        // Hateful Strike targets the highest non-MT threat in melee range on 10man
                        // and the higher HP target out of the two highest non-MT threats in melee range on 25man
                        float MostThreat = 0.0f;
                        Unit* secondThreatTarget = NULL;
                        Unit* thirdThreatTarget = NULL;

                        std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                        for (; i != me->getThreatManager().getThreatList().end(); ++i)
                        { // find second highest
                            Unit* target = (*i)->getTarget();
                            if (target->IsAlive() && target != me->GetVictim() && (*i)->getThreat() >= MostThreat && me->IsWithinMeleeRange(target))
                            {
                                MostThreat = (*i)->getThreat();
                                secondThreatTarget = target;
                            }
                        }

                        if (secondThreatTarget && Is25ManRaid())
                        { // find third highest
                            MostThreat = 0.0f;
                            i = me->getThreatManager().getThreatList().begin();
                            for (; i != me->getThreatManager().getThreatList().end(); ++i)
                            {
                                Unit* target = (*i)->getTarget();
                                if (target->IsAlive() && target != me->GetVictim() && target != secondThreatTarget && (*i)->getThreat() >= MostThreat && me->IsWithinMeleeRange(target))
                                {
                                    MostThreat = (*i)->getThreat();
                                    thirdThreatTarget = target;
                                }
                            }
                        }

                        Unit* pHatefulTarget = NULL;
                        if (!thirdThreatTarget)
                            pHatefulTarget = secondThreatTarget;
                        else if (secondThreatTarget)
                            pHatefulTarget = (secondThreatTarget->GetHealth() < thirdThreatTarget->GetHealth()) ? thirdThreatTarget : secondThreatTarget;

                        if (!pHatefulTarget)
                            pHatefulTarget = me->GetVictim();

                        DoCast(pHatefulTarget, RAID_MODE(SPELL_HATEFUL_STRIKE_10, SPELL_HATEFUL_STRIKE_25), true);

                        // add threat to highest threat targets
                        if (me->GetVictim() && me->IsWithinMeleeRange(me->GetVictim()))
                            me->getThreatManager().addThreat(me->GetVictim(), HATEFUL_THREAT_AMT);
                        if (secondThreatTarget)
                            me->getThreatManager().addThreat(secondThreatTarget, HATEFUL_THREAT_AMT);
                        if (thirdThreatTarget)
                            me->getThreatManager().addThreat(thirdThreatTarget, HATEFUL_THREAT_AMT); // this will only ever be used in 25m

                        events.ScheduleEvent(EVENT_HATEFUL, 1*IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK, true);
                        DoSendQuantumText(EMOTE_BERSERK, me);
                        events.ScheduleEvent(EVENT_SLIME, 2*IN_MILLISECONDS);
                        break;
                    case EVENT_SLIME:
                        DoCastAOE(SPELL_SLIME_BOLT, true);
                        events.ScheduleEvent(EVENT_SLIME, 2*IN_MILLISECONDS);
                        break;
				}
            }

            if (!Enraged && HealthBelowPct(HEALTH_PERCENT_5))
            {
                DoCast(me, SPELL_FRENZY, true);
                DoSendQuantumText(EMOTE_ENRAGE, me);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_patchwerkAI(creature);
    }
};

void AddSC_boss_patchwerk()
{
    new boss_patchwerk();
}