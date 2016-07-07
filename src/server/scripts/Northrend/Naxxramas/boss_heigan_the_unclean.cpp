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
	SAY_AGGRO_1       = -1533109,
	SAY_AGGRO_2       = -1533110,
	SAY_AGGRO_3       = -1533111,
	SAY_SLAY          = -1533112,
	SAY_TAUNT_1       = -1533113,
	SAY_TAUNT_2       = -1533114,
	SAY_TAUNT_3       = -1533115,
	SAY_TAUNT_4       = -1533116,
	SAY_TAUNT_5       = -1533117,
	SAY_DEATH         = -1533118,
	EMOTE_TELEPORT_1  = -1533130,
	EMOTE_TELEPORT_2  = -1533131,
};

enum Spells
{
	SPELL_SPELL_DISRUPTION  = 29310,
	SPELL_DECREPIT_FEVER_10 = 29998,
	SPELL_DECREPIT_FEVER_25 = 55011,
	SPELL_PLAGUE_CLOUD      = 29350,
};

enum Events
{
    EVENT_NONE,
    EVENT_DISRUPT,
    EVENT_FEVER,
    EVENT_ERUPT,
    EVENT_PHASE,
};

enum Phases
{
	PHASE_FIGHT = 1,
    PHASE_DANCE = 2,
};

class boss_heigan_the_unclean : public CreatureScript
{
public:
    boss_heigan_the_unclean() : CreatureScript("boss_heigan_the_unclean") { }

    struct boss_heigan_the_uncleanAI : public BossAI
    {
        boss_heigan_the_uncleanAI(Creature* creature) : BossAI(creature, BOSS_HEIGAN) {}

        uint32 EruptSection;

        bool EruptDirection;
		bool Intro;

        Phases phase;

        void Reset()
        {
            _Reset();

			Intro = true;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit *who)
        {
            if (!me->IsWithinDistInMap(who, 30.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
            {
				if (Intro)
				{
					DoSendQuantumText(RAND(SAY_TAUNT_1, SAY_TAUNT_2, SAY_TAUNT_3, SAY_TAUNT_4, SAY_TAUNT_5), me);
					Intro = false;
				}
            }

            QuantumBasicAI::MoveInLineOfSight(who);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            TeleportHeiganCheaters();

			DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

            EnterPhase(PHASE_FIGHT);
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

            if (instance->GetData(DATA_HEIGAN_PLAYER_DEATHS) == 0)
                instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_THE_SAFETY_DANCE_10, ACHIEVEMENT_THE_SAFETY_DANCE_25));
        }

        void TeleportHeiganCheaters()
        {
            float x, y, z;
            me->GetPosition(x, y, z);

            uint64 tempDoorGuid_1 = instance->GetData64(DATA_GO_ROOM_HEIGAN);
            uint64 tempDoorGuid_2 = instance->GetData64(DATA_GO_PASSAGE_HEIGAN);

            std::list<HostileReference*> &m_threatlist = me->getThreatManager().getThreatList();
			for (std::list<HostileReference*>::iterator itr = m_threatlist.begin(); itr != m_threatlist.end(); ++itr)
			{
				if ((*itr)->getTarget()->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (Player* player = (*itr)->getTarget()->ToPlayer())
					{
						if (GameObject* door1 = GameObject::GetGameObject(*me,tempDoorGuid_1))
						{
							if (player->GetPositionX() > door1->GetPositionX())
								player->NearTeleportTo(x, y, z, 0);

							continue;
						}

						if (GameObject* door2 = GameObject::GetGameObject(*me,tempDoorGuid_1))
						{
							if (player->GetPositionY() < door2->GetPositionY())
								player->NearTeleportTo(x, y, z, 0);

							continue;
						}
                    }
				}
			}
        }

        void EnterPhase(Phases newPhase)
        {
            phase = newPhase;
            events.Reset();
            EruptSection = 3;
            if (phase == PHASE_FIGHT)
            {
				DoSendQuantumText(EMOTE_TELEPORT_1, me);
                events.ScheduleEvent(EVENT_DISRUPT, urand(10000, 25000));
                events.ScheduleEvent(EVENT_FEVER, urand(15000, 20000));
                events.ScheduleEvent(EVENT_PHASE, 90000);
                events.ScheduleEvent(EVENT_ERUPT, 15000);
				me->GetMotionMaster()->MoveChase(me->GetVictim());
            }
            else
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
				me->NearTeleportTo(x, y, z, o - G3D::halfPi());
				me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
				me->SetTarget(0);
				DoSendQuantumText(EMOTE_TELEPORT_2, me);
                DoCastAOE(SPELL_PLAGUE_CLOUD);
                events.ScheduleEvent(EVENT_PHASE, 45000);
                events.ScheduleEvent(EVENT_ERUPT, 8000);
            }
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
                    case EVENT_DISRUPT:
                        DoCastAOE(SPELL_SPELL_DISRUPTION);
                        events.ScheduleEvent(EVENT_DISRUPT, urand(5000, 10000));
                        break;
                    case EVENT_FEVER:
						DoCastAOE(RAID_MODE(SPELL_DECREPIT_FEVER_10, SPELL_DECREPIT_FEVER_25));
                        events.ScheduleEvent(EVENT_FEVER, urand(20000, 25000));
                        break;
                    case EVENT_PHASE:
                        EnterPhase(phase == PHASE_FIGHT ? PHASE_DANCE : PHASE_FIGHT);
                        break;
                    case EVENT_ERUPT:
                        instance->SetData(DATA_HEIGAN_ERUPT, EruptSection);
                        TeleportHeiganCheaters();

                        if (EruptSection == 0)
                            EruptDirection = true;
                        else if (EruptSection == 3)
                            EruptDirection = false;

                        EruptDirection ? ++EruptSection : --EruptSection;

                        events.ScheduleEvent(EVENT_ERUPT, phase == PHASE_FIGHT ? 10000 : 4000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_heigan_the_uncleanAI(creature);
    }
};

void AddSC_boss_heigan_the_unclean()
{
    new boss_heigan_the_unclean();
}