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
#include "QuantumEscortAI.h"
#include "QuantumSystemText.h"

#define GOSSIP_RALIQ "You owe Sim'salabim money. Hand them over or die!"

enum Raliq
{
    SPELL_UPPERCUT     = 10966,

    QUEST_CRACK_SKULLS = 10009,
    FACTION_HOSTILE_RD = 45,
};

class npc_raliq_the_drunk : public CreatureScript
{
public:
    npc_raliq_the_drunk() : CreatureScript("npc_raliq_the_drunk") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_CRACK_SKULLS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_RALIQ, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(9440, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(FACTION_HOSTILE_RD);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_raliq_the_drunkAI : public QuantumBasicAI
    {
        npc_raliq_the_drunkAI(Creature* creature) : QuantumBasicAI(creature)
        {
			NormFaction = creature->GetFaction();
        }

        uint32 NormFaction;
        uint32 UppercutTimer;

        void Reset()
        {
            UppercutTimer = 5000;
            me->RestoreFaction();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (UppercutTimer <= diff)
            {
                DoCastVictim(SPELL_UPPERCUT);
                UppercutTimer = 15000;
            }
			else UppercutTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raliq_the_drunkAI(creature);
    }
};

enum Salsalabim
{
	FACTION_HOSTILE_SA  = 90,
	FACTION_FRIENDLY_SA = 35,
	QUEST_10004         = 10004,
	SPELL_MAGNETIC_PULL = 31705,
};

class npc_salsalabim : public CreatureScript
{
public:
    npc_salsalabim() : CreatureScript("npc_salsalabim") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_10004) == QUEST_STATUS_INCOMPLETE)
        {
            creature->SetCurrentFaction(FACTION_HOSTILE_SA);
            creature->AI()->AttackStart(player);
        }
        else
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    struct npc_salsalabimAI : public QuantumBasicAI
    {
        npc_salsalabimAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MagneticPullTimer;

        void Reset()
        {
            MagneticPullTimer = 15000;
            me->RestoreFaction();
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (done_by->GetTypeId() == TYPE_ID_PLAYER)
				if (me->HealthBelowPctDamaged(20, damage))
            {
                CAST_PLR(done_by)->GroupEventHappens(QUEST_10004, me);
                damage = 0;
                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MagneticPullTimer <= diff)
            {
                DoCastVictim(SPELL_MAGNETIC_PULL);
                MagneticPullTimer = 15000;
            }
			else MagneticPullTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_salsalabimAI(creature);
    }
};

class npc_shattrathflaskvendors : public CreatureScript
{
public:
    npc_shattrathflaskvendors() : CreatureScript("npc_shattrathflaskvendors") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->GetEntry() == 23484)
        {
            // Aldor vendor
            if (creature->IsVendor() && (player->GetReputationRank(932) == REP_EXALTED) && (player->GetReputationRank(935) == REP_EXALTED) && (player->GetReputationRank(942) == REP_EXALTED))
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
                player->SEND_GOSSIP_MENU(11085, creature->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(11083, creature->GetGUID());
        }

        if (creature->GetEntry() == 23483)
        {
            // Scryers vendor
            if (creature->IsVendor() && (player->GetReputationRank(934) == REP_EXALTED) && (player->GetReputationRank(935) == REP_EXALTED) && (player->GetReputationRank(942) == REP_EXALTED))
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
                player->SEND_GOSSIP_MENU(11085, creature->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(11084, creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_TRADE)
			player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
	}
};

#define GOSSIP_HZ "Take me to the Caverns of Time."

class npc_zephyr : public CreatureScript
{
public:
    npc_zephyr() : CreatureScript("npc_zephyr") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetReputationRank(989) >= REP_REVERED)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HZ, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
            player->CastSpell(player, 37778, false);

        return true;
    }
};

enum Says
{
	SAY1          = -1000234,
	SAY_WHISP1    = -1000235,
	SAY_WHISP2    = -1000236,
	SAY_WHISP3    = -1000237,
	SAY_WHISP4    = -1000238,
	SAY_WHISP5    = -1000239,
	SAY_WHISP6    = -1000240,
	SAY_WHISP7    = -1000241,
	SAY_WHISP8    = -1000242,
	SAY_WHISP9    = -1000243,
	SAY_WHISP10   = -1000244,
	SAY_WHISP11   = -1000245,
	SAY_WHISP12   = -1000246,
	SAY_WHISP13   = -1000247,
	SAY_WHISP14   = -1000248,
	SAY_WHISP15   = -1000249,
	SAY_WHISP16   = -1000250,
	SAY_WHISP17   = -1000251,
	SAY_WHISP18   = -1000252,
	SAY_WHISP19   = -1000253,
	SAY_WHISP20   = -1000254,
	SAY_WHISP21   = -1000255,
};

class npc_kservant : public CreatureScript
{
public:
    npc_kservant() : CreatureScript("npc_kservant") { }

    struct npc_kservantAI : public npc_escortAI
    {
    public:
        npc_kservantAI(Creature* creature) : npc_escortAI(creature) {}

		void Reset() {}

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
                case 0:
					DoSendQuantumText(SAY1, me, player);
					break;
                case 4:
					DoSendQuantumText(SAY_WHISP1, me, player);
					break;
                case 6:
					DoSendQuantumText(SAY_WHISP2, me, player);
					break;
                case 7:
					DoSendQuantumText(SAY_WHISP3, me, player);
					break;
                case 8:
					DoSendQuantumText(SAY_WHISP4, me, player);
					break;
                case 17:
					DoSendQuantumText(SAY_WHISP5, me, player);
					break;
                case 18:
					DoSendQuantumText(SAY_WHISP6, me, player);
					break;
                case 19:
					DoSendQuantumText(SAY_WHISP7, me, player);
					break;
                case 33:
					DoSendQuantumText(SAY_WHISP8, me, player);
					break;
                case 34:
					DoSendQuantumText(SAY_WHISP9, me, player);
					break;
                case 35:
					DoSendQuantumText(SAY_WHISP10, me, player);
					break;
                case 36:
					DoSendQuantumText(SAY_WHISP11, me, player);
					break;
                case 43:
					DoSendQuantumText(SAY_WHISP12, me, player);
					break;
                case 44:
					DoSendQuantumText(SAY_WHISP13, me, player);
					break;
                case 49:
					DoSendQuantumText(SAY_WHISP14, me, player);
					break;
                case 50:
					DoSendQuantumText(SAY_WHISP15, me, player);
					break;
                case 51:
					DoSendQuantumText(SAY_WHISP16, me, player);
					break;
                case 52:
					DoSendQuantumText(SAY_WHISP17, me, player);
					break;
                case 53:
					DoSendQuantumText(SAY_WHISP18, me, player);
					break;
                case 54:
					DoSendQuantumText(SAY_WHISP19, me, player);
					break;
                case 55:
					DoSendQuantumText(SAY_WHISP20, me, player);
					break;
                case 56:
					DoSendQuantumText(SAY_WHISP21, me, player);
                    if (player)
						player->GroupEventHappens(10211, me);
					break;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (CAST_PLR(who)->GetQuestStatus(10211) == QUEST_STATUS_INCOMPLETE)
                {
					float Radius = 10.0f;

					if (me->IsWithinDistInMap(who, Radius))
						Start(false, false, who->GetGUID());
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kservantAI(creature);
    }
};

#define GOSSIP_BOOK "Ezekiel said that you might have a certain book..."

enum DirtyLarry
{
	SAY_1          = -1000274,
	SAY_2          = -1000275,
	SAY_3          = -1000276,
	SAY_4          = -1000277,
	SAY_5          = -1000278,
	SAY_GIVEUP     = -1000279,
	QUEST_WBI      = 10231,
	NPC_CREEPJACK  = 19726,
	NPC_MALONE     = 19725,
};

class npc_dirty_larry : public CreatureScript
{
public:
    npc_dirty_larry() : CreatureScript("npc_dirty_larry") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_WBI) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BOOK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            CAST_AI(npc_dirty_larry::npc_dirty_larryAI, creature->AI())->Event = true;
            CAST_AI(npc_dirty_larry::npc_dirty_larryAI, creature->AI())->PlayerGUID = player->GetGUID();
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    struct npc_dirty_larryAI : public QuantumBasicAI
    {
        npc_dirty_larryAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Event;
        bool Attack;
        bool Done;

        uint64 PlayerGUID;

        uint32 SayTimer;
        uint32 Step;

        void Reset()
        {
            Event = false;
            Attack = false;
            Done = false;

            PlayerGUID = 0;
            SayTimer = 0;
            Step = 0;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetCurrentFaction(1194);
            Unit* Creepjack = me->FindCreatureWithDistance(NPC_CREEPJACK, 20.0f);
            if (Creepjack)
            {
                CAST_CRE(Creepjack)->AI()->EnterEvadeMode();
                Creepjack->SetCurrentFaction(1194);
                Creepjack->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            Unit* Malone = me->FindCreatureWithDistance(NPC_MALONE, 20.0f);
            if (Malone)
            {
                CAST_CRE(Malone)->AI()->EnterEvadeMode();
                Malone->SetCurrentFaction(1194);
                Malone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
        }

        uint32 NextStep(uint32 Step)
        {
            Player* player = Unit::GetPlayer(*me, PlayerGUID);

            switch (Step)
            {
                case 0:
                {
                    me->SetInFront(player);
                    Unit* Creepjack = me->FindCreatureWithDistance(NPC_CREEPJACK, 20.0f);
                    if (Creepjack)
                        Creepjack->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                    Unit* Malone = me->FindCreatureWithDistance(NPC_MALONE, 20.0f);
                    if (Malone)
                        Malone->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    return 2000;
                }
                case 1:
                    DoSendQuantumText(SAY_1, me, player);
                    return 3000;
                case 2:
                    DoSendQuantumText(SAY_2, me, player);
                    return 5000;
                case 3:
                    DoSendQuantumText(SAY_3, me, player);
                    return 2000;
                case 4:
                    DoSendQuantumText(SAY_4, me, player);
                    return 2000;
                case 5:
                    DoSendQuantumText(SAY_5, me, player);
                    return 2000;
                case 6:
                    Attack = true;
                    return 2000;
                default:
                    return 0;
            }
        }

        void EnterToBattle(Unit* /*who*/) {}

        void DamageTaken(Unit* /*doneBy*/, uint32 &damage)
        {
            if (damage >= me->GetHealth() && !Done)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->RemoveAllAuras();
                damage = me->GetHealth() - 1;

                Unit* Creepjack = me->FindCreatureWithDistance(NPC_CREEPJACK, 20.0f);
                if (Creepjack)
                {
                    CAST_CRE(Creepjack)->AI()->EnterEvadeMode();
                    Creepjack->SetCurrentFaction(1194);
                    Creepjack->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                Unit* Malone = me->FindCreatureWithDistance(NPC_MALONE, 20.0f);
                if (Malone)
                {
                    CAST_CRE(Malone)->AI()->EnterEvadeMode();
                    Malone->SetCurrentFaction(1194);
                    Malone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                me->SetCurrentFaction(1194);
                Done = true;
                DoSendQuantumText(SAY_GIVEUP, me, NULL);
                Player* player = Unit::GetPlayer(*me, PlayerGUID);
                if (player)
                    CAST_PLR(player)->GroupEventHappens(QUEST_WBI, me);

                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (SayTimer <= diff)
            {
                if (Event)
                    SayTimer = NextStep(++Step);
            }
			else SayTimer -= diff;

            if (Attack)
            {
                Player* player = Unit::GetPlayer(*me, PlayerGUID);

                me->SetCurrentFaction(14);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (!player || !me->CanCreatureAttack(player))
                {
                    EnterEvadeMode();
                    return;
                }

                Creature* Creepjack = me->FindCreatureWithDistance(NPC_CREEPJACK, 20);
                if (Creepjack)
                {
                    Creepjack->AI()->AttackStart(player);
                    Creepjack->SetCurrentFaction(14);
                    Creepjack->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                Creature* Malone = me->FindCreatureWithDistance(NPC_MALONE, 20);
                if (Malone)
                {
                    Malone->AI()->AttackStart(player);
                    Malone->SetCurrentFaction(14);
                    Malone->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                AttackStart(player);
                Attack = false;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dirty_larryAI(creature);
    }
};

#define ISANAH_GOSSIP_1 "Who are the Sha'tar?"
#define ISANAH_GOSSIP_2 "Isn't Shattrath a draenei city? Why do you allow others here?"

class npc_ishanah : public CreatureScript
{
public:
    npc_ishanah() : CreatureScript("npc_ishanah") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ISANAH_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ISANAH_GOSSIP_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
            player->SEND_GOSSIP_MENU(9458, creature->GetGUID());
        else if (action == GOSSIP_ACTION_INFO_DEF+2)
            player->SEND_GOSSIP_MENU(9459, creature->GetGUID());

        return true;
    }
};

#define KHADGAR_GOSSIP_1 "I've heard your name spoken only in whispers, mage. Who are you?"
#define KHADGAR_GOSSIP_2 "Go on, please."
#define KHADGAR_GOSSIP_3 "I see." //6th too this
#define KHADGAR_GOSSIP_4 "What did you do then?"
#define KHADGAR_GOSSIP_5 "What happened next?"
#define KHADGAR_GOSSIP_7 "There was something else I wanted to ask you."

class npc_khadgar : public CreatureScript
{
public:
    npc_khadgar() : CreatureScript("npc_khadgar") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (!player->HasQuest(10211))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(9243, creature->GetGUID());
			return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(9876, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(9877, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->SEND_GOSSIP_MENU(9878, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            player->SEND_GOSSIP_MENU(9879, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            player->SEND_GOSSIP_MENU(9880, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
            player->SEND_GOSSIP_MENU(9881, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, KHADGAR_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(9243, creature->GetGUID());
            break;
        }
        return true;
    }
};

enum ShattrathBanish
{
	SPELL_BANISH_1        = 36642,
	SPELL_BANISH_2        = 36671,
	SPELL_DEBUFF_EXILE    = 39533,
	SPELL_DEBUFF_SICKNESS = 15007,
	SPELL_TELEPORT        = 36643,
};

class npc_shattrath_banish : public CreatureScript
{
public:
    npc_shattrath_banish() : CreatureScript("npc_shattrath_banish") { }

    struct npc_shattrath_banishAI : public QuantumBasicAI
    {
        npc_shattrath_banishAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 BanishTimer;
        uint32 BanishteleportTimer;

        void Reset()
        {
            BanishTimer = 2000;
            BanishteleportTimer = 9000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BanishTimer <= diff)
            {
                if (me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER)
					DoCastVictim(RAND(SPELL_BANISH_1, SPELL_BANISH_2));

				BanishteleportTimer = 8000;
				BanishTimer = 12000;
            }
			else BanishTimer -= diff;

            if (BanishteleportTimer <= diff)
            {
                if (me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER)
                {
                    Unit* victim = me->GetVictim();
                    if (!victim->HasAura(SPELL_BANISH_1,1) && !victim->HasAura(SPELL_BANISH_2, 1))
                    {
                        victim->CastSpell(victim, SPELL_DEBUFF_EXILE, true);
                        victim->CastSpell(victim, SPELL_DEBUFF_SICKNESS, true);
                        victim->CastSpell(victim, SPELL_TELEPORT, true);
                    
                        BanishteleportTimer = 12000;
                        EnterEvadeMode();
                    }
                }
            }
			else BanishteleportTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattrath_banishAI(creature);
    }
};

void AddSC_shattrath_city()
{
    new npc_raliq_the_drunk();
    new npc_salsalabim();
    new npc_shattrathflaskvendors();
    new npc_zephyr();
    new npc_kservant();
    new npc_dirty_larry();
    new npc_ishanah();
    new npc_khadgar();
	new npc_shattrath_banish();
}