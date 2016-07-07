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
#include "Spell.h"

enum OnlyForFlight
{
    SPELL_ARCANE_CHARGES    = 45072,
};

class item_only_for_flight : public ItemScript
{
public:
    item_only_for_flight() : ItemScript("item_only_for_flight") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
    {
        uint32 itemId = item->GetEntry();
        bool Disabled = false;

        switch (itemId)
        {
           case 24538:
                if (player->GetAreaId() != 3628)
                    Disabled = true;
                    break;
           case 34489:
                if (player->GetZoneId() != 4080)
                    Disabled = true;
                    break;
           case 34475:
                if (const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(SPELL_ARCANE_CHARGES))
                    Spell::SendCastResult(player, spellInfo, 1, SPELL_FAILED_NOT_ON_GROUND);
                    break;
        }

        // allow use in flight only
        if (player->IsInFlight() && !Disabled)
            return false;

        // error
        player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
        return true;
    }
};

class item_nether_wraith_beacon : public ItemScript
{
public:
    item_nether_wraith_beacon() : ItemScript("item_nether_wraith_beacon") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
    {
        if (player->GetQuestStatus(10832) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* nether = player->SummonCreature(22408, player->GetPositionX(), player->GetPositionY()+20, player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 180000))
                nether->AI()->AttackStart(player);

            if (Creature* nether = player->SummonCreature(22408, player->GetPositionX(), player->GetPositionY()-20, player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 180000))
                nether->AI()->AttackStart(player);
        }
        return false;
    }
};

class item_gor_dreks_ointment : public ItemScript
{
public:
    item_gor_dreks_ointment() : ItemScript("item_gor_dreks_ointment") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        if (targets.GetUnitTarget() && targets.GetUnitTarget()->GetTypeId() == TYPE_ID_UNIT && targets.GetUnitTarget()->GetEntry() == 20748 && !targets.GetUnitTarget()->HasAura(32578))
            return false;

        player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
        return true;
    }
};

enum Explosives
{
	NPC_NORTHERN_SINKHOLE_CREDIT = 26248,
	NPC_SOUTHERN_SINKHOLE_CREDIT = 26249,
};

class item_incendiary_explosives : public ItemScript
{
public:
    item_incendiary_explosives() : ItemScript("item_incendiary_explosives") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const & /*targets*/)
    {
        if (player->FindCreatureWithDistance(NPC_NORTHERN_SINKHOLE_CREDIT, 15.0f) || player->FindCreatureWithDistance(NPC_SOUTHERN_SINKHOLE_CREDIT, 15.0f))
            return false;
        else
        {
            player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
            return true;
        }
    }
};

class item_mysterious_egg : public ItemScript
{
public:
    item_mysterious_egg() : ItemScript("item_mysterious_egg") { }

    bool OnExpire(Player* player, ItemTemplate const* /*itemId*/)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 39883, 1); // Cracked Egg
        if (msg == EQUIP_ERR_OK)
            player->StoreNewItem(dest, 39883, true, Item::GenerateItemRandomPropertyId(39883));

        return true;
    }
};

class item_disgusting_jar : public ItemScript
{
public:
    item_disgusting_jar() : ItemScript("item_disgusting_jar") {}

    bool OnExpire(Player* player, ItemTemplate const* /*pItemProto*/)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 44718, 1); // Ripe Disgusting Jar
        if (msg == EQUIP_ERR_OK)
            player->StoreNewItem(dest, 44718, true, Item::GenerateItemRandomPropertyId(44718));

        return true;
    }
};

enum PileFakeFur
{
    GO_CARIBOU_TRAP_1           = 187982,
    GO_CARIBOU_TRAP_2           = 187995,
    GO_CARIBOU_TRAP_3           = 187996,
    GO_CARIBOU_TRAP_4           = 187997,
    GO_CARIBOU_TRAP_5           = 187998,
    GO_CARIBOU_TRAP_6           = 187999,
    GO_CARIBOU_TRAP_7           = 188000,
    GO_CARIBOU_TRAP_8           = 188001,
    GO_CARIBOU_TRAP_9           = 188002,
    GO_CARIBOU_TRAP_10          = 188003,
    GO_CARIBOU_TRAP_11          = 188004,
    GO_CARIBOU_TRAP_12          = 188005,
    GO_CARIBOU_TRAP_13          = 188006,
    GO_CARIBOU_TRAP_14          = 188007,
    GO_CARIBOU_TRAP_15          = 188008,
    GO_HIGH_QUALITY_FUR         = 187983,
    NPC_NESINGWARY_TRAPPER      = 25835,
};

#define CaribouTrapsNum 15

const uint32 CaribouTraps[CaribouTrapsNum] =
{
    GO_CARIBOU_TRAP_1,
	GO_CARIBOU_TRAP_2,
	GO_CARIBOU_TRAP_3,
	GO_CARIBOU_TRAP_4,
	GO_CARIBOU_TRAP_5,
    GO_CARIBOU_TRAP_6,
	GO_CARIBOU_TRAP_7,
	GO_CARIBOU_TRAP_8,
	GO_CARIBOU_TRAP_9,
	GO_CARIBOU_TRAP_10,
    GO_CARIBOU_TRAP_11,
	GO_CARIBOU_TRAP_12,
	GO_CARIBOU_TRAP_13,
	GO_CARIBOU_TRAP_14,
	GO_CARIBOU_TRAP_15,
};

class item_pile_fake_furs : public ItemScript
{
public:
    item_pile_fake_furs() : ItemScript("item_pile_fake_furs") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const & /*targets*/)
    {
        GameObject* go = NULL;
        for (uint8 i = 0; i < CaribouTrapsNum; ++i)
        {
            go = player->FindGameobjectWithDistance(CaribouTraps[i], 5.0f);
            if (go)
                break;
        }

        if (!go)
            return false;

        if (go->FindCreatureWithDistance(NPC_NESINGWARY_TRAPPER, 10.0f, true) || go->FindCreatureWithDistance(NPC_NESINGWARY_TRAPPER, 10.0f, false) || go->FindGameobjectWithDistance(GO_HIGH_QUALITY_FUR, 2.0f))
            return true;

        float x, y, z;
        go->GetClosePoint(x, y, z, go->GetObjectSize() / 3, 7.0f);
        go->SummonGameObject(GO_HIGH_QUALITY_FUR, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, 0, 0, 0, 0, 1000);
        if (TempSummon* summon = player->SummonCreature(NPC_NESINGWARY_TRAPPER, x, y, z, go->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 1000))
        {
            summon->SetVisible(false);
            summon->SetReactState(REACT_PASSIVE);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
        }
        return false;
    }
};

enum PetrovClusterBombs
{
    SPELL_PETROV_BOMB         = 42406,
    AREA_ID_SHATTERED_STRAITS = 4064,
    ZONE_ID_HOWLING           = 495,
};

class item_petrov_cluster_bombs : public ItemScript
{
public:
    item_petrov_cluster_bombs() : ItemScript("item_petrov_cluster_bombs") { }

    bool OnUse(Player* player, Item* item, const SpellCastTargets & /*pTargets*/)
    {
        if (player->GetZoneId() != ZONE_ID_HOWLING)
            return false;

        if (!player->GetTransport() || player->GetAreaId() != AREA_ID_SHATTERED_STRAITS)
        {
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);

            if (const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(SPELL_PETROV_BOMB))
                Spell::SendCastResult(player, spellInfo, 1, SPELL_FAILED_NOT_HERE);

            return true;
        }
        return false;
    }
};

enum HelpThemselves
{
    QUEST_CANNOT_HELP_THEMSELVES  =  11876,
    NPC_TRAPPED_MAMMOTH_CALF      =  25850,
    GO_MAMMOTH_TRAP_1             = 188022,
    GO_MAMMOTH_TRAP_2             = 188024,
    GO_MAMMOTH_TRAP_3             = 188025,
    GO_MAMMOTH_TRAP_4             = 188026,
    GO_MAMMOTH_TRAP_5             = 188027,
    GO_MAMMOTH_TRAP_6             = 188028,
    GO_MAMMOTH_TRAP_7             = 188029,
    GO_MAMMOTH_TRAP_8             = 188030,
    GO_MAMMOTH_TRAP_9             = 188031,
    GO_MAMMOTH_TRAP_10            = 188032,
    GO_MAMMOTH_TRAP_11            = 188033,
    GO_MAMMOTH_TRAP_12            = 188034,
    GO_MAMMOTH_TRAP_13            = 188035,
    GO_MAMMOTH_TRAP_14            = 188036,
    GO_MAMMOTH_TRAP_15            = 188037,
    GO_MAMMOTH_TRAP_16            = 188038,
    GO_MAMMOTH_TRAP_17            = 188039,
    GO_MAMMOTH_TRAP_18            = 188040,
    GO_MAMMOTH_TRAP_19            = 188041,
    GO_MAMMOTH_TRAP_20            = 188042,
    GO_MAMMOTH_TRAP_21            = 188043,
    GO_MAMMOTH_TRAP_22            = 188044,
};

#define MammothTrapsNum 22

const uint32 MammothTraps[MammothTrapsNum] =
{
    GO_MAMMOTH_TRAP_1,
	GO_MAMMOTH_TRAP_2,
	GO_MAMMOTH_TRAP_3,
	GO_MAMMOTH_TRAP_4,
	GO_MAMMOTH_TRAP_5,
    GO_MAMMOTH_TRAP_6,
	GO_MAMMOTH_TRAP_7,
	GO_MAMMOTH_TRAP_8,
	GO_MAMMOTH_TRAP_9,
	GO_MAMMOTH_TRAP_10,
    GO_MAMMOTH_TRAP_11,
	GO_MAMMOTH_TRAP_12,
	GO_MAMMOTH_TRAP_13,
	GO_MAMMOTH_TRAP_14,
	GO_MAMMOTH_TRAP_15,
    GO_MAMMOTH_TRAP_16,
	GO_MAMMOTH_TRAP_17,
	GO_MAMMOTH_TRAP_18,
	GO_MAMMOTH_TRAP_19,
	GO_MAMMOTH_TRAP_20,
    GO_MAMMOTH_TRAP_21,
	GO_MAMMOTH_TRAP_22
};

class item_dehta_trap_smasher : public ItemScript
{
public:
    item_dehta_trap_smasher() : ItemScript("item_dehta_trap_smasher") { }

    bool OnUse(Player* player, Item* /*item*/, const SpellCastTargets & /*pTargets*/)
    {
        if (player->GetQuestStatus(QUEST_CANNOT_HELP_THEMSELVES) != QUEST_STATUS_INCOMPLETE)
            return false;

        Creature* mammoth;
        mammoth = player->FindCreatureWithDistance(NPC_TRAPPED_MAMMOTH_CALF, 5.0f);
        if (!mammoth)
            return false;

        GameObject* trap;
        for (uint8 i = 0; i < MammothTrapsNum; ++i)
        {
            trap = player->FindGameobjectWithDistance(MammothTraps[i], 11.0f);
            if (trap)
            {
                mammoth->AI()->DoAction(1);
                trap->SetGoState(GO_STATE_READY);
                player->KilledMonsterCredit(NPC_TRAPPED_MAMMOTH_CALF, 0);
                return true;
            }
        }
        return false;
    }
};

enum TheEmissary
{
    QUEST_THE_EMISSARY  = 11626,
    NPC_LEVIROTH        = 26452,
};

class item_trident_of_nazjan : public ItemScript
{
public:
    item_trident_of_nazjan() : ItemScript("item_Trident_of_Nazjan") { }

    bool OnUse(Player* player, Item* item, const SpellCastTargets & /*pTargets*/)
    {
        if (player->GetQuestStatus(QUEST_THE_EMISSARY) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* leviroth = player->FindCreatureWithDistance(NPC_LEVIROTH, 10.0f)) // spell range
            {
                leviroth->AI()->AttackStart(player);
                return false;
			}
			else
                player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
        }
		else
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
        return true;
    }
};

enum CapturedFrog
{
    QUEST_THE_PERFECT_SPIES    = 25444,
    NPC_VANIRAS_SENTRY_TOTEM   = 40187,
};

class item_captured_frog : public ItemScript
{
public:
    item_captured_frog() : ItemScript("item_captured_frog") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
    {
        if (player->GetQuestStatus(QUEST_THE_PERFECT_SPIES) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->FindCreatureWithDistance(NPC_VANIRAS_SENTRY_TOTEM, 10.0f))
                return false;
            else
                player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
        }
        else
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
        return true;
    }
};

class item_fiery_soul_fragment : public ItemScript
{
public:
	item_fiery_soul_fragment(): ItemScript("fiery_soul_fragment_item"){ }

	bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
	{
		if (player->FindCreatureWithDistance(21311, 5, true))
		{
			player->CastSpell(player, 36587, true);
			player->CompleteQuest(10525);
		}
		return true;
	}
};

enum Misc
{
    QUEST_A_CLEANSING_SONG	 = 12735,
    NPC_SPIRIT_OF_ATHA		 = 29033,
    NPC_SPIRIT_OF_HA_KHALAN	 = 29018,
    NPC_SPIRIT_OF_KOOSU		 = 29034,
    SPELL_SONG_OF_CLEANSING  = 52941,
};

class item_chime_of_cleansing : public ItemScript
{
	public:
		item_chime_of_cleansing() : ItemScript("item_chime_of_cleansing"){ }

		bool OnUse(Player* player,Item* item, SpellCastTargets const& /*targets*/)
		{
            if (player->GetQuestStatus(QUEST_A_CLEANSING_SONG) == QUEST_STATUS_INCOMPLETE)
            {
                if (player->GetAreaId() == 4385)
                {
                    Creature* Atha = player->SummonCreature(NPC_SPIRIT_OF_ATHA, player->GetPositionX()+10, player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    if (Atha)
                        Atha->AI()->AttackStart(player);
                }

                if (player->GetAreaId() == 4290)
                {
                    Creature* HaKhalan = player->SummonCreature(NPC_SPIRIT_OF_HA_KHALAN, player->GetPositionX()+10, player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    if (HaKhalan)
                        HaKhalan->AI()->AttackStart(player);
                }

                if (player->GetAreaId() == 4388)
                {
                    Creature* Koosu = player->SummonCreature(NPC_SPIRIT_OF_KOOSU, player->GetPositionX()+10, player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    if (Koosu)
                        Koosu->AI()->AttackStart(player);
                }

                else
                {
                    player->SendEquipError(EQUIP_ERR_NONE, item, NULL);

                    if (const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(SPELL_SONG_OF_CLEANSING))
                        Spell::SendCastResult(player, spellInfo, 1, SPELL_FAILED_NOT_HERE);
                    return false;
                }
            }

            else
                player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
            return false;
        }
};

enum AlumethsRemainsData
{
	QUEST_NO_REST_FOR_THE_WICKED_A   = 13346,
	QUEST_NO_REST_FOR_THE_WICKED_H   = 13367,
	QUEST_NO_REST_FOR_THE_WICKED_A_D = 13350,
	QUEST_NO_REST_FOR_THE_WICKED_H_D = 13368,
	NPC_ALUMETH                      = 32300,
};

class item_alumeths_remains : public ItemScript
{
public:
	item_alumeths_remains() : ItemScript("item_alumeths_remains") { }

	bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
	{
		if (player->GetQuestStatus(QUEST_NO_REST_FOR_THE_WICKED_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_NO_REST_FOR_THE_WICKED_H) == QUEST_STATUS_INCOMPLETE ||
			player->GetQuestStatus(QUEST_NO_REST_FOR_THE_WICKED_A_D) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_NO_REST_FOR_THE_WICKED_H_D) == QUEST_STATUS_INCOMPLETE)
			player->SummonCreature(NPC_ALUMETH,8229.606445f,2179.215820f,499.736694f,3.161045f,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,30000);
		return true;
	}
};

enum WrithingMassData
{
	QUEST_RETEST_NOW_A   = 13321,
	QUEST_RETEST_NOW_H   = 13356,
	QUEST_RETEST_NOW_A_D = 13322,
	QUEST_RETEST_NOW_H_D = 13357,
	NPC_RETEST_NOW_KC    = 32266,
};

class item_writhing_mass : public ItemScript
{
public:
	item_writhing_mass() : ItemScript("item_writhing_mass") { }

	bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
	{
		if (player->GetQuestStatus(QUEST_RETEST_NOW_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_RETEST_NOW_H) == QUEST_STATUS_INCOMPLETE ||
			player->GetQuestStatus(QUEST_RETEST_NOW_A_D) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_RETEST_NOW_H_D) == QUEST_STATUS_INCOMPLETE)
			player->KilledMonsterCredit(NPC_RETEST_NOW_KC, 0);
		return true;
	}
};

class item_attuned_crystal_cores : public ItemScript
{
public:
    item_attuned_crystal_cores() : ItemScript("item_attuned_crystal_cores") { }

	bool ItemUse_item_attuned_crystal_cores(Player* player, Item* item, SpellCastTargets const& targets)
	{
		if (targets.GetUnitTarget() && targets.GetUnitTarget()->GetTypeId() == TYPE_ID_UNIT && targets.GetUnitTarget()->GetEntry() == 24972 && targets.GetUnitTarget()->IsDead() && (player->GetQuestStatus(11524) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(11525) == QUEST_STATUS_INCOMPLETE))
			return false;

		player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
		return true;
    }
};

enum Purification
{
	N_QTARGET          = 22288,
	Q_DARKSTONE        = 10839,
	S_PURIFICATION_ROD = 38736,
};

class item_purification_rod : public ItemScript
{
public:
    item_purification_rod() : ItemScript("item_purification_rod") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        {
			Creature* quest = GetClosestCreatureWithEntry(player, N_QTARGET, 20);
            if (quest)
            {
				((Player*)player)->GroupEventHappens(Q_DARKSTONE, quest);
                return false;
            }
            else
            {
				player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
				return true;
			}
		}
    }
};

class item_rod_of_compulsion : public ItemScript
{
public:
    item_rod_of_compulsion() : ItemScript("item_rod_of_compulsion") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        if (Unit *target = targets.GetUnitTarget())
        {
            if (target->GetTypeId() == TYPE_ID_PLAYER || 
                (target->GetEntry() != 27237 && target->GetEntry() != 27235 &&  target->GetEntry() != 27234 &&  target->GetEntry() != 27236))
            {
                player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
                return true;
            }
            return false;
        }
		else
        {
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
            return true;
        }
    }
};

enum Marks
{
	N_NMARK = 20333,
	N_SMARK = 20337,
	N_WMARK = 20338,
	N_OMARK = 20336,
};

class item_warp_orb : public ItemScript
{
public:
    item_warp_orb() : ItemScript("item_warp_orb") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        if (player->GetQuestStatus(10313) == QUEST_STATUS_INCOMPLETE)
        {
            {
				Creature* quest1 = GetClosestCreatureWithEntry(player, N_NMARK,75);
				Creature* quest2 = GetClosestCreatureWithEntry(player, N_SMARK,75);
				Creature* quest3 = GetClosestCreatureWithEntry(player, N_WMARK,75);
				Creature* quest4 = GetClosestCreatureWithEntry(player, N_OMARK,75);

                if (quest1)
                {
                    player->KilledMonsterCredit(N_NMARK, quest1->GetGUID());
                    return false;
                }
                else 
					if (quest2)
                {
                    player->KilledMonsterCredit(N_SMARK, quest2->GetGUID());
                    return false;
                }
                else 
					if (quest3)
                {
                    player->KilledMonsterCredit(N_WMARK, quest3->GetGUID());
                    return false;
                }
                else 
					if (quest4)
                {
                    player->KilledMonsterCredit(N_OMARK, quest4->GetGUID());
                    return false;
                }
                else
                {
                    player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
                    return true;
                }
            }
        }
        else
        {
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
            return true;
        }
    }
};

enum SnqControlUnit
{
	QUEST_DELPOY_THE_SKAKE_N_QUAKE = 11723,
	NPC_LORD_KRYXIX                = 25629,
};

class item_snq_control_unit : public ItemScript
{
public:
	item_snq_control_unit() : ItemScript("item_snq_control_unit") { }

	bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
	{
		if (player->GetQuestStatus(QUEST_DELPOY_THE_SKAKE_N_QUAKE) == QUEST_STATUS_INCOMPLETE)
			player->SummonCreature(NPC_LORD_KRYXIX, 3511.959961f, 4527.180176f, -12.994900f, 0.357893f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

		return true;
	}
};

enum InterdimensionalRefabricator
{
	QUEST_POSTPONING_THE_INEVATABLE = 11905,
	SPELL_ACTIVATE_REFABRICATOR     = 46547,
	NPC_QUEST_INVISMAN_BUYING_TIME  = 26105,
};

class item_interdimensional_refabricator : public ItemScript
{
public:
	item_interdimensional_refabricator() : ItemScript("item_interdimensional_refabricator") { }

	bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
	{
		if (player->GetQuestStatus(QUEST_POSTPONING_THE_INEVATABLE) == QUEST_STATUS_INCOMPLETE)
			player->CastSpell(player, SPELL_ACTIVATE_REFABRICATOR, false);

		player->KilledMonsterCredit(NPC_QUEST_INVISMAN_BUYING_TIME, 0);
		return true;
	}
};

enum UnboundValanyr
{
	SPELL_DEAFENING_ROAR = 64189,
	NPC_YOGG_SARON       = 33288,
};

class item_unbound_fragments_of_valanyr : public ItemScript
{
public:
    item_unbound_fragments_of_valanyr() : ItemScript("item_unbound_fragments_of_valanyr") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
    {
        if (Creature* yogg = player->FindCreatureWithDistance(NPC_YOGG_SARON, 250.0f))
        {
			if (yogg->FindCurrentSpellBySpellId(SPELL_DEAFENING_ROAR))
                return false;
        }
        player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
        return true;
    }
};

enum WildThings
{
	QUEST_WHERE_THE_WILD_THINGRS_ROAR = 12111,

	SPELL_RECENTLY_INOCULATED         = 47675,

	NPC_SNOWFALL_ELK                  = 26615,
	NPC_ARCTIC_GRIZZLY                = 26482,
	NPC_SNOWFALL_ELK_CREDIT           = 26895,
	NPC_ARCTIC_GRIZZLY_CREDIT         = 26882,
};

class item_pack_of_vaccine : public ItemScript
{
public:
    item_pack_of_vaccine() : ItemScript("item_pack_of_vaccine") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const & /*targets*/)
    {
        if (player->GetQuestStatus(QUEST_WHERE_THE_WILD_THINGRS_ROAR) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* elk = player->FindCreatureWithDistance(NPC_SNOWFALL_ELK, 20.0f, true))
			{
				if (!elk->IsInCombatActive())
				{
					player->KilledMonsterCredit(NPC_SNOWFALL_ELK_CREDIT, 0);

					elk->CastSpell(elk, SPELL_RECENTLY_INOCULATED, true);
					elk->DespawnAfterAction(10*IN_MILLISECONDS);
				}
			}

            if (Creature* grizzly = player->FindCreatureWithDistance(NPC_ARCTIC_GRIZZLY, 20.0f, true))
			{
				if (!grizzly->IsInCombatActive())
				{
					player->KilledMonsterCredit(NPC_ARCTIC_GRIZZLY_CREDIT, 0);

					grizzly->CastSpell(grizzly, SPELL_RECENTLY_INOCULATED, true);
					grizzly->DespawnAfterAction(10*IN_MILLISECONDS);
				}
			}
        }
        return false;
    }
};

class item_titanium_seal_of_dalaran : public ItemScript
{
public:
	item_titanium_seal_of_dalaran() : ItemScript("item_titanium_seal_of_dalaran") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
    {
		uint32 cd_time = player->GetSpellCooldownDelay(60458);

		if (cd_time <= 0)
		{
			player->CastSpell(player, 60458, true);
			player->AddSpellCooldown(60458, 0, time(NULL)+10);

			if (urand(0, 99) < 50)
			{
				player->TextEmote("casually flips his |cff0070dd|Hitem:44430:0:0:0:0:0:0:0:80|h[Titanium Seal of Dalaran]|h|r.");
				player->TextEmote("catches the coin heads up!");
				return true;
			}
			else
			{
				player->TextEmote("casually flips his |cff0070dd|Hitem:44430:0:0:0:0:0:0:0:80|h[Titanium Seal of Dalaran]|h|r.");
				player->TextEmote("catches the coin face down for tails!");
				return true;
			}
		}
		else
		{
			return false;
		}
		return true;
	}
};

void AddSC_item_scripts()
{
    new item_only_for_flight();
    new item_nether_wraith_beacon();
    new item_gor_dreks_ointment();
    new item_incendiary_explosives();
    new item_mysterious_egg();
    new item_disgusting_jar();
    new item_pile_fake_furs();
    new item_petrov_cluster_bombs();
    new item_dehta_trap_smasher();
    new item_trident_of_nazjan();
    new item_captured_frog();
    new item_fiery_soul_fragment();
    new item_chime_of_cleansing();
	new item_alumeths_remains();
	new item_writhing_mass();
	new item_attuned_crystal_cores();
	new item_purification_rod();
    new item_rod_of_compulsion();
    new item_warp_orb();
	new item_snq_control_unit();
	new item_interdimensional_refabricator();
	new item_unbound_fragments_of_valanyr();
	new item_pack_of_vaccine();
	new item_titanium_seal_of_dalaran();
}