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
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "zulaman.h"

//Unimplemented SoundIDs
/*
#define SOUND_NALORAKK_EVENT1   12078
#define SOUND_NALORAKK_EVENT2   12079
*/

enum Texts
{
	SAY_AGGRO       = -1534086,
	SAY_DEATH       = -1534087,
	SAY_SLAY_1      = -1534088,
	SAY_SLAY_2      = -1534089,
	SAY_BERSERK     = -1534090,
	SAY_SURGE       = -1534091,
	SAY_WAVE_1      = -1534092,
	SAY_WAVE_2      = -1534093,
	SAY_WAVE_3      = -1534094,
	SAY_WAVE_4      = -1534095,
	SAY_BEAR_FORM   = -1534096,
	SAY_TROLL_FORM  = -1534097,
};

enum Spells
{
	SPELL_BERSERK           = 45078,
	// Troll form
	SPELL_BRUTAL_SWIPE      = 42384,
	SPELL_MANGLE            = 42389,
	SPELL_MANGLE_EFFECT     = 44955,
	SPELL_SURGE             = 42402,
	SPELL_BEAR_FORM         = 42377,
	// Bear form
	SPELL_LACERATING_SLASH  = 42395,
	SPELL_REND_FLESH        = 42397,
	SPELL_DEAFENING_ROAR    = 42398,
};

//Trash Waves
float NalorakkWay[8][3] =
{
    { 18.569f, 1414.512f, 11.42f}, // waypoint 1
    {-17.264f, 1419.551f, 12.62f},
    {-52.642f, 1419.357f, 27.31f}, // waypoint 2
    {-69.908f, 1419.721f, 27.31f},
    {-79.929f, 1395.958f, 27.31f},
    {-80.072f, 1374.555f, 40.87f}, // waypoint 3
    {-80.072f, 1314.398f, 40.87f},
    {-80.072f, 1295.775f, 48.60f} // waypoint 4
};

class boss_nalorakk : public CreatureScript
{
   public:
	   boss_nalorakk() : CreatureScript("boss_nalorakk") {}

        struct boss_nalorakkAI : public QuantumBasicAI
        {
            boss_nalorakkAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

				MoveEvent = true;
                MovePhase = 0;
            }

            InstanceScript* instance;

            uint32 BrutalSwipeTimer;
            uint32 MangleTimer;
            uint32 SurgeTimer;

            uint32 LaceratingSlashTimer;
            uint32 RendFleshTimer;
            uint32 DeafeningRoarTimer;

            uint32 ShapeShiftTimer;
            uint32 BerserkTimer;

            bool InBearForm;
            bool MoveEvent;
            bool InMove;

            uint32 MovePhase;
            uint32 WaitTimer;

            void Reset()
			{
				SurgeTimer = urand(15000, 20000);
                BrutalSwipeTimer = urand(7000, 12000);
                MangleTimer = urand(10000, 15000);
                ShapeShiftTimer = urand(45000, 50000);
                BerserkTimer = 10*MINUTE*IN_MILLISECONDS;

				InBearForm = false;

				instance->SetData(DATA_NALORAKK_EVENT, NOT_STARTED);

				me->RemoveAurasDueToSpell(SPELL_BEAR_FORM);

				if (MoveEvent)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    InMove = false;
                    WaitTimer = 0;
                    me->SetSpeed(MOVE_RUN, 2.5f);
                    me->SetWalk(false);                }
				else
                    me->GetMotionMaster()->MovePoint(0, NalorakkWay[7][0], NalorakkWay[7][1], NalorakkWay[7][2]);
            }

            void SendAttacker(Unit* target)
            {
                std::list<Creature*> templist;
                float x, y, z;
                me->GetPosition(x, y, z);
                {
                    CellCoord pair(Trinity::ComputeCellCoord(x, y));
                    Cell cell(pair);
                    cell.SetNoCreate();

                    Trinity::AllFriendlyCreaturesInGrid check(me);
                    Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(me, templist, check);

                    TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid>, GridTypeMapContainer> cSearcher(searcher);

                    cell.Visit(pair, cSearcher, *(me->GetMap()), *me, me->GetGridActivationRange());
                }

                if (templist.empty())
                    return;

                for (std::list<Creature*>::const_iterator i = templist.begin(); i != templist.end(); ++i)
                {
                    if ((*i) && me->IsWithinDistInMap((*i), 25.0f))
                    {
                        (*i)->SetNoCallAssistance(true);
                        (*i)->AI()->AttackStart(target);
                    }
                }
            }

            void AttackStart(Unit* who)
            {
                if (!MoveEvent)
                    QuantumBasicAI::AttackStart(who);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!MoveEvent)
                {
                    QuantumBasicAI::MoveInLineOfSight(who);
                }
                else
                {
                    if (me->IsHostileTo(who))
                    {
                        if (!InMove)
                        {
                            switch (MovePhase)
                            {
                                case 0:
                                    if (me->IsWithinDistInMap(who, 50.0f))
                                    {
										DoSendQuantumText(SAY_WAVE_1, me);
                                        me->GetMotionMaster()->MovePoint(1, NalorakkWay[1][0], NalorakkWay[1][1], NalorakkWay[1][2]);
                                        MovePhase ++;
                                        InMove = true;
                                        SendAttacker(who);
                                    }
                                    break;
                                case 2:
                                    if (me->IsWithinDistInMap(who, 40.0f))
                                    {
										DoSendQuantumText(SAY_WAVE_2, me);
                                        me->GetMotionMaster()->MovePoint(3, NalorakkWay[3][0], NalorakkWay[3][1], NalorakkWay[3][2]);
                                        MovePhase ++;
                                        InMove = true;
                                        SendAttacker(who);
                                    }
                                    break;
                                case 5:
                                    if (me->IsWithinDistInMap(who, 40.0f))
                                    {
										DoSendQuantumText(SAY_WAVE_3, me);
                                        me->GetMotionMaster()->MovePoint(6, NalorakkWay[6][0], NalorakkWay[6][1], NalorakkWay[6][2]);
                                        MovePhase ++;
                                        InMove = true;
                                        SendAttacker(who);
                                    }
                                    break;
                                case 7:
                                    if (me->IsWithinDistInMap(who, 50.0f))
                                    {
                                        SendAttacker(who);
										DoSendQuantumText(SAY_WAVE_4, me);
                                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                        MoveEvent = false;
                                    }
                                    break;
                            }
                        }
                    }
                }
            }

			void EnterToBattle(Unit* /*who*/)
            {
				DoSendQuantumText(SAY_AGGRO, me);

                DoZoneInCombat();

				me->SetPowerType(POWER_RAGE);
				me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

				instance->SetData(DATA_NALORAKK_EVENT, IN_PROGRESS);
            }

			void KilledUnit(Unit* /*victim*/)
            {
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
				DoSendQuantumText(SAY_DEATH, me);

				instance->SetData(DATA_NALORAKK_EVENT, DONE);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (MoveEvent)
                {
                    if (type != POINT_MOTION_TYPE)
                        return;

                    if (!InMove)
                        return;

                    if (MovePhase != id)
                        return;

                    switch (MovePhase)
                    {
                        case 2:
                            me->SetOrientation(3.1415f*2);
                            InMove = false;
                            return;
                        case 1:
                        case 3:
                        case 4:
                        case 6:
                            MovePhase ++;
                            WaitTimer = 1;
                            InMove = true;
                            return;
                        case 5:
                            me->SetOrientation(3.1415f*0.5f);
                            InMove = false;
                            return;
                        case 7:
                            me->SetOrientation(3.1415f*0.5f);
                            InMove = false;
                            return;
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (WaitTimer && InMove)
                {
                    if (WaitTimer <= diff)
                    {
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MovePoint(MovePhase, NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2]);
                        WaitTimer = 0;
                    }
					else WaitTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (BerserkTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK);
					DoSendQuantumText(SAY_BERSERK, me);
                    BerserkTimer = 10*MINUTE*IN_MILLISECONDS;
                }
				else BerserkTimer -= diff;

                if (ShapeShiftTimer <= diff)
                {
                    if (InBearForm)
                    {
						me->RemoveAurasDueToSpell(SPELL_BEAR_FORM);
						DoSendQuantumText(SAY_TROLL_FORM, me);

                        SurgeTimer = urand(15000, 20000);
                        BrutalSwipeTimer = urand(7000, 12000);
                        MangleTimer = urand(10000, 15000);
                        ShapeShiftTimer = urand(45000, 50000);

                        InBearForm = false;
                    }
                    else
                    {
						DoSendQuantumText(SAY_BEAR_FORM, me);

                        me->CastSpell(me, SPELL_BEAR_FORM, true);

                        LaceratingSlashTimer = 2000; // dur 18s
                        RendFleshTimer = 3000;  // dur 5s
                        DeafeningRoarTimer = urand(5000, 10000);  // dur 2s
                        ShapeShiftTimer = urand(20000, 25000); // dur 30s

                        InBearForm = true;
                    }
                }
				else ShapeShiftTimer -= diff;

                if (!InBearForm)
                {
                    if (BrutalSwipeTimer <= diff)
                    {
                        DoCastVictim(SPELL_BRUTAL_SWIPE);
                        BrutalSwipeTimer = urand(7000, 12000);
                    }
					else BrutalSwipeTimer -= diff;

                    if (MangleTimer <= diff)
                    {
						if (me->GetVictim() && !me->EnsureVictim()->HasAura(SPELL_MANGLE))
                        {
                            DoCastVictim(SPELL_MANGLE);
                            MangleTimer = 1000;
                        }
						else MangleTimer = urand(10000, 15000);
                    }
					else MangleTimer -= diff;

                    if (SurgeTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoSendQuantumText(SAY_SURGE, me);
                            DoCast(target, SPELL_SURGE);
							SurgeTimer = urand(15000, 20000);
						}
                    }
					else SurgeTimer -= diff;
                }
                else
                {
                    if (LaceratingSlashTimer <= diff)
                    {
                        DoCastVictim(SPELL_LACERATING_SLASH);
                        LaceratingSlashTimer = urand(18000, 23000);
                    }
					else LaceratingSlashTimer -= diff;

                    if (RendFleshTimer <= diff)
                    {
                        DoCastVictim(SPELL_REND_FLESH);
                        RendFleshTimer = urand(5000, 10000);
                    }
					else RendFleshTimer -= diff;

                    if (DeafeningRoarTimer <= diff)
                    {
                        DoCastAOE(SPELL_DEAFENING_ROAR);
                        DeafeningRoarTimer = urand(15000, 20000);
                    }
					else DeafeningRoarTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_nalorakkAI(creature);
        }
};

void AddSC_boss_nalorakk()
{
    new boss_nalorakk();
}