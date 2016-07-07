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

enum BathrahTheWindwatcher
{
	SPELL_SUMMON_CYCLONIAN = 8606,
	SPELL_SPIRIT_TOTEM     = 25000,
	QUEST_CYCLONIAN        = 1712,
};

class npc_bathrah_the_windwatcher : public CreatureScript
{
public:
    npc_bathrah_the_windwatcher() : CreatureScript("npc_bathrah_the_windwatcher") {}

	bool OnQuestAccept(Player* /*player*/, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_CYCLONIAN)
			creature->CastSpell(creature, SPELL_SUMMON_CYCLONIAN, true);

		return true;
	}

    struct npc_bathrah_the_windwatcherAI : public QuantumBasicAI
    {
        npc_bathrah_the_windwatcherAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCastAOE(SPELL_SPIRIT_TOTEM);
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bathrah_the_windwatcherAI(creature);
    }
};

void AddSC_hillsbrad_foothills()
{
	new npc_bathrah_the_windwatcher();
}