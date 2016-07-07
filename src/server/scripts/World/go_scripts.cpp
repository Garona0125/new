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
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "GameObjectAI.h"
#include "Spell.h"
#include "Player.h"

enum CatFigurine
{
    SPELL_SUMMON_GHOST_SABER = 5968,
};

class go_cat_figurine : public GameObjectScript
{
public:
    go_cat_figurine() : GameObjectScript("go_cat_figurine") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        player->CastSpell(player, SPELL_SUMMON_GHOST_SABER, true);
        return false;
    }
};

enum CrystalPylon
{
	QUEST_THE_NORTHERN_PYLON = 4285,
};

class go_northern_crystal_pylon : public GameObjectScript
{
public:
    go_northern_crystal_pylon() : GameObjectScript("go_northern_crystal_pylon") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER)
        {
            player->PrepareQuestMenu(go->GetGUID());
            player->SendPreparedQuest(go->GetGUID());
        }

        if (player->GetQuestStatus(QUEST_THE_NORTHERN_PYLON) == QUEST_STATUS_INCOMPLETE)
            player->AreaExploredOrEventHappens(QUEST_THE_NORTHERN_PYLON);

        return true;
    }
};

class go_eastern_crystal_pylon : public GameObjectScript
{
public:
    go_eastern_crystal_pylon() : GameObjectScript("go_eastern_crystal_pylon") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER)
        {
            player->PrepareQuestMenu(go->GetGUID());
            player->SendPreparedQuest(go->GetGUID());
        }

        if (player->GetQuestStatus(4287) == QUEST_STATUS_INCOMPLETE)
            player->AreaExploredOrEventHappens(4287);

        return true;
    }
};

class go_western_crystal_pylon : public GameObjectScript
{
public:
    go_western_crystal_pylon() : GameObjectScript("go_western_crystal_pylon") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER)
        {
            player->PrepareQuestMenu(go->GetGUID());
            player->SendPreparedQuest(go->GetGUID());
        }

        if (player->GetQuestStatus(4288) == QUEST_STATUS_INCOMPLETE)
            player->AreaExploredOrEventHappens(4288);

        return true;
    }
};

class go_barov_journal : public GameObjectScript
{
public:
    go_barov_journal() : GameObjectScript("go_barov_journal") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->HasSkill(SKILL_TAILORING) && player->GetBaseSkillValue(SKILL_TAILORING) >= 280 && !player->HasSpell(26086))
            player->CastSpell(player, 26095, false);

        return true;
    }
};

class go_field_repair_bot_74A : public GameObjectScript
{
public:
    go_field_repair_bot_74A() : GameObjectScript("go_field_repair_bot_74A") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->HasSkill(SKILL_ENGINEERING) && player->GetBaseSkillValue(SKILL_ENGINEERING) >= 300 && !player->HasSpell(22704))
            player->CastSpell(player, 22864, false);

        return true;
    }
};

enum GildedBrazier
{
	QUEST_THE_FIRST_TRIAL = 9678,
    NPC_STILLBLADE        = 17716,
};

class go_gilded_brazier : public GameObjectScript
{
public:
    go_gilded_brazier() : GameObjectScript("go_gilded_brazier") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        {
            if (player->GetQuestStatus(QUEST_THE_FIRST_TRIAL) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* Stillblade = player->SummonCreature(NPC_STILLBLADE, 8106.11f, -7542.06f, 151.775f, 3.02598f, TEMPSUMMON_DEAD_DESPAWN, 60000))
                    Stillblade->AI()->AttackStart(player);
            }
        }
        return true;
    }
};

class go_orb_of_command : public GameObjectScript
{
public:
    go_orb_of_command() : GameObjectScript("go_orb_of_command") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->GetQuestRewardStatus(7761))
            player->CastSpell(player, 23460, true);

        return true;
    }
};

class go_tablet_of_madness : public GameObjectScript
{
public:
    go_tablet_of_madness() : GameObjectScript("go_tablet_of_madness") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->HasSkill(SKILL_ALCHEMY) && player->GetSkillValue(SKILL_ALCHEMY) >= 300 && !player->HasSpell(24266))
            player->CastSpell(player, 24267, false);

        return true;
    }
};

class go_tablet_of_the_seven : public GameObjectScript
{
public:
    go_tablet_of_the_seven() : GameObjectScript("go_tablet_of_the_seven") { }

    //TODO: use gossip option ("Transcript the Tablet") instead, if Trinity adds support.
    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() != GAMEOBJECT_TYPE_QUEST_GIVER)
            return true;

        if (player->GetQuestStatus(4296) == QUEST_STATUS_INCOMPLETE)
            player->CastSpell(player, 15065, false);

        return true;
    }
};

class go_jump_a_tron : public GameObjectScript
{
public:
    go_jump_a_tron() : GameObjectScript("go_jump_a_tron") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
		if (player->GetQuestStatus(10111) == QUEST_STATUS_INCOMPLETE)
			player->CastSpell(player, 33382, true);

		return true;
    }
};

enum EthereumPrison
{
    SPELL_REP_LC     = 39456,
    SPELL_REP_SHAT   = 39457,
    SPELL_REP_CE     = 39460,
    SPELL_REP_CON    = 39474,
    SPELL_REP_KT     = 39475,
    SPELL_REP_SPOR   = 39476,
};

const uint32 NpcPrisonEntry[] =
{
    22810, 22811, 22812, 22813, 22814, 22815,         // Good Guys
    20783, 20784, 20785, 20786, 20788, 20789, 20790   // Bad Guys
};

class go_ethereum_prison : public GameObjectScript
{
public:
    go_ethereum_prison() : GameObjectScript("go_ethereum_prison") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        int Random = rand() % (sizeof(NpcPrisonEntry) / sizeof(uint32));

        if (Creature* creature = player->SummonCreature(NpcPrisonEntry[Random], go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetAngle(player), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
        {
            if (!creature->IsHostileTo(player))
            {
                if (FactionTemplateEntry const* faction = creature->getFactionTemplateEntry())
                {
                    uint32 Spell = 0;

                    switch (faction->faction)
                    {
                        case 1011:
							Spell = SPELL_REP_LC;
							break;
                        case 935:
							Spell = SPELL_REP_SHAT;
							break;
                        case 942:
							Spell = SPELL_REP_CE;
							break;
                        case 933:
							Spell = SPELL_REP_CON;
							break;
                        case 989:
							Spell = SPELL_REP_KT;
							break;
                        case 970:
							Spell = SPELL_REP_SPOR;
							break;
                    }

                    if (Spell)
                        creature->CastSpell(player, Spell, false);
                    else
                        sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: go_ethereum_prison summoned Creature (entry %u) but faction (%u) are not expected by script.", creature->GetEntry(), creature->GetFaction());
                }
            }
        }
        return false;
    }
};

const uint32 NpcStasisEntry[] =
{
    22825, 20888, 22827, 22826, 22828
};

class go_ethereum_stasis : public GameObjectScript
{
public:
    go_ethereum_stasis() : GameObjectScript("go_ethereum_stasis") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        int Random = rand() % (sizeof(NpcStasisEntry) / sizeof(uint32));

        player->SummonCreature(NpcStasisEntry[Random],go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetAngle(player), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        return false;
    }
};

enum ResoniteCask
{
    NPC_GOGGEROC = 11920,
};

class go_resonite_cask : public GameObjectScript
{
public:
    go_resonite_cask() : GameObjectScript("go_resonite_cask") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
            go->SummonCreature(NPC_GOGGEROC, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

        return false;
    }
};

enum FireOfLife
{
	NPC_ARIKARA = 10882,
};

class go_sacred_fire_of_life : public GameObjectScript
{
public:
    go_sacred_fire_of_life() : GameObjectScript("go_sacred_fire_of_life") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
            player->SummonCreature(NPC_ARIKARA, -5008.338f, -2118.894f, 83.657f, 0.874f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

        return true;
    }
};

enum ShrineOfTheBirds
{
    NPC_HAWK_GUARD      = 22992,
    NPC_EAGLE_GUARD     = 22993,
    NPC_FALCON_GUARD    = 22994,
    GO_SHRINE_HAWK      = 185551,
    GO_SHRINE_EAGLE     = 185547,
    GO_SHRINE_FALCON    = 185553,
};

class go_shrine_of_the_birds : public GameObjectScript
{
public:
    go_shrine_of_the_birds() : GameObjectScript("go_shrine_of_the_birds") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        uint32 BirdEntry = 0;

        float fX, fY, fZ;
        go->GetClosePoint(fX, fY, fZ, go->GetObjectSize(), INTERACTION_DISTANCE);

        switch (go->GetEntry())
        {
            case GO_SHRINE_HAWK:
                BirdEntry = NPC_HAWK_GUARD;
                break;
            case GO_SHRINE_EAGLE:
                BirdEntry = NPC_EAGLE_GUARD;
                break;
            case GO_SHRINE_FALCON:
                BirdEntry = NPC_FALCON_GUARD;
                break;
        }

        if (BirdEntry)
			player->SummonCreature(BirdEntry, fX, fY, fZ, go->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

		return false;
    }
};

enum Southfury
{
    NPC_RIZZLE             = 23002,
    SPELL_BLACKJACK        = 39865,
    SPELL_SUMMON_RIZZLE    = 39866,
};

class go_southfury_moonstone : public GameObjectScript
{
public:
    go_southfury_moonstone() : GameObjectScript("go_southfury_moonstone") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (Creature* creature = player->SummonCreature(NPC_RIZZLE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
            creature->CastSpell(player, SPELL_BLACKJACK, false);

        return false;
    }
};

enum DalaranCrystal
{
    QUEST_LEARN_LEAVE_RETURN = 12790,
    QUEST_TELE_CRYSTAL_FLAG  = 12845,
};

#define GO_TELE_TO_DALARAN_CRYSTAL_FAILED "This teleport crystal cannot be used until the teleport crystal in Dalaran has been used at least once."

class go_tele_to_dalaran_crystal : public GameObjectScript
{
public:
    go_tele_to_dalaran_crystal() : GameObjectScript("go_tele_to_dalaran_crystal") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->GetQuestRewardStatus(QUEST_TELE_CRYSTAL_FLAG))
        {
            return false;
        }
        else
			player->GetSession()->SendNotification(GO_TELE_TO_DALARAN_CRYSTAL_FAILED);

        return true;
    }
};

class go_tele_to_violet_stand : public GameObjectScript
{
public:
    go_tele_to_violet_stand() : GameObjectScript("go_tele_to_violet_stand") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->GetQuestRewardStatus(QUEST_LEARN_LEAVE_RETURN) || player->GetQuestStatus(QUEST_LEARN_LEAVE_RETURN) == QUEST_STATUS_INCOMPLETE)
            return false;

        return true;
    }
};

#define GOSSIP_FEL_CRYSTALFORGE_TEXT 31000
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_TEXT_RETURN 31001
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_1 "Purchase 1 Unstable Flask of the Beast for the cost of 10 Apexis Shards"
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_5 "Purchase 5 Unstable Flask of the Beast for the cost of 50 Apexis Shards"
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_RETURN "Use the fel crystalforge to make another purchase."

enum FelCrystalforge
{
    SPELL_CREATE_1_FLASK_OF_BEAST = 40964,
    SPELL_CREATE_5_FLASK_OF_BEAST = 40965,
};

class go_fel_crystalforge : public GameObjectScript
{
public:
    go_fel_crystalforge() : GameObjectScript("go_fel_crystalforge") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER) /* != GAMEOBJECT_TYPE_QUEST_GIVER) */
            player->PrepareQuestMenu(go->GetGUID()); /* return true*/

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_TEXT, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->CastSpell(player, SPELL_CREATE_1_FLASK_OF_BEAST, false);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_ITEM_TEXT_RETURN, go->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CastSpell(player, SPELL_CREATE_5_FLASK_OF_BEAST, false);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_ITEM_TEXT_RETURN, go->GetGUID());
                break;
        case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_TEXT, go->GetGUID());
                break;
        }
        return true;
    }
};

#define GOSSIP_BASHIR_CRYSTALFORGE_TEXT 31100
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_TEXT_RETURN 31101
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_1 "Purchase 1 Unstable Flask of the Sorcerer for the cost of 10 Apexis Shards"
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_5 "Purchase 5 Unstable Flask of the Sorcerer for the cost of 50 Apexis Shards"
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_RETURN "Use the bashir crystalforge to make another purchase."

enum BashirCrystalforge
{
    SPELL_CREATE_1_FLASK_OF_SORCERER = 40968,
    SPELL_CREATE_5_FLASK_OF_SORCERER = 40970,
};

class go_bashir_crystalforge : public GameObjectScript
{
public:
    go_bashir_crystalforge() : GameObjectScript("go_bashir_crystalforge") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER) /* != GAMEOBJECT_TYPE_QUEST_GIVER) */
            player->PrepareQuestMenu(go->GetGUID()); /* return true*/

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_TEXT, go->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->CastSpell(player, SPELL_CREATE_1_FLASK_OF_SORCERER, false);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_ITEM_TEXT_RETURN, go->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CastSpell(player, SPELL_CREATE_5_FLASK_OF_SORCERER, false);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_ITEM_TEXT_RETURN, go->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_TEXT, go->GetGUID());
                break;
        }
        return true;
    }
};

enum MatrixPunchograph
{
    ITEM_WHITE_PUNCH_CARD      = 9279,
    ITEM_YELLOW_PUNCH_CARD     = 9280,
    ITEM_BLUE_PUNCH_CARD       = 9282,
    ITEM_RED_PUNCH_CARD        = 9281,
    ITEM_PRISMATIC_PUNCH_CARD  = 9316,
    SPELL_YELLOW_PUNCH_CARD    = 11512,
    SPELL_BLUE_PUNCH_CARD      = 11525,
    SPELL_RED_PUNCH_CARD       = 11528,
    SPELL_PRISMATIC_PUNCH_CARD = 11545,
    MATRIX_PUNCHOGRAPH_3005_A  = 142345,
    MATRIX_PUNCHOGRAPH_3005_B  = 142475,
    MATRIX_PUNCHOGRAPH_3005_C  = 142476,
    MATRIX_PUNCHOGRAPH_3005_D  = 142696,
};

class go_matrix_punchograph : public GameObjectScript
{
public:
    go_matrix_punchograph() : GameObjectScript("go_matrix_punchograph") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        switch (go->GetEntry())
        {
            case MATRIX_PUNCHOGRAPH_3005_A:
                if (player->HasItemCount(ITEM_WHITE_PUNCH_CARD, 1))
                {
                    player->DestroyItemCount(ITEM_WHITE_PUNCH_CARD, 1, true);
                    player->CastSpell(player, SPELL_YELLOW_PUNCH_CARD, true);
                }
                break;
            case MATRIX_PUNCHOGRAPH_3005_B:
                if (player->HasItemCount(ITEM_YELLOW_PUNCH_CARD, 1))
                {
                    player->DestroyItemCount(ITEM_YELLOW_PUNCH_CARD, 1, true);
                    player->CastSpell(player, SPELL_BLUE_PUNCH_CARD, true);
                }
                break;
            case MATRIX_PUNCHOGRAPH_3005_C:
                if (player->HasItemCount(ITEM_BLUE_PUNCH_CARD, 1))
                {
                    player->DestroyItemCount(ITEM_BLUE_PUNCH_CARD, 1, true);
                    player->CastSpell(player, SPELL_RED_PUNCH_CARD, true);
                }
                break;
            case MATRIX_PUNCHOGRAPH_3005_D:
                if (player->HasItemCount(ITEM_RED_PUNCH_CARD, 1))
                {
                    player->DestroyItemCount(ITEM_RED_PUNCH_CARD, 1, true);
                    player->CastSpell(player, SPELL_PRISMATIC_PUNCH_CARD, true);
                }
                break;
            default:
                break;
        }
        return false;
    }
};

enum ArcanePrison
{
    QUEST_PRISON_BREAK                = 11587,
	NPC_ARCANE_PRISONER               = 25318,
    SPELL_ARCANE_PRISONER_KILL_CREDIT = 45456,
};

class go_arcane_prison : public GameObjectScript
{
public:
    go_arcane_prison() : GameObjectScript("go_arcane_prison") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_PRISON_BREAK) == QUEST_STATUS_INCOMPLETE)
        {
            go->SummonCreature(NPC_ARCANE_PRISONER, 3485.089844f, 6115.7422188f, 70.966812f, 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
            player->CastSpell(player, SPELL_ARCANE_PRISONER_KILL_CREDIT, true);
            return true;
        }
		else
			return false;
    }
};

enum FilledOrb
{
	NPC_ZELEMAR = 17830,
};

class go_blood_filled_orb : public GameObjectScript
{
public:
    go_blood_filled_orb() : GameObjectScript("go_blood_filled_orb") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
            player->SummonCreature(NPC_ZELEMAR, -369.746f, 166.759f, -21.50f, 5.235f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

        return true;
    }
};

enum JotunheimCage
{
    NPC_EBON_BLADE_PRISONER_HUMAN   = 30186,
    NPC_EBON_BLADE_PRISONER_NE      = 30194,
    NPC_EBON_BLADE_PRISONER_TROLL   = 30196,
    NPC_EBON_BLADE_PRISONER_ORC     = 30195,

    SPELL_SUMMON_BLADE_KNIGHT_H     = 56207,
    SPELL_SUMMON_BLADE_KNIGHT_NE    = 56209,
    SPELL_SUMMON_BLADE_KNIGHT_ORC   = 56212,
    SPELL_SUMMON_BLADE_KNIGHT_TROLL = 56214,
};

class go_jotunheim_cage : public GameObjectScript
{
public:
    go_jotunheim_cage() : GameObjectScript("go_jotunheim_cage") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        Creature* prisoner = go->FindCreatureWithDistance(NPC_EBON_BLADE_PRISONER_HUMAN, 5.0f, true);
        if (!prisoner)
        {
            prisoner = go->FindCreatureWithDistance(NPC_EBON_BLADE_PRISONER_TROLL, 5.0f, true);
            if (!prisoner)
            {
                prisoner = go->FindCreatureWithDistance(NPC_EBON_BLADE_PRISONER_ORC, 5.0f, true);
                if (!prisoner)
                    prisoner = go->FindCreatureWithDistance(NPC_EBON_BLADE_PRISONER_NE, 5.0f, true);
            }
        }
        if (!prisoner || !prisoner->IsAlive())
            return false;

        prisoner->DisappearAndDie();
        player->KilledMonsterCredit(NPC_EBON_BLADE_PRISONER_HUMAN, 0);
        switch (prisoner->GetEntry())
        {
            case NPC_EBON_BLADE_PRISONER_HUMAN:
                player->CastSpell(player, SPELL_SUMMON_BLADE_KNIGHT_H, true);
                break;
            case NPC_EBON_BLADE_PRISONER_NE:
                player->CastSpell(player, SPELL_SUMMON_BLADE_KNIGHT_NE, true);
                break;
            case NPC_EBON_BLADE_PRISONER_TROLL:
                player->CastSpell(player, SPELL_SUMMON_BLADE_KNIGHT_TROLL, true);
                break;
            case NPC_EBON_BLADE_PRISONER_ORC:
                player->CastSpell(player, SPELL_SUMMON_BLADE_KNIGHT_ORC, true);
                break;
        }
        return true;
    }
};

enum TableTheka
{
    GOSSIP_TABLE_THEKA = 1653,
    QUEST_SPIDER_GOLD  = 2936,
};

class go_table_theka : public GameObjectScript
{
public:
    go_table_theka() : GameObjectScript("go_table_theka") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_SPIDER_GOLD) == QUEST_STATUS_INCOMPLETE)
            player->AreaExploredOrEventHappens(QUEST_SPIDER_GOLD);

        player->SEND_GOSSIP_MENU(GOSSIP_TABLE_THEKA, go->GetGUID());

        return true;
    }
};

enum InconspicuousLandmark
{
    SPELL_SUMMON_PIRATES_TREASURE_AND_TRIGGER_MOB = 11462,
    ITEM_CUERGOS_KEY                              = 9275,
    GO_PIRATES_TREASURE                           = 142194,
    NPC_TH_BUCANEER                               = 7902,
    NPC_TH_PIRATE                                 = 7899,
    NPC_TH_SWASHBUCKLER                           = 7901,
};

const Position SpawnsLocation[] =
{
    {-10278.858398f, -3885.251221f, 1.106929f, 2.862739f}, // pirate 1
    {-10278.865234f, -3881.008057f, 1.466278f, 3.514619f}, // pirate 2
    {-10283.445312f, -3875.822266f, 1.624895f, 4.649519f}, // pirate 3
    {-10290.837891f, -3880.942871f, 0.911992f, 5.984700f}, // pirate 4
    {-10287.276367f, -3888.869385f, 0.621543f, 1.020983f}, // pirate 5
};

class go_inconspicuous_landmark : public GameObjectScript
{
public:
    go_inconspicuous_landmark() : GameObjectScript("go_inconspicuous_landmark") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        go->SummonGameObject(GO_PIRATES_TREASURE, -10282.488281f, -3884.029785f, 1.041096f, 0, 0, 0, 0, 0, 180000); // chest

        if (player->HasItemCount(ITEM_CUERGOS_KEY, 1))
            return false;

        player->SummonCreature(NPC_TH_BUCANEER, SpawnsLocation[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT , 20000);
        player->SummonCreature(NPC_TH_PIRATE, SpawnsLocation[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
        player->SummonCreature(NPC_TH_SWASHBUCKLER, SpawnsLocation[2], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
        player->SummonCreature(NPC_TH_PIRATE, SpawnsLocation[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
        player->SummonCreature(NPC_TH_BUCANEER, SpawnsLocation[4], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
        //player->CastSpell(player, SPELL_SUMMON_PIRATES_TREASURE_AND_TRIGGER_MOB, true); // trigger mob part not working
        return true;
    }
};

enum EtherealTeleportPad
{
	NPC_IMAGE_WIND_TRADER      = 20518,
	ITEM_TELEPORTER_POWER_PACK = 28969,
};

class go_ethereal_teleport_pad : public GameObjectScript
{
public:
    go_ethereal_teleport_pad() : GameObjectScript("go_ethereal_teleport_pad") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (!player->HasItemCount(ITEM_TELEPORTER_POWER_PACK, 1))
            return false;

        go->SummonCreature(NPC_IMAGE_WIND_TRADER, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetAngle(player), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        return true;
    }
};

enum SoulWellData
{
    GO_SOUL_WELL_R1                     = 181621,
    GO_SOUL_WELL_R2                     = 193169,

    SPELL_IMPROVED_HEALTH_STONE_R1      = 18692,
    SPELL_IMPROVED_HEALTH_STONE_R2      = 18693,

    SPELL_CREATE_MASTER_HEALTH_STONE_R0 = 34130,
    SPELL_CREATE_MASTER_HEALTH_STONE_R1 = 34149,
    SPELL_CREATE_MASTER_HEALTH_STONE_R2 = 34150,

    SPELL_CREATE_FEL_HEALTH_STONE_R0    = 58890,
    SPELL_CREATE_FEL_HEALTH_STONE_R1    = 58896,
    SPELL_CREATE_FEL_HEALTH_STONE_R2    = 58898,
};

class go_soulwell : public GameObjectScript
{
    public:
        go_soulwell() : GameObjectScript("go_soulwell") {}

        struct go_soulwellAI : public GameObjectAI
        {
            go_soulwellAI(GameObject* go) : GameObjectAI(go)
            {
                _stoneSpell = 0;
                _stoneId = 0;
                switch (go->GetEntry())
                {
                    case GO_SOUL_WELL_R1:
                        _stoneSpell = SPELL_CREATE_MASTER_HEALTH_STONE_R0;
                        if (Unit* owner = go->GetOwner())
                        {
                            if (owner->HasAura(SPELL_IMPROVED_HEALTH_STONE_R1))
                                _stoneSpell = SPELL_CREATE_MASTER_HEALTH_STONE_R1;
                            else if (owner->HasAura(SPELL_CREATE_MASTER_HEALTH_STONE_R2))
                                _stoneSpell = SPELL_CREATE_MASTER_HEALTH_STONE_R2;
                        }
                        break;
                    case GO_SOUL_WELL_R2:
                        _stoneSpell = SPELL_CREATE_FEL_HEALTH_STONE_R0;
                        if (Unit* owner = go->GetOwner())
                        {
                            if (owner->HasAura(SPELL_IMPROVED_HEALTH_STONE_R1))
                                _stoneSpell = SPELL_CREATE_FEL_HEALTH_STONE_R1;
                            else if (owner->HasAura(SPELL_CREATE_MASTER_HEALTH_STONE_R2))
                                _stoneSpell = SPELL_CREATE_FEL_HEALTH_STONE_R2;
                        }
                        break;
                }
                if (_stoneSpell == 0) // Should never happen
                    return;

                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_stoneSpell);
                if (!spellInfo)
                    return;

                _stoneId = spellInfo->Effects[EFFECT_0].ItemType;
            }

			uint32 _stoneSpell;
			uint32 _stoneId;

            bool GossipHello(Player* player)
            {
                Unit* owner = go->GetOwner();
                if (_stoneSpell == 0 || _stoneId == 0)
                    return true;

                if (!owner || owner->GetTypeId() != TYPE_ID_PLAYER || !player->IsInSameRaidWith(owner->ToPlayer()))
                    return true;

                // Don't try to add a stone if we already have one.
                if (player->HasItemCount(_stoneId, 1))
                {
                    if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(_stoneSpell))
                        Spell::SendCastResult(player, spell, 0, SPELL_FAILED_TOO_MANY_OF_ITEM);
                    return true;
                }

                owner->CastSpell(player, _stoneSpell, true);
                // Item has to actually be created to remove a charge on the well.
                if (player->HasItemCount(_stoneId, 1))
                    go->AddUse();

                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const
        {
            return new go_soulwellAI(go);
        }
};

enum PrisonersOfWyrmskull
{
    QUEST_PRISONERS_OF_WYRMSKULL = 11255,
    NPC_PRISONER_PRIEST          = 24086,
    NPC_PRISONER_MAGE            = 24088,
    NPC_PRISONER_WARRIOR         = 24089,
    NPC_PRISONER_PALADIN         = 24090,
};

class go_dragonflayer_cage : public GameObjectScript
{
public:
    go_dragonflayer_cage() : GameObjectScript("go_dragonflayer_cage") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_PRISONERS_OF_WYRMSKULL) != QUEST_STATUS_INCOMPLETE)
            return true;

        Creature* prisoner = go->FindCreatureWithDistance(NPC_PRISONER_PRIEST, 2.0f);
        if (!prisoner)
        {
            prisoner = go->FindCreatureWithDistance(NPC_PRISONER_MAGE, 2.0f);
            if (!prisoner)
            {
                prisoner = go->FindCreatureWithDistance(NPC_PRISONER_WARRIOR, 2.0f);
                if (!prisoner)
                    prisoner = go->FindCreatureWithDistance(NPC_PRISONER_PALADIN, 2.0f);
            }
        }

        if (!prisoner || !prisoner->IsAlive())
            return true;

        Quest const* qInfo = sObjectMgr->GetQuestTemplate(QUEST_PRISONERS_OF_WYRMSKULL);
        if (qInfo)
        {
            //TODO: prisoner should help player for a short period of time
            player->KilledMonsterCredit(qInfo->RequiredNpcOrGo[0], 0);
            prisoner->DisappearAndDie();
        }
        return true;
    }
};

enum Tadpoles
{
    QUEST_OH_NOES_THE_TADPOLES = 11560,
    NPC_WINTERFIN_TADPOLE      = 25201,
};

class go_tadpole_cage : public GameObjectScript
{
public:
    go_tadpole_cage() : GameObjectScript("go_tadpole_cage") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_OH_NOES_THE_TADPOLES) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* pTadpole = go->FindCreatureWithDistance(NPC_WINTERFIN_TADPOLE, 1.0f);
            if (pTadpole)
            {
                go->UseDoorOrButton();
                pTadpole->DisappearAndDie();
                player->KilledMonsterCredit(NPC_WINTERFIN_TADPOLE, 0);
                //FIX: Summon minion tadpole
            }
        }
        return true;
    }
};

#define GOSSIP_USE_OUTHOUSE "Use the outhouse."
#define GO_ANDERHOLS_SLIDER_CIDER_NOT_FOUND "Quest item Anderhol's Slider Cider not found."

enum AmberpineOuthouse
{
    ITEM_ANDERHOLS_SLIDER_CIDER = 37247,
    NPC_OUTHOUSE_BUNNY          = 27326,
    QUEST_DOING_YOUR_DUTY       = 12227,
    SPELL_INDISPOSED            = 53017,
    SPELL_INDISPOSED_III        = 48341,
    SPELL_CREATE_AMBERSEEDS     = 48330,
    GOSSIP_OUTHOUSE_INUSE       = 12775,
    GOSSIP_OUTHOUSE_VACANT      = 12779,
};

class go_amberpine_outhouse : public GameObjectScript
{
public:
    go_amberpine_outhouse() : GameObjectScript("go_amberpine_outhouse") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        QuestStatus status = player->GetQuestStatus(QUEST_DOING_YOUR_DUTY);
        if (status == QUEST_STATUS_INCOMPLETE || status == QUEST_STATUS_COMPLETE || status == QUEST_STATUS_REWARDED)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_USE_OUTHOUSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_OUTHOUSE_VACANT, go->GetGUID());
            return true;
        }
        else
            player->SEND_GOSSIP_MENU(GOSSIP_OUTHOUSE_INUSE, go->GetGUID());
            return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF +1)
        {
            player->CLOSE_GOSSIP_MENU();
            Creature* target = GetClosestCreatureWithEntry(player, NPC_OUTHOUSE_BUNNY, 3.0f);
            if (target)
            {
                target->AI()->SetData(1, player->GetCurrentGender());
                go->CastSpell(target, SPELL_INDISPOSED_III);
            }
            go->CastSpell(player, SPELL_INDISPOSED);
            if (player->HasItemCount(ITEM_ANDERHOLS_SLIDER_CIDER, 1))
                go->CastSpell(player, SPELL_CREATE_AMBERSEEDS);
            return true;
        }
        else
        {
            player->CLOSE_GOSSIP_MENU();
            player->GetSession()->SendNotification(GO_ANDERHOLS_SLIDER_CIDER_NOT_FOUND);
            return false;
        }
    }
};

enum Hives
{
    QUEST_HIVE_IN_THE_TOWER = 9544,
    NPC_HIVE_AMBUSHER       = 13301,
};

class go_hive_pod : public GameObjectScript
{
public:
    go_hive_pod() : GameObjectScript("go_hive_pod") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        player->SendLoot(go->GetGUID(), LOOT_CORPSE);
        go->SummonCreature(NPC_HIVE_AMBUSHER, go->GetPositionX()+1, go->GetPositionY(), go->GetPositionZ(), go->GetAngle(player), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        go->SummonCreature(NPC_HIVE_AMBUSHER, go->GetPositionX(), go->GetPositionY()+1, go->GetPositionZ(), go->GetAngle(player), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        return true;
    }
};

class go_massive_seaforium_charge : public GameObjectScript
{
public:
	go_massive_seaforium_charge() : GameObjectScript("go_massive_seaforium_charge") { }

	bool OnGossipHello(Player* /*player*/, GameObject* go)
	{
		go->SetLootState(GO_JUST_DEACTIVATED);
		return true;
	}
};

enum LabWorkReagents
{
   QUEST_LAB_WORK                          = 12557,

   SPELL_WIRHERED_BATWING_KILL_CREDIT      = 51226,
   SPELL_MUDDY_MIRE_MAGGOT_KILL_CREDIT     = 51227,
   SPELL_AMBERSEED_KILL_CREDIT             = 51228,
   SPELL_CHILLED_SERPENT_MUCUS_KILL_CREDIT = 51229,

   GO_AMBERSEED                            = 190459,
   GO_CHILLED_SERPENT_MUCUS                = 190462,
   GO_WITHERED_BATWING                     = 190473,
   GO_MUDDY_MIRE_MAGGOTS                   = 190478,
};

class go_lab_work_reagents : public GameObjectScript
{
public:
    go_lab_work_reagents() : GameObjectScript("go_lab_work_reagents") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_LAB_WORK) == QUEST_STATUS_INCOMPLETE)
        {
            uint32 CreditSpellId = 0;
            switch (go->GetEntry())
            {
                case GO_AMBERSEED:
					CreditSpellId = SPELL_AMBERSEED_KILL_CREDIT;
					break;
                case GO_CHILLED_SERPENT_MUCUS:
					CreditSpellId = SPELL_CHILLED_SERPENT_MUCUS_KILL_CREDIT;
					break;
                case GO_WITHERED_BATWING:
					CreditSpellId = SPELL_WIRHERED_BATWING_KILL_CREDIT;
					break;
                case GO_MUDDY_MIRE_MAGGOTS:
					CreditSpellId = SPELL_MUDDY_MIRE_MAGGOT_KILL_CREDIT;
					break;
            }
            if (CreditSpellId)
                player->CastSpell(player, CreditSpellId, true);
        }
        return false;
    }
};

enum OfKeysAndCages
{
    QUEST_ALLIANCE_OF_KEYS_AND_CAGES = 11231,
    QUEST_HORDE_OF_KEYS_AND_CAGES    = 11265,
    NPC_GJALERBRON_PRISONER          = 24035,
    SAY_FREE                         = 0,
};

class go_gjalerbron_cage : public GameObjectScript
{
    public:
        go_gjalerbron_cage() : GameObjectScript("go_gjalerbron_cage") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            if ((player->GetTeamId() == TEAM_ALLIANCE && player->GetQuestStatus(QUEST_ALLIANCE_OF_KEYS_AND_CAGES) == QUEST_STATUS_INCOMPLETE) ||
                (player->GetTeamId() == TEAM_HORDE && player->GetQuestStatus(QUEST_HORDE_OF_KEYS_AND_CAGES) == QUEST_STATUS_INCOMPLETE))
            {
                if (Creature* prisoner = go->FindCreatureWithDistance(NPC_GJALERBRON_PRISONER, 5.0f))
                {
                    go->UseDoorOrButton();

                    if (player)
                        player->KilledMonsterCredit(NPC_GJALERBRON_PRISONER, 0);

                    prisoner->AI()->Talk(SAY_FREE);
                    prisoner->DespawnAfterAction(6*IN_MILLISECONDS);
                }
            }
            return true;
        }
};

class go_large_gjalerbron_cage : public GameObjectScript
{
    public:
        go_large_gjalerbron_cage() : GameObjectScript("go_large_gjalerbron_cage") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            if ((player->GetTeamId() == TEAM_ALLIANCE && player->GetQuestStatus(QUEST_ALLIANCE_OF_KEYS_AND_CAGES) == QUEST_STATUS_INCOMPLETE) ||
                (player->GetTeamId() == TEAM_HORDE && player->GetQuestStatus(QUEST_HORDE_OF_KEYS_AND_CAGES) == QUEST_STATUS_INCOMPLETE))
            {
                std::list<Creature*> prisonerList;
                GetCreatureListWithEntryInGrid(prisonerList, go, NPC_GJALERBRON_PRISONER, INTERACTION_DISTANCE);
                for (std::list<Creature*>::const_iterator itr = prisonerList.begin(); itr != prisonerList.end(); ++itr)
                {
                    go->UseDoorOrButton();
                    player->KilledMonsterCredit(NPC_GJALERBRON_PRISONER, (*itr)->GetGUID());
                    (*itr)->DespawnAfterAction(6*IN_MILLISECONDS);
                    (*itr)->AI()->Talk(SAY_FREE);
                }
            }
            return false;
		}
};

enum TheCleansing
{
   QUEST_THE_CLEANSING_HORDE    = 11317,
   QUEST_THE_CLEANSING_ALLIANCE = 11322,
   SPELL_CLEANSING_SOUL         = 43351,
   SPELL_RECENT_MEDITATION      = 61720,
};

class go_frostblade_shrine : public GameObjectScript
{
public:
    go_frostblade_shrine() : GameObjectScript("go_frostblade_shrine") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (!player->HasAura(SPELL_RECENT_MEDITATION))
		{
            if (player->GetQuestStatus(QUEST_THE_CLEANSING_HORDE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_CLEANSING_ALLIANCE) == QUEST_STATUS_INCOMPLETE)
            {
                go->UseDoorOrButton(10);
                player->CastSpell(player, SPELL_CLEANSING_SOUL);
                player->SetStandState(UNIT_STAND_STATE_SIT);
			}
		}
		return true;
	}
};

enum MidsummerBonfire
{
    STAMP_OUT_BONFIRE_QUEST_COMPLETE = 45458,
};

class go_midsummer_bonfire : public GameObjectScript
{
public:
    go_midsummer_bonfire() : GameObjectScript("go_midsummer_bonfire") { }

    bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/)
    {
        player->CastSpell(player, STAMP_OUT_BONFIRE_QUEST_COMPLETE, true);
        player->CLOSE_GOSSIP_MENU();
        return false;
    }
};

enum UdedDispenser
{
	QUEST_AMPLE_INSPIRATION   = 12828,

	SPELL_FORCE_ITEM_UDED     = 54576,
	SPELL_UNSTABLE_DETONATION = 54584,
};

class go_uded_dispenser : public GameObjectScript
{
public:
    go_uded_dispenser() : GameObjectScript("go_uded_dispenser") { }

	bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/)
    {
        if (player->GetQuestStatus(QUEST_AMPLE_INSPIRATION) == QUEST_STATUS_INCOMPLETE)
		{
			player->CastSpell(player, SPELL_FORCE_ITEM_UDED, true);
			player->CastSpell(player, SPELL_UNSTABLE_DETONATION, true);
		}

		player->CLOSE_GOSSIP_MENU();
        return false;
	}
};

enum MagharPrison
{
	QUEST_FINDING_THE_SIRVIVORS = 9948,
	NPC_MAGHAR_PRISONER         = 18428,
};

class go_maghar_prison : public GameObjectScript
{
public:
    go_maghar_prison() : GameObjectScript("go_maghar_prison") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
		Creature* maghar = GetClosestCreatureWithEntry(player, NPC_MAGHAR_PRISONER, 10.0f);

		if (maghar)
		{
			if (player->GetQuestStatus(QUEST_FINDING_THE_SIRVIVORS) == QUEST_STATUS_INCOMPLETE)
			{
				player->KilledMonsterCredit(NPC_MAGHAR_PRISONER, maghar->GetGUID());
				return false;
			}
		}
		return true;
	}
};

enum VeilSkithCage
{
	QUEST_MISSING_FRIENDS = 10852,
	NPC_CAPTIVE_CHILD     = 22314,
};

class go_veil_skith_cage_1 : public GameObjectScript
{
public:
    go_veil_skith_cage_1() : GameObjectScript("go_veil_skith_cage_1") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
		Creature* child = GetClosestCreatureWithEntry(player, NPC_CAPTIVE_CHILD, 10);

		if (child)
		{
			if (player->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
			{
				player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, child->GetGUID());
				return false;
			}
		}
		return true;
    }
};

class go_veil_skith_cage_2 : public GameObjectScript
{
public:
    go_veil_skith_cage_2() : GameObjectScript("go_veil_skith_cage_2") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
		Creature* child = GetClosestCreatureWithEntry(player, NPC_CAPTIVE_CHILD, 10);

		if (child)
		{
			if (player->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
			{
				player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, child->GetGUID());
				player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, child->GetGUID());
				return false;
			}
		}
		return true;
    }
};

class go_veil_skith_cage_3 : public GameObjectScript
{
public:
    go_veil_skith_cage_3() : GameObjectScript("go_veil_skith_cage_3") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
		Creature* child = GetClosestCreatureWithEntry(player, NPC_CAPTIVE_CHILD, 10);

		if (child)
		{
			if (player->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
			{
				player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, child->GetGUID());
				player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, child->GetGUID());
				player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, child->GetGUID());
				return false;
			}
		}
		return true;
    }
};

enum ConstructWorkbank
{
	QUEST_IRON_RUNE_CONST_JUMP  = 11485,
	QUEST_IRON_RUNE_CONST_DATA  = 11489,
	QUEST_IRON_RUNE_CONST_BLUFF = 11491,

	NPC_IRON_RUNE_CONST_DATA    = 24821,
    NPC_IRON_RUNE_CONST_BLUFF   = 24825,
	NPC_IRON_RUNE_CONST_JUMP    = 24823,
};

class go_iron_rune_construct_workbank : public GameObjectScript
{
public:
	go_iron_rune_construct_workbank() : GameObjectScript("go_iron_rune_construct_workbank") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_IRON_RUNE_CONST_JUMP) == QUEST_STATUS_INCOMPLETE)
		{
			if (Creature* pConstruct = player->SummonCreature(NPC_IRON_RUNE_CONST_JUMP, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation()))
			{
				pConstruct->SetLevel(player->GetCurrentLevel());
				player->EnterVehicle(pConstruct);
				player->GroupEventHappens(QUEST_IRON_RUNE_CONST_JUMP, pConstruct);
			}
			return true;
		}

		if (player->GetQuestStatus(QUEST_IRON_RUNE_CONST_DATA) == QUEST_STATUS_INCOMPLETE)
		{
			if (Creature* construct = player->SummonCreature(NPC_IRON_RUNE_CONST_DATA, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation()))
			{
				construct->SetLevel(player->GetCurrentLevel());
				player->EnterVehicle(construct);
			}
			return true;
		}

		if (player->GetQuestStatus(QUEST_IRON_RUNE_CONST_BLUFF) == QUEST_STATUS_INCOMPLETE)
		{
			if (Creature* construct = player->SummonCreature(NPC_IRON_RUNE_CONST_BLUFF, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation()))
			{
				construct->SetLevel(player->GetCurrentLevel());
				player->EnterVehicle(construct);
			}
			return true;
		}
		return true;
	}
};

#define GOSSIP_ICE_STONE "Place your hand on the Ice Stone"

enum IceStone
{
	SPELL_ICE_STONE_SPAWN    = 37846,

	NPC_FROSTWAVE_LIEUTENANT = 26116,
	NPC_HAILSTONE_LIEUTENANT = 26178,
	NPC_CHILLWIND_LIEUTENANT = 26204,
	NPC_FRIGID_LIEUTENANT    = 26214,
	NPC_GLACIAL_LIEUTENANT   = 26215,
	NPC_GLACIAL_TEMPLAR      = 26216,

	QUEST_STRIKING_BACK_1    = 11917,
	QUEST_STRIKING_BACK_2    = 11947,
	QUEST_STRIKING_BACK_3    = 11948,
	QUEST_STRIKING_BACK_4    = 11952,
	QUEST_STRIKING_BACK_5    = 11953,
	QUEST_STRIKING_BACK_6    = 11954,

	ZONE_ASHENVALE           = 331,
	ZONE_DESOLACE            = 405,
	ZONE_STRANGLETHORN       = 33,
	ZONE_SEARING_GORGE       = 51,
	ZONE_SILITHUS            = 1377,
	ZONE_HELLFIRE            = 3483,
};

class go_ice_stone : public GameObjectScript
{
public:
    go_ice_stone() : GameObjectScript("go_ice_stone") { }

	uint32 GetIceStoneSummonEntry(GameObject* go, Player* player)
    {
        if (player->GetQuestStatus(QUEST_STRIKING_BACK_1) == QUEST_STATUS_INCOMPLETE && go->GetZoneId() == ZONE_ASHENVALE)
            return NPC_FROSTWAVE_LIEUTENANT;

        if (player->GetQuestStatus(QUEST_STRIKING_BACK_2) == QUEST_STATUS_INCOMPLETE && go->GetZoneId() == ZONE_DESOLACE)
            return NPC_HAILSTONE_LIEUTENANT;

        if (player->GetQuestStatus(QUEST_STRIKING_BACK_3) == QUEST_STATUS_INCOMPLETE && go->GetZoneId() == ZONE_STRANGLETHORN)
            return NPC_CHILLWIND_LIEUTENANT;

        if (player->GetQuestStatus(QUEST_STRIKING_BACK_4) == QUEST_STATUS_INCOMPLETE && go->GetZoneId() == ZONE_SEARING_GORGE)
            return NPC_FRIGID_LIEUTENANT;

        if (player->GetQuestStatus(QUEST_STRIKING_BACK_5) == QUEST_STATUS_INCOMPLETE && go->GetZoneId() == ZONE_SILITHUS)
            return NPC_GLACIAL_LIEUTENANT;

        if (player->GetQuestStatus(QUEST_STRIKING_BACK_6) == QUEST_STATUS_INCOMPLETE && go->GetZoneId() == ZONE_HELLFIRE)
            return NPC_GLACIAL_TEMPLAR;

		return 0;
    }

    bool ShowIceStoneOption(GameObject*go, Player* player)
    {
        return go_ice_stone::GetIceStoneSummonEntry(go, player) > 0;
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
		if (go_ice_stone::ShowIceStoneOption(go, player))
		{
			if (!player->IsInCombatActive())
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ICE_STONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		}

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
        return true;
    }

	bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
    {
        switch (sender)
        {
            case GOSSIP_SENDER_MAIN:
        	    go_ice_stone::SendActionMenu(player, go, action);
				player->CastSpell(player, SPELL_ICE_STONE_SPAWN, true);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }

	void SendActionMenu(Player* player, GameObject* go, uint32 action)
	{
		switch (action)
		{
		    case GOSSIP_ACTION_INFO_DEF + 1:
				uint32 entry = go_ice_stone::GetIceStoneSummonEntry(go, player);
				if (entry > 0)
				{
					float x, y, z;
					go->GetPosition(x, y, z);
					Creature* creature = go->SummonCreature(entry, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
					if (creature && player)
						creature->AI()->AttackStart(player);
				}
				break;
		}
	}
};

enum SummoningStoneData
{
	QUEST_NOT_A_BUG_A   = 13342,
	QUEST_NOT_A_BUG_H   = 13358,
	QUEST_NOT_A_BUG_A_D = 13344,
	QUEST_NOT_A_BUG_H_D = 13365,
	NPC_DARK_MESSENGER  = 32316,
	NPC_NOT_A_BUG_KC    = 32314,
};

class go_not_a_bug : public GameObjectScript
{
public:
	go_not_a_bug() : GameObjectScript("go_not_a_bug") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_NOT_A_BUG_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_NOT_A_BUG_H) == QUEST_STATUS_INCOMPLETE ||
			player->GetQuestStatus(QUEST_NOT_A_BUG_A_D) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_NOT_A_BUG_H_D) == QUEST_STATUS_INCOMPLETE)
		{
			go->SummonCreature(NPC_DARK_MESSENGER, 8028.225098f,2077.155518f,499.729156f, 3.947409f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 5000);
			player->KilledMonsterCredit(NPC_NOT_A_BUG_KC, 0);
		}
		return true;
	}
};

enum AncientSkullPileData
{
	ITEM_TIMELOST_OFFERING = 32720,
	NPC_TEROKK             = 21838,
};

class go_ancient_skull_pile : public GameObjectScript
{
public:
	go_ancient_skull_pile() : GameObjectScript("go_ancient_skull_pile") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->HasItemCount(ITEM_TIMELOST_OFFERING, 1))
			player->ADD_GOSSIP_ITEM(9, "Invocar a Terokk", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
	{
		if (action = GOSSIP_ACTION_INFO_DEF+1)
		{
			if (Creature* terokk = player->SummonCreature(NPC_TEROKK, go->GetPositionX() + 4.0f, go->GetPositionY() + 2.0f, go->GetPositionZ(), go->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
			{
				terokk->AI()->AttackStart(player);
				terokk->GetMotionMaster()->Clear();
				terokk->GetMotionMaster()->MoveChase(player);
				player->DestroyItemCount(ITEM_TIMELOST_OFFERING, 1, true);
			}
		}

		player->CLOSE_GOSSIP_MENU();
		return true;
	}
};

enum RazormawData
{
	QUEST_RAZORMAW = 9689,
	NPC_RAZORMAW   = 17592,
};

class go_razormaw_pyre : public GameObjectScript
{
public:
    go_razormaw_pyre() : GameObjectScript("go_razormaw_pyre") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
		if (player->GetQuestStatus(QUEST_RAZORMAW) == QUEST_STATUS_INCOMPLETE)
		{
			player->SummonCreature(NPC_RAZORMAW, -1230.449341f, -12456.349609f, 95.382370f, 0.323573f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			return true;
		}
		return true;
    }
};

enum MistwhisperTreasure
{
	QUEST_LOST_MISTWHISPER_TREASURE = 12575,
	NPC_WARLORD_TARTEK              = 28105,
	ITEM_MISTWHISPER_TREASURE       = 38601,
};

class go_mistwhisper_treasure : public GameObjectScript
{
public:
    go_mistwhisper_treasure() : GameObjectScript("go_mistwhisper_treasure") { }

	uint64 tartekGUID;

    bool OnGossipHello(Player* player, GameObject* go)
    {
		if (player->HasItemCount(ITEM_MISTWHISPER_TREASURE, 1) && player->GetQuestStatus(QUEST_LOST_MISTWHISPER_TREASURE) == QUEST_STATUS_INCOMPLETE)
        {
			if (tartekGUID)
			{
                if (Creature* tartek = player->GetCreature(*player, tartekGUID))
				{
                    if (tartek->IsAlive())
					{
                        return false;
					}
                    else
						tartek->DespawnAfterAction();
				}
			}

            if (Creature*  tartek = go->SummonCreature(NPC_WARLORD_TARTEK, 6708.30f, 5147.15f, 0.92712f, 4.9878f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
            {
                tartekGUID = tartek->GetGUID();
                tartek->GetMotionMaster()->MovePoint(0, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
            }
        }
        return true;
    }
};

#define GOSSIP_WICKERMAN_EMBER "Smear the ash on my face like war paint!"

enum WickermanEmber
{
	SPELL_GRIM_VISAGE = 24705,
};

class go_wickerman_ember : public GameObjectScript
{
public:
    go_wickerman_ember() : GameObjectScript("go_wickerman_ember") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (!player->HasAura(SPELL_GRIM_VISAGE))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WICKERMAN_EMBER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF)
            go->CastSpell(player, SPELL_GRIM_VISAGE);

        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        return true;
    }
};

enum SoothsayingData
{
    TALK_SWORD_UNLEARN              = 10329,    // "Forgetting your Swordsmithing skill is not something to do lightly. If you choose to abandon it you will forget all recipes that require Swordsmithing to create!"
    TALK_TRANSMUTE_UNLEARN          = 11076,    // "Forgetting your skill in Transmutation Mastery is not something to do lightly.  Are you absolutely sure?"
    TALK_ELIXIR_UNLEARN             = 11075,    // "Forgetting your skill in Elixir Mastery is not something to do lightly.  Are you absolutely sure?"
    TALK_ELEMENTAL_UNLEARN          = 10302,    // "Forgetting elemental leatherworking is not something to do lightly.  If you choose to abandon it you will forget all recipes that require elemental leatherworking as well!"
    TALK_LEATHER_SPEC_LEARN         = 8326,     // The assortment of images, shapes, and markings come together before your eyes. The book seems to know your skill with leatherworking and offers you a choice - but be warned - you may only select one and once you have done so, may not change your mind!:
    // engineering specsialisations
    S_GOBLIN                        = 20222,
    S_GNOMISH                       = 20219,
    S_LEARN_GOBLIN                  = 20221,
    S_LEARN_GNOMISH                 = 20220,
    S_UNLEARN_GOBLIN                = 68334,
    S_UNLEARN_GNOMISH               = 68333,
    // leatherworking specialisations
    S_DRAGON                        = 10656,
    S_ELEMENTAL                     = 10658,
    S_TRIBAL                        = 10660,
    S_LEARN_DRAGON                  = 10657,
    S_LEARN_ELEMENTAL               = 10659,
    S_LEARN_TRIBAL                  = 10661,
    // generic check
    GOSSIP_SENDER_CHECK             = 52,
};

#define TALK_ENGINEER_SPEC_LEARN    "Hundreds of various diagrams and schematics begin to take shape on the pages of the book. You recognize some of the diagrams while others remain foreign but familiar."
#define BOX_UNLEARN_GOBLIN_SPEC     "Do you really want to unlearn your Goblin Engineering specialization and lose all asociated recipes?"
#define BOX_UNLEARN_GNOMISH_SPEC    "Do you really want to unlearn your Gnomish Engineering specialization and lose all asociated recipes?"

#define GOSSIP_LEARN_GOBLIN         "I am 100% confident that I wish to learn in the ways of goblin engineering."
#define GOSSIP_UNLEARN_GOBLIN       "I wish to unlearn my Goblin Engineering specialization!"
#define GOSSIP_LEARN_GNOMISH        "I am 100% confident that I wish to learn in the ways of gnomish engineering."
#define GOSSIP_UNLEARN_GNOMISH      "I wish to unlearn my Gnomish Engineering specialization!"

#define GOSSIP_LEARN_DRAGON         "I am absolutely certain that i want to learn dragonscale leatherworking"
#define GOSSIP_UNLEARN_DRAGON       "I wish to unlearn Dragonscale Leatherworking"
#define GOSSIP_LEARN_ELEMENTAL      "I am absolutely certain that i want to learn elemental leatherworking"
#define GOSSIP_UNLEARN_ELEMENTAL    "I wish to unlearn Elemental Leatherworking"
#define GOSSIP_LEARN_TRIBAL         "I am absolutely certain that i want to learn tribal leatherworking"
#define GOSSIP_UNLEARN_TRIBAL       "I wish to unlearn Tribal Leatherworking"

bool HasLeatherSpell(Player* player)
{
    if (player->HasSpell(S_ELEMENTAL) || player->HasSpell(S_DRAGON) || player->HasSpell(S_TRIBAL))
        return true;
    return false;
}

bool HasEngineerSpell(Player* player)
{
    if (player->HasSpell(S_GNOMISH) || player->HasSpell(S_GOBLIN))
        return true;
    return false;
}

class go_soothsaying_for_dummies : public GameObjectScript
{
    public:
        go_soothsaying_for_dummies() : GameObjectScript("go_soothsaying_for_dummies") { }

        bool OnGossipHello(Player* player, GameObject* gameObject)
        {
            uint32 eGameObject = gameObject->GetEntry();

            if (player->HasSkill(SKILL_LEATHERWORKING) && player->GetBaseSkillValue(SKILL_LEATHERWORKING) >= 225 && player->GetCurrentLevel() > 39)
            {
                if (player->GetQuestRewardStatus(5143) || player->GetQuestRewardStatus(5144) || player->GetQuestRewardStatus(5145))
                {
                    if (!HasLeatherSpell(player))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_DRAGON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_ELEMENTAL, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_TRIBAL, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 3);
                        player->SEND_GOSSIP_MENU(TALK_LEATHER_SPEC_LEARN, gameObject->GetGUID());
                        return true;
                    }
                }
            }

            if (player->HasSkill(SKILL_ENGINEERING) && player->GetBaseSkillValue(SKILL_ENGINEERING) >= 200 && player->GetCurrentLevel() > 9)
            {
                if (player->GetQuestRewardStatus(3643) || player->GetQuestRewardStatus(3641) || player->GetQuestRewardStatus(3639))
                {
                    if (!HasEngineerSpell(player))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_GNOMISH, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 4);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEARN_GOBLIN, GOSSIP_SENDER_CHECK, GOSSIP_ACTION_INFO_DEF + 5);
                        player->SEND_GOSSIP_MENU(player->GetGossipTextId(gameObject), gameObject->GetGUID());
                        return true;
                    }

                    if (player->HasSpell(S_GNOMISH))
                        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GOSSIP_UNLEARN_GNOMISH, GOSSIP_SENDER_CHECK,  GOSSIP_ACTION_INFO_DEF + 6, BOX_UNLEARN_GNOMISH_SPEC, 1500000,false);
                    if (player->HasSpell(S_GOBLIN))
                        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GOSSIP_UNLEARN_GOBLIN, GOSSIP_SENDER_CHECK,  GOSSIP_ACTION_INFO_DEF + 7, BOX_UNLEARN_GNOMISH_SPEC, 1500000,false);
                }
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(gameObject), gameObject->GetGUID()); // Book's default text
            return true;
        }


        void SendActionMenu(Player* player, GameObject* gameObject, uint32 action)
        {
            switch(action)
            {
                //Learn Leather spec
                case GOSSIP_ACTION_INFO_DEF + 1:
                    if (!player->HasSpell(S_DRAGON))
                        player->CastSpell(player, S_LEARN_DRAGON, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    if (!player->HasSpell(S_ELEMENTAL))
                        player->CastSpell(player, S_LEARN_ELEMENTAL, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    if (!player->HasSpell(S_TRIBAL))
                        player->CastSpell(player, S_LEARN_TRIBAL, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                //Learn Engineering spec
                case GOSSIP_ACTION_INFO_DEF + 4:
                    if (!player->HasSpell(S_GNOMISH))
                        player->CastSpell(player, S_LEARN_GNOMISH, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5:
                    if (!player->HasSpell(S_GOBLIN))
                        player->CastSpell(player, S_LEARN_GOBLIN, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                //Unlearn Engineering spec
                case GOSSIP_ACTION_INFO_DEF + 6:
                    if (EquippedOk(player,S_UNLEARN_GNOMISH))
                    {
                        if (player->HasEnoughMoney(1500000))
                        {
                            player->CastSpell(player, S_UNLEARN_GNOMISH, true);
                            player->removeSpell(30575);                     // Gnomish Battle Goggles
                            player->removeSpell(30574);                     // Gnomish Power Goggles
                            player->removeSpell(56473);                     // Gnomish X-Ray Specs
                            player->removeSpell(30569);                     // Gnomish Poultryizer
                            player->removeSpell(30563);                     // Ultrasafe Transporter - Toshley's Station
                            player->removeSpell(23489);                     // Ultrasafe Transporter - Gadgetzan
                            player->removeSpell(23129);                     // World Enlarger
                            player->ModifyMoney(-1500000);
                        }
                        else
                            player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
                    }
                    else
                        player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW,NULL,NULL);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 7:
                    if (EquippedOk(player,S_UNLEARN_GOBLIN))
                    {
                        if (player->HasEnoughMoney(1500000))
                        {
                            player->CastSpell(player, S_UNLEARN_GOBLIN, true);
                            player->removeSpell(30565);                     // Foreman's Enchanted Helmet
                            player->removeSpell(30566);                     // Foreman's Reinforced Helmet
                            player->removeSpell(30563);                     // Goblin Rocket Launcher
                            player->removeSpell(56514);                     // Global Thermal Sapper Charge
                            player->removeSpell(36954);                     // Dimensional Ripper - Area 52
                            player->removeSpell(23486);                     // Dimensional Ripper - Everlook
                            player->ModifyMoney(-1500000);
                        }
                        else
                            player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
                    }
                    else
                        player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW,NULL,NULL);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
        }


        bool OnGossipSelect(Player* player, GameObject* gameObject, uint32 sender, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (sender)
            {
                case GOSSIP_SENDER_MAIN:
                    SendActionMenu(player, gameObject, action);
                    break;
                case GOSSIP_SENDER_CHECK:
                    SendActionMenu(player, gameObject, action);
                    break;
            }
            return true;
        }

        bool EquippedOk(Player* player, uint32 spellId)
        {
            SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
            if (!spell)
                return false;

            for (uint8 i = 0; i < 3; ++i)
            {
                uint32 reqSpell = spell->Effects[i].TriggerSpell;
                if (!reqSpell)
                    continue;

                Item* item = NULL;
                for (uint8 j = EQUIPMENT_SLOT_START; j < EQUIPMENT_SLOT_END; ++j)
                {
                    item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, j);
                    if (item && item->GetTemplate()->RequiredSpell == reqSpell)
                    {
                        sLog->outDebug(LOG_FILTER_QCSCR, "player attempt to unlearn spell %u, but item %u is equipped.", reqSpell, item->GetEntry());
                        return false;
                    }
                }
            }
            return true;
        }
};

class go_dalaran_portal : public GameObjectScript
{
public:
	go_dalaran_portal() : GameObjectScript("go_dalaran_portal_sunwell") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->SetPhaseMask(1, true);
		player->TeleportTo(571, 5804.15f, 624.771f, 647.767f, 1.64f);
		return false;
	}
};

enum Blastenheimer5000
{
	SPELL_CANON_PREP    = 24832,
	SPELL_MAGIC_WINGS_1 = 24742,
	SPELL_MAGIC_WINGS_2 = 42867,
};

class go_blastenheimer_5000_mulgore : public GameObjectScript
{
public:
	go_blastenheimer_5000_mulgore() : GameObjectScript("go_blastenheimer_5000_mulgore") { }

	struct go_blastenheimer_5000_mulgoreAI : public GameObjectAI
	{
		go_blastenheimer_5000_mulgoreAI(GameObject* gameobject) : GameObjectAI(gameobject) { }

		uint32 CannonShootTimer;

		bool RespawnInFlow;

		void OnStateChanged(uint32 state, Unit* /*unit*/)
		{
			if (state == GO_ACTIVATED)
			{
				if (Player* player = go->FindPlayerWithDistance(2.0f))
				{
					player->SetMovement(MOVE_ROOT);
					player->CastSpell(player, SPELL_CANON_PREP, true);
					player->NearTeleportTo(-1326.711914f, 86.301125f, 133.093918f, 3.510725f, true);
					RespawnInFlow = true;
				}
				CannonShootTimer = 5000;
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (!RespawnInFlow)
				return;

			if (CannonShootTimer <= diff)
			{
				if (Player* player = go->FindPlayerWithDistance(2.0f))
				{
					player->SetMovement(MOVE_UNROOT);
					go->CastSpell(player, SPELL_MAGIC_WINGS_1);
					RespawnInFlow = false;
				}
			}
			else CannonShootTimer -= diff;
		}
	};

	GameObjectAI* GetAI(GameObject* go) const
	{
		return new go_blastenheimer_5000_mulgoreAI(go);
	}
};

class go_blastenheimer_5000_shattrath : public GameObjectScript
{
public:
	go_blastenheimer_5000_shattrath() : GameObjectScript("go_blastenheimer_5000_shattrath") { }

	struct go_blastenheimer_5000_shattrathAI : public GameObjectAI
	{
		go_blastenheimer_5000_shattrathAI(GameObject* gameobject) : GameObjectAI(gameobject) { }

		uint32 CannonShootTimer;

		bool RespawnInFlow;

		void OnStateChanged(uint32 state, Unit* /*unit*/)
		{
			if (state == GO_ACTIVATED)
			{
				if (Player* player = go->FindPlayerWithDistance(5.0f))
				{
					player->SetMovement(MOVE_ROOT);
					player->CastSpell(player, SPELL_CANON_PREP, true);
					RespawnInFlow = true;
				}
				CannonShootTimer = 5000;
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (!RespawnInFlow)
				return;

			if (CannonShootTimer <= diff)
			{
				if (Player* player = go->FindPlayerWithDistance(5.0f))
				{
					player->SetMovement(MOVE_UNROOT);
					go->CastSpell(player, SPELL_MAGIC_WINGS_1);
					player->TeleportTo(530, -1806.32f, 4967.38f, 77.6719f, 4.43879f);
					RespawnInFlow = false;
				}
			}
			else CannonShootTimer -= diff;
		}
	};

	GameObjectAI* GetAI(GameObject* go) const
	{
		return new go_blastenheimer_5000_shattrathAI(go);
	}
};

enum MidsummerFlames
{
	SPELL_HIT_FLAMES_DARNASSUS     = 29126,
	SPELL_HIT_FLAMES_IRONFORGE     = 29135,
	SPELL_HIT_FLAMES_ORGRIMMAR     = 29136,
	SPELL_HIT_FLAMES_STORMWIND     = 29137,
	SPELL_HIT_FLAMES_THUNDER_BLUFF = 29138,
	SPELL_HIT_FLAMES_UNDERCITY     = 29139,
	SPELL_HIT_FLAMES_EXODAR        = 46671,
	SPELL_HIT_FLAMES_SILVERMOON    = 46672,
};

class go_flame_of_orgrimmar : public GameObjectScript
{
public:
	go_flame_of_orgrimmar() : GameObjectScript("go_flame_of_orgrimmar") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_ORGRIMMAR, true);
		return false;
	}
};

class go_flame_of_undercity : public GameObjectScript
{
public:
	go_flame_of_undercity() : GameObjectScript("go_flame_of_undercity") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_UNDERCITY, true);
		return false;
	}
};

class go_flame_of_thunder_bluff : public GameObjectScript
{
public:
	go_flame_of_thunder_bluff() : GameObjectScript("go_flame_of_thunder_bluff") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_THUNDER_BLUFF, true);
		return false;
	}
};

class go_flame_of_silvermoon : public GameObjectScript
{
public:
	go_flame_of_silvermoon() : GameObjectScript("go_flame_of_silvermoon") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_SILVERMOON, true);
		return false;
	}
};

class go_flame_of_stormwind : public GameObjectScript
{
public:
	go_flame_of_stormwind() : GameObjectScript("go_flame_of_stormwind") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_STORMWIND, true);
		return false;
	}
};

class go_flame_of_darnassus : public GameObjectScript
{
public:
	go_flame_of_darnassus() : GameObjectScript("go_flame_of_darnassus") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_DARNASSUS, true);
		return false;
	}
};

class go_flame_of_ironforge : public GameObjectScript
{
public:
	go_flame_of_ironforge() : GameObjectScript("go_flame_of_ironforge") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_IRONFORGE, true);
		return false;
	}
};

class go_flame_of_exodar : public GameObjectScript
{
public:
	go_flame_of_exodar() : GameObjectScript("go_flame_of_exodar") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		player->CastSpell(player, SPELL_HIT_FLAMES_EXODAR, true);
		return false;
	}
};

enum RibbonPole
{
	SPELL_RIBBON_POLE_EXP         = 29175,
	SPELL_RIBBON_POLE_START_DANCE = 29531,
	SPELL_HAS_FULL_MIDSUMMER_SET  = 58933,

	ACHIEVEMENT_HOT_POLE_DANCE    = 271,

	SOUND_RIBBON_POLE_1           = 12319,
};

class go_ribbon_pole : public GameObjectScript
{
public:
	go_ribbon_pole() : GameObjectScript("go_ribbon_pole") { }

	bool OnGossipHello(Player* player, GameObject* /*go*/)
	{
		if (player->HasAura(SPELL_HAS_FULL_MIDSUMMER_SET)
			&& !player->HasAura(SPELL_RIBBON_POLE_EXP)
			&& !player->HasAchieved(ACHIEVEMENT_HOT_POLE_DANCE))
		{
			player->CastSpell(player, SPELL_RIBBON_POLE_START_DANCE, true);
			player->PlayDirectSound(SOUND_RIBBON_POLE_1);
		}
		return false;
	}
};

enum ScourgeCage
{
    NPC_SCOURGE_PRISONER   = 25610,
	QUEST_MERCIFUL_FREEDOM = 11676,
};

class go_scourge_cage_borean_tundra : public GameObjectScript
{
public:
	go_scourge_cage_borean_tundra() : GameObjectScript("go_scourge_cage_borean_tundra") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_MERCIFUL_FREEDOM) == QUEST_STATUS_INCOMPLETE)
		{
			if (Creature* prisoner = go->FindCreatureWithDistance(NPC_SCOURGE_PRISONER, 5.0f, true))
			{
				go->SetGoState(GO_STATE_ACTIVE);
				player->KilledMonsterCredit(NPC_SCOURGE_PRISONER, prisoner->GetGUID());
				prisoner->DisappearAndDie();
			}
		}
		return true;
	}
};

void AddSC_go_scripts()
{
    new go_cat_figurine();
    new go_northern_crystal_pylon();
    new go_eastern_crystal_pylon();
    new go_western_crystal_pylon();
    new go_barov_journal();
    new go_field_repair_bot_74A();
    new go_gilded_brazier();
    new go_orb_of_command();
    new go_shrine_of_the_birds();
    new go_southfury_moonstone();
    new go_tablet_of_madness();
    new go_tablet_of_the_seven();
    new go_jump_a_tron();
    new go_ethereum_prison();
    new go_ethereum_stasis();
    new go_resonite_cask();
    new go_sacred_fire_of_life();
    new go_tele_to_dalaran_crystal();
    new go_tele_to_violet_stand();
    new go_fel_crystalforge();
    new go_bashir_crystalforge();
    new go_matrix_punchograph();
    new go_arcane_prison();
    new go_blood_filled_orb();
    new go_jotunheim_cage();
    new go_table_theka();
    new go_inconspicuous_landmark();
	new go_ethereal_teleport_pad;
    new go_soulwell();
    new go_tadpole_cage();
    new go_dragonflayer_cage();
    new go_amberpine_outhouse();
    new go_hive_pod();
    new go_massive_seaforium_charge();
    new go_lab_work_reagents();
    new go_gjalerbron_cage();
    new go_large_gjalerbron_cage();
	new go_frostblade_shrine();
	new go_midsummer_bonfire();
	new go_uded_dispenser();
	new go_maghar_prison();
	new go_veil_skith_cage_1();
	new go_veil_skith_cage_2();
	new go_veil_skith_cage_3();
	new go_iron_rune_construct_workbank();
	new go_ice_stone();
	new go_not_a_bug();
	new go_ancient_skull_pile();
	new go_razormaw_pyre();
	new go_mistwhisper_treasure();
	new go_wickerman_ember();
	new go_soothsaying_for_dummies();
	new go_dalaran_portal();
	new go_blastenheimer_5000_mulgore();
	new go_blastenheimer_5000_shattrath();
	new go_flame_of_orgrimmar();
	new go_flame_of_undercity();
	new go_flame_of_thunder_bluff();
	new go_flame_of_silvermoon();
	new go_flame_of_stormwind();
	new go_flame_of_darnassus();
	new go_flame_of_ironforge();
	new go_flame_of_exodar();
	new go_ribbon_pole();
	new go_scourge_cage_borean_tundra();
}