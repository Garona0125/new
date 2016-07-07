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
#include "molten_core.h"

enum Texts
{
	SAY_ARRIVAL_1_RAG     = -1409009,
    SAY_ARRIVAL_2_MAJ     = -1409010,
    SAY_ARRIVAL_3_RAG     = -1409011,
    SAY_ARRIVAL_5_RAG     = -1409012,
    SAY_REINFORCEMENTS_1  = -1409013,
    SAY_REINFORCEMENTS_2  = -1409014,
    SAY_HAND_OF_RAGNAROS  = -1409015,
    SAY_WRATH_OF_RAGNAROS = -1409016,
    SAY_SLAY              = -1409017,
    SAY_MAGMA_BURST       = -1409018,
};

enum Spells
{
    SPELL_HAND_OF_RAGNAROS      = 19780,
    SPELL_WRATH_OF_RAGNAROS     = 20566,
    SPELL_LAVA_BURST            = 21158,
    SPELL_MAGMA_BLAST           = 20565,
    SPELL_SONS_OF_FLAME_DUMMY   = 21108,
    SPELL_RAGSUBMERGE           = 21107,
    SPELL_RAGEMERGE             = 20568,
    SPELL_MELT_WEAPON           = 21388,
    SPELL_ELEMENTAL_FIRE        = 20564,
    SPELL_ERRUPTION             = 17731,

	SPELL_RAGNAROS_SUBMERGE_VIS = 20567, // ??
};

enum Events
{
    EVENT_ERUPTION          = 1,
    EVENT_WRATH_OF_RAGNAROS = 2,
    EVENT_HAND_OF_RAGNAROS  = 3,
    EVENT_LAVA_BURST        = 4,
    EVENT_ELEMENTAL_FIRE    = 5,
    EVENT_MAGMA_BLAST       = 6,
    EVENT_SUBMERGE          = 7,
    EVENT_INTRO_1           = 8,
    EVENT_INTRO_2           = 9,
    EVENT_INTRO_3           = 10,
    EVENT_INTRO_4           = 11,
    EVENT_INTRO_5           = 12,
};

class boss_ragnaros : public CreatureScript
{
    public:
        boss_ragnaros() : CreatureScript("boss_ragnaros") { }

        struct boss_ragnarosAI : public BossAI
        {
            boss_ragnarosAI(Creature* creature) : BossAI(creature, BOSS_RAGNAROS)
            {
                IntroState = 0;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				SetCombatMovement(false);
            }

			uint32 EmergeTimer;
            uint8 IntroState;

            bool HasYelledMagmaBurst;
            bool HasSubmergedOnce;
            bool IsBanished;

            void Reset()
            {
                BossAI::Reset();
                EmergeTimer = 90000;
                HasYelledMagmaBurst = false;
                HasSubmergedOnce = false;
                IsBanished = false;
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
            }

            void EnterToBattle(Unit* who)
            {
                BossAI::EnterToBattle(who);
                events.ScheduleEvent(EVENT_ERUPTION, 15000);
                events.ScheduleEvent(EVENT_WRATH_OF_RAGNAROS, 30000);
                events.ScheduleEvent(EVENT_HAND_OF_RAGNAROS, 25000);
                events.ScheduleEvent(EVENT_LAVA_BURST, 10000);
                events.ScheduleEvent(EVENT_ELEMENTAL_FIRE, 3000);
                events.ScheduleEvent(EVENT_MAGMA_BLAST, 2000);
                events.ScheduleEvent(EVENT_SUBMERGE, 180000);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_SLAY, me);
            }

            void UpdateAI(const uint32 diff)
            {
                if (IntroState != 2)
                {
                    if (!IntroState)
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                        events.ScheduleEvent(EVENT_INTRO_1, 4000);
                        events.ScheduleEvent(EVENT_INTRO_2, 23000);
                        events.ScheduleEvent(EVENT_INTRO_3, 42000);
                        events.ScheduleEvent(EVENT_INTRO_4, 43000);
                        events.ScheduleEvent(EVENT_INTRO_5, 53000);
                        IntroState = 1;
                    }

                    events.Update(diff);

                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                        case EVENT_INTRO_1:
                            DoSendQuantumText(SAY_ARRIVAL_1_RAG, me);
                            break;
                        case EVENT_INTRO_2:
                            DoSendQuantumText(SAY_ARRIVAL_3_RAG, me);
                            break;
                        case EVENT_INTRO_3:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_1H);
                            break;
                        case EVENT_INTRO_4:
                            DoSendQuantumText(SAY_ARRIVAL_5_RAG, me);
							if (instance)
							{
								if (Creature* executus = Unit::GetCreature(*me, instance->GetData64(BOSS_MAJORDOMO_EXECUTUS)))
									me->Kill(executus);
							}
                            break;
                        case EVENT_INTRO_5:
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            IntroState = 2;
                            break;
                        default:
                            break;
                        }
                    }
                }
                else
                {
                    if (instance)
                    {
                        if (IsBanished && ((EmergeTimer <= diff) || (instance->GetData(DATA_RAGNAROS_ADDS)) > 8))
                        {
                            //Become unbanished again
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->SetCurrentFaction(16);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                AttackStart(target);
                            instance->SetData(DATA_RAGNAROS_ADDS, 0);

                            //DoCast(me, SPELL_RAGEMERGE); //"phase spells" didnt worked correctly so Ive commented them and wrote solution witch doesnt need core support
                            IsBanished = false;
                        }
                        else if (IsBanished)
                        {
                            EmergeTimer -= diff;
                            //Do nothing while banished
                            return;
                        }
                    }

                    //Return since we have no target
                    if (!UpdateVictim())
                        return;

                    events.Update(diff);

                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_ERUPTION:
                                DoCastVictim(SPELL_ERRUPTION);
                                events.ScheduleEvent(EVENT_ERUPTION, urand(20000, 45000));
                                break;
                            case EVENT_WRATH_OF_RAGNAROS:
                                DoCastVictim(SPELL_WRATH_OF_RAGNAROS);
								DoSendQuantumText(SAY_WRATH_OF_RAGNAROS, me);
                                events.ScheduleEvent(EVENT_WRATH_OF_RAGNAROS, 25000);
                                break;
                            case EVENT_HAND_OF_RAGNAROS:
                                DoCast(me, SPELL_HAND_OF_RAGNAROS);
								DoSendQuantumText(SAY_HAND_OF_RAGNAROS, me);
                                events.ScheduleEvent(EVENT_HAND_OF_RAGNAROS, 20000);
                                break;
                            case EVENT_LAVA_BURST:
                                DoCastVictim(SPELL_LAVA_BURST);
                                events.ScheduleEvent(EVENT_LAVA_BURST, 10000);
                                break;
                            case EVENT_ELEMENTAL_FIRE:
                                DoCastVictim(SPELL_ELEMENTAL_FIRE);
                                events.ScheduleEvent(EVENT_ELEMENTAL_FIRE, urand(10000, 14000));
                                break;
                            case EVENT_MAGMA_BLAST:
                                if (!me->IsWithinMeleeRange(me->GetVictim()))
                                {
                                    DoCastVictim(SPELL_MAGMA_BLAST);
                                    if (!HasYelledMagmaBurst)
                                    {
                                        //Say our dialog
                                        DoSendQuantumText(SAY_MAGMA_BURST, me);
                                        HasYelledMagmaBurst = true;
                                    }
                                }
                                events.ScheduleEvent(EVENT_MAGMA_BLAST, 2500);
                                break;
                            case EVENT_SUBMERGE:
                            {
                                if (instance && !IsBanished)
                                {
                                    //Creature spawning and ragnaros becomming unattackable
                                    //is not very well supported in the core //no it really isnt
                                    //so added normaly spawning and banish workaround and attack again after 90 secs.
                                    me->AttackStop();
                                    DoResetThreat();
                                    me->SetReactState(REACT_PASSIVE);
                                    me->InterruptNonMeleeSpells(false);
                                    //Root self
                                    //DoCast(me, 23973);
                                    me->SetCurrentFaction(35);
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SUBMERGED);
                                    me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
                                    instance->SetData(DATA_RAGNAROS_ADDS, 0);

                                    if (!HasSubmergedOnce)
                                    {
                                        DoSendQuantumText(SAY_REINFORCEMENTS_1, me);

                                        // summon 8 elementals
                                        for (uint8 i = 0; i < 8; ++i)
										{
                                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
											{
                                                if (Creature* summoned = me->SummonCreature(NPC_SON_OF_FLAME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 900000))
                                                    summoned->AI()->AttackStart(target);
											}
										}
                                        HasSubmergedOnce = true;
                                        IsBanished = true;
                                        //DoCast(me, SPELL_RAGSUBMERGE);
                                        EmergeTimer = 90000;
                                    }
                                    else
                                    {
                                        DoSendQuantumText(SAY_REINFORCEMENTS_2, me);

                                        for (uint8 i = 0; i < 8; ++i)
										{
                                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
											{
                                                if (Creature* summoned = me->SummonCreature(NPC_SON_OF_FLAME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 900000))
                                                    summoned->AI()->AttackStart(target);
											}
										}
                                        IsBanished = true;
                                        //DoCast(me, SPELL_RAGSUBMERGE);
                                        EmergeTimer = 90000;
                                    }
                                }
                                events.ScheduleEvent(EVENT_SUBMERGE, 180000);
                                break;
                            }
                            default:
                                break;
                        }
                    }

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ragnarosAI(creature);
        }
};

void AddSC_boss_ragnaros()
{
    new boss_ragnaros();
}