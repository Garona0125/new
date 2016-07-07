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

#define EMOTE_WEB_WRAP   "Maexxna spins her web into a coccon!"
#define EMOTE_SPIDERLING "Spiderlings appear on the web!"
#define EMOTE_SPRAY      "Maexxna sprays strands of web everywhere!"

enum Spells
{
    SPELL_WEB_WRAP              = 28622,
    SPELL_WEB_SPRAY_10          = 29484,
    SPELL_WEB_SPRAY_25          = 54125,
    SPELL_POISON_SHOCK_10       = 28741,
    SPELL_POISON_SHOCK_25       = 54122,
    SPELL_NECROTIC_POISON_10    = 28776,
    SPELL_NECROTIC_POISON_25    = 54121,
    SPELL_FRENZY_10             = 54123,
    SPELL_FRENZY_25             = 54124,
};

#define MAX_POS_WRAP 3

const Position PosWrap[MAX_POS_WRAP] =
{
    {3546.796f, -3869.082f, 296.450f, 0.0f},
    {3531.271f, -3847.424f, 299.450f, 0.0f},
    {3497.067f, -3843.384f, 302.384f, 0.0f},
};

enum Events
{
    EVENT_NONE   = 0,
    EVENT_SPRAY  = 1,
    EVENT_SHOCK  = 2,
    EVENT_POISON = 3,
    EVENT_WRAP   = 4,
    EVENT_SUMMON = 5,
    EVENT_FRENZY = 6,
};

class boss_maexxna : public CreatureScript
{
public:
    boss_maexxna() : CreatureScript("boss_maexxna") { }

    struct boss_maexxnaAI : public BossAI
    {
        boss_maexxnaAI(Creature* creature) : BossAI(creature, BOSS_MAEXXNA) {}

        bool Enraged;

        void Reset()
        {
            _Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            Enraged = false;
            events.ScheduleEvent(EVENT_WRAP, 20000);
            events.ScheduleEvent(EVENT_SPRAY, 40000);
            events.ScheduleEvent(EVENT_SHOCK, urand(5000, 10000));
            events.ScheduleEvent(EVENT_POISON, urand(10000, 15000));
            events.ScheduleEvent(EVENT_SUMMON, 30000);
        }

		void JustDied(Unit* /*killer*/)
		{
			_JustDied();

			if (Creature* kel = me->FindCreatureWithDistance(NPC_MB_KEL_THUZAD, 500.0f))
				DoSendQuantumText(SAY_KELTHUZAD_TAUNT_1, kel);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || !CheckInRoom())
                return;

            if (!Enraged && HealthBelowPct(30))
            {
                Enraged = true;
                events.ScheduleEvent(EVENT_FRENZY, 0);
            }

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_WRAP:
                        me->MonsterTextEmote(EMOTE_WEB_WRAP, 0, true);
                        for (uint8 i = 0; i < RAID_MODE(1, 2); ++i)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0, true, -SPELL_WEB_WRAP))
                            {
                                target->RemoveAura(RAID_MODE(SPELL_WEB_SPRAY_10, SPELL_WEB_SPRAY_25));
                                uint8 pos = rand()%MAX_POS_WRAP;
                                target->GetMotionMaster()->MoveJump(PosWrap[pos].GetPositionX(), PosWrap[pos].GetPositionY(), PosWrap[pos].GetPositionZ(), 20, 20);

                                if (Creature* wrap = DoSummon(NPC_WEB_WRAP, PosWrap[pos], 0, TEMPSUMMON_CORPSE_DESPAWN))
                                    wrap->AI()->SetGUID(target->GetGUID());
                            }
                        }
                        events.ScheduleEvent(EVENT_WRAP, 40000);
                        break;
                    case EVENT_SPRAY:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
							me->MonsterTextEmote(EMOTE_SPRAY, 0, true);
                            DoCastAOE(RAID_MODE(SPELL_WEB_SPRAY_10, SPELL_WEB_SPRAY_25));
                            events.ScheduleEvent(EVENT_SPRAY, 40000);
                        }
                        break;
                    case EVENT_SHOCK:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCastAOE(RAID_MODE(SPELL_POISON_SHOCK_10, SPELL_POISON_SHOCK_25));
                            events.ScheduleEvent(EVENT_SHOCK, urand(10000,20000));
                        }
                        break;
                    case EVENT_POISON:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCastVictim(RAID_MODE(SPELL_NECROTIC_POISON_10, SPELL_NECROTIC_POISON_25));
                            events.ScheduleEvent(EVENT_POISON, urand(10000, 20000));
                        }
                        break;
                    case EVENT_FRENZY:
                        DoCast(me, RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25), true);
                        events.ScheduleEvent(EVENT_FRENZY, 600000);
                        break;
                    case EVENT_SUMMON:
                        me->MonsterTextEmote(EMOTE_SPIDERLING, 0, true);
                        uint8 amount = urand(8, 10);
                        for (uint8 i = 0; i < amount; ++i)
                            DoSummon(NPC_SPIDERLING, me, 0, TEMPSUMMON_CORPSE_DESPAWN);
                        events.ScheduleEvent(EVENT_SUMMON, 40000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_maexxnaAI(creature);
    }
};

class mob_webwrap : public CreatureScript
{
public:
    mob_webwrap() : CreatureScript("mob_webwrap") { }

    struct mob_webwrapAI : public NullCreatureAI
    {
        mob_webwrapAI(Creature* creature) : NullCreatureAI(creature), victimGUID(0) {}

        uint64 victimGUID;

        void SetGUID(uint64 guid, int32 /*param*/)
        {
            victimGUID = guid;

            if (me->m_spells[0] && victimGUID)
			{
                if (Unit* victim = Unit::GetUnit(*me, victimGUID))
                    victim->CastSpell(victim, me->m_spells[0], true, NULL, NULL, me->GetGUID());
			}
        }

        void JustDied(Unit* /*killer*/)
        {
            if (me->m_spells[0] && victimGUID)
			{
                if (Unit* victim = Unit::GetUnit(*me, victimGUID))
                    victim->RemoveAurasDueToSpell(me->m_spells[0], me->GetGUID());
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_webwrapAI(creature);
    }
};

void AddSC_boss_maexxna()
{
    new boss_maexxna();
    new mob_webwrap();
}