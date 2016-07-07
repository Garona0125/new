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
SDName: Boss_Hakkar
SD%Complete: 95
SDComment: Blood siphon spell buggy cause of Core Issue.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "zulgurub.h"

enum Texts
{
	SAY_AGGRO                   = 0,
	SAY_FLEEING                 = 1,
	SAY_MINION_DESTROY          = 2,
	SAY_PROTECT_ALTAR           = 3,
};

enum Spells
{
	SPELL_BLOODSIPHON           = 24322,
	SPELL_CORRUPTEDBLOOD        = 24328,
	SPELL_CAUSEINSANITY         = 24327,
	SPELL_WILLOFHAKKAR          = 24178,
	SPELL_ENRAGE                = 24318,
	// The Aspects of all High Priests
	SPELL_ASPECT_OF_JEKLIK      = 24687,
	SPELL_ASPECT_OF_VENOXIS     = 24688,
	SPELL_ASPECT_OF_MARLI       = 24686,
	SPELL_ASPECT_OF_THEKAL      = 24689,
	SPELL_ASPECT_OF_ARLOKK      = 24690,
};

class boss_hakkar : public CreatureScript
{
    public:
        boss_hakkar() : CreatureScript("boss_hakkar") {}

        struct boss_hakkarAI : public QuantumBasicAI
        {
            boss_hakkarAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 BloodSiphonTimer;
            uint32 CorruptedBloodTimer;
            uint32 CauseInsanityTimer;
            uint32 WillOfHakkarTimer;
            uint32 EnrageTimer;

            uint32 CheckJeklikTimer;
            uint32 CheckVenoxisTimer;
            uint32 CheckMarliTimer;
            uint32 CheckThekalTimer;
            uint32 CheckArlokkTimer;

            uint32 AspectOfJeklikTimer;
            uint32 AspectOfVenoxisTimer;
            uint32 AspectOfMarliTimer;
            uint32 AspectOfThekalTimer;
            uint32 AspectOfArlokkTimer;

            bool Enraged;

            void Reset()
            {
                BloodSiphonTimer = 90000;
                CorruptedBloodTimer = 25000;
                CauseInsanityTimer = 17000;
                WillOfHakkarTimer = 17000;
                EnrageTimer = 600000;

                CheckJeklikTimer = 1000;
                CheckVenoxisTimer = 2000;
                CheckMarliTimer = 3000;
                CheckThekalTimer = 4000;
                CheckArlokkTimer = 5000;

                AspectOfJeklikTimer = 4000;
                AspectOfVenoxisTimer = 7000;
                AspectOfMarliTimer = 12000;
                AspectOfThekalTimer = 8000;
                AspectOfArlokkTimer = 18000;

                Enraged = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
				Talk(SAY_AGGRO);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //BloodSiphonTimer
                if (BloodSiphonTimer <= diff)
                {
                    DoCastVictim(SPELL_BLOODSIPHON);
                    BloodSiphonTimer = 90000;
                }
				else BloodSiphonTimer -= diff;

                //CorruptedBloodTimer
                if (CorruptedBloodTimer <= diff)
                {
                    DoCastVictim(SPELL_CORRUPTEDBLOOD);
                    CorruptedBloodTimer = urand(30000, 45000);
                }
				else CorruptedBloodTimer -= diff;

                //CauseInsanityTimer
                /*if (CauseInsanityTimer <= diff)
                {
                if (Unit* target = SelectUnit(TARGET_RANDOM, 0))
                DoCast(target, SPELL_CAUSEINSANITY);

                CauseInsanityTimer = urand(35000, 43000);
                } else CauseInsanityTimer -= diff;*/

                //WillOfHakkarTimer
                if (WillOfHakkarTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        DoCast(target, SPELL_WILLOFHAKKAR);

                    WillOfHakkarTimer = urand(25000, 35000);
                }
				else WillOfHakkarTimer -= diff;

                if (!Enraged && EnrageTimer <= diff)
                {
                    DoCast(me, SPELL_ENRAGE);
                    Enraged = true;
                }
				else EnrageTimer -= diff;

                //Checking if Jeklik is dead. If not we cast her Aspect
                if (CheckJeklikTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_JEKLIK) != DONE)
                        {
                            if (AspectOfJeklikTimer <= diff)
                            {
                                DoCastVictim(SPELL_ASPECT_OF_JEKLIK);
                                AspectOfJeklikTimer = urand(10000, 14000);
                            }
							else AspectOfJeklikTimer -= diff;
                        }
                    }
                    CheckJeklikTimer = 1000;
                }
				else CheckJeklikTimer -= diff;

                //Checking if Venoxis is dead. If not we cast his Aspect
                if (CheckVenoxisTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_VENOXIS) != DONE)
                        {
                            if (AspectOfVenoxisTimer <= diff)
                            {
                                DoCastVictim(SPELL_ASPECT_OF_VENOXIS);
                                AspectOfVenoxisTimer = 8000;
                            }
							else AspectOfVenoxisTimer -= diff;
                        }
                    }
                    CheckVenoxisTimer = 1000;
                }
				else CheckVenoxisTimer -= diff;

                //Checking if Marli is dead. If not we cast her Aspect
                if (CheckMarliTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_MARLI) != DONE)
                        {
                            if (AspectOfMarliTimer <= diff)
                            {
                                DoCastVictim(SPELL_ASPECT_OF_MARLI);
                                AspectOfMarliTimer = 10000;
                            }
							else AspectOfMarliTimer -= diff;
                        }
                    }
                    CheckMarliTimer = 1000;
                }
				else CheckMarliTimer -= diff;

                //Checking if Thekal is dead. If not we cast his Aspect
                if (CheckThekalTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_THEKAL) != DONE)
                        {
                            if (AspectOfThekalTimer <= diff)
                            {
                                DoCast(me, SPELL_ASPECT_OF_THEKAL);
                                AspectOfThekalTimer = 15000;
                            }
							else AspectOfThekalTimer -= diff;
                        }
                    }
                    CheckThekalTimer = 1000;
                }
				else CheckThekalTimer -= diff;

                //Checking if Arlokk is dead. If yes we cast her Aspect
                if (CheckArlokkTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_ARLOKK) != DONE)
                        {
                            if (AspectOfArlokkTimer <= diff)
                            {
                                DoCast(me, SPELL_ASPECT_OF_ARLOKK);
                                DoResetThreat();

                                AspectOfArlokkTimer = urand(10000, 15000);
                            }
							else AspectOfArlokkTimer -= diff;
                        }
                    }
                    CheckArlokkTimer = 1000;
                }
				else CheckArlokkTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_hakkarAI(creature);
        }
};

void AddSC_boss_hakkar()
{
    new boss_hakkar();
}