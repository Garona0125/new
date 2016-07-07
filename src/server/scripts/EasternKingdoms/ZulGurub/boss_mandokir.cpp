/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Mandokir
SD%Complete: 90
SDComment: Ohgan function needs improvements.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "zulgurub.h"

enum Says
{
	SAY_AGGRO               = 0,
	SAY_DING_KILL           = 1,
	SAY_WATCH               = 2,
	SAY_WATCH_WHISPER       = 3,                    //is this text for real? easter egg?
	SAY_GRATS_JINDO         = 0,
};

enum Spells
{
	SPELL_CHARGE           = 24408,
	SPELL_CLEAVE           = 7160,
	SPELL_FEAR             = 29321,
	SPELL_WHIRLWIND        = 15589,
	SPELL_MORTAL_STRIKE    = 16856,
	SPELL_ENRAGE           = 24318,
	SPELL_WATCH            = 24314,
	SPELL_LEVEL_UP         = 24312,
	SPELL_SUNDER_ARMOR     = 24317,
	SPELL_ORANGE_RAPTOR    = 23243,
	// Vilebranch Speaker
	SPELL_CLEAVE_T           = 15284,
	SPELL_DEMORALIZING_SHOUT = 13730,
	SPELL_ENRAGE_T           = 8599,
};

enum Madokir
{
	NPC_SPEAKER  = 11391,
	NPC_MANDOKIR = 11382,
};

class boss_mandokir : public CreatureScript
{
    public:
        boss_mandokir() : CreatureScript("boss_mandokir") {}

        struct boss_mandokirAI : public QuantumBasicAI
        {
            boss_mandokirAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 KillCount;
            uint32 WatchTimer;
            uint32 TargetInRange;
            uint32 CleaveTimer;
            uint32 WhirlwindTimer;
            uint32 FearTimer;
            uint32 MortalStrikeTimer;
            uint32 CheckTimer;
            float targetX;
            float targetY;
            float targetZ;

            InstanceScript* instance;

            bool endWatch;
            bool someWatched;
            bool RaptorDead;
            bool CombatStart;
			bool SpeakerDead;

            uint64 WatchTarget;

            void Reset()
            {
                KillCount = 0;
                WatchTimer = 33000;
                CleaveTimer = 7000;
                WhirlwindTimer = 20000;
                FearTimer = 1000;
                MortalStrikeTimer = 1000;
                CheckTimer = 1000;

                targetX = 0.0f;
                targetY = 0.0f;
                targetZ = 0.0f;
                TargetInRange = 0;

                WatchTarget = 0;

                someWatched = false;
                endWatch = false;
                RaptorDead = false;
                CombatStart = false;
				SpeakerDead = false;

                DoCast(me, SPELL_ORANGE_RAPTOR);
				me->SummonCreature(NPC_SPEAKER, -12196.299f, -1948.369f, 130.36f, 0.541052f, TEMPSUMMON_MANUAL_DESPAWN);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPE_ID_PLAYER)
                {
                    ++KillCount;

                    if (KillCount == 3)
                    {
						Talk(SAY_DING_KILL);

                        if (instance)
                        {
                            uint64 JindoGUID = instance->GetData64(DATA_JINDO);
                            if (JindoGUID)
                            {
                                if (Creature* jTemp = Creature::GetCreature(*me, JindoGUID))
                                {
                                    if (jTemp->IsAlive())
										jTemp->AI()->Talk(SAY_GRATS_JINDO);
                                }
                            }
                        }
						DoCast(me, SPELL_LEVEL_UP, true);
						KillCount = 0;
					}
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
				Talk(SAY_AGGRO);
            }

            void UpdateAI(const uint32 diff)
            {
				if (!SpeakerDead)
				{
					if (!me->FindCreatureWithDistance(NPC_SPEAKER, 100.0f, true))
					{
						me->GetMotionMaster()->MovePoint(0, -12196.3f, -1948.37f, 130.36f);
						SpeakerDead = true;
					}
				}

				if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE && SpeakerDead)
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

                if (!UpdateVictim())
                    return;

                if (me->GetVictim() && me->IsAlive())
                {
                    if (!CombatStart)
                    {
                        //At combat Start Mandokir is mounted so we must unmount it first
                        me->RemoveMount();

                        //And summon his raptor
                        me->SummonCreature(14988, me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 35000);
                        CombatStart = true;
                    }

                    if (WatchTimer <= diff)                         //Every 20 Sec Mandokir will check this
                    {
                        if (WatchTarget)                             //If someone is watched and If the Position of the watched target is different from the one stored, or are attacking, mandokir will charge him
                        {
                            Unit* unit = Unit::GetUnit(*me, WatchTarget);

                            if (unit && (
                                targetX != unit->GetPositionX() ||
                                targetY != unit->GetPositionY() ||
                                targetZ != unit->GetPositionZ() ||
                                unit->IsInCombatActive()))
							{
								if (me->IsWithinMeleeRange(unit))
                                {
                                    DoCast(unit, 24316);
                                }
                                else
                                {
                                    DoCast(unit, SPELL_CHARGE);
                                    //me->SendMonsterMove(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), 0, true, 1);
                                    AttackStart(unit);
                                }
                            }
                        }
                        someWatched = false;
                        WatchTimer = 20000;
                    }
					else WatchTimer -= diff;

                    if ((WatchTimer <= 8000) && !someWatched)       //8 sec(cast time + expire time) before the check for the watch effect mandokir will cast watch debuff on a random target
                    {
                        if (Unit* watcher = SelectTarget(TARGET_RANDOM, 0))
                        {
							Talk(SAY_WATCH, watcher->GetGUID());
                            DoCast(watcher, SPELL_WATCH);
                            WatchTarget = watcher->GetGUID();
                            someWatched = true;
                            endWatch = true;
                        }
                    }

                    if ((WatchTimer <= 1000) && endWatch)           //1 sec before the debuf expire, store the target position
                    {
                        Unit* unit = Unit::GetUnit(*me, WatchTarget);
                        if (unit)
                        {
                            targetX = unit->GetPositionX();
                            targetY = unit->GetPositionY();
                            targetZ = unit->GetPositionZ();
                        }
                        endWatch = false;
                    }

                    if (!someWatched)
                    {
                        //Cleave
                        if (CleaveTimer <= diff)
                        {
                            DoCastVictim(SPELL_CLEAVE);
                            CleaveTimer = 7000;
                        }
						else CleaveTimer -= diff;

                        //Whirlwind
                        if (WhirlwindTimer <= diff)
                        {
                            DoCast(me, SPELL_WHIRLWIND);
                            WhirlwindTimer = 18000;
                        }
						else WhirlwindTimer -= diff;

                        //If more then 3 targets in melee range mandokir will cast fear
                        if (FearTimer <= diff)
                        {
                            TargetInRange = 0;

                            std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                            for (; i != me->getThreatManager().getThreatList().end(); ++i)
                            {
                                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                                if (unit && me->IsWithinMeleeRange(unit))
                                    ++TargetInRange;
                            }

                            if (TargetInRange > 3)
                                DoCastVictim(SPELL_FEAR);

                            FearTimer = 4000;
                        }
						else FearTimer -=diff;

                        //Mortal Strike if target below 50% hp
                        if (me->GetVictim() && me->GetVictim()->HealthBelowPct(HEALTH_PERCENT_50))
                        {
                            if (MortalStrikeTimer <= diff)
                            {
                                DoCastVictim(SPELL_MORTAL_STRIKE);
                                MortalStrikeTimer = 15000;
                            }
							else MortalStrikeTimer -= diff;
                        }
                    }

                    if (CheckTimer <= diff)
                    {
                        if (instance)
                        {
                            if (instance->GetData(DATA_OHGAN) == DONE)
                            {
                                if (!RaptorDead)
                                {
                                    DoCast(me, SPELL_ENRAGE);
                                    RaptorDead = true;
                                }
                            }
                        }

                        CheckTimer = 1000;
                    }
					else CheckTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_mandokirAI(creature);
        }
};

//Ohgan
class mob_ohgan : public CreatureScript
{
    public:
        mob_ohgan() : CreatureScript("mob_ohgan") {}

        struct mob_ohganAI : public QuantumBasicAI
        {
            mob_ohganAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 SunderArmorTimer;
            InstanceScript* instance;

            void Reset()
            {
                SunderArmorTimer = 5000;
            }

            void EnterToBattle(Unit* /*who*/) {}

            void JustDied(Unit* /*Killer*/)
            {
                if (instance)
                    instance->SetData(DATA_OHGAN, DONE);
            }

            void UpdateAI (const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (SunderArmorTimer <= diff)
                {
                    DoCastVictim(SPELL_SUNDER_ARMOR);
                    SunderArmorTimer = urand(10000, 15000);
                }
				else SunderArmorTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_ohganAI(creature);
        }
};

class npc_vilebranch_speaker : public CreatureScript
{
public:
	npc_vilebranch_speaker() : CreatureScript("npc_vilebranch_speaker") { }

	struct npc_vilebranch_speakerAI : public QuantumBasicAI
	{
		npc_vilebranch_speakerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		uint32 CleaveTimer;
		uint32 EnrageTimer;
		uint32 DemoralizingShoutTimer;
		InstanceScript* instance;

		void Reset()
		{
			CleaveTimer = 2000;
			EnrageTimer = 10000;
			DemoralizingShoutTimer = 3500;
		}

		void EnterToBattle(Unit* /*who*/) {}

            void JustDied(Unit* killer)
            {
				if (Creature* mandokir = me->FindCreatureWithDistance(NPC_MANDOKIR, 90.0f))
				{
					mandokir->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_NON_ATTACKABLE);
					mandokir->CombatStart(killer);
					mandokir->AddThreat(killer, 100.0f);
				}
   
			}

            void UpdateAI (const uint32 diff)
            {
				if (!UpdateVictim())
                    return;

                if (CleaveTimer <= diff)
                {
                    DoCastVictim(SPELL_CLEAVE_T);
                    CleaveTimer = urand(2000, 3000);
                }
				else CleaveTimer -= diff;

				if (EnrageTimer <= diff)
                {
                    DoCast(me, SPELL_ENRAGE_T);
                    EnrageTimer = urand(20000, 30000);
                }
				else EnrageTimer -= diff;

				if (DemoralizingShoutTimer <= diff)
                {
                    DoCastVictim(SPELL_DEMORALIZING_SHOUT);
                    DemoralizingShoutTimer = urand(10000, 20000);
                } 
				else DemoralizingShoutTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_vilebranch_speakerAI(creature);
        }
};

void AddSC_boss_mandokir()
{
    new boss_mandokir();
    new mob_ohgan();
	new npc_vilebranch_speaker();
}