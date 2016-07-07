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
#include "world_transmogrification_system.h"
#include "../../../scripts/QuantumCoreScripts/SystemScripts/world_fun_system.h"

class npc_transmogrification_master : public CreatureScript
{
public:
    npc_transmogrification_master() : CreatureScript("npc_transmogrification_master") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        WorldSession* session = player->GetSession();
        for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++) // EQUIPMENT_SLOT_TABARD
        {
            if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            {
                if (sTransmogrification->AllowedQuality(newItem->GetTemplate()->Quality))
                {
                    if (const char* slotName = GetSlotName(slot, session))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, slotName, EQUIPMENT_SLOT_END, slot);
                }
            }
        }
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, session->GetQuantumSystemText(LANG_OPTION_REMOVE_ALL), EQUIPMENT_SLOT_END+2, 0, session->GetQuantumSystemText(LANG_POPUP_REMOVE_ALL), 0, false);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetQuantumSystemText(LANG_OPTION_UPDATE_MENU), EQUIPMENT_SLOT_END+1, 0);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        WorldSession* session = player->GetSession();
        player->PlayerTalkClass->ClearMenus();
        switch(sender)
        {
        case EQUIPMENT_SLOT_END: // Show items you can use
            {
                if (Item* oldItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, action))
                {
                    uint32 lowGUID = player->GetGUIDLow();
                    _items[lowGUID].clear();
                    uint32 limit = 0;
                    uint32 price = 0;
                    switch (sTransmogrification->GetRequireGold())
                    {
                        case 1:
						{
							price = (unsigned int)(GetFakePrice(oldItem)*sTransmogrification->GetGoldModifier());
						}
						break;
						case 2:
						{
							price = (unsigned int)sTransmogrification->GetGoldCost();
						}
						break;
					}
					char tokenCost[250] = "\n";
                    if (sTransmogrification->GetRequireToken())
                        snprintf(tokenCost, 250, "\n\n%u x %s", sTransmogrification->GetTokenAmount(), GetItemName(sObjectMgr->GetItemTemplate(sTransmogrification->GetTokenEntry()), session).c_str());

                    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
                    {
                        if (limit > 30)
                            break;
                        if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                        {
                            uint32 display = newItem->GetTemplate()->DisplayInfoID;
                            if (player->SuitableForTransmogrification(oldItem, newItem) == ERR_FAKE_OK)
                            {
                                if (_items[lowGUID].find(display) == _items[lowGUID].end())
                                {
                                    limit++;
                                    _items[lowGUID][display] = newItem;
                                    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, GetItemName(newItem->GetTemplate(), session), action, display, session->GetQuantumSystemText(LANG_POPUP_TRANSMOGRIFY)+GetItemName(newItem->GetTemplate(), session)+tokenCost, price, false);
                                }
                            }
                        }
                    }

                    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
                    {
                        if (Bag* bag = player->GetBagByPos(i))
                        {
                            for (uint32 j = 0; j < bag->GetBagSize(); j++)
                            {
                                if (limit > 30)
                                    break;
                                if (Item* newItem = player->GetItemByPos(i, j))
                                {
                                    uint32 display = newItem->GetTemplate()->DisplayInfoID;
                                    if (player->SuitableForTransmogrification(oldItem, newItem) == ERR_FAKE_OK)
                                    {
                                        if (_items[lowGUID].find(display) == _items[lowGUID].end())
                                        {
                                            limit++;
                                            _items[lowGUID][display] = newItem;
                                            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, GetItemName(newItem->GetTemplate(), session), action, display, session->GetQuantumSystemText(LANG_POPUP_TRANSMOGRIFY)+GetItemName(newItem->GetTemplate(), session)+tokenCost, price, false);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    char removeOnePopup[250];
                    snprintf(removeOnePopup, 250, session->GetQuantumSystemText(LANG_POPUP_REMOVE_ONE), GetSlotName(action, session));
                    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, session->GetQuantumSystemText(LANG_OPTION_REMOVE_ONE), EQUIPMENT_SLOT_END+3, action, removeOnePopup, 0, false);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetQuantumSystemText(LANG_OPTION_BACK), EQUIPMENT_SLOT_END+1, 0);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
                else
                    OnGossipHello(player, creature);
            }
			break;
        case EQUIPMENT_SLOT_END+1: // Back
            {
                OnGossipHello(player, creature);
            }
			break;
        case EQUIPMENT_SLOT_END+2: // Remove Transmogrifications
            {
                bool removed = false;
                for (uint8 Slot = EQUIPMENT_SLOT_START; Slot < EQUIPMENT_SLOT_END; Slot++)
                {
                    if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, Slot))
                    {
                        if (newItem->DeleteFakeEntry() && !removed)
                            removed = true;
                    }
                }
                if (removed)
                {
                    session->SendAreaTriggerMessage(session->GetQuantumSystemText(LANG_REM_TRANSMOGRIFICATIONS_ITEMS));
                    player->PlayDirectSound(3337);
                }
                else
                    session->SendNotification(session->GetQuantumSystemText(LANG_ERR_NO_TRANSMOGRIFICATIONS));
                OnGossipHello(player, creature);
            }
			break;
        case EQUIPMENT_SLOT_END+3: // Remove Transmogrification from single item
            {
                if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, action))
                {
                    if (newItem->DeleteFakeEntry())
                    {
                        session->SendAreaTriggerMessage(session->GetQuantumSystemText(LANG_REM_TRANSMOGRIFICATION_ITEM), GetSlotName(action, session));
                        player->PlayDirectSound(3337);
                    }
                    else
                        session->SendNotification(session->GetQuantumSystemText(LANG_ERR_NO_TRANSMOGRIFICATION), GetSlotName(action, session));
                }
                OnGossipSelect(player, creature, EQUIPMENT_SLOT_END, action);
            }
			break;
        default: // Transmogrify
            {
                uint32 lowGUID = player->GetGUIDLow();
                if (!sTransmogrification->GetRequireToken() || player->GetItemCount(sTransmogrification->GetTokenEntry()) >= sTransmogrification->GetTokenAmount())
                {
                    if (Item* oldItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, sender))
                    {
                        if (_items[lowGUID].find(action) != _items[lowGUID].end() && _items[lowGUID][action]->IsInWorld())
                        {
                            Item* newItem = _items[lowGUID][action];
                            if (newItem->GetOwnerGUID() == player->GetGUIDLow() && (newItem->IsInBag() || newItem->GetBagSlot() == INVENTORY_SLOT_BAG_0) && player->SuitableForTransmogrification(oldItem, newItem) == ERR_FAKE_OK)
                            {
                                switch(sTransmogrification->GetRequireGold())
                                {
								    case 1:
									{
										player->ModifyMoney(-1*(uint32)(GetFakePrice(oldItem)*sTransmogrification->GetGoldModifier()));
									}
									break;
									case 2:
									{
										player->ModifyMoney(-1*(unsigned int)sTransmogrification->GetGoldCost());
									}
									break;
                                }
                                if (sTransmogrification->GetRequireToken())
                                    player->DestroyItemCount(sTransmogrification->GetTokenEntry(), sTransmogrification->GetTokenAmount(), true);
                                oldItem->SetFakeEntry(newItem->GetEntry());
                                newItem->SetNotRefundable(player);
                                newItem->SetBinding(true);
                                player->PlayDirectSound(3337);
                                session->SendAreaTriggerMessage(session->GetQuantumSystemText(LANG_ITEM_TRANSMOGRIFIED), GetSlotName(sender, session));
                            }
                            else
                                session->SendNotification(session->GetQuantumSystemText(LANG_ERR_NO_ITEM_SUITABLE));
                        }
                        else
                            session->SendNotification(session->GetQuantumSystemText(LANG_ERR_NO_ITEM_EXISTS));
                    }
                    else
                        session->SendNotification(session->GetQuantumSystemText(LANG_ERR_EQUIP_SLOT_EMPTY));
                }
                else
                    session->SendNotification(session->GetQuantumSystemText(LANG_ERR_NO_TOKEN), GetItemName(sObjectMgr->GetItemTemplate(sTransmogrification->GetTokenEntry()), session).c_str());
                _items[lowGUID].clear();
                OnGossipSelect(player, creature, EQUIPMENT_SLOT_END, sender);
            }
			break;
        }
        return true;
    }

	struct npc_transmogrification_masterAI : public QuantumBasicAI
    {
        npc_transmogrification_masterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BuffTimer;

        void Reset()
        {
			BuffTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

        void UpdateAI(uint32 const diff)
        {
			if (!me->IsInCombatActive())
			{
				if (!me->HasAura(SPELL_CASCADE_OF_ROSES))
					DoCast(me, SPELL_CASCADE_OF_ROSES, true);
			}

			if (BuffTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_WATER_SHIELD, true);
				DoCast(me, SPELL_STAMINA, true);
				DoCast(me, SPELL_FLASK_OF_PURE_MOJO, true);

				BuffTimer = 10*MINUTE*IN_MILLISECONDS;
			}
			else BuffTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_transmogrification_masterAI(creature);
    }

private:
    std::map<uint64, std::map<uint32, Item*> > _items; // _items[lowGUID][DISPLAY] = item

    const char* GetSlotName(uint8 slot, WorldSession* session)
    {
        switch (slot)
        {
            case EQUIPMENT_SLOT_HEAD:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_HEAD);
			case EQUIPMENT_SLOT_SHOULDERS:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_SHOULDERS);
			case EQUIPMENT_SLOT_BODY:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_BODY);
			case EQUIPMENT_SLOT_CHEST:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_CHEST);
			case EQUIPMENT_SLOT_WAIST:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_WAIST);
			case EQUIPMENT_SLOT_LEGS:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_LEGS);
			case EQUIPMENT_SLOT_FEET:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_FEET);
			case EQUIPMENT_SLOT_WRISTS:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_WRISTS);
			case EQUIPMENT_SLOT_HANDS:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_HANDS);
			case EQUIPMENT_SLOT_BACK:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_BACK);
			case EQUIPMENT_SLOT_MAIN_HAND:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_MAIN_HAND);
			case EQUIPMENT_SLOT_OFF_HAND:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_OFF_HAND);
			case EQUIPMENT_SLOT_RANGED:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_RANGED);
			case EQUIPMENT_SLOT_TABARD:
				return session->GetQuantumSystemText(LANG_SLOT_NAME_TABARD);
			default:
				return NULL;
		}
    }

    std::string GetItemName(const ItemTemplate* itemTemplate, WorldSession* session)
    {
        std::string name = itemTemplate->Name1;
        int loc_idx = session->GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
            if (ItemLocale const* il = sObjectMgr->GetItemLocale(itemTemplate->ItemId))
                sObjectMgr->GetLocaleString(il->Name, loc_idx, name);
        return name;
    }

    uint32 GetFakePrice(Item* item)
    {
        uint32 sellPrice = item->GetTemplate()->SellPrice;
        uint32 minPrice = item->GetTemplate()->RequiredLevel * 1176;
        if (sellPrice < minPrice)
            sellPrice = minPrice;
        return sellPrice;
    }
};

class transmogrification_config : public WorldScript
{
public:
    transmogrification_config() : WorldScript("transmogrification_config") { }

    void OnConfigLoad(bool reload)
    {
        sTransmogrification->LoadConfig();
    }
};

void AddSC_world_transmogrification_system()
{
    new npc_transmogrification_master();
    new transmogrification_config();
}