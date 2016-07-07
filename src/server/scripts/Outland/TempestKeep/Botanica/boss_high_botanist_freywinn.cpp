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
#include "GameEventMgr.h"
#include "the_botanica.h"

enum Texts
{
    SAY_AGGRO              = -1441008,
    SAY_SLAY_1             = -1441009,
	SAY_SLAY_2             = -1441010,
    SAY_TREE               = -1441011,
    SAY_SUMMON             = -1441012,
    SAY_DEATH              = -1441013,
    SAY_RANDOM_1           = -1441014,
	SAY_RANDOM_2           = -1441015,
	SAY_RANDOM_3           = -1441016,
	SAY_RANDOM_4           = -1441017,
};

enum Spells
{
    SPELL_TRANQUILITY          = 34550,
    SPELL_TREE_FORM            = 34551,
    SPELL_SUMMON_FRAYER        = 34557,
    SPELL_PLANT_WHITE          = 34759,
    SPELL_PLANT_GREEN          = 34761,
    SPELL_PLANT_BLUE           = 34762,
    SPELL_PLANT_RED            = 34763,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_high_botanist_freywinn : public CreatureScript
{
    public:
        boss_high_botanist_freywinn(): CreatureScript("boss_high_botanist_freywinn"){}

        struct boss_high_botanist_freywinnAI : public BossAI
        {
            boss_high_botanist_freywinnAI(Creature* creature) : BossAI(creature, DATA_HIGH_BOTANIST_FREYWINN) { }

            std::list<uint64> SummonList;

			uint32 RandomSayTimer;
            uint32 SummonSeedlingTimer;
            uint32 TreeFormTimer;
            uint32 MoveCheckTimer;
            uint32 DeadAddsCount;
            bool MoveFree;

            void Reset()
            {
                SummonList.clear();

				RandomSayTimer = 500;
                SummonSeedlingTimer = 6000;
                TreeFormTimer = 30000;
                MoveCheckTimer = 1000;
                DeadAddsCount = 0;
                MoveFree = true;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
					me->SetDisplayId(MODEL_CHRISTMAS_BOTANIST_FREYWINN);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);
            }

            void JustSummoned(Creature* summoned)
            {
                if (summoned->GetEntry() == NPC_FRAYER)
                    SummonList.push_back(summoned->GetGUID());
            }

            void DoSummonSeedling()
            {
				DoSendQuantumText(SAY_SUMMON, me);

                switch (rand()%4)
                {
                    case 0:
						DoCast(me, SPELL_PLANT_WHITE);
						break;
                    case 1:
						DoCast(me, SPELL_PLANT_GREEN);
						break;
                    case 2:
						DoCast(me, SPELL_PLANT_BLUE);
						break;
                    case 3:
						DoCast(me, SPELL_PLANT_RED);
						break;
                }
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
            }

            void UpdateAI(uint32 const diff)
            {
				// Out of Combat Timer
				if (RandomSayTimer <= diff && !me->IsInCombatActive())
				{
					DoSendQuantumText(RAND(SAY_RANDOM_1, SAY_RANDOM_2, SAY_RANDOM_3, SAY_RANDOM_4), me);
					RandomSayTimer = 60000; // 1 Minutes
				}
				else RandomSayTimer -= diff;

                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (TreeFormTimer <= diff)
                {
                    DoSendQuantumText(SAY_TREE, me);

                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(true);

                    me->RemoveAllAuras();

                    DoCast(me, SPELL_SUMMON_FRAYER, true);
                    DoCast(me, SPELL_TRANQUILITY, true);
                    DoCast(me, SPELL_TREE_FORM, true);

                    me->GetMotionMaster()->MoveIdle();
                    MoveFree = false;

                    TreeFormTimer = 75000;
                }
                else TreeFormTimer -= diff;

                if (!MoveFree)
                {
                    if (MoveCheckTimer <= diff)
                    {
                        if (!SummonList.empty())
                        {
                            for (std::list<uint64>::iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                            {
                                if (Unit* temp = Unit::GetUnit(*me, *itr))
                                {
                                    if (!temp->IsAlive())
                                    {
                                        SummonList.erase(itr);
                                        ++DeadAddsCount;
                                        break;
                                    }
                                }
                            }
                        }

                        if (DeadAddsCount < 3 && TreeFormTimer-30000 <= diff)
                            DeadAddsCount = 3;

                        if (DeadAddsCount >= 3)
                        {
                            SummonList.clear();
                            DeadAddsCount = 0;

                            me->InterruptNonMeleeSpells(true);
                            me->RemoveAllAuras();
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            MoveFree = true;
                        }
                        MoveCheckTimer = 500;
                    }
                    else MoveCheckTimer -= diff;
                    return;
                }

                /*if (me->HasAura(SPELL_TREE_FORM, 0) || me->HasAura(SPELL_TRANQUILITY, 0))
                    return;*/

                //one random seedling every 5 secs, but not in tree form
                if (SummonSeedlingTimer <= diff)
                {
                    DoSummonSeedling();
                    SummonSeedlingTimer = 6000;
                }
                else SummonSeedlingTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_high_botanist_freywinnAI(creature);
        }
};

void AddSC_boss_high_botanist_freywinn()
{
    new boss_high_botanist_freywinn();
}