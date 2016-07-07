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
#include "LFGMgr.h"
#include "the_slave_pens.h"

#define EMOTE_SUBMERGE "Ahune Retreats. His Defenses Diminish."
#define EMOTE_EMERGE_SOON "Ahune will soon resurface."

#define GOSSIP_STONE_ITEM "Disturb the stone and summon Lord Ahune."

enum Spells
{
    SPELL_SLIPPERY_FLOOR_AMBIENT  = 46314,
    SPELL_SUMMON_ICE_SPEAR_BUNNY  = 46359,
    SPELL_SUMMON_ICE_SPEAR_OBJECT = 46369,
    SPELL_SELF_STUN               = 46416,
    SPELL_RESURFACE               = 46402,
    SPELL_AHUNES_SHIELD           = 45954,
    SPELL_BEAM_ATTACK             = 46336,
    SPELL_BEAM_ATTACK_BEAM        = 46363,
    SPELL_SUBMERGE                = 37550,
    SPELL_EMERGE                  = 50142,
    SPELL_SNOWY                   = 46423,
	//
    SPELL_CHILLING_AURA           = 46542,
	SPELL_BITTER_BLAST            = 46406,
    SPELL_PULVERIZE               = 31757, // 742
    SPELL_WIND_BUFFET             = 46568,
    SPELL_LIGHTNING_SHIELD        = 51620,
	//
    SPELL_ICE_SPEAR_KNOCKBACK     = 46360,
    SPELL_SLIP                    = 45947,
    SPELL_YOU_SLIPPED             = 45946,
    SPELL_SLIPPERY_FLOOR          = 45945,
	SPELL_SUMMON_AHUNES_LOOT_5N   = 45939,
	SPELL_SUMMON_AHUNES_LOOT_5H   = 46622,
	SPELL_AHUNE_DIES              = 62043,
	SPELL_AHUNE_SPAWN_COLD_EARTH  = 45938,
};

enum Actions
{
    ACTION_START_EVENT = 1,
};

enum Events
{
    EVENT_EMERGE              = 1,
    EVENT_EMERGE_SOON         = 2,
    EVENT_SUBMERGED           = 3,
    EVENT_EARTHEN_RING_ATTACK = 4,
    EVENT_SUMMON_HAILSTONE    = 5,
    EVENT_SUMMON_COLDWAVE     = 6,
    EVENT_ICE_SPEAR           = 7,
};

class npc_frostlord_ahune : public CreatureScript
{
    public:
        npc_frostlord_ahune() : CreatureScript("npc_frostlord_ahune") { }

        struct npc_frostlord_ahuneAI : public QuantumBasicAI
        {
            npc_frostlord_ahuneAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
			{
				SetCombatMovement(false);
				me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			}

			SummonList summons;
            EventMap events;

            bool submerged;

            uint8 Attacks;

            void Reset()
            {
                summons.DespawnAll();
                events.Reset();
                submerged = false;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetCorpseDelay(20);
                me->SetVisible(false);
                DoCast(me, SPELL_AHUNES_SHIELD, true);
                DoCast(me, SPELL_SUBMERGE, true);

                me->SummonGameObject(GO_ICE_STONE, -57.1839f, -163.09f, -1.86789f, 3.03712f, 0, 0, 0, 0, 1*DAY);
            }

			void EnterToBattle(Unit* /*who*/)
			{
				DoCast(me, SPELL_AHUNE_SPAWN_COLD_EARTH, true);
			}

			void JustDied(Unit* /*killer*/)
            {
				if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 35.0f, true))
					trigger->CastSpell(trigger, DUNGEON_MODE(SPELL_SUMMON_AHUNES_LOOT_5N, SPELL_SUMMON_AHUNES_LOOT_5H), true);

				me->RemoveAurasDueToSpell(SPELL_AHUNE_SPAWN_COLD_EARTH);

                summons.DespawnAll();

				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (player->GetDistance(me) < 150.0f)
							player->CastSpell(player, SPELL_AHUNE_DIES, true);
					}
				}

				Map::PlayerList const& RewardPlayers = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = RewardPlayers.begin(); itr != RewardPlayers.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (player->GetDistance(me) < 150.0f)
							sLFGMgr->RewardDungeonDoneFor(286, player);
					}
				}
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_FROZEN_CORE:
                        summon->SetHealth(me->GetHealth());
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
						summon->SetReactState(REACT_PASSIVE);
						summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
						summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                        break;
                    case NPC_SLIPPERY_FLOOR:
                        summon->SetCurrentFaction(1081);
                        summon->AddAura(SPELL_SLIPPERY_FLOOR_AMBIENT, summon);
						summon->AddAura(SPELL_SLIPPERY_FLOOR, summon);
						summon->CastSpell(summon, SPELL_YOU_SLIPPED);
						summon->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_ICE_SPEAR_BUNNY:
                        summon->SetInCombatWithZone();
                        return;
                }

                summons.Summon(summon);
                summon->SetInCombatWithZone();
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                if (me->IsDead())
                    return;

                if (summon->GetEntry() == NPC_FROZEN_CORE)
                    me->SetHealth(summon->GetHealth());
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                if (summon->GetEntry() == NPC_FROZEN_CORE)
                    me->DealDamage(me, me->GetHealth());
            }

            void DoAction(int32 const action)
            {
                me->SetVisible(true);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetInCombatWithZone();
                events.ScheduleEvent(EVENT_EMERGE, 10000);

                if (GameObject* chest = me->FindGameobjectWithDistance(GO_ICE_CHEST, 150.0f))
                    chest->Delete();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                while (uint32 EventId = events.ExecuteEvent())
                {
                    switch (EventId)
                    {
                        case EVENT_EMERGE:
							// Ice Floor
							me->SummonCreature(NPC_SLIPPERY_FLOOR, -111.753f, -174.774f, -1.77584f, 3.30165f, TEMPSUMMON_TIMED_DESPAWN, 90000);
							me->SummonCreature(NPC_SLIPPERY_FLOOR, -109.502f, -254.019f, -1.48344f, 3.04088f, TEMPSUMMON_TIMED_DESPAWN, 90000);
							me->SummonCreature(NPC_SLIPPERY_FLOOR, -86.97f, -249.732f, -1.01416f, 4.30773f, TEMPSUMMON_TIMED_DESPAWN, 90000);
							me->SummonCreature(NPC_SLIPPERY_FLOOR, -88.4155f, -218.811f, -1.16138f, 1.44495f, TEMPSUMMON_TIMED_DESPAWN, 90000);
							me->SummonCreature(NPC_SLIPPERY_FLOOR, -106.569f, -218.814f, -1.39246f, 4.85798f, TEMPSUMMON_TIMED_DESPAWN, 90000);
							me->SummonCreature(NPC_SLIPPERY_FLOOR, -89.2555f, -168.14f, -3.13713f, 4.55829f, TEMPSUMMON_TIMED_DESPAWN, 90000);
                            me->RemoveAurasDueToSpell(SPELL_SELF_STUN);
                            me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            DoCastAOE(SPELL_RESURFACE, true);
                            DoCast(SPELL_EMERGE);
                            Attacks = 0;
                            events.ScheduleEvent(EVENT_EARTHEN_RING_ATTACK, 10000);
                            events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 2000);
                            events.ScheduleEvent(EVENT_SUMMON_COLDWAVE, 5000);
                            events.ScheduleEvent(EVENT_ICE_SPEAR, 10000);
                            break;
                        case EVENT_EMERGE_SOON:
                            me->MonsterTextEmote(EMOTE_EMERGE_SOON, LANG_UNIVERSAL, true);
                            break;
                        case EVENT_SUBMERGED:
                            DoCast(me, SPELL_SELF_STUN, true);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            me->SummonCreature(NPC_FROZEN_CORE, *me, TEMPSUMMON_TIMED_DESPAWN, 30000);
                            submerged = true;
                            events.ScheduleEvent(EVENT_EMERGE_SOON, 25000);
                            events.ScheduleEvent(EVENT_EMERGE, 30000);
                            break;
                        case EVENT_EARTHEN_RING_ATTACK:
                            ++Attacks;
                            if (Attacks >= 9)
                            {
                                me->MonsterTextEmote(EMOTE_SUBMERGE, LANG_UNIVERSAL, true);
                                DoCast(SPELL_SUBMERGE);
                                events.Reset();
                                events.ScheduleEvent(EVENT_SUBMERGED, 4500);
                            }
                            else
                            {
								std::list<Creature*> TotemList;
								me->GetCreatureListWithEntryInGrid(TotemList, NPC_EARTHEN_RING_TOTEM, 150.0f);

								if (!TotemList.empty())
								{
									for (std::list<Creature*>::const_iterator itr = TotemList.begin(); itr != TotemList.end(); ++itr)
									{
										if (Creature* totem = *itr)
											 totem->CastSpell(me, SPELL_BEAM_ATTACK_BEAM, true);
									}
								}

                                events.ScheduleEvent(EVENT_EARTHEN_RING_ATTACK, 10000);
                            }
                            break;
                        case EVENT_SUMMON_HAILSTONE:
                            me->SummonCreature(NPC_AHUNITE_HAILSTONE, float(irand(-110, -80)), float(irand(-225, -215)), -1.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                            break;
                        case EVENT_SUMMON_COLDWAVE:
                            for (uint8 i = 0; i < 2; ++i)
                                me->SummonCreature(NPC_AHUNITE_COLDWAVE, float(irand(-110, -80)), float(irand(-225, -215)), -1.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            if (submerged)
                                me->SummonCreature(NPC_AHUNITE_FROSTWIND, float(irand(-110, -80)), float(irand(-225, -215)), -1.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            events.ScheduleEvent(EVENT_SUMMON_COLDWAVE, 7000);
                            break;
                        case EVENT_ICE_SPEAR:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_SUMMON_ICE_SPEAR_BUNNY);

                            events.ScheduleEvent(EVENT_ICE_SPEAR, 7000);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_frostlord_ahuneAI(creature);
        }
};

class npc_ahune_ice_spear : public CreatureScript
{
    public:
        npc_ahune_ice_spear() : CreatureScript("npc_ahune_ice_spear") { }

        struct npc_ahune_ice_spearAI : public QuantumBasicAI
        {
            npc_ahune_ice_spearAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);

                SpikeTimer = 1500;

                Spiked = false;

                DoCast(me, SPELL_SUMMON_ICE_SPEAR_OBJECT, true);
            }

			uint32 SpikeTimer;

            bool Spiked;

            void UpdateAI(uint32 const diff)
            {
                if (SpikeTimer <= diff)
                {
                    GameObject* spike = me->FindGameobjectWithDistance(GO_ICE_SPEAR, 10.0f);
                    if (spike && !Spiked)
                    {
                        if (Unit* target = SelectTarget(TARGET_NEAREST, 0, 3.0f, true))
                            target->CastSpell(target, SPELL_ICE_SPEAR_KNOCKBACK, true);
                        spike->UseDoorOrButton();
                        Spiked = true;
                        SpikeTimer = 3500;
                    }
                    else if (spike)
                    {
                        spike->Delete();
                        me->DespawnAfterAction();
                    }
                }
                else SpikeTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahune_ice_spearAI(creature);
        }
};

class go_ahune_ice_stone : public GameObjectScript
{
public:
	go_ahune_ice_stone() : GameObjectScript("go_ahune_ice_stone") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER)
		{
			player->PrepareQuestMenu(go->GetGUID());
			player->SendPreparedQuest(go->GetGUID());
		}

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STONE_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
		{
			if (Creature* ahune = go->FindCreatureWithDistance(NPC_FROSTLORD_AHUNE, 150.0f, true))
			{
				ahune->AI()->DoAction(ACTION_START_EVENT);
				go->Delete();
			}
		}
		return true;
	}
};

void AddSC_boss_frostlord_ahune()
{
    new npc_frostlord_ahune();
    new npc_ahune_ice_spear();
    new go_ahune_ice_stone();
}