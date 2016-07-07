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
#include "shattered_halls.h"

enum Spells
{
	SPELL_EXECUTION_TIMER_BUFF_1 = 39288,
	SPELL_EXECUTION_TIMER_BUFF_2 = 39289,
	SPELL_EXECUTION_TIMER_BUFF_3 = 39290,
};

float ExecutionerPos[3] = {164.53f,-82.73f,1.93f};

float VictimPos[3][3] =
{
    {149.77f,-77.59f,1.93f},
    {143.97f,-84.15f,1.93f},
    {149.77f,-90.33f,1.93f},
};

class mob_shattered_hand_executioner : public CreatureScript
{
public:
    mob_shattered_hand_executioner() : CreatureScript("mob_shattered_hand_executioner") { }

    struct  mob_shattered_hand_executionerAI : public QuantumBasicAI
    {   
        mob_shattered_hand_executionerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            for(int i = 0; i < 3; i++)
                victim[i] = 0;

            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        bool eventstarted;
        uint32 phase;

        uint32 CheckTimer;

        uint64 victim[3];

        uint32 GetFactionOfPlayersInInstance()
        {
            uint32 faction = 1;

            Map* map = me->GetMap();
            if (map->IsDungeon())
            {
                InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
                for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* player = i->getSource())
                    {
                        faction = player->GetFaction();
                        return faction;
                    }
                }
            }
            return 0;
        }

        void SpawnVictims(uint32 phase)
        {
            uint32 faction = GetFactionOfPlayersInInstance();
            bool isHorde = (faction == 1610 || faction == 2 || faction == 6 || faction == 116 || faction == 5) ;

            Creature* temp;
            if (phase < 3)
                eventstarted = true;

            if (phase > 2 && victim[0] == 0)
            {
                temp = me->SummonCreature(isHorde ? NPC_SCOUT_ORGARR : NPC_RIFLEMAN_BROWNBEARD,VictimPos[0][0],VictimPos[0][1],VictimPos[0][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
                temp->SetActive(true);
                victim[0] = temp->GetGUID();
            }

            if (phase > 1 && victim[1] == 0)
            {
                temp = me->SummonCreature(isHorde ? NPC_KORAG : NPC_PROVATE_JACINT,VictimPos[1][0],VictimPos[1][1],VictimPos[1][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
                temp->SetActive(true);
                victim[1] = temp->GetGUID();
            }

            if (phase > 0 && victim[2] == 0)
            {
                temp = me->SummonCreature(isHorde ? NPC_CAPT_BONESHATTER : NPC_CAPT_ALINA,VictimPos[2][0],VictimPos[2][1],VictimPos[2][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
                temp->SetActive(true);
                victim[2] = temp->GetGUID();
            }
        }

        void executeVictim()
        {
            if (me->GetVictim())
                return;

            uint64 tVictim;
            switch (phase)
            {
            case 3:
				tVictim = victim[0];
				break;
            case 2:
				tVictim = victim[1];
				break;
            case 1:
				tVictim = victim[2];
				break;
            }
            Unit* temp = Unit::GetUnit(*me,tVictim);
            if (temp)
            {
                //error_log("Victim Attack Entry: %d GUID: %d",temp->GetEntry(),temp->GetGUID());
                //me->getThreatManager().addThreat(temp,0.0f);
                me->AI()->AttackStart(temp);
            }
            //executing = true; 
                        //me->AI()->AttackStart(temp);
        }

        void Reset()
        {
            me->SetActive(true);
            phase = 0;

            if (instance)
            {
                phase = instance->GetData(EXECUTION_PHASE);

                //error_log("Phase %d Loaded ", phase);
                SpawnVictims(phase);
                uint32 status = instance->GetData(TIMER_STATE);

                eventstarted = true;
                if (phase == 3 && status == TIMER_NOT_STARTED)
                    eventstarted = false;
                //if (status == TIMER_NEXTPHASE)
                    //executeVictim();
            }
            CheckTimer = 5000;
        }

        void KilledUnit(Unit* killed_victim)
        {
            if (killed_victim->GetTypeId() == TYPE_ID_UNIT)
            {
                switch(phase)
                {
                case 3:
                    if ((killed_victim->GetEntry() == NPC_SCOUT_ORGARR || killed_victim->GetEntry() == NPC_RIFLEMAN_BROWNBEARD) && victim[0] == killed_victim->GetGUID())
                    {
                        phase = 2;
                        SetInstanceWideBuff(phase);
                        instance->SetData(EXECUTION_PHASE,phase);
                    }
                    break;
                case 2:
                    if ((killed_victim->GetEntry() == NPC_KORAG || killed_victim->GetEntry() == NPC_PROVATE_JACINT) && victim[1] == killed_victim->GetGUID())
                    {
                        phase = 1;
                        SetInstanceWideBuff(phase);
                        instance->SetData(EXECUTION_PHASE, phase);
                    }
                    break;
                case 1:
                    if ((killed_victim->GetEntry() == NPC_CAPT_BONESHATTER || killed_victim->GetEntry() == NPC_CAPT_ALINA ) && victim[2] == killed_victim->GetGUID())
                    {
                        phase = 0;
                        instance->SetData(EXECUTION_PHASE,phase);
                        //me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        //me->RemoveCorpse();
                    }
                    break;
                }
            }
        }

        /*void JustDied(Unit* killer)
        {
            if (!eventstarted || phase < 1)
                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }*/

        void EnterToBattle(Unit* /*who*/){}

        void SetInstanceWideBuff(uint32 buffphase)
        {
            Map* map = me->GetMap();
            if (map->IsDungeon())
            {
                InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
                for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* player = i->getSource())
					{
                        if (player->IsAlive())
                        {
                            switch (buffphase)
                            {
                                case 3:
									player->CastSpell(player,SPELL_EXECUTION_TIMER_BUFF_1, true);
									break;
                                case 2:
									player->CastSpell(player,SPELL_EXECUTION_TIMER_BUFF_2, true);
									break;
                                case 1:
									player->CastSpell(player,SPELL_EXECUTION_TIMER_BUFF_3, true);
									break;
							}
						}
					}
				}
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!eventstarted)
            {
                if (instance)
                {
                    uint32 status = instance->GetData(TIMER_STATE);
                    if (status != TIMER_NOT_STARTED)
                    {
                        SetInstanceWideBuff(3);
                        eventstarted = true;
                    }
                }
            }

            if (CheckTimer <= diff)
            {
                if (instance)
                {
                    uint32 status = instance->GetData(TIMER_STATE);
                    //error_log("Checked %d %d",status, phase);
                    if (status == TIMER_NEXT_PHASE)
                        executeVictim();
                }
                CheckTimer = 5000;
            }
			else CheckTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_shattered_hand_executionerAI(creature);
    }
};

void AddSC_shattred_halls()
{
    new mob_shattered_hand_executioner();
}