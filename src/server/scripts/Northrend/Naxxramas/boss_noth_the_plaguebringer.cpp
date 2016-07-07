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
	SAY_AGGRO_1     = -1533075,
	SAY_AGGRO_2     = -1533076,
	SAY_AGGRO_3     = -1533077,
	SAY_SUMMON      = -1533078,
	SAY_SLAY_1      = -1533079,
	SAY_SLAY_2      = -1533080,
	SAY_DEATH       = -1533081,
};

enum Spells
{
	SPELL_BERSERK                = 27680,
	SPELL_TELEPORT               = 29216,
	SPELL_CURSE_PLAGUEBRINGER_10 = 29213,
	SPELL_CURSE_PLAGUEBRINGER_25 = 54835,
	SPELL_CRIPPLE_10             = 29216,
	SPELL_CRIPPLE_25             = 54814,
	SPELL_BLINK_1                = 29208,
	SPELL_BLINK_2                = 29208,
	SPELL_BLINK_3                = 29210,
	SPELL_BLINK_4                = 29211,
};

#define SPELL_BLINK RAND(29208, 29209, 29210, 29211)

// Teleport position of Noth on his balcony
#define TELE_X 2631.370f
#define TELE_Y -3529.680f
#define TELE_Z 274.040f
#define TELE_O 6.277f

#define MAX_SUMMON_POS 5

const float SummonPos[MAX_SUMMON_POS][4] =
{
    {2728.12f, -3544.43f, 261.91f, 6.04f},
    {2729.05f, -3544.47f, 261.91f, 5.58f},
    {2728.24f, -3465.08f, 264.20f, 3.56f},
    {2704.11f, -3456.81f, 265.53f, 4.51f},
    {2663.56f, -3464.43f, 262.66f, 5.20f},
};

enum Events
{
    EVENT_NONE    = 0,
    EVENT_BERSERK = 1,
    EVENT_CURSE   = 2,
    EVENT_BLINK   = 3,
    EVENT_WARRIOR = 4,
    EVENT_BALCONY = 5,
    EVENT_WAVE    = 6,
    EVENT_GROUND  = 7,
};

class boss_noth_the_plaguebringer : public CreatureScript
{
public:
    boss_noth_the_plaguebringer() : CreatureScript("boss_noth_the_plaguebringer") { }

    struct boss_noth_the_plaguebringerAI : public BossAI
    {
        boss_noth_the_plaguebringerAI(Creature* creature) : BossAI(creature, BOSS_NOTH) {}

        uint32 waveCount, balconyCount;

        void Reset()
        {
			_Reset();

            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
            balconyCount = 0;
            EnterPhaseGround();
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            summon->SetActive(true);
            summon->AI()->DoZoneInCombat();
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

            DoSendQuantumText(SAY_DEATH, me);
        }

		void EnterPhaseGround()
        {
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            DoZoneInCombat();
            if (me->getThreatManager().isThreatListEmpty())
                EnterEvadeMode();
            else
            {
                me->getThreatManager().resetAllAggro();
                events.ScheduleEvent(EVENT_BALCONY, 110000);
                events.ScheduleEvent(EVENT_CURSE, 10000+rand()%15000);
                events.ScheduleEvent(EVENT_WARRIOR, 30000);
                if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                    events.ScheduleEvent(EVENT_BLINK, urand(20000, 40000));
            }
        }

        void SummonUndead(uint32 entry, uint32 num)
        {
            for (uint32 i = 0; i < num; ++i)
            {
                uint32 pos = RAID_MODE(RAND(2,3), rand()%MAX_SUMMON_POS);
                me->SummonCreature(entry, SummonPos[pos][0], SummonPos[pos][1], SummonPos[pos][2],
                    SummonPos[pos][3], TEMPSUMMON_CORPSE_DESPAWN, 60000);
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
                    case EVENT_CURSE:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCastAOE(RAID_MODE(SPELL_CURSE_PLAGUEBRINGER_10, SPELL_CURSE_PLAGUEBRINGER_25));
							events.ScheduleEvent(EVENT_CURSE, urand(50000, 60000));
                        }
                        return;
                    case EVENT_WARRIOR:
                        DoSendQuantumText(SAY_SUMMON, me);
                        SummonUndead(NPC_SKELETAL_WARRIOR, RAID_MODE(2, 3));
                        events.ScheduleEvent(EVENT_WARRIOR, 30000);
                        return;
                    case EVENT_BLINK:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCastAOE(RAID_MODE(SPELL_CRIPPLE_10, SPELL_CRIPPLE_25), true);
                            DoCastAOE(SPELL_BLINK);
                            DoResetThreat();
                            events.ScheduleEvent(EVENT_BLINK, 20000);
                        }
                        return;
                    case EVENT_BALCONY:
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->AttackStop();
                        me->RemoveAllAuras();
                        me->NearTeleportTo(TELE_X, TELE_Y, TELE_Z, TELE_O);
                        me->getThreatManager().resetAllAggro();
                        events.Reset();
                        events.ScheduleEvent(EVENT_WAVE, 10000);
                        waveCount = 0;
                        return;
                    case EVENT_WAVE:
                        DoSendQuantumText(SAY_SUMMON, me);
                        switch (balconyCount)
                        {
                            case 0:
								SummonUndead(NPC_SKELETAL_CHAMPION, RAID_MODE(2, 4));
								break;
                            case 1:
								SummonUndead(NPC_SKELETAL_CHAMPION, RAID_MODE(1, 2));
								SummonUndead(NPC_SKELETAL_GUARDIAN, RAID_MODE(1, 2));
								break;
                            case 2:
								SummonUndead(NPC_SKELETAL_GUARDIAN, RAID_MODE(2, 4));
								break;
                            default:
								SummonUndead(NPC_SKELETAL_CHAMPION, RAID_MODE(5, 10));
								SummonUndead(NPC_SKELETAL_GUARDIAN, RAID_MODE(5, 10));
								break;
                        }
                        ++waveCount;
                        events.ScheduleEvent(waveCount < 2 ? EVENT_WAVE : EVENT_GROUND, 30000);
                        return;
                    case EVENT_GROUND:
                    {
                        ++balconyCount;
                        float x, y, z, o;
                        me->GetHomePosition(x, y, z, o);
                        me->NearTeleportTo(x, y, z, o);
                        events.ScheduleEvent(EVENT_BALCONY, 110000);
                        EnterPhaseGround();
                        return;
                    }
                }
            }

            if (balconyCount > 3)
            {
                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) && !me->HasAura(SPELL_BERSERK))
                    DoCast(me, SPELL_BERSERK, true);
            }

            if (me->HasReactState(REACT_AGGRESSIVE))
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_noth_the_plaguebringerAI(creature);
    }
};

void AddSC_boss_noth_the_plaguebringer()
{
    new boss_noth_the_plaguebringer();
}