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
#include "QuantumGossip.h"

enum Misc
{
	SAY_CONVERTED        = 0,

	SPELL_CONVERT_CREDIT = 45009,
};

class npc_converted_sentry : public CreatureScript
{
public:
    npc_converted_sentry() : CreatureScript("npc_converted_sentry") { }

    struct npc_converted_sentryAI : public QuantumBasicAI
    {
		npc_converted_sentryAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Credit;
        uint32 Timer;

        void Reset()
        {
            Credit = false;
            Timer = 2500;
        }

        void MoveInLineOfSight(Unit* /*who*/){}

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!Credit)
            {
                if (Timer <= diff)
                {
					Talk(SAY_CONVERTED);

                    DoCast(me, SPELL_CONVERT_CREDIT);
                    if (me->IsPet())
                        me->ToPet()->SetDuration(7500);
                    Credit = true;
                }
				else Timer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_converted_sentryAI(creature);
    }
};

enum Greengill
{
	ENRAGE  = 45111,
	ORB     = 45109,
	QUESTG  = 11541,
	DM      = 25060,
};

class npc_greengill_slave : public CreatureScript
{
public:
    npc_greengill_slave() : CreatureScript("npc_greengill_slave") { }

    struct npc_greengill_slaveAI : public QuantumBasicAI
    {
        npc_greengill_slaveAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;

        void EnterToBattle(Unit* /*who*/){}

        void Reset()
        {
			PlayerGUID = 0;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (!caster)
                return;

            if (caster->GetTypeId() == TYPE_ID_PLAYER && spell->Id == ORB && !me->HasAura(ENRAGE))
            {
                PlayerGUID = caster->GetGUID();
                if (PlayerGUID)
                {
                    Player* player = Unit::GetPlayer(*me, PlayerGUID);
                    if (player && player->GetQuestStatus(QUESTG) == QUEST_STATUS_INCOMPLETE)
                        DoCast(player, 45110, true);
                }
                DoCast(me, ENRAGE);
                Unit* myrmidon = me->FindCreatureWithDistance(DM, 70.0f);
                if (myrmidon)
                {
                    me->AddThreat(myrmidon, 10000.0f);
                    AttackStart(myrmidon);
                }
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greengill_slaveAI(creature);
    }
};

enum Says
{
    SAY_THALORIEN_1  = -1016769,
    SAY_THALORIEN_2  = -1016770,
    SAY_THALORIEN_3  = -1016771,
    SAY_THALORIEN_4  = -1016772,
    SAY_THALORIEN_5  = -1016773,
    SAY_THALORIEN_6  = -1016774,
    SAY_THALORIEN_7  = -1016775,
    SAY_THALORIEN_8  = -1016776,
    SAY_THALORIEN_9  = -1016777,
    SAY_THALORIEN_10 = -1016778,
};

class npc_thalorien : public CreatureScript
{
public:
    npc_thalorien() : CreatureScript("npc_thalorien") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PrepareGossipMenu(creature, 0);

		if ((player->GetQuestStatus(24535) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(24563) == QUEST_STATUS_INCOMPLETE))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "To examine the remains.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

		player->SendPreparedGossip(creature);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch(action)
		{
		    case GOSSIP_ACTION_INFO_DEF+1:
				player->CLOSE_GOSSIP_MENU();
				creature->AI()->SetGUID(player->GetGUID());
				creature->SetPhaseMask(2, true);
				player->SetPhaseMask(2, true);
				creature->AI()->DoAction(1);
				break;
			default:
				return false;
		}
		return true;
	}

    struct npc_thalorienAI : public QuantumBasicAI
    {
        npc_thalorienAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;

		EventMap events;
		uint64 uiThalorien;
		uint64 uiMorlen;
		uint64 uiPlayer;

		void Reset()
		{
			if (Player* player = me->GetPlayer(*me, uiPlayer))
				player->SetPhaseMask(1, true);

			if (Creature* pMorlen = me->GetCreature(*me, uiMorlen))
				pMorlen->DisappearAndDie();
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			me->SetPhaseMask(1, true);
			events.Reset();
			uiThalorien = 0;
			uiMorlen = 0;
			uiPlayer = 0;
		}

        void UpdateAI(const uint32 diff)
        {
			events.Update(diff);

			switch(events.ExecuteEvent())
			{
                case 1:
					// spawn all creatures
					if (Creature* pThalorien = me->SummonCreature(37205, 11797.0f, -7074.06f, 26.379f, 0.242908f, TEMPSUMMON_CORPSE_DESPAWN, 0))
					{
						pThalorien->SetPhaseMask(2,true);
                        uiThalorien = pThalorien->GetGUID();
					}

					if (Creature* pGuard = me->SummonCreature(37211, 11807.0f, -7070.37f, 25.372f, 3.218f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						pGuard->SetPhaseMask(2,true);
					
					if (Creature* pGuard = me->SummonCreature(37211, 11805.7f, -7073.64f, 25.5694f, 3.218f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						pGuard->SetPhaseMask(2,true);
					
					if (Creature* pGuard = me->SummonCreature(37211, 11805.6f, -7077.0f, 25.9643f, 3.218f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						pGuard->SetPhaseMask(2,true);
					
					if (Creature* pGuard = me->SummonCreature(37211, 11806.0f, -7079.71f, 26.2067f, 3.218f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						pGuard->SetPhaseMask(2,true);

					if (Creature* pMorlen = me->SummonCreature(37542, 11776.8f, -7050.72f, 25.2394f, 5.13752f, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    {
						pMorlen->Mount(25678);
						pMorlen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        pMorlen->SetPhaseMask(2,true);
                        pMorlen->SetReactState(REACT_PASSIVE);
                        uiMorlen = pMorlen->GetGUID();
					}
					events.ScheduleEvent(2,100);
					break;
				case 2:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_1, pThalorien);
					events.ScheduleEvent(3,5000);
					break;
				case 3:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_2, pThalorien);
					events.ScheduleEvent(4,5000);
					break;
				case 4:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_3, pThalorien);
					events.ScheduleEvent(5,10000);
					break;
				case 5:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_4, pThalorien);
					events.ScheduleEvent(6,6000);
					break;
				case 6:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						pThalorien->GetMotionMaster()->MovePoint(0, 11787.3f, -7064.11f, 25.8395f);
					events.ScheduleEvent(7,6000);
					break;
				case 7:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_5, pThalorien);
					events.ScheduleEvent(8,9000);
					break;
				case 8:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_6, pThalorien);

					if (Creature* pMorlen = me->GetCreature(*me, uiMorlen))
						pMorlen->CastSpell(pMorlen, 50689, true);
					events.ScheduleEvent(9,9000);
					break;
				// Intro terminada, comienzan las oleadas.
				case 9:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						pThalorien->SetHomePosition(pThalorien->GetPositionX(), pThalorien->GetPositionY(), pThalorien->GetPositionZ(), pThalorien->GetOrientation());
					SummonWave(37538);
					events.ScheduleEvent(10, 30000);
					break;
				case 10:
					SummonWave(37539);
					events.ScheduleEvent(11, 30000);
					break;
				case 11:
					SummonWave(37541);
					events.ScheduleEvent(12, 30000);
					break;
				case 12:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
					{
						if (Creature* pMorlen = me->GetCreature(*me, uiMorlen))
						{
							pMorlen->SetReactState(REACT_AGGRESSIVE);
							pMorlen->AddThreat(pThalorien, 100);
							pMorlen->AI()->AttackStart(pThalorien);
						}
					}
					break;
				// Outro
				case 13:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_7, pThalorien);
					events.ScheduleEvent(14, 3500);
					break;
				case 14:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_8, pThalorien);
					events.ScheduleEvent(15, 3500);
					break;
				case 15:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_9, pThalorien);
					events.ScheduleEvent(16, 3000);
					break;
				case 16:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						DoSendQuantumText(SAY_THALORIEN_10, pThalorien);
					if (Player* player = me->GetPlayer(*me, uiPlayer))
						player->KilledMonsterCredit(37601, player->GetGUID());
					events.ScheduleEvent(17, 12000);
					break;
				case 17:
					if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
						pThalorien->DisappearAndDie();
					Reset();
					break;
             }
        }

        void SummonWave(uint32 entry)
		{
			if (Creature* pThalorien = me->GetCreature(*me, uiThalorien))
			{
				if (Creature* pZombie = me->SummonCreature(entry, 11768.7f, -7065.83f, 24.0971f, 0.125877f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
				{
					pZombie->SetPhaseMask(2,true);
					pZombie->AddThreat(pThalorien, 100);
					pZombie->AI()->AttackStart(pThalorien);
				}
				if (Creature* pZombie = me->SummonCreature(entry, 11769.5f, -7063.83f, 24.1399f, 6.06035f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
				{
					pZombie->SetPhaseMask(2,true);
					pZombie->AddThreat(pThalorien, 100);
					pZombie->AI()->AttackStart(pThalorien);

					//if (pZombie->GetEntry() == 37541)
						//pZombie->DisappearAndDie();
				}
				if (Creature* pZombie = me->SummonCreature(entry, 11782.2f, -7059.6f, 25.5211f, 5.49589f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
				{
					pZombie->SetPhaseMask(2,true);
					pZombie->AddThreat(pThalorien, 100);
					pZombie->AI()->AttackStart(pThalorien);
				}
				if (Creature* pZombie = me->SummonCreature(entry, 11782.2f, -7059.6f, 25.5211f, 5.49589f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
				{
					pZombie->SetPhaseMask(2,true);
					pZombie->AddThreat(pThalorien, 100);
					pZombie->AI()->AttackStart(pThalorien);
				}
				if (Creature* pZombie = me->SummonCreature(entry, 11782.2f, -7059.6f, 25.5211f, 5.49589f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
				{
					pZombie->SetPhaseMask(2,true);
					pZombie->AddThreat(pThalorien, 100);
					pZombie->AI()->AttackStart(pThalorien);
				}
			}
		}

		void SummonedCreatureDespawn(Creature* summon)
		{
			if (summon->GetEntry() == 37205)
			{
				Reset();
				return;
			}

			if (summon->GetEntry() == 37542)
				events.ScheduleEvent(13, 3000);
		}

		void SetGUID(uint64 guid, int32 /*id*/)
		{
			uiPlayer = guid;
		}

		void DoAction(const int32 actionId)
		{
			switch(actionId)
			{
			    case 1:
					me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					events.ScheduleEvent(1,0);
					break;
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thalorienAI(creature);
    }
};
        
class npc_warden_sunwell : public CreatureScript
{
public:
    npc_warden_sunwell() : CreatureScript("npc_warden_sunwell") { }

    bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PrepareGossipMenu(creature, 0);

		if ((player->GetQuestStatus(24553) == QUEST_STATUS_INCOMPLETE)    // The Purification of Quel'Delar
			|| (player->GetQuestStatus(24564) == QUEST_STATUS_INCOMPLETE) // The Purification of Quel'Delar
			|| (player->GetQuestStatus(24594) == QUEST_STATUS_INCOMPLETE) // The Purification of Quel'Delar
			|| (player->GetQuestStatus(24595) == QUEST_STATUS_INCOMPLETE) // The Purification of Quel'Delar
			|| (player->GetQuestStatus(24596) == QUEST_STATUS_INCOMPLETE) // The Purification of Quel'Delar
			|| (player->GetQuestStatus(24598) == QUEST_STATUS_INCOMPLETE)) //The Purification of Quel'Delar
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Im ready to enter the Sunwell.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		player->SendPreparedGossip(creature);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
		    case GOSSIP_ACTION_INFO_DEF+1:
				player->CLOSE_GOSSIP_MENU();
				player->SetGameMaster(true);
				player->TeleportTo(580, 1728.5f, 709.219f, 71.1905f, 2.78676f);
				player->SetGameMaster(false);
				player->SetPhaseMask(2, true);
				break;
			default:
				return false;                                 
		}
		return true;                                           
	}

    struct npc_warden_sunwellAI : public QuantumBasicAI
    {
		npc_warden_sunwellAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset() {}

        void UpdateAI(const uint32 /*diff*/){}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warden_sunwellAI(creature);
    }
};

enum ErraticSentry
{
	SPELL_CONVERTING_SENTRY = 44997,
};

class npc_erratic_sentry : public CreatureScript
{
public:
    npc_erratic_sentry() : CreatureScript("npc_erratic_sentry") { }

    struct npc_erratic_sentryAI : public QuantumBasicAI
    {
        npc_erratic_sentryAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, SpellInfo* const spell)
        {
			if (spell->Id == SPELL_CONVERTING_SENTRY)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (me->IsDead())
					{
						me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
						me->RemoveCorpse();
					}
				}
			}
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_erratic_sentryAI(creature);
    }
};

enum EmissaryOfHate
{
	SPELL_IMPALE_EMISSARY = 45030,

	NPC_EMISSARY_CREDIT   = 25065,
};

class npc_emissary_of_hate : public CreatureScript
{
public:
    npc_emissary_of_hate() : CreatureScript("npc_emissary_of_hate") { }

    struct npc_emissary_of_hateAI : public QuantumBasicAI
    {
		npc_emissary_of_hateAI(Creature* creature) : QuantumBasicAI(creature) { }

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, SpellInfo* const spell)
        {
			if (Player* player = caster->ToPlayer())
            {
                if (spell->Id == SPELL_IMPALE_EMISSARY)
					player->KilledMonsterCredit(NPC_EMISSARY_CREDIT, 0);
            }
        }

		void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emissary_of_hateAI(creature);
    }
};

class npc_bombingrun_target : public CreatureScript
{
public:
    npc_bombingrun_target() : CreatureScript("npc_bombingrun_target") { }

    struct npc_bombingrun_targetAI : public QuantumBasicAI
    {
        npc_bombingrun_targetAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            me->RemoveCorpse();
            me->SetRespawnTime(30);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bombingrun_targetAI(creature);
    }
};

void AddSC_isle_of_queldanas()
{
    new npc_converted_sentry();
    new npc_greengill_slave();
	new npc_thalorien();
	new npc_warden_sunwell();
	new npc_erratic_sentry();
	new npc_emissary_of_hate();
	new npc_bombingrun_target();
}