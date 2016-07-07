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
#include "QuantumSystemText.h"

enum JudjeWeldon
{
	SPELL_BLESSING_OF_KINGS = 25898,
	SPELL_ARCANE_INTELLECT  = 36880,
	SPELL_ICE_ARMOR         = 36881,
	SPELL_DAMPEN_MAGIC      = 43015,
	SPELL_SNOWFLAKES        = 44755,
};

#define TEXT_NO_VIP "Sorry my friend, you not Vip..."

class npc_judje_weldon : public CreatureScript
{
public:
	npc_judje_weldon() : CreatureScript("npc_judje_weldon") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsVendor() && player->GetSession()->IsPremiumAccount() || player->IsGameMaster())
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
		else
			creature->MonsterWhisper(TEXT_NO_VIP, player->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }

    struct npc_judje_weldonAI : public QuantumBasicAI
    {
		npc_judje_weldonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BuffTimer;

        void Reset()
        {
			BuffTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

        void UpdateAI(uint32 const diff)
        {
			if (!me->HasAura(SPELL_SNOWFLAKES) && !me->IsInCombatActive())
				DoCast(me, SPELL_SNOWFLAKES, true);

			if (BuffTimer <= diff)
			{
				DoCast(me, SPELL_BLESSING_OF_KINGS, true);
				DoCast(me, SPELL_ICE_ARMOR, true);
				DoCast(me, SPELL_ARCANE_INTELLECT, true);
				DoCast(me, SPELL_DAMPEN_MAGIC, true);

				BuffTimer = 10*MINUTE*IN_MILLISECONDS;
			}
			else BuffTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_judje_weldonAI(creature);
    }
};

void AddSC_npc_vip_vendor()
{
	new npc_judje_weldon();
}