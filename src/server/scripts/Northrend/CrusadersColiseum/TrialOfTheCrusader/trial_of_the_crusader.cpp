/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumGossip.h"
#include "SpellScript.h"
#include "trial_of_the_crusader.h"

enum Yells
{
    SAY_STAGE_0_01            = -1649070,
    SAY_STAGE_0_02            = -1649071,
    SAY_STAGE_0_03a           = -1649072,
    SAY_STAGE_0_03h           = -1649073,
    SAY_STAGE_0_04            = -1649074,
    SAY_STAGE_0_05            = -1649075,
    SAY_STAGE_0_06            = -1649076,
    SAY_STAGE_0_WIPE          = -1649077,
    SAY_STAGE_1_01            = -1649080,
    SAY_STAGE_1_02            = -1649081,
    SAY_STAGE_1_03            = -1649082,
    SAY_STAGE_1_04            = -1649083,
    SAY_STAGE_1_05            = -1649030, //INTRO Jaraxxus
    SAY_STAGE_1_06            = -1649084,
    SAY_STAGE_1_07            = -1649086,
    SAY_STAGE_1_08            = -1649087,
    SAY_STAGE_1_09            = -1649088,
    SAY_STAGE_1_10            = -1649089,
    SAY_STAGE_1_11            = -1649090,
    SAY_STAGE_2_01            = -1649091,
    SAY_STAGE_2_02a           = -1649092,
    SAY_STAGE_2_02h           = -1649093,
    SAY_STAGE_2_03            = -1649094,
    SAY_STAGE_2_04a           = -1649095,
    SAY_STAGE_2_04h           = -1649096,
    SAY_STAGE_2_05a           = -1649097,
    SAY_STAGE_2_05h           = -1649098,
    SAY_STAGE_2_06            = -1649099,
    SAY_STAGE_3_01            = -1649100,
    SAY_STAGE_3_02            = -1649101,
    SAY_STAGE_3_03a           = -1649102,
    SAY_STAGE_3_03h           = -1649103,
    SAY_STAGE_4_01            = -1649104,
    SAY_STAGE_4_02            = -1649105,
    SAY_STAGE_4_03            = -1649106,
    SAY_STAGE_4_04            = -1649107,
    SAY_STAGE_4_05            = -1649108,
    SAY_STAGE_4_06            = -1649109,
    SAY_STAGE_4_07            = -1649110,
};

struct _Messages
{
    AnnouncerMessages msgnum;
    uint32 id;
    bool state;
    uint32 encounter;
};

static _Messages _GossipMessage[] =
{
    {MSG_BEASTS, GOSSIP_ACTION_INFO_DEF+1, false, TYPE_BEASTS},
    {MSG_JARAXXUS, GOSSIP_ACTION_INFO_DEF+2, false, TYPE_JARAXXUS},
    {MSG_CRUSADERS, GOSSIP_ACTION_INFO_DEF+3, false, TYPE_CRUSADERS},
    {MSG_VALKIRIES, GOSSIP_ACTION_INFO_DEF+4, false, TYPE_VALKIRIES},
    {MSG_LICH_KING, GOSSIP_ACTION_INFO_DEF+5, false, TYPE_ANUBARAK},
    {MSG_ANUBARAK, GOSSIP_ACTION_INFO_DEF+6, true, TYPE_ANUBARAK},
};

enum eMessages
{
    NUM_MESSAGES = 6,
};

class npc_announcer_toc10 : public CreatureScript
{
    public:
        npc_announcer_toc10() : CreatureScript("npc_announcer_toc10") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            InstanceScript* instance = creature->GetInstanceScript();

            if (!instance)
                return true;

            char const* _message = "We are ready!";

            if (player->IsInCombatActive() || instance->IsEncounterInProgress() || instance->GetData(TYPE_EVENT))
                return true;

            uint8 i = 0;
            for (; i < NUM_MESSAGES; ++i)
            {
                if ((!_GossipMessage[i].state && instance->GetData(_GossipMessage[i].encounter) != DONE)
                    || (_GossipMessage[i].state && instance->GetData(_GossipMessage[i].encounter) == DONE))
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN, _GossipMessage[i].id);
                    break;
                }
            }

            player->SEND_GOSSIP_MENU(_GossipMessage[i].msgnum, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            InstanceScript* instance = creature->GetInstanceScript();

            if (!instance)
                return true;

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    if (instance->GetData(TYPE_BEASTS) != DONE)
                    {
                        instance->SetData(TYPE_EVENT, 110);
                        instance->SetData(TYPE_NORTHREND_BEASTS, NOT_STARTED);
                        instance->SetData(TYPE_BEASTS, NOT_STARTED);
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    if (Creature* jaraxxus = Unit::GetCreature(*player, instance->GetData64(NPC_LORD_JARAXXUS)))
                    {
                        jaraxxus->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                        jaraxxus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        jaraxxus->SetReactState(REACT_AGGRESSIVE);
                        jaraxxus->SetInCombatWithZone();
                    }
                    else if (instance->GetData(TYPE_JARAXXUS) != DONE)
                    {
                        instance->SetData(TYPE_EVENT, 1010);
                        instance->SetData(TYPE_JARAXXUS, NOT_STARTED);
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    if (instance->GetData(TYPE_CRUSADERS) != DONE)
                    {
                        if (player->GetTeam() == ALLIANCE)
                            instance->SetData(TYPE_EVENT, 3000);
                        else
                            instance->SetData(TYPE_EVENT, 3001);
                        instance->SetData(TYPE_CRUSADERS, NOT_STARTED);
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF+4:
                    if (instance->GetData(TYPE_VALKIRIES) != DONE)
                    {
                        instance->SetData(TYPE_EVENT, 4000);
                        instance->SetData(TYPE_VALKIRIES, NOT_STARTED);
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF+5:
                {
                    if (instance->GetData(TYPE_LICH_KING) != DONE && !player->IsGameMaster())
                        return true;

                    if (GameObject* Floor = GameObject::GetGameObject(*player, instance->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
                        Floor->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);

					creature->CastSpell(creature, SPELL_CORPSE_TELEPORT, false);
					creature->CastSpell(creature, SPELL_DESTROY_FLOOR_KNOCK_UP, false);

                    Creature* AnubArak = Unit::GetCreature(*creature, instance->GetData64(NPC_ANUBARAK));
                    if (!AnubArak || !AnubArak->IsAlive())
                        AnubArak = creature->SummonCreature(NPC_ANUBARAK, AnubarakLoc[0].GetPositionX(), AnubarakLoc[0].GetPositionY(), AnubarakLoc[0].GetPositionZ(), 3, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);

					if (creature->IsVisible())
						creature->SetVisible(false);

					instance->SetData(TYPE_ANUBARAK, NOT_STARTED);
                    break;
                }
            }
            creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            return true;
        }

        struct npc_announcer_toc10AI : public QuantumBasicAI
        {
            npc_announcer_toc10AI(Creature* creature) : QuantumBasicAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset()
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if (Creature* pAlly = GetClosestCreatureWithEntry(me, NPC_THRALL, 300.0f))
                    pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if (Creature* pAlly = GetClosestCreatureWithEntry(me, NPC_JAINA_PROUDMOORE, 300.0f))
                    pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void AttackStart(Unit* /*who*/) {}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_announcer_toc10AI(creature);
        }
};

class boss_lich_king_toc : public CreatureScript
{
    public:
        boss_lich_king_toc() : CreatureScript("boss_lich_king_toc") { }

        struct boss_lich_king_tocAI : public QuantumBasicAI
        {
            boss_lich_king_tocAI(Creature* creature) : QuantumBasicAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 UpdateTimer;

            void Reset()
            {
                UpdateTimer = 0;
                me->SetReactState(REACT_PASSIVE);
                if (Creature* summoned = me->SummonCreature(NPC_PURPLE_ARGENT_TRIGGER, ToCCommonLoc[2].GetPositionX(), ToCCommonLoc[2].GetPositionY(), ToCCommonLoc[2].GetPositionZ(), 5, TEMPSUMMON_TIMED_DESPAWN, 60000))
                {
                    summoned->CastSpell(summoned, SPELL_ARTHAS_PORTAL, false);
                    summoned->SetDisplayId(MODEL_ID_INVISIBLE);
                }
                if (instance)
					instance->SetData(TYPE_LICH_KING, IN_PROGRESS);
                me->SetWalk(true);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;
                switch (id)
                {
                    case 0:
                        instance->SetData(TYPE_EVENT, 5030);
                        break;
                    case 1:
                        instance->SetData(TYPE_EVENT, 5050);
                        break;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
					return;

                if (instance->GetData(TYPE_EVENT_NPC) != NPC_LICH_KING_1)
					return;

                UpdateTimer = instance->GetData(TYPE_EVENT_TIMER);
                if (UpdateTimer <= diff)
                {
                    switch (instance->GetData(TYPE_EVENT))
                    {
                        case 5010:
                            DoSendQuantumText(SAY_STAGE_4_02, me);
                            UpdateTimer = 5000;
                            me->GetMotionMaster()->MovePoint(0, LichKingLoc[0]);
                            instance->SetData(TYPE_EVENT, 5020);
                            break;
                        case 5030:
                            DoSendQuantumText(SAY_STAGE_4_04, me);
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
                            UpdateTimer = 12000;
                            instance->SetData(TYPE_EVENT, 5040);
                            break;
                        case 5040:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_NONE);
                            me->GetMotionMaster()->MovePoint(1, LichKingLoc[1]);
                            UpdateTimer = 1000;
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 5050:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            UpdateTimer = 4000;
                            instance->SetData(TYPE_EVENT, 5060);
                            break;
                        case 5060:
                            DoSendQuantumText(SAY_STAGE_4_05, me);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                            UpdateTimer = 3500;
                            instance->SetData(TYPE_EVENT, 5070);
                            break;
                        case 5070:
                            me->CastSpell(me, SPELL_ARTHAS_FROST_NOVA, false);
                            UpdateTimer = 1500;
                            instance->SetData(TYPE_EVENT, 5080);
                            break;
                        case 5080:
							// Despawn Valryries after destroed platform and Barrent
							if (Creature* darkbane = me->FindCreatureWithDistance(NPC_EYDIS_DARKBANE, 250.0f, true))
								darkbane->DespawnAfterAction();

							if (Creature* lightbane = me->FindCreatureWithDistance(NPC_FJOLA_LIGHTBANE, 250.0f, true))
								lightbane->DespawnAfterAction();

							if (Creature* barrent = me->FindCreatureWithDistance(NPC_BARRENT, 250.0f, true))
								barrent->DespawnAfterAction();

                            if (GameObject* floor = instance->instance->GetGameObject(instance->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
							{
								floor->SetDisplayId(DISPLAY_ID_DESTROYED_FLOOR);
								floor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NO_DESPAWN);
								floor->SetGoState(GO_STATE_ACTIVE);
							}
                            
							me->CastSpell(me, SPELL_CORPSE_TELEPORT, true);
							me->CastSpell(me, SPELL_DESTROY_FLOOR_KNOCK_UP, true);

                            if (instance)
                            {
                                instance->SetData(TYPE_LICH_KING, DONE);
                                Creature* creature = Unit::GetCreature(*me, instance->GetData64(NPC_ANUBARAK));

                                if (!creature || !creature->IsAlive())
                                    creature = me->SummonCreature(NPC_ANUBARAK, AnubarakLoc[0].GetPositionX(), AnubarakLoc[0].GetPositionY(), AnubarakLoc[0].GetPositionZ(), 3, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);

                                instance->SetData(TYPE_EVENT, 0);
                            }
                            me->DespawnAfterAction();
                            UpdateTimer = 20000;
                            break;
                    }
                }
				else UpdateTimer -= diff;
                instance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_lich_king_tocAI(creature);
        }
};

class npc_fizzlebang_toc : public CreatureScript
{
public:
	npc_fizzlebang_toc() : CreatureScript("npc_fizzlebang_toc") { }

        struct npc_fizzlebang_tocAI : public QuantumBasicAI
        {
            npc_fizzlebang_tocAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
				instance = creature->GetInstanceScript();

				me->SetSpeed(MOVE_WALK, 1.2f, true);
				me->SetSpeed(MOVE_RUN, 1.2f, true);
            }

            InstanceScript* instance;
            SummonList Summons;

            uint32 UpdateTimer;
            uint64 PortalGUID;
            uint64 TriggerGUID;

			void Reset()
            {
                PortalGUID = 0;
                me->GetMotionMaster()->MovePoint(1, ToCCommonLoc[10].GetPositionX(), ToCCommonLoc[10].GetPositionY()-60, ToCCommonLoc[10].GetPositionZ());
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_STAGE_1_06, me);

				if (Creature* tirion = Unit::GetCreature(*me, instance->GetData64(NPC_TIRION_FORDRING_1)))
					DoSendQuantumText(SAY_STAGE_1_07, tirion);

                if (Creature* jaraxxus = Unit::GetCreature(*me, instance->GetData64(NPC_LORD_JARAXXUS)))
                {
					jaraxxus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    jaraxxus->SetReactState(REACT_AGGRESSIVE);
                }
            }

            void MovementInform(uint32 Type, uint32 id)
            {
                if (Type != POINT_MOTION_TYPE)
					return;

                switch (id)
                {
                    case 1:
                        if (instance)
                        {
                            instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                            instance->SetData(TYPE_EVENT, 1120);
                            instance->SetData(TYPE_EVENT_TIMER, 1000);
                        }
                        break;
                }
            }

            void JustSummoned(Creature* summoned)
            {
                Summons.Summon(summoned);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
					return;

                if (instance->GetData(TYPE_EVENT_NPC) != NPC_WILFRED_FIZZLEBANG)
					return;

                UpdateTimer = instance->GetData(TYPE_EVENT_TIMER);
                if (UpdateTimer <= diff)
                {
                    switch (instance->GetData(TYPE_EVENT))
                    {
                        case 1110:
                            instance->SetData(TYPE_EVENT, 1120);
                            UpdateTimer = 4000;
                            break;
                        case 1120:
                            DoSendQuantumText(SAY_STAGE_1_02, me);
                            instance->SetData(TYPE_EVENT, 1130);
                            UpdateTimer = 12000;
                            break;
                        case 1130:
                            me->GetMotionMaster()->MovementExpired();
                            DoSendQuantumText(SAY_STAGE_1_03, me);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_SPELL_CAST_OMNI);
                            if (Unit* trigger = me->SummonCreature(NPC_PURPLE_ARGENT_TRIGGER, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 4.69494f, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                TriggerGUID = trigger->GetGUID();
                                trigger->SetObjectScale(2.0f);
                                trigger->SetDisplayId(22862);
                                trigger->CastSpell(trigger, SPELL_WILFRED_PORTAL, false);
                            }
                            instance->SetData(TYPE_EVENT, 1132);
                            UpdateTimer = 4000;
                            break;
                        case 1132:
                            me->GetMotionMaster()->MovementExpired();
                            instance->SetData(TYPE_EVENT, 1134);
                            UpdateTimer = 4000;
                            break;
                        case 1134:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_SPELL_CAST_OMNI);
                            if (Creature* wlfportal = me->SummonCreature(NPC_WILFRED_PORTAL, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 4.71239f, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                wlfportal->SetReactState(REACT_PASSIVE);
                                wlfportal->SetObjectScale(2.0f);
                                wlfportal->CastSpell(wlfportal, SPELL_WILFRED_PORTAL, false);
                                PortalGUID = wlfportal->GetGUID();
                            }
                            UpdateTimer = 4000;
                            instance->SetData(TYPE_EVENT, 1135);
                            break;
                        case 1135:
                            instance->SetData(TYPE_EVENT, 1140);
                            UpdateTimer = 3000;
                            break;
                        case 1140:
                            DoSendQuantumText(SAY_STAGE_1_04, me);
                            if (Creature* jaraxxus = me->SummonCreature(NPC_LORD_JARAXXUS, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 5.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME))
                            {
                                jaraxxus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                jaraxxus->SetReactState(REACT_PASSIVE);
                                jaraxxus->GetMotionMaster()->MovePoint(0, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY()-10, ToCCommonLoc[1].GetPositionZ());
                            }
                            instance->SetData(TYPE_EVENT, 1142);
                            UpdateTimer = 5000;
                            break;
                        case 1142:
                            if (Creature* jaraxxus = Unit::GetCreature(*me, instance->GetData64(NPC_LORD_JARAXXUS)))
                                jaraxxus->SetTarget(me->GetGUID());

                            if (Creature* trigger = Unit::GetCreature(*me, TriggerGUID))
                                trigger->DespawnAfterAction();

                            if (Creature* wlfportal = Unit::GetCreature(*me, PortalGUID))
                                wlfportal->DespawnAfterAction();

                            instance->SetData(TYPE_EVENT, 1144);
                            UpdateTimer = 8000;
                            break;
                        case 1144:
                            if (Creature* jaraxxus = Unit::GetCreature(*me, instance->GetData64(NPC_LORD_JARAXXUS)))
                                DoSendQuantumText(SAY_STAGE_1_05, jaraxxus);
                            instance->SetData(TYPE_EVENT, 1150);
                            UpdateTimer = 5000;
                            break;
                        case 1150:
                            if (Creature* jaraxxus = Unit::GetCreature(*me, instance->GetData64(NPC_LORD_JARAXXUS)))
                            {
                                jaraxxus->CastSpell(me, SPELL_LEGION_LIGHTNING, false);
                                me->SetInCombatWith(jaraxxus);
                                jaraxxus->AddThreat(me, 1000.0f);
                                jaraxxus->AI()->AttackStart(me);
                            }
                            instance->SetData(TYPE_EVENT, 1160);
                            UpdateTimer = 3000;
                            break;
                    }
                }
				else UpdateTimer -= diff;
                instance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_fizzlebang_tocAI(creature);
        }
};

class npc_tirion_toc : public CreatureScript
{
    public:
		npc_tirion_toc() : CreatureScript("npc_tirion_toc") { }

        struct npc_tirion_tocAI : public QuantumBasicAI
        {
            npc_tirion_tocAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 UpdateTimer;

            void Reset() {}

            void AttackStart(Unit* /*who*/) {}

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
					return;

                if (instance->GetData(TYPE_EVENT_NPC) != NPC_TIRION_FORDRING_1)
					return;

                UpdateTimer = instance->GetData(TYPE_EVENT_TIMER);
                if (UpdateTimer <= diff)
                {
					switch (instance->GetData(TYPE_EVENT))
                    {
                        case 110:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_TALK);
                            DoSendQuantumText(SAY_STAGE_0_01, me);
                            UpdateTimer = 22000;
                            instance->SetData(TYPE_EVENT, 120);
                            break;
                        case 140:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_TALK);
                            DoSendQuantumText(SAY_STAGE_0_02, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 150);
                            break;
                        case 150:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
                            if (instance->GetData(TYPE_BEASTS) != DONE)
                            {
                                instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));

                                if (Creature* gormok = me->SummonCreature(NPC_GORMOK_THE_IMPALER, ToCSpawnLoc[0].GetPositionX(), ToCSpawnLoc[0].GetPositionY(), ToCSpawnLoc[0].GetPositionZ(), 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS))
                                {
                                    gormok->GetMotionMaster()->MovePoint(0, ToCCommonLoc[5].GetPositionX(), ToCCommonLoc[5].GetPositionY(), ToCCommonLoc[5].GetPositionZ());
                                    gormok->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                    gormok->SetReactState(REACT_PASSIVE);
                                }
                            }
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 155);
                            break;
                        case 155:
                            instance->SetData(TYPE_BEASTS, IN_PROGRESS);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 160);
                            break;
                        case 200:
                            DoSendQuantumText(SAY_STAGE_0_04, me);
                            UpdateTimer = 8000;
                            instance->SetData(TYPE_EVENT, 205);
                            break;
                        case 205:
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 210);
                            break;
                        case 210:
                            if (instance->GetData(TYPE_BEASTS) != DONE)
							{
                                instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                                if (Creature* dreadscale = me->SummonCreature(NPC_DREADSCALE, ToCSpawnLoc[1].GetPositionX(), ToCSpawnLoc[1].GetPositionY(), ToCSpawnLoc[1].GetPositionZ(), 5, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    dreadscale->GetMotionMaster()->MovePoint(0, ToCCommonLoc[8].GetPositionX(), ToCCommonLoc[8].GetPositionY(), ToCCommonLoc[8].GetPositionZ());
                                    dreadscale->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                    dreadscale->SetReactState(REACT_PASSIVE);
                                }
                                if (Creature* acidmaw = me->SummonCreature(NPC_ACIDMAW, ToCCommonLoc[9].GetPositionX(), ToCCommonLoc[9].GetPositionY(), ToCCommonLoc[9].GetPositionZ(), 5, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    acidmaw->SetVisible(true);
                                    acidmaw->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                    acidmaw->SetReactState(REACT_PASSIVE);
                                }
                            }
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 220);
                            break;
                        case 220:
                            instance->SetData(TYPE_EVENT, 230);
                            break;
                        case 300:
                            DoSendQuantumText(SAY_STAGE_0_05, me);
                            UpdateTimer = 8000;
                            instance->SetData(TYPE_EVENT, 305);
                            break;
                        case 305:
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 310);
                            break;
                        case 310:
                            if (instance->GetData(TYPE_BEASTS) != DONE)
                            {
                                instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                                if (Creature* icehowl = me->SummonCreature(NPC_ICEHOWL, ToCSpawnLoc[0].GetPositionX(), ToCSpawnLoc[0].GetPositionY(), ToCSpawnLoc[0].GetPositionZ(), 5, TEMPSUMMON_DEAD_DESPAWN))
                                {
                                    icehowl->GetMotionMaster()->MovePoint(2, ToCCommonLoc[5].GetPositionX(), ToCCommonLoc[5].GetPositionY(), ToCCommonLoc[5].GetPositionZ());
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                    me->SetReactState(REACT_PASSIVE);
                                }
                            }
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 315);
                            break;
                        case 315:
                            instance->SetData(TYPE_EVENT, 320);
                            break;
                        case 400:
                            DoSendQuantumText(SAY_STAGE_0_06, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 666:
                            DoSendQuantumText(SAY_STAGE_0_WIPE, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 1010:
                            DoSendQuantumText(SAY_STAGE_1_01, me);
                            UpdateTimer = 7000;
                            instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                            me->SummonCreature(NPC_WILFRED_FIZZLEBANG, ToCSpawnLoc[0].GetPositionX(), ToCSpawnLoc[0].GetPositionY(), ToCSpawnLoc[0].GetPositionZ(), 2, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 2000:
                            DoSendQuantumText(SAY_STAGE_1_08, me);
                            UpdateTimer = 18000;
                            instance->SetData(TYPE_EVENT, 2010);
                            break;
                        case 2030:
                            DoSendQuantumText(SAY_STAGE_1_11, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 3000:
                            DoSendQuantumText(SAY_STAGE_2_01, me);
                            UpdateTimer = 12000;
                            instance->SetData(TYPE_EVENT, 3050);
                            break;
                        case 3001:
                            DoSendQuantumText(SAY_STAGE_2_01, me);
                            UpdateTimer = 12000;
                            instance->SetData(TYPE_EVENT, 3051);
                            break;
                        case 3060:
                            DoSendQuantumText(SAY_STAGE_2_03, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 3070);
                            break;
                        case 3061:
                            DoSendQuantumText(SAY_STAGE_2_03, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 3071);
                            break;
                        //Summoning crusaders
                        case 3091:
                            if (Creature* pChampionController = me->SummonCreature(NPC_CHAMPIONS_CONTROLLER, ToCCommonLoc[1]))
                                pChampionController->AI()->SetData(0, HORDE);
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 3092);
                            break;
                        //Summoning crusaders
                        case 3090:
                            if (Creature* pChampionController = me->SummonCreature(NPC_CHAMPIONS_CONTROLLER, ToCCommonLoc[1]))
                                pChampionController->AI()->SetData(0, ALLIANCE);
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 3092);
                            break;
                        case 3092:
                            if (Creature* pChampionController = Unit::GetCreature(*me, instance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
                                pChampionController->AI()->SetData(1, NOT_STARTED);
                            instance->SetData(TYPE_EVENT, 3095);
                            break;
                        //Crusaders battle end
                        case 3100:
                            DoSendQuantumText(SAY_STAGE_2_06, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 4000:
                            DoSendQuantumText(SAY_STAGE_3_01, me);
                            UpdateTimer = 13000;
                            instance->SetData(TYPE_EVENT, 4010);
                            break;
						case 4010:
                            DoSendQuantumText(SAY_STAGE_3_02, me);
                            if (Creature* lightbane = me->SummonCreature(NPC_FJOLA_LIGHTBANE, ToCSpawnLoc[1].GetPositionX(), ToCSpawnLoc[1].GetPositionY(), ToCSpawnLoc[1].GetPositionZ(), 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME))
                            {
                                //lightbane->SetVisible(false);
                                lightbane->SetReactState(REACT_PASSIVE);
                                lightbane->SummonCreature(NPC_LIGHT_ESSENCE, TwinValkyrsLoc[0].GetPositionX(), TwinValkyrsLoc[0].GetPositionY(), TwinValkyrsLoc[0].GetPositionZ());
                                lightbane->SummonCreature(NPC_LIGHT_ESSENCE, TwinValkyrsLoc[1].GetPositionX(), TwinValkyrsLoc[1].GetPositionY(), TwinValkyrsLoc[1].GetPositionZ());
								//---Light Bullets-----------------------------------------------------------------------------------------------------------------------------//
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[0].GetPositionX(), LightBulletsLoc[0].GetPositionY(),LightBulletsLoc[0].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[1].GetPositionX(), LightBulletsLoc[1].GetPositionY(),LightBulletsLoc[1].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[2].GetPositionX(), LightBulletsLoc[2].GetPositionY(),LightBulletsLoc[2].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[3].GetPositionX(), LightBulletsLoc[3].GetPositionY(),LightBulletsLoc[3].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[4].GetPositionX(), LightBulletsLoc[4].GetPositionY(),LightBulletsLoc[4].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[5].GetPositionX(), LightBulletsLoc[5].GetPositionY(),LightBulletsLoc[5].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[6].GetPositionX(), LightBulletsLoc[6].GetPositionY(),LightBulletsLoc[6].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[7].GetPositionX(), LightBulletsLoc[7].GetPositionY(),LightBulletsLoc[7].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[8].GetPositionX(), LightBulletsLoc[8].GetPositionY(),LightBulletsLoc[8].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[9].GetPositionX(), LightBulletsLoc[9].GetPositionY(),LightBulletsLoc[9].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[10].GetPositionX(), LightBulletsLoc[10].GetPositionY(),LightBulletsLoc[10].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[11].GetPositionX(), LightBulletsLoc[11].GetPositionY(),LightBulletsLoc[11].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[12].GetPositionX(), LightBulletsLoc[12].GetPositionY(),LightBulletsLoc[12].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[13].GetPositionX(), LightBulletsLoc[13].GetPositionY(),LightBulletsLoc[13].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[14].GetPositionX(), LightBulletsLoc[14].GetPositionY(),LightBulletsLoc[14].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[15].GetPositionX(), LightBulletsLoc[15].GetPositionY(),LightBulletsLoc[15].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[16].GetPositionX(), LightBulletsLoc[16].GetPositionY(),LightBulletsLoc[16].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[17].GetPositionX(), LightBulletsLoc[17].GetPositionY(),LightBulletsLoc[17].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[18].GetPositionX(), LightBulletsLoc[18].GetPositionY(),LightBulletsLoc[18].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[19].GetPositionX(), LightBulletsLoc[19].GetPositionY(),LightBulletsLoc[19].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[20].GetPositionX(), LightBulletsLoc[20].GetPositionY(),LightBulletsLoc[20].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[21].GetPositionX(), LightBulletsLoc[21].GetPositionY(),LightBulletsLoc[21].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[22].GetPositionX(), LightBulletsLoc[22].GetPositionY(),LightBulletsLoc[22].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[23].GetPositionX(), LightBulletsLoc[23].GetPositionY(),LightBulletsLoc[23].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[24].GetPositionX(), LightBulletsLoc[24].GetPositionY(),LightBulletsLoc[24].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[25].GetPositionX(), LightBulletsLoc[25].GetPositionY(),LightBulletsLoc[25].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[26].GetPositionX(), LightBulletsLoc[26].GetPositionY(),LightBulletsLoc[26].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[27].GetPositionX(), LightBulletsLoc[27].GetPositionY(),LightBulletsLoc[27].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[28].GetPositionX(), LightBulletsLoc[28].GetPositionY(),LightBulletsLoc[28].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[29].GetPositionX(), LightBulletsLoc[29].GetPositionY(),LightBulletsLoc[29].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[30].GetPositionX(), LightBulletsLoc[30].GetPositionY(),LightBulletsLoc[30].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[31].GetPositionX(), LightBulletsLoc[31].GetPositionY(),LightBulletsLoc[31].GetPositionZ());
								lightbane->SummonCreature(NPC_BULLET_STALKER_LIGHT, LightBulletsLoc[32].GetPositionX(), LightBulletsLoc[32].GetPositionY(),LightBulletsLoc[32].GetPositionZ());
                            }
                            if (Creature* darkbane = me->SummonCreature(NPC_EYDIS_DARKBANE, ToCSpawnLoc[2].GetPositionX(), ToCSpawnLoc[2].GetPositionY(), ToCSpawnLoc[2].GetPositionZ(), 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME))
                            {
                                //darkbane->SetVisible(false);
                                darkbane->SetReactState(REACT_PASSIVE);
                                darkbane->SummonCreature(NPC_DARK_ESSENCE, TwinValkyrsLoc[2].GetPositionX(), TwinValkyrsLoc[2].GetPositionY(), TwinValkyrsLoc[2].GetPositionZ());
                                darkbane->SummonCreature(NPC_DARK_ESSENCE, TwinValkyrsLoc[3].GetPositionX(), TwinValkyrsLoc[3].GetPositionY(), TwinValkyrsLoc[3].GetPositionZ());
								//---Dark Bullets------------------------------------------------------------------------------------------------------------------------------//
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[0].GetPositionX(), DarkBulletsLoc[0].GetPositionY(),DarkBulletsLoc[0].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[1].GetPositionX(), DarkBulletsLoc[1].GetPositionY(),DarkBulletsLoc[1].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[2].GetPositionX(), DarkBulletsLoc[2].GetPositionY(),DarkBulletsLoc[2].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[3].GetPositionX(), DarkBulletsLoc[3].GetPositionY(),DarkBulletsLoc[3].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[4].GetPositionX(), DarkBulletsLoc[4].GetPositionY(),DarkBulletsLoc[4].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[5].GetPositionX(), DarkBulletsLoc[5].GetPositionY(),DarkBulletsLoc[5].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[6].GetPositionX(), DarkBulletsLoc[6].GetPositionY(),DarkBulletsLoc[6].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[7].GetPositionX(), DarkBulletsLoc[7].GetPositionY(),DarkBulletsLoc[7].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[8].GetPositionX(), DarkBulletsLoc[8].GetPositionY(),DarkBulletsLoc[8].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[9].GetPositionX(), DarkBulletsLoc[9].GetPositionY(),DarkBulletsLoc[9].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[10].GetPositionX(), DarkBulletsLoc[10].GetPositionY(),DarkBulletsLoc[10].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[11].GetPositionX(), DarkBulletsLoc[11].GetPositionY(),DarkBulletsLoc[11].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[12].GetPositionX(), DarkBulletsLoc[12].GetPositionY(),DarkBulletsLoc[12].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[13].GetPositionX(), DarkBulletsLoc[13].GetPositionY(),DarkBulletsLoc[13].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[14].GetPositionX(), DarkBulletsLoc[14].GetPositionY(),DarkBulletsLoc[14].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[15].GetPositionX(), DarkBulletsLoc[15].GetPositionY(),DarkBulletsLoc[15].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[16].GetPositionX(), DarkBulletsLoc[16].GetPositionY(),DarkBulletsLoc[16].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[17].GetPositionX(), DarkBulletsLoc[17].GetPositionY(),DarkBulletsLoc[17].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[18].GetPositionX(), DarkBulletsLoc[18].GetPositionY(),DarkBulletsLoc[18].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[19].GetPositionX(), DarkBulletsLoc[19].GetPositionY(),DarkBulletsLoc[19].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[20].GetPositionX(), DarkBulletsLoc[20].GetPositionY(),DarkBulletsLoc[20].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[21].GetPositionX(), DarkBulletsLoc[21].GetPositionY(),DarkBulletsLoc[21].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[22].GetPositionX(), DarkBulletsLoc[22].GetPositionY(),DarkBulletsLoc[22].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[23].GetPositionX(), DarkBulletsLoc[23].GetPositionY(),DarkBulletsLoc[23].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[24].GetPositionX(), DarkBulletsLoc[24].GetPositionY(),DarkBulletsLoc[24].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[25].GetPositionX(), DarkBulletsLoc[25].GetPositionY(),DarkBulletsLoc[25].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[26].GetPositionX(), DarkBulletsLoc[26].GetPositionY(),DarkBulletsLoc[26].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[27].GetPositionX(), DarkBulletsLoc[27].GetPositionY(),DarkBulletsLoc[27].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[28].GetPositionX(), DarkBulletsLoc[28].GetPositionY(),DarkBulletsLoc[28].GetPositionZ());
								darkbane->SummonCreature(NPC_BULLET_STALKER_DARK, DarkBulletsLoc[29].GetPositionX(), DarkBulletsLoc[29].GetPositionY(),DarkBulletsLoc[29].GetPositionZ());
                            }
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 4015);
                            break;
						case 4015:
							instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
							if (Creature* lightbane = Unit::GetCreature(*me, instance->GetData64(NPC_FJOLA_LIGHTBANE)))
                                lightbane->GetMotionMaster()->MovePath(NPC_FJOLA_LIGHTBANE, false);

                            if (Creature* darkbane = Unit::GetCreature(*me, instance->GetData64(NPC_EYDIS_DARKBANE)))
								darkbane->GetMotionMaster()->MovePath(NPC_EYDIS_DARKBANE, false);

                            UpdateTimer = 10000;
                            instance->SetData(TYPE_EVENT, 4016);
                            break;
                        case 4016:
							instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                            instance->SetData(TYPE_EVENT, 4017);
                            break;
                        case 4040:
                            UpdateTimer = 25000; // 60000
                            instance->SetData(TYPE_EVENT, 5000);
                            break;
                        case 5000:
                            DoSendQuantumText(SAY_STAGE_4_01, me);
                            UpdateTimer = 10000;
                            instance->SetData(TYPE_EVENT, 5005);
                            break;
                        case 5005:
                            UpdateTimer = 8000;
                            instance->SetData(TYPE_EVENT, 5010);
							me->SummonCreature(NPC_LICH_KING_1, ToCCommonLoc[2].GetPositionX(), ToCCommonLoc[2].GetPositionY(), ToCCommonLoc[2].GetPositionZ(), 5);
                            break;
                        case 5020:
                            DoSendQuantumText(SAY_STAGE_4_03, me);
                            UpdateTimer = 5000; // 1000
                            instance->SetData(TYPE_EVENT, 0);
                            break;
                        case 6000:
                            me->NearTeleportTo(AnubarakLoc[0].GetPositionX(), AnubarakLoc[0].GetPositionY(), AnubarakLoc[0].GetPositionZ(), 4.0f);
                            UpdateTimer = 14000;
                            instance->SetData(TYPE_EVENT, 6005);
                            break;
                        case 6005:
                            DoSendQuantumText(SAY_STAGE_4_06, me);
							me->SummonCreature(NPC_ARGENT_MAGE, me->GetPositionX(), me->GetPositionY()+5, me->GetPositionZ(), me->GetOrientation());
							UpdateTimer = 10000;
							instance->SetData(TYPE_EVENT, 6008);
							break;
						case 6008:
						{
							Creature* mage = me->FindCreatureWithDistance(NPC_ARGENT_MAGE, 100.0f, true);
							mage->SetOrientation(me->GetOrientation());
							mage->AI()->DoCastAOE(SPELL_DALARAN_PORTAL);
							UpdateTimer = 1000;
							instance->SetData(TYPE_EVENT, 6010);
							break;
						}
                        case 6010:
                            if (IsHeroic())
                            {
                                DoSendQuantumText(SAY_STAGE_4_07, me);
                                UpdateTimer = 60000;
                                instance->SetData(TYPE_ANUBARAK, SPECIAL);
                                instance->SetData(TYPE_EVENT, 6020);
                            }
							else
								instance->SetData(TYPE_EVENT, 6030);
							break;
						case 6020:
							me->DespawnAfterAction();
							UpdateTimer = 5000;
							instance->SetData(TYPE_EVENT, 6030);
							break;
                        }
                   }
                   else UpdateTimer -= diff;
				   instance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
              }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tirion_tocAI(creature);
        }
};

class npc_garrosh_toc : public CreatureScript
{
    public:
        npc_garrosh_toc() : CreatureScript("npc_garrosh_toc") { }

        struct npc_garrosh_tocAI : public QuantumBasicAI
        {
            npc_garrosh_tocAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 UpdateTimer;

            void Reset() {}

            void AttackStart(Unit* /*who*/) {}

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
					return;

                if (instance->GetData(TYPE_EVENT_NPC) != NPC_GARROSH_HELLSCREAM)
					return;

                UpdateTimer = instance->GetData(TYPE_EVENT_TIMER);

                if (UpdateTimer <= diff)
                {
                    switch (instance->GetData(TYPE_EVENT))
                    {
                        case 130:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_TALK);
                            DoSendQuantumText(SAY_STAGE_0_03h, me);
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 132);
                            break;
                        case 132:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 140);
                            break;
                        case 2010:
                            DoSendQuantumText(SAY_STAGE_1_09, me);
                            UpdateTimer = 9000;
                            instance->SetData(TYPE_EVENT, 2020);
                            break;
                        case 3050:
                            DoSendQuantumText(SAY_STAGE_2_02h, me);
                            UpdateTimer = 15000;
                            instance->SetData(TYPE_EVENT, 3060);
                            break;
                        case 3070:
                            DoSendQuantumText(SAY_STAGE_2_04h, me);
                            UpdateTimer = 6000;
                            instance->SetData(TYPE_EVENT, 3080);
                            break;
                        case 3081:
                            DoSendQuantumText(SAY_STAGE_2_05h, me);
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 3091);
                            break;
                        case 4030:
                            DoSendQuantumText(SAY_STAGE_3_03h, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 4040);
                            break;
                    }
                }
				else UpdateTimer -= diff;
                instance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_garrosh_tocAI(creature);
        }
};

class npc_varian_toc : public CreatureScript
{
    public:
        npc_varian_toc() : CreatureScript("npc_varian_toc") { }

        struct npc_varian_tocAI : public QuantumBasicAI
        {
            npc_varian_tocAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 UpdateTimer;

            void Reset() {}

            void AttackStart(Unit* /*who*/) {}

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
					return;

                if (instance->GetData(TYPE_EVENT_NPC) != NPC_KING_VARIAN_WRYNN)
					return;
				
				UpdateTimer = instance->GetData(TYPE_EVENT_TIMER);
				if (UpdateTimer <= diff)
                {
                    switch (instance->GetData(TYPE_EVENT))
                    {
                        case 120:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_TALK);
                            DoSendQuantumText(SAY_STAGE_0_03a, me);
                            UpdateTimer = 2000;
                            instance->SetData(TYPE_EVENT, 122);
                            break;
                        case 122:
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 130);
                            break;
                        case 2020:
                            DoSendQuantumText(SAY_STAGE_1_10, me);
                            UpdateTimer = 6000;
                            instance->SetData(TYPE_EVENT, 2030);
                            break;
                        case 3051:
                            DoSendQuantumText(SAY_STAGE_2_02a, me);
                            UpdateTimer = 15000;
                            instance->SetData(TYPE_EVENT, 3061);
                            break;
                        case 3071:
                            DoSendQuantumText(SAY_STAGE_2_04a, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 3081);
                            break;
                        case 3080:
                            DoSendQuantumText(SAY_STAGE_2_05a, me);
                            UpdateTimer = 3000;
                            instance->SetData(TYPE_EVENT, 3090);
                            break;
                        case 4020:
                            DoSendQuantumText(SAY_STAGE_3_03a, me);
                            UpdateTimer = 5000;
                            instance->SetData(TYPE_EVENT, 4040);
                            break;
                    }
                }
				else UpdateTimer -= diff;
                instance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_varian_tocAI(creature);
        }
};

void AddSC_trial_of_the_crusader()
{
    new npc_announcer_toc10();
    new boss_lich_king_toc();
    new npc_fizzlebang_toc();
    new npc_tirion_toc();
    new npc_garrosh_toc();
    new npc_varian_toc();
}