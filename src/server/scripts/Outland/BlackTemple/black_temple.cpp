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
#include "black_temple.h"

enum Events
{
    // Wrathbone Flayer
    EVENT_GET_CHANNELERS = 1,
    EVENT_SET_CHANNELERS = 2,
    EVENT_CLEAVE         = 3,
    EVENT_IGNORED        = 4,
};

enum Spells
{
    SPELL_TELEPORT       = 41566,
    SPELL_CLEAVE         = 15496,
    SPELL_IGNORED        = 39544,
    SPELL_SUMMON_CHANNEL = 40094,
};

enum Creatures
{
    NPC_BLOOD_MAGE    = 22945,
    NPC_DEATHSHAPER   = 22882,
	NPC_AKAMA_ILLIDAN = 23089,
};

#define GOSSIP_OLUM1 "Teleport me to the other Ashtongue Deathsworn"

class npc_spirit_of_olum : public CreatureScript
{
public:
    npc_spirit_of_olum() : CreatureScript("npc_spirit_of_olum") { }

    bool OnGossipHello (Player* player, Creature* creature)
    {
        InstanceScript* instance = creature->GetInstanceScript();

        if (instance->GetData(DATA_SUPREMUSEVENT) >= DONE && instance->GetData(DATA_HIGHWARLORDNAJENTUSEVENT) >= DONE)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_OLUM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelectMenu (Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
            player->CLOSE_GOSSIP_MENU();

        player->InterruptNonMeleeSpells(false);
        player->CastSpell(player, SPELL_TELEPORT, false);
        return true;
    }
};

class npc_wrathbone_flayer : public CreatureScript
{
public:
    npc_wrathbone_flayer() : CreatureScript("npc_wrathbone_flayer") { }

    struct npc_wrathbone_flayerAI : public QuantumBasicAI
    {
        npc_wrathbone_flayerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

		InstanceScript* instance;
		EventMap events;
		std::list<uint64> bloodmage;
		std::list<uint64> deathshaper;
		bool EnteredCombat;

        void Reset()
        {
            events.ScheduleEvent(EVENT_GET_CHANNELERS, 3000);

            EnteredCombat = false;
        }

        void JustDied(Unit* /*killer*/){}

        void EnterToBattle(Unit* /*who*/) 
        {
            events.ScheduleEvent(EVENT_CLEAVE, 5000);
            events.ScheduleEvent(EVENT_IGNORED, 7000);

            EnteredCombat = true;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!EnteredCombat)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GET_CHANNELERS:
                        {
                            std::list<Creature*> BloodMageList;
                            me->GetCreatureListWithEntryInGrid(BloodMageList, NPC_BLOOD_MAGE, 15.0f);

                            if (!BloodMageList.empty())
							{
                                for (std::list<Creature*>::const_iterator itr = BloodMageList.begin(); itr != BloodMageList.end(); ++itr)
                                {
                                    bloodmage.push_back((*itr)->GetGUID());
                                    if ((*itr)->IsDead())
                                        (*itr)->Respawn();
                                }
							}
                            std::list<Creature*> DeathShaperList;
                            me->GetCreatureListWithEntryInGrid(DeathShaperList, NPC_DEATHSHAPER, 15.0f);

                            if (!DeathShaperList.empty())
							{
                                for (std::list<Creature*>::const_iterator itr = DeathShaperList.begin(); itr != DeathShaperList.end(); ++itr)
                                {
                                    deathshaper.push_back((*itr)->GetGUID());
                                    if ((*itr)->IsDead())
                                        (*itr)->Respawn();
                                }
							}
                            events.ScheduleEvent(EVENT_SET_CHANNELERS, 3000);
                            break;
                        }
                        case EVENT_SET_CHANNELERS:
                        {
                            for (std::list<uint64>::const_iterator itr = bloodmage.begin(); itr != bloodmage.end(); ++itr)
							{
                                if (Creature* bloodmage = (Unit::GetCreature(*me, *itr)))
                                    bloodmage->AI()->DoCast(SPELL_SUMMON_CHANNEL);
							}

                            for (std::list<uint64>::const_iterator itr = deathshaper.begin(); itr != deathshaper.end(); ++itr)
							{
                                if (Creature* deathshaper = (Unit::GetCreature(*me, *itr)))
                                    deathshaper->AI()->DoCast(SPELL_SUMMON_CHANNEL);
							}
                            events.ScheduleEvent(EVENT_SET_CHANNELERS, 12000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLEAVE:
                        DoCastVictim(SPELL_CLEAVE);
                        events.ScheduleEvent(EVENT_CLEAVE, urand (1000, 2000));
                        break;
                    case EVENT_IGNORED:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            DoCast(target, SPELL_IGNORED);

                        events.ScheduleEvent(EVENT_IGNORED, 10000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrathbone_flayerAI(creature);
    }
};

void AddSC_black_temple()
{
    new npc_spirit_of_olum();
	new npc_wrathbone_flayer();
}