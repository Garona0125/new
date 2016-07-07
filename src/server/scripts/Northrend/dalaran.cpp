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

enum Zone
{
	ZONE_DALARAN = 4395,
};

enum Spells
{
    SPELL_SUNREAVER_DISGUISE_FEMALE       = 70973,
    SPELL_SUNREAVER_DISGUISE_MALE         = 70974,
    SPELL_SILVER_COVENANT_DISGUISE_FEMALE = 70971,
    SPELL_SILVER_COVENANT_DISGUISE_MALE   = 70972,
	SPELL_TRESPASSER_ALLIANCE             = 54028,
	SPELL_TRESPASSER_HORDE                = 54029,
};

enum Creatures
{
	NPC_SILVER_COVENANT_GUARDIAN = 29254,
	NPC_SUNREAVER_GUARDIAN_MAGE  = 29255,
    NPC_APPLEBOUGH_ALLIANCE      = 29547,
    NPC_SWEETBERRY_HORDE         = 29715,
};

class npc_mageguard_dalaran : public CreatureScript
{
public:
    npc_mageguard_dalaran() : CreatureScript("npc_mageguard_dalaran") { }

    struct npc_mageguard_dalaranAI : public QuantumBasicAI
    {
        npc_mageguard_dalaranAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_NORMAL, true);
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
        }

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who->IsWithinDistInMap(me, 5.0f))
                return;

            Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
            
            if (!player || player->IsGameMaster() || player->IsBeingTeleported() ||
                player->HasAura(SPELL_SUNREAVER_DISGUISE_FEMALE) || player->HasAura(SPELL_SUNREAVER_DISGUISE_MALE) ||
                player->HasAura(SPELL_SILVER_COVENANT_DISGUISE_FEMALE) || player->HasAura(SPELL_SILVER_COVENANT_DISGUISE_MALE))
                return;

            switch (me->GetEntry())
            {
                case NPC_SILVER_COVENANT_GUARDIAN:
					if (player->GetTeam() == HORDE)
                    {
						if (who->IsWithinDistInMap(me, 5.0f))
						{
							if (Player* player = who->ToPlayer())
							{
								player->TeleportTo(571, 5781.86f, 698.341f, 642.659f, 2.47039f);
								me->CastSpell(player, SPELL_TRESPASSER_ALLIANCE, true);
							}
						}
                        else
						{
							if (Player* player = who->ToPlayer())
							{
								player->TeleportTo(571, 5781.86f, 698.341f, 642.659f, 2.47039f);
								me->CastSpell(player, SPELL_TRESPASSER_ALLIANCE, true);
							}
						}
                    }
                    break;
                case NPC_SUNREAVER_GUARDIAN_MAGE:
                    if (player->GetTeam() == ALLIANCE)
                    {
						if (who->IsWithinDistInMap(me, 5.0f))
						{
							if (Player* player = who->ToPlayer())
							{
								player->TeleportTo(571, 5823.02f, 625.094f, 647.254f, 647.254f);
								me->CastSpell(player, SPELL_TRESPASSER_HORDE, true);
							}
						}
                        else
						{
							if (Player* player = who->ToPlayer())
							{
								player->TeleportTo(571, 5823.02f, 625.094f, 647.254f, 647.254f);
								me->CastSpell(player, SPELL_TRESPASSER_HORDE, true);
							}
						}
                    }
                    break;
            }
            me->SetOrientation(me->GetHomePosition().GetOrientation());
            return;
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mageguard_dalaranAI(creature);
    }
};

enum ArchmageVargoth
{
	SPELL_CREATE_FAMILAR     = 61457,
    SPELL_FAMILAR_PET        = 61472,

    ITEM_ACANE_MAGIC_MASTERY = 43824,
	ITEM_FAMILAR_PET         = 44738,
};

#define GOSSIP_TEXT_FAMILIAR_WELCOME "I have a book that might interest you. Would you like to take a look?"
#define GOSSIP_TEXT_FAMILIAR_THANKS  "Thank you! I will be sure to notify you if I find anything else."

class npc_archmage_vargoth : public CreatureScript
{
public:
    npc_archmage_vargoth() : CreatureScript("npc_archmage_vargoth") {}

    bool OnGossipHello (Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver() && creature->GetZoneId() != ZONE_DALARAN)
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->HasItemCount(ITEM_ACANE_MAGIC_MASTERY ,1 ,false))
        {
			if (!player->HasSpell(SPELL_FAMILAR_PET) && !player->HasItemCount(ITEM_FAMILAR_PET, 1, true))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_FAMILIAR_WELCOME, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect (Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
		player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_FAMILIAR_THANKS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(40000, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                creature->CastSpell(player, SPELL_CREATE_FAMILAR, false);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }
};

#define GOSSIP_TEXT_ARCANIST_TYBALIN "I'm ready to deliver the tome, Arcanist Tybalin"

enum ArcanistData
{
	SPELL_CREATE_DRAGONFORGED_BLADES = 69722,
	QUEST_AUDIENCE_WITH_THE_ARCANIST = 24451,
	QUEST_MEETING_WITH_THE_MAGISTER  = 20439,
};

class npc_arcanist_tybalin : public CreatureScript
{
public:
    npc_arcanist_tybalin() : CreatureScript("npc_arcanist_tybalin") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->GetQuestStatus(QUEST_AUDIENCE_WITH_THE_ARCANIST)!= QUEST_STATUS_NONE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_ARCANIST_TYBALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

		player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
	}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
		player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_TRAIN)
			player->CastSpell(player, SPELL_CREATE_DRAGONFORGED_BLADES, true);

        return true;
    }
};

enum Rhonin
{
	SPELL_THE_SCHOOLS_OF_ARCANE_MAGIC = 59983,
	ITEM_THE_SCHOOLS_OF_ARCANE_MAGIC  = 43824,
    ACHIEVEMENT_HIGHER_LEARNING       = 1956,
};

#define GOSSIP_TEXT_RESTORE_ITEM "Request give me new <The schools of the arcane magic - control>"

class npc_rhonin : public CreatureScript
{
public:
	npc_rhonin() : CreatureScript("npc_rhonin") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->HasAchieved(ACHIEVEMENT_HIGHER_LEARNING) && !player->HasItemCount(ITEM_THE_SCHOOLS_OF_ARCANE_MAGIC, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_RESTORE_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CastSpell(player, SPELL_THE_SCHOOLS_OF_ARCANE_MAGIC, false);
			player->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
};

enum Gossips
{
	GOSSIP_MESSAGE_MEMORIAL     = 15921,
	MOVIE_FALL_OF_THE_LICH_KING = 16,

	FALL_OF_THE_LICH_KING,
};

#define GOSSIP_TEXT_SEE_FALL_OF_THE_LICH_KING "See the fall of the Lich King."

class go_dedication_of_honor : public GameObjectScript
{
public: 
	go_dedication_of_honor() : GameObjectScript("go_dedication_of_honor") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_SEE_FALL_OF_THE_LICH_KING, GOSSIP_SENDER_MAIN, FALL_OF_THE_LICH_KING);
		player->SEND_GOSSIP_MENU(GOSSIP_MESSAGE_MEMORIAL, go->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
			case FALL_OF_THE_LICH_KING:
			{
				player->SendMovieStart(MOVIE_FALL_OF_THE_LICH_KING);
				player->CLOSE_GOSSIP_MENU();
				break;
			}
		}
		return true;
	}
};

enum Manabonk
{
	SPELL_TELEPORT_VISUAL = 51347,
	SPELL_IMPROVED_BLINK  = 61995,
	SPELL_MANABONKED      = 61839,
	MAIL_MINIGOB_ID       = 264,

	EVENT_BLINK          = 1,
	EVENT_LAUGH          = 2,
	EVENT_INVISIBILITY   = 3,
	EVENT_DESPAWN        = 4,

	MAIL_DELIVER_DELAY_MIN  = 5*MINUTE,
	MAIL_DELIVER_DELAY_MAX  = 15*MINUTE
};

#define SAY_FRIENDS "Your friends."

class npc_minigob_manabonk : public CreatureScript
{
public:
    npc_minigob_manabonk() : CreatureScript("npc_minigob_manabonk") { }

    struct npc_minigob_manabonkAI  : public QuantumBasicAI
    {
        npc_minigob_manabonkAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		bool SheepPlayer;

		void Reset()
		{
			events.Reset();

			SheepPlayer = false;
		}

		void MoveInLineOfSight(Unit* who)
        {
			Player* player = who->ToPlayer();

            if (SheepPlayer || !player || !player->IsWithinDistInMap(me, 15.0f))
				return;

			if (SheepPlayer == false && player && player->IsWithinDistInMap(me, 15.0f))
			{
				me->SetFacingToObject(player);
				DoCast(player, SPELL_MANABONKED, true);
				SendMailToPlayer(player);
				events.ScheduleEvent(EVENT_BLINK, 4*IN_MILLISECONDS);
				SheepPlayer = true;
			}
		}

		void SendMailToPlayer(Player* player)
		{
			SQLTransaction trans = CharacterDatabase.BeginTransaction();
			int16 deliverDelay = irand(MAIL_DELIVER_DELAY_MIN, MAIL_DELIVER_DELAY_MAX);
			MailDraft(MAIL_MINIGOB_ID, true).SendMailTo(trans, MailReceiver(player), MailSender(MAIL_CREATURE, me->GetEntry()), MAIL_CHECK_MASK_NONE, deliverDelay);
			CharacterDatabase.CommitTransaction(trans);
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_BLINK:
						DoCast(me, SPELL_IMPROVED_BLINK, true);
						events.ScheduleEvent(EVENT_LAUGH, 3*IN_MILLISECONDS);
						break;
					case EVENT_LAUGH:
						me->MonsterSay(SAY_FRIENDS, LANG_UNIVERSAL, 0);
						me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
						events.ScheduleEvent(EVENT_INVISIBILITY, 3*IN_MILLISECONDS);
						break;
					case EVENT_INVISIBILITY:
						DoCast(me, SPELL_TELEPORT_VISUAL, true);
						events.ScheduleEvent(EVENT_DESPAWN, 0.5*IN_MILLISECONDS);
						break;
					case EVENT_DESPAWN:
						me->DisappearAndDie();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_minigob_manabonkAI(creature);
    }
};

enum DalaranBook
{
	SPELL_DND_DWARF_BOOK_VISUAL    = 61378,
	SPELL_DND_HUMAN_BOOK_VISUAL_01 = 61403,
	SPELL_DND_HUMAN_BOOK_VISUAL_03 = 61404,
	SPELL_DND_HUMAN_BOOK_VISUAL_04 = 61405,
	SPELL_DND_TROLL_BOOK_VISUAL_02 = 61406,
};

class npc_dnd_cosmetic_book : public CreatureScript
{
public:
    npc_dnd_cosmetic_book() : CreatureScript("npc_dnd_cosmetic_book") {}

    struct npc_dnd_cosmetic_bookAI : public QuantumBasicAI
    {
        npc_dnd_cosmetic_bookAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, RAND(SPELL_DND_DWARF_BOOK_VISUAL, SPELL_DND_HUMAN_BOOK_VISUAL_01, SPELL_DND_HUMAN_BOOK_VISUAL_03, SPELL_DND_HUMAN_BOOK_VISUAL_04, SPELL_DND_TROLL_BOOK_VISUAL_02), true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dnd_cosmetic_bookAI(creature);
    }
};

void AddSC_dalaran()
{
    new npc_mageguard_dalaran();
	new npc_archmage_vargoth();
	new npc_arcanist_tybalin();
	new npc_rhonin();
	new go_dedication_of_honor();
	new npc_minigob_manabonk();
	new npc_dnd_cosmetic_book();
}