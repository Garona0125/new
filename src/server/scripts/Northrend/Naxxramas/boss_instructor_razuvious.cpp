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
	SAY_AGGRO_1   = -1533120,
	SAY_AGGRO_2   = -1533121,
	SAY_AGGRO_3   = -1533122,
	SAY_SLAY_1    = -1533123,
	SAY_SLAY_2    = -1533124,
	SAY_COMMAND_1 = -1533125,
	SAY_COMMAND_2 = -1533126,
	SAY_COMMAND_3 = -1533127,
	SAY_COMMAND_4 = -1533128,
	SAY_DEATH     = -1533129,
};

enum Spells
{
	SPELL_UNBALANCING_STRIKE   = 26613,
	SPELL_DISRUPTING_SHOUT     = 29107,
	H_SPELL_DISRUPTING_SHOUT   = 55543,
	SPELL_JAGGED_KNIFE         = 55550,
	SPELL_HOPELESS             = 29125,
	SPELL_JUDJEMENT_OF_JUSTICE = 20184,
};

enum Events
{
    EVENT_NONE,
    EVENT_STRIKE,
    EVENT_SHOUT,
    EVENT_KNIFE,
    EVENT_COMMAND,
};

class boss_instructor_razuvious : public CreatureScript
{
public:
    boss_instructor_razuvious() : CreatureScript("boss_instructor_razuvious") { }

    struct boss_instructor_razuviousAI : public BossAI
    {
        boss_instructor_razuviousAI(Creature* creature) : BossAI(creature, BOSS_RAZUVIOUS) {}

        void Reset()
        {
			_Reset();

            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_JUDJEMENT_OF_JUSTICE, true);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void DamageTaken(Unit* DoneBy, uint32& damage)
        {
            if (DoneBy->GetTypeId() == TYPE_ID_UNIT && (DoneBy->GetEntry() == NPC_DEATH_KNIGHT_UNDERSTUDY_1 || DoneBy->GetEntry() == NPC_DEATH_KNIGHT_UNDERSTUDY_2))
                me->LowerPlayerDamageReq(damage);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

			DoSendQuantumText(SAY_DEATH, me);

            me->CastSpell(me, SPELL_HOPELESS, true);

            std::list<Creature*> lList;
            me->GetCreatureListWithEntryInGrid(lList , NPC_OBEDIENCE_CRYSTAL, 200);

            if (!lList.size())
                return;

            for (std::list<Creature*>::const_iterator i = lList.begin(); i != lList.end(); ++i)
                (*i)->DealDamage((*i),(*i)->GetHealth());
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

			DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
            events.ScheduleEvent(EVENT_STRIKE, 30000);
            events.ScheduleEvent(EVENT_SHOUT, 25000);
            events.ScheduleEvent(EVENT_COMMAND, 40000);
            events.ScheduleEvent(EVENT_KNIFE, 10000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_STRIKE:
                        DoCastVictim(SPELL_UNBALANCING_STRIKE);
                        events.ScheduleEvent(EVENT_STRIKE, 30000);
                        return;
                    case EVENT_SHOUT:
						DoCastAOE(RAID_MODE(SPELL_DISRUPTING_SHOUT, H_SPELL_DISRUPTING_SHOUT));
                        events.ScheduleEvent(EVENT_SHOUT, 25000);
                        return;
                    case EVENT_KNIFE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 45.0f))
						{
                            DoCast(target, SPELL_JAGGED_KNIFE);
						}
                        events.ScheduleEvent(EVENT_KNIFE, 10000);
                        return;
                    case EVENT_COMMAND:
						DoSendQuantumText(RAND(SAY_COMMAND_1, SAY_COMMAND_2, SAY_COMMAND_3, SAY_COMMAND_4), me);
                        events.ScheduleEvent(EVENT_COMMAND, 40000);
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_instructor_razuviousAI(creature);
    }
};

void AddSC_boss_instructor_razuvious()
{
    new boss_instructor_razuvious();
}