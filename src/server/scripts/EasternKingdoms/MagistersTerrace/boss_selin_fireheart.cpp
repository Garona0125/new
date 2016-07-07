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

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "magisters_terrace.h"

enum Misc
{
	CRYSTAL_NUMBER = 5,
};

enum Texts
{
	SAY_AGGRO                       = 0,
	SAY_ENERGY                      = 1,
	SAY_EMPOWERED                   = 2,
	SAY_KILL                        = 3,
	SAY_DEATH                       = 4,
	EMOTE_CRYSTAL                   = 5,
};

enum Spells
{
	SPELL_FEL_CRYSTAL_COSMETIC     = 44374,
	SPELL_FEL_CRYSTAL_DUMMY        = 44329,
	SPELL_FEL_CRYSTAL_VISUAL       = 44355,
	SPELL_MANA_RAGE                = 44320,
	SPELL_DRAIN_LIFE               = 44294,
	SPELL_FEL_EXPLOSION            = 44314,
	SPELL_DRAIN_MANA               = 46153,
};

class boss_selin_fireheart : public CreatureScript
{
public:
    boss_selin_fireheart() : CreatureScript("boss_selin_fireheart") { }

    struct boss_selin_fireheartAI : public QuantumBasicAI
    {
        boss_selin_fireheartAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

            Crystals.clear();

            if (instance)
            {
                uint32 size = instance->GetData(DATA_FEL_CRYSTAL_SIZE);
                for (uint8 i = 0; i < size; ++i)
                {
                    uint64 guid = instance->GetData64(DATA_FEL_CRYSTAL);
                    sLog->outDebug(LOG_FILTER_QCSCR, "QUANTUMCORE SCRIPTS: Selin: Adding Fel Crystal " UI64FMTD " to list", guid);
                    Crystals.push_back(guid);
                }
            }
        }

        InstanceScript* instance;
        std::list<uint64> Crystals;
		uint32 DelayDoorTimer;
        uint32 DrainLifeTimer;
        uint32 DrainManaTimer;
        uint32 FelExplosionTimer;
        uint32 DrainCrystalTimer;
        uint32 EmpowerTimer;

        bool IsDraining;
        bool DrainingCrystal;

        uint64 CrystalGUID;

        void Reset()
        {
            if (instance)
            {
                //for (uint8 i = 0; i < CRYSTALS_NUMBER; ++i)
                for (std::list<uint64>::const_iterator itr = Crystals.begin(); itr != Crystals.end(); ++itr)
                {
                    //Unit* unit = Unit::GetUnit(*me, FelCrystals[i]);
                    Unit* unit = Unit::GetUnit(*me, *itr);
                    if (unit)
                    {
                        if (!unit->IsAlive())
                            CAST_CRE(unit)->Respawn();

                        unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                instance->SetData(DATA_SELIN_EVENT, NOT_STARTED);
            }
			else
				sLog->OutErrorConsole(ERROR_INST_DATA);

			DelayDoorTimer = 0;
            DrainLifeTimer = urand(3000, 7000);
            DrainManaTimer = DrainLifeTimer + 5000;
            FelExplosionTimer = 2100;

            if (IsHeroic())
                DrainCrystalTimer = urand(10000, 15000);
            else
                DrainCrystalTimer = urand(20000, 25000);
            EmpowerTimer = 10000;

            IsDraining = false;
            DrainingCrystal = false;
            CrystalGUID = 0;
        }

        void SelectNearestCrystal()
        {
            if (Crystals.empty())
                return;

            //float ShortestDistance = 0;
            CrystalGUID = 0;
            Unit* crystal = NULL;
            Unit* CrystalChosen = NULL;
            //for (uint8 i =  0; i < CRYSTALS_NUMBER; ++i)
            for (std::list<uint64>::const_iterator itr = Crystals.begin(); itr != Crystals.end(); ++itr)
            {
                crystal = NULL;
                // crystal = Unit::GetUnit(*me, FelCrystals[i]);
                crystal = Unit::GetUnit(*me, *itr);
                if (crystal && crystal->IsAlive())
                {
                    if (!CrystalChosen || me->GetDistanceOrder(crystal, CrystalChosen, false))
                    {
                        CrystalGUID = crystal->GetGUID();
                        CrystalChosen = crystal;
                    }
                }
            }

            if (CrystalChosen)
            {
				Talk(SAY_ENERGY);
				Talk(EMOTE_CRYSTAL);

                CrystalChosen->CastSpell(CrystalChosen, SPELL_FEL_CRYSTAL_COSMETIC, true);

                float x, y, z;
                CrystalChosen->GetClosePoint(x, y, z, me->GetObjectSize(), CONTACT_DISTANCE);

                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(1, x, y, z);
                DrainingCrystal = true;
            }
        }

        void ShatterRemainingCrystals()
        {
            if (Crystals.empty())
                return;

            //for (uint8 i = 0; i < CRYSTALS_NUMBER; ++i)
            for (std::list<uint64>::const_iterator itr = Crystals.begin(); itr != Crystals.end(); ++itr)
            {
                //Creature* crystal = (Unit::GetCreature(*me, FelCrystals[i]));
                Creature* crystal = Unit::GetCreature(*me, *itr);
                if (crystal && crystal->IsAlive())
                    crystal->Kill(crystal);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
			DelayDoorTimer = 0;
		}

        void KilledUnit(Unit* /*who*/)
        {
            Talk(SAY_KILL);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
            {
                Unit* CrystalChosen = Unit::GetUnit(*me, CrystalGUID);
                if (CrystalChosen && CrystalChosen->IsAlive())
                {
                    CrystalChosen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                    CrystalChosen->CastSpell(me, SPELL_MANA_RAGE, true);
                    IsDraining = true;
                }
                else
                {
                    sLog->OutErrorConsole("Selin Fireheart unable to drain crystal as the crystal is either dead or despawned");
                    DrainingCrystal = false;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);

            if (!instance)
                return;

            instance->SetData(DATA_SELIN_EVENT, DONE);         // Encounter complete!
            ShatterRemainingCrystals();
        }

		void DelayCloseDoor()
		{
			if (instance)
				instance->SetData(DATA_SELIN_EVENT, IN_PROGRESS);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (DelayDoorTimer && DelayDoorTimer > 5000)
				DelayCloseDoor();
			else DelayDoorTimer+= diff;

            if (!DrainingCrystal)
            {
                uint32 maxPowerMana = me->GetMaxPower(POWER_MANA);
                if (maxPowerMana && ((me->GetPower(POWER_MANA)*100 / maxPowerMana) < 10))
                {
                    if (DrainLifeTimer <= diff)
                    {
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_DRAIN_LIFE);
							DrainLifeTimer = 10000;
						}
                    }
					else DrainLifeTimer -= diff;

                    if (IsHeroic())
                    {
                        if (DrainManaTimer <= diff)
                        {
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								DoCast(target, SPELL_DRAIN_MANA);
								DrainManaTimer = 10000;
							}
                        }
						else DrainManaTimer -= diff;
                    }
                }

                if (FelExplosionTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_FEL_EXPLOSION);
                        FelExplosionTimer = 2000;
                    }
                }
				else FelExplosionTimer -= diff;

                maxPowerMana = me->GetMaxPower(POWER_MANA);
                if (maxPowerMana && ((me->GetPower(POWER_MANA)*100 / maxPowerMana) < 10))
                {
                    if (DrainCrystalTimer <= diff)
                    {
                        SelectNearestCrystal();
                        if (IsHeroic())
                            DrainCrystalTimer = urand(10000, 15000);
                        else
                            DrainCrystalTimer = urand(20000, 25000);
                    }
					else DrainCrystalTimer -= diff;
                }
            }
			else
            {
                if (IsDraining)
                {
                    if (EmpowerTimer <= diff)
                    {
                        IsDraining = false;
                        DrainingCrystal = false;

						Talk(SAY_EMPOWERED);

                        Unit* CrystalChosen = Unit::GetUnit(*me, CrystalGUID);

                        if (CrystalChosen && CrystalChosen->IsAlive())

							CrystalChosen->Kill(CrystalChosen);
						CrystalGUID = 0;
						
						me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
					else EmpowerTimer -= diff;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_selin_fireheartAI(creature);
    }
};

class npc_fel_crystal : public CreatureScript
{
public:
    npc_fel_crystal() : CreatureScript("npc_fel_crystal") { }

    struct npc_fel_crystalAI : public QuantumBasicAI
    {
        npc_fel_crystalAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset() {}

        void EnterToBattle(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 /*diff*/) {}

        void JustDied(Unit* /*killer*/)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
            {
                Creature* Selin = (Unit::GetCreature(*me, instance->GetData64(DATA_SELIN)));
                if (Selin && Selin->IsAlive())
                {
                    if (CAST_AI(boss_selin_fireheart::boss_selin_fireheartAI, Selin->AI())->CrystalGUID == me->GetGUID())
                    {
                        CAST_AI(boss_selin_fireheart::boss_selin_fireheartAI, Selin->AI())->DrainingCrystal = false;
                        CAST_AI(boss_selin_fireheart::boss_selin_fireheartAI, Selin->AI())->IsDraining = false;
                        CAST_AI(boss_selin_fireheart::boss_selin_fireheartAI, Selin->AI())->EmpowerTimer = 10000;
                        if (Selin->GetVictim())
                        {
                            Selin->AI()->AttackStart(Selin->GetVictim());
                            Selin->GetMotionMaster()->MoveChase(Selin->GetVictim());
                        }
                    }
                }
            }
			else sLog->OutErrorConsole(ERROR_INST_DATA);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fel_crystalAI (creature);
    }
};

void AddSC_boss_selin_fireheart()
{
    new boss_selin_fireheart();
    new npc_fel_crystal();
}