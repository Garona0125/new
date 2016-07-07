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
#include "QuantumEscortAI.h"
#include "sethekk_halls.h"

#define SAY_THANKS "Thanks that you have saved me"

class npc_lakka : public CreatureScript
{
public:
    npc_lakka() : CreatureScript("npc_lakka") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        npc_lakkaAI* lakkaAI = new npc_lakkaAI(creature);

        lakkaAI->AddWaypoint(0, -157.42f, 179.51f, 0.0f);
        lakkaAI->AddWaypoint(1, -140.22f, 183.01f, 0.0f);
        lakkaAI->AddWaypoint(2, -131.91f, 175.58f, 0.0f);
        lakkaAI->AddWaypoint(3, -78.13f, 172.95f, 0.0f);
        lakkaAI->AddWaypoint(4, -73.97f, 156.24f, 0.0f);
        return lakkaAI;
    }

    struct npc_lakkaAI : public npc_escortAI
    {
		npc_lakkaAI(Creature* creature) : npc_escortAI(creature){}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

        void WaypointReached(uint32 id)
        {
            Unit* player = Unit::GetUnit(*me, GetEventStarterGUID());
        
            if (!player)
                return;

            switch(id) 
            {
            case 1:
                me->MonsterSay(SAY_THANKS, LANG_UNIVERSAL, NULL);

                Map* map = me->GetMap();

                if (map->IsDungeon())
                {
                    InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
                    for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* player = i->getSource())
                        {
                            if (player->GetQuestStatus(QUEST_BROTHER_AGAINST_BROTHER) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(NPC_LAKKA, 0);
                        }
                    }
                }
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
			npc_escortAI::UpdateAI(diff);
        }
    };
};

class go_lakka_cage : public GameObjectScript
{
public:
    go_lakka_cage() : GameObjectScript("go_lakka_cage") {}

    bool OnGossipHello (Player* player, GameObject* go)
    {
        if (Creature* Lakka = GetClosestCreatureWithEntry(player, NPC_LAKKA, 35.0f))
            CAST_AI(npc_escortAI,Lakka->AI())->Start(false, true, player->GetGUID());

        return false;
    }
};

void AddSC_sethekk_halls()
{
    new npc_lakka();
    new go_lakka_cage();
}