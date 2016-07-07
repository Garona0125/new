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
#include "shadow_labyrinth.h"

enum Texts
{
	SAY_INTRO_1        = -1555008,
	SAY_INTRO_2        = -1555009,
	SAY_INTRO_3        = -1555010,
	SAY_AGGRO_1        = -1555011,
	SAY_AGGRO_2        = -1555012,
	SAY_AGGRO_3        = -1555013,
	SAY_SLAY_1         = -1555014,
	SAY_SLAY_2         = -1555015,
	SAY_HELP           = -1555016,
	SAY_DEATH          = -1555017,
	SAY_2_INTRO_1      = -1555018,
	SAY_2_INTRO_2      = -1555019,
	SAY_2_INTRO_3      = -1555020,
	SAY_2_AGGRO_1      = -1555021,
	SAY_2_AGGRO_2      = -1555022,
	SAY_2_AGGRO_3      = -1555023,
	SAY_2_SLAY_1       = -1555024,
	SAY_2_SLAY_2       = -1555025,
	SAY_2_HELP         = -1555026,
	SAY_2_DEATH        = -1555027,
};

enum Spells
{
	SPELL_INCITE_CHAOS   = 33676,
	SPELL_INCITE_CHAOS_B = 33684,
	SPELL_CHARGE         = 33709,
	SPELL_WAR_STOMP      = 33707,
};

class boss_blackheart_the_inciter : public CreatureScript
{
    public:
        boss_blackheart_the_inciter() : CreatureScript("boss_blackheart_the_inciter") { }

        struct boss_blackheart_the_inciterAI : public QuantumBasicAI
        {
            boss_blackheart_the_inciterAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;

            bool InciteChaos;
			bool Intro;
			bool Greet;

			uint32 ChargeTimer;
            uint32 InciteChaosTimer;
            uint32 InciteChaosWaitTimer;
            uint32 WarStompTimer;
			uint32 IntroTimer;
			uint32 IntroStep;

            void Reset()
            {
                InciteChaos = false;
				Intro = false;
				Greet = false;

				ChargeTimer = 500;
                InciteChaosTimer = 20000;
                InciteChaosWaitTimer = 15000;
                WarStompTimer = 15000;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);

				instance->SetData(DATA_BLACK_HEART_THE_INCITER, NOT_STARTED);
            }

			void MoveInLineOfSight(Unit* who)
			{
				if (Greet)
					return;

				if (me->IsWithinDistInMap(who, 35.0f) && who->IsAlive() && who->GetTypeId() == TYPE_ID_PLAYER && !who->ToPlayer()->IsGameMaster())
				{
					IntroStep = 1;
					IntroTimer = 100;
					IntroEvent();
					Greet = true;
				}
			}

			void IntroEvent()
			{
				if (!IntroStep)
					return;

				if (Intro)
					return;

				switch (IntroStep)
				{
			        case 1:
						DoSendQuantumText(SAY_INTRO_1, me);
						JumpIntro(5000);
						break;
					case 2:
						DoSendQuantumText(SAY_INTRO_2, me);
						JumpIntro(5000);
						break;
					case 3:
						DoSendQuantumText(SAY_INTRO_3, me);
						JumpIntro(5000);
						break;
					case 4:
						DoSendQuantumText(SAY_2_INTRO_1, me);
						JumpIntro(5000);
						break;
					case 5:
						DoSendQuantumText(SAY_2_INTRO_2, me);
						JumpIntro(5000);
						break;
					case 6:
						DoSendQuantumText(SAY_2_INTRO_3, me);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
						Intro = true;
						break;
					default:
						break;
				}
			}

			void JumpIntro(uint32 TimeIntro)
			{
				IntroTimer = TimeIntro;
				IntroStep++;
			}

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_2_SLAY_1, SAY_2_SLAY_2), me);
            }

            void JustDied(Unit* /*victim*/)
            {
                DoSendQuantumText(SAY_DEATH, me);

				instance->SetData(DATA_BLACK_HEART_THE_INCITER, DONE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3, SAY_2_AGGRO_1, SAY_2_AGGRO_2, SAY_2_AGGRO_3), me);

				instance->SetData(DATA_BLACK_HEART_THE_INCITER, IN_PROGRESS);
            }

            void UpdateAI(uint32 const diff)
            {
				if (IntroTimer < diff)
					IntroEvent();
				else
					IntroTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (InciteChaos)
                {
                    if (InciteChaosWaitTimer <= diff)
                    {
                        InciteChaos = false;
                        InciteChaosWaitTimer = 15000;
                    }
                    else InciteChaosWaitTimer -= diff;
                    return;
                }

                if (InciteChaosTimer <= diff)
                {
                    DoCast(me, SPELL_INCITE_CHAOS, true);

					std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
					for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
					{
						if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
						{
							if (target->GetTypeId() == TYPE_ID_PLAYER)
								me->CastSpell(target, SPELL_INCITE_CHAOS_B, true);
						}
					}
					DoResetThreat();
                    InciteChaos = true;
                    InciteChaosTimer = 40000;
                    return;
                }
                else InciteChaosTimer -= diff;

                if (ChargeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_CHARGE);
						ChargeTimer = urand(15000, 25000);
					}
                }
                else ChargeTimer -= diff;

                if (WarStompTimer <= diff)
                {
                    DoCastAOE(SPELL_WAR_STOMP);
                    WarStompTimer = urand(18000, 24000);
                }
                else WarStompTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_blackheart_the_inciterAI(creature);
        }
};

void AddSC_boss_blackheart_the_inciter()
{
    new boss_blackheart_the_inciter();
}