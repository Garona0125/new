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

enum LunarFestival
{
    NPC_OMEN                    = 15467,

	SPELL_SPOTLIGHT             = 25824,
	SPELL_SPOTLIGHT_SIZE        = 50236,
    SPELL_OMEN_CLEAVE           = 15284,
    SPELL_OMEN_STARFALL         = 26540,
    SPELL_OMEN_SUMMON_SPOTLIGHT = 26392,
    SPELL_ELUNE_CANDLE          = 26374,
	SPELL_SPIRIT_PARTICLES      = 17327,
	SPELL_ELUNE_CANDLE_EFFECT   = 26636,
	SPELL_FIREWORK_DAZZED       = 25495,

    GO_ELUNE_TRAP_1             = 180876,
    GO_ELUNE_TRAP_2             = 180877,

    EVENT_CAST_CLEAVE           = 1,
    EVENT_CAST_STARFALL         = 2,
    EVENT_DESPAWN               = 3,
};

class npc_elder_lunar_festival : public CreatureScript
{
public:
    npc_elder_lunar_festival() : CreatureScript("npc_elder_lunar_festival") {}

    struct npc_elder_lunar_festivalAI : public QuantumBasicAI
    {
        npc_elder_lunar_festivalAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_SPOTLIGHT, true);
        }

        void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elder_lunar_festivalAI(creature);
    }
};

class npc_lunar_festival_harbinger : public CreatureScript
{
public:
    npc_lunar_festival_harbinger() : CreatureScript("npc_lunar_festival_harbinger") {}

    struct npc_lunar_festival_harbingerAI : public QuantumBasicAI
    {
        npc_lunar_festival_harbingerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_SPOTLIGHT);
        }

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lunar_festival_harbingerAI(creature);
    }
};

class npc_omen : public CreatureScript
{
public:
    npc_omen() : CreatureScript("npc_omen") {}

    struct npc_omenAI : public QuantumBasicAI
    {
        npc_omenAI(Creature* creature) : QuantumBasicAI(creature) {}

        EventMap events;

        void Reset()
		{
			me->SetHealth(me->GetMaxHealth());
		}

        void EnterToBattle(Unit* /*who*/)
        {
            events.Reset();
            events.ScheduleEvent(EVENT_CAST_CLEAVE, urand(3000, 5000));
            events.ScheduleEvent(EVENT_CAST_STARFALL, urand(8000, 10000));
        }

        void JustDied(Unit* /*killer*/)
        {
            DoCast(SPELL_OMEN_SUMMON_SPOTLIGHT);
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ELUNE_CANDLE)
            {
                if (me->HasAura(SPELL_OMEN_STARFALL))
                    me->RemoveAurasDueToSpell(SPELL_OMEN_STARFALL);

                events.RescheduleEvent(EVENT_CAST_STARFALL, urand(14000, 16000));
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            switch (events.ExecuteEvent())
            {
                case EVENT_CAST_CLEAVE:
                    DoCastVictim(SPELL_OMEN_CLEAVE);
                    events.ScheduleEvent(EVENT_CAST_CLEAVE, urand(8000, 10000));
                    break;
                case EVENT_CAST_STARFALL:
                    DoCast(SPELL_OMEN_STARFALL);
                    events.ScheduleEvent(EVENT_CAST_STARFALL, urand(14000, 16000));
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_omenAI(creature);
    }
};

class npc_giant_spotlight : public CreatureScript
{
public:
    npc_giant_spotlight() : CreatureScript("npc_giant_spotlight") { }

    struct npc_giant_spotlightAI : public QuantumBasicAI
    {
        npc_giant_spotlightAI(Creature* creature) : QuantumBasicAI(creature) {}

        EventMap events;

        void Reset()
        {
			DoCast(me, SPELL_SPOTLIGHT_SIZE, true);

            events.Reset();
            events.ScheduleEvent(EVENT_DESPAWN, 5*MINUTE*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            if (events.ExecuteEvent() == EVENT_DESPAWN)
            {
                if (GameObject* trap = me->FindGameobjectWithDistance(GO_ELUNE_TRAP_1, 5.0f))
                    trap->RemoveFromWorld();

                if (GameObject* trap = me->FindGameobjectWithDistance(GO_ELUNE_TRAP_2, 5.0f))
                    trap->RemoveFromWorld();

                if (Creature* omen = me->FindCreatureWithDistance(NPC_OMEN, 5.0f, false))
                    omen->DespawnAfterAction();

                me->DespawnAfterAction();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_giant_spotlightAI(creature);
    }
};

class npc_minion_of_omen : public CreatureScript
{
public:
    npc_minion_of_omen() : CreatureScript("npc_minion_of_omen") {}

    struct npc_minion_of_omenAI : public QuantumBasicAI
    {
        npc_minion_of_omenAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_SPIRIT_PARTICLES);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ELUNE_CANDLE_EFFECT)
				DoCast(me, SPELL_FIREWORK_DAZZED);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_minion_of_omenAI(creature);
    }
};

/**********************************
* SCOURGE INVASION EVENT KARAZHAN *
***********************************/
enum PrinceTenrisMirkblood
{
	SPELL_SANGUE_SPIRIT_PRE_AURA_1 = 51282,
	SPELL_SANGUE_SPIRIT_PRE_AURA_2 = 51283,
	SPELL_SHADOW_BOLT_VOLLEY       = 45031,
	SPELL_VAMPIRIC_BOLT            = 51016,
	SPELL_VAMPIRE_PRINCE_TELEPORT  = 42982,
	SPELL_ORB_VISUAL               = 48044,
	NPC_SANGUE_SPIRIT_TRIGGER      = 34304,
	SOUND_ENTER_ROOM               = 14213,
	SAY_ENTER_ROOM                 = -1502200,
};

const Position SangueSpiritSpawnLocations[] =
{
	{-11083.1f, -1972.67f, 83.874f, 2.85354f},
	{-11086.6f, -1959.15f, 83.1614f, 4.37328f},
	{-11100.6f, -1966.87f, 84.3993f, 5.41393f},
	{-11089.3f, -1991.44f, 84.8295f, 2.02101f},
	{-11104.1f, -1977.07f, 84.9808f, 0.300987f},
};

class boss_prince_tenris_mirkblood : public CreatureScript
{
public:
    boss_prince_tenris_mirkblood() : CreatureScript("boss_prince_tenris_mirkblood") {}

    struct boss_prince_tenris_mirkbloodAI : public QuantumBasicAI
    {
        boss_prince_tenris_mirkbloodAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShadowBoltVolleyTimer;
		uint32 VampiricBoltTimer;

		bool Intro;
		SummonList Summons;

        void Reset()
        {
			Intro = false;

			Summons.DespawnAll();

			ShadowBoltVolleyTimer = 500;
			VampiricBoltTimer = 3000;

			DoCast(me, SPELL_SANGUE_SPIRIT_PRE_AURA_1);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 35.0f))
				return;

			if (Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 35.0f))
			{
				me->MonsterYellToZone(SAY_ENTER_ROOM, LANG_UNIVERSAL, 0);
				DoPlaySoundToSet(me, SOUND_ENTER_ROOM);
				Intro = true;
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_VAMPIRE_PRINCE_TELEPORT);

			for (uint8 i = 0; i < 5; i++)
			{
				if (Creature* spirit = me->SummonCreature(NPC_SANGUE_SPIRIT_TRIGGER, SangueSpiritSpawnLocations[i], TEMPSUMMON_CORPSE_DESPAWN))
					spirit->CastSpell(spirit, SPELL_ORB_VISUAL, true);
			}
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SANGUE_SPIRIT_TRIGGER)
				Summons.Summon(summon);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
					ShadowBoltVolleyTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltVolleyTimer -= diff;

			if (VampiricBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VAMPIRIC_BOLT);
					VampiricBoltTimer = urand(5000, 6000);
				}
			}
			else VampiricBoltTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAura(SPELL_SANGUE_SPIRIT_PRE_AURA_2, 0))
					DoCast(me, SPELL_SANGUE_SPIRIT_PRE_AURA_2);
			}

			if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 30)
			{
				EnterEvadeMode();
				return;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_prince_tenris_mirkbloodAI(creature);
    }
};

enum Brewfest
{
	NPC_SELF_TURNING_KILL_CREDIT  = 24108,

	SPELL_DRINK_INVISIBILITY_PINK = 36440,
	SPELL_DRINK                   = 42436,
	SPELL_WOLPERTINGER_NET        = 41621,
	SPELL_CREATE_WOLPERTINGER     = 41622,
};

class npc_self_turning_and_oscillating_utility_target : public CreatureScript
{
public:
    npc_self_turning_and_oscillating_utility_target() : CreatureScript("npc_self_turning_and_oscillating_utility_target") {}

    struct npc_self_turning_and_oscillating_utility_targetAI : public QuantumBasicAI
    {
		npc_self_turning_and_oscillating_utility_targetAI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_DRINK)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_SELF_TURNING_KILL_CREDIT, 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_self_turning_and_oscillating_utility_targetAI(creature);
    }
};

class npc_wild_wolpertinger : public CreatureScript
{
public:
    npc_wild_wolpertinger() : CreatureScript("npc_wild_wolpertinger") {}

    struct npc_wild_wolpertingerAI : public QuantumBasicAI
    {
		npc_wild_wolpertingerAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(me, SPELL_DRINK_INVISIBILITY_PINK);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_WOLPERTINGER_NET)
			{
				if (Player* player = caster->ToPlayer())
					caster->CastSpell(caster, SPELL_CREATE_WOLPERTINGER, true);

				me->DespawnAfterAction();
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wild_wolpertingerAI(creature);
    }
};

enum HaloweenEvent
{
	NPC_SHADE_HORSEMAN             = 23543,
	NPC_HEADLESS_FIRE              = 23537,
	NPC_FIRE_DUMMY                 = 23686,
	
	QUEST_LET_THE_FIRES_COME_A     = 12135,
	QUEST_LET_THE_FIRES_COME_H     = 12139,
	QUEST_STOP_THE_FIRES_A         = 11131,
	QUEST_STOP_THE_FIRES_H         = 11219,

	SPELL_HORSEMAN_CONFLAGRATION_1 = 42380,
	SPELL_HORSEMAN_CONFLAGRATION_2 = 42869,
	SPELL_HORSEMAN_JACK_O_LANTERN  = 44185,
	SPELL_HORSEMAN_CLEAVE          = 15496,
	SPELL_FIRE_CREATE_NODE         = 42118,
    SPELL_WATER_SPOUT_VISUAL       = 42348,
    SPELL_FIRE_VISUAL_BUFF         = 42074,
    SPELL_FIRE_SIZE_STACK          = 42091,
    SPELL_FIRE_STARTING_SIZE       = 42096,
    SPELL_QUESTS_CREDITS           = 42242,
    SPELL_CREATE_WATER_BUCKET      = 42349,
	SPELL_BUCKET_LANDS             = 42339,

	GO_FIRE_EFFIGY                 = 186720,
	GO_LARGE_JACK_O_LANTERN        = 186887,
	GO_WATER_BARREL                = 186234,

	SAY_HORSEMAN_SPAWN             = 11966,
    SAY_HORSEMAN_FIRES_OUT         = 11968,
    SAY_HORSEMAN_FIRES_FAIL        = 11967,
    SAY_HORSEMAN_LAUGHS            = 11965,
	SAY_HORSEMAN_DEATH             = 11964,

    EVENT_FIRE_NONE                = 1,
    EVENT_FIRE_HIT_BY_BUCKET       = 2,
    EVENT_FIRE_VISUAL_WATER        = 3,
    EVENT_FIRE_GROW_FIRE           = 4,
    EVENT_HORSEMAN_CONFLAGRATION   = 5,
    EVENT_HORSEMAN_CLEAVE          = 6,
    EVENT_HORSEMAN_LAUGHS          = 7,
    EVENT_FIRE_FINISH              = 8,
    EVENT_FIRE_FAIL                = 9,

	FIRE_NODES_PER_AREA            = 13,

	MOUNT_HORSEMAN                 = 25159,
};

class npc_halloween_orphan_matron : public CreatureScript
{
public:
    npc_halloween_orphan_matron() : CreatureScript("npc_halloween_orphan_matron") { }

    uint64 HeadlessHoresemanGUID;

    bool OnGossipHello(Player* player, Creature* me)
    {
        player->PrepareQuestMenu(me->GetGUID());

        if (Creature* horseman = me->GetCreature(*me, HeadlessHoresemanGUID))
        {
            QuestMenu &qm = player->PlayerTalkClass->GetQuestMenu();
            QuestMenu qm2;

            uint32 quest1 = player->GetTeam() == ALLIANCE ? QUEST_LET_THE_FIRES_COME_A : QUEST_LET_THE_FIRES_COME_H;
            uint32 quest2 = player->GetTeam() == ALLIANCE ? QUEST_STOP_THE_FIRES_A : QUEST_STOP_THE_FIRES_H;

            for (uint32 i = 0; i<qm.GetMenuItemCount(); ++i)
            {
                if (qm.GetItem(i).QuestId == quest1 || qm.GetItem(i).QuestId == quest2)
                    continue;

                qm2.AddMenuItem(qm.GetItem(i).QuestId, qm.GetItem(i).QuestIcon);
            }

            if (player->GetQuestStatus(quest1) == QUEST_STATUS_NONE)
            {
                if (player->GetQuestStatus(quest2) == QUEST_STATUS_NONE)
                    qm2.AddMenuItem(quest2, 2);

                else if (player->GetQuestStatus(quest2) != QUEST_STATUS_REWARDED)
                    qm2.AddMenuItem(quest2, 4);
            }
            else
			{
                if (player->GetQuestStatus(quest1) != QUEST_STATUS_REWARDED)
                    qm2.AddMenuItem(quest1, 4);
			}

            qm.ClearMenu();

            for (uint32 i = 0; i<qm2.GetMenuItemCount(); ++i)
                qm.AddMenuItem(qm2.GetItem(i).QuestId, qm2.GetItem(i).QuestIcon);
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnQuestAccept(Player* player, Creature* me, Quest const* quest)
    {
        if (!(me->GetAreaId() == 87 || me->GetAreaId() == 362))
            return true;

        if (quest->GetQuestId() == QUEST_LET_THE_FIRES_COME_A || quest->GetQuestId() == QUEST_LET_THE_FIRES_COME_H)
        {
            Creature* horseman = me->GetCreature(*me, HeadlessHoresemanGUID);

            if (!horseman)
            {
				sLog->outBasic("HEADLESS HORSEMAN TRACKING. SUMMON: GUID Player: %u. Area %u.", player->GetGUID(), player->GetAreaId());
                if (Creature* newHorseman = player->SummonCreature(NPC_SHADE_HORSEMAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 20.0f, 0, TEMPSUMMON_DEAD_DESPAWN, 180000))
                    HeadlessHoresemanGUID = newHorseman->GetGUID();
            }
        }
        return true;
    }
};

const Position FireNodesGoldShire[FIRE_NODES_PER_AREA + 1] =
{
    {-9459.41f, 43.90f, 64.23f, 0.00f},
    {-9472.57f, 41.11f, 64.17f, 0.00f},
    {-9467.22f, 85.86f, 66.20f, 0.00f},
    {-9472.94f, 93.84f, 69.20f, 0.00f},
    {-9462.50f, 103.90f, 68.51f, 0.00f},
    {-9467.84f, 100.69f, 66.12f, 0.00f},
    {-9456.91f, 112.81f, 66.12f, 0.00f},
    {-9478.22f, 41.65f, 69.85f, 0.00f},
    {-9481.30f, 24.87f, 69.08f, 0.00f},
    {-9482.69f, 14.39f, 62.94f, 0.00f},
    {-9471.16f, -6.65f, 70.76f, 0.00f},
    {-9451.26f, 38.83f, 68.02f, 0.00f},
    {-9450.13f, 89.46f, 66.22f, 0.00f},
    {-9464.28f,68.1982f,56.2331f,0.0f}, // Center of Town
};

const Position FireNodesRazorHill[FIRE_NODES_PER_AREA+1] =
{
    {372.70f, -4714.64f, 23.11f, 0.00f},
    {343.11f, -4708.87f, 29.19f, 0.00f},
    {332.06f, -4703.21f, 24.52f, 0.00f},
    {317.20f, -4694.22f, 16.78f, 0.00f},
    {326.30f, -4693.24f, 34.59f, 0.00f},
    {281.18f, -4705.37f, 22.38f, 0.00f},
    {293.32f, -4773.45f, 25.03f, 0.00f},
    {280.17f, -4831.90f, 22.25f, 0.00f},
    {319.04f, -4770.23f, 31.47f, 0.00f},
    {362.50f, -4676.11f, 28.63f, 0.00f},
    {348.71f, -4805.08f, 32.23f, 0.00f},
    {342.88f, -4837.07f, 26.29f, 0.00f},
    {361.80f, -4769.27f, 18.49f, 0.00f},
    {317.837f,-4734.06f,9.76272f,0.0f}, // Center of Town
};

class npc_halloween_fire : public CreatureScript
{
public:
    npc_halloween_fire() : CreatureScript("npc_halloween_fire") { }

    struct npc_halloween_fireAI : public QuantumBasicAI
    {
        npc_halloween_fireAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool FireEffigy;
        bool Off;

        EventMap events;
        uint64 playerGUID;

        void Reset()
        {
            Off = false;
            FireEffigy = false;
            playerGUID = 0;
            events.Reset();
            // Mark the npc if is for handling effigy instead of horseman fires
            if(GameObject* effigy = me->FindGameobjectWithDistance(GO_FIRE_EFFIGY, 1.0f))
                FireEffigy = true;

            me->CastSpell(me, SPELL_FIRE_STARTING_SIZE, true);
            events.ScheduleEvent(EVENT_FIRE_GROW_FIRE, 1000);
        }

		void SetGUID(uint64 guid, int32 id)
        {
            if (Off)
				return;

            if (id == EVENT_FIRE_HIT_BY_BUCKET)
            {
                playerGUID = guid;
                if (FireEffigy)
                {
                    if (GameObject* effigy = me->FindGameobjectWithDistance(GO_FIRE_EFFIGY, 0.5f))
					{
                        if (effigy->GetGoState() == GO_STATE_ACTIVE)
                            events.ScheduleEvent(EVENT_FIRE_VISUAL_WATER, 1000);
					}
                }
				else
                {
                    if (Creature* horseman = me->GetCreature(*me, me->GetCreatorGUID()))
                        horseman->AI()->SetGUID(playerGUID, EVENT_FIRE_HIT_BY_BUCKET);

                    events.ScheduleEvent(EVENT_FIRE_VISUAL_WATER, 1000);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            switch (events.ExecuteEvent())
            {
                case EVENT_FIRE_VISUAL_WATER:
                    me->CastSpell(me, SPELL_WATER_SPOUT_VISUAL, true);
					
					if (Player* player = me->GetPlayer(*me, playerGUID))
					{
						player->AreaExploredOrEventHappens(QUEST_LET_THE_FIRES_COME_A);
						player->AreaExploredOrEventHappens(QUEST_LET_THE_FIRES_COME_H);
						player->AreaExploredOrEventHappens(QUEST_STOP_THE_FIRES_A);
						player->AreaExploredOrEventHappens(QUEST_STOP_THE_FIRES_H);
					}

                    if (FireEffigy)
                    {
                        if (GameObject* effigy = me->FindGameobjectWithDistance(GO_FIRE_EFFIGY, 1.0f))
                        {
                            effigy->SetGoState(GO_STATE_READY);

                            if (Player* player = me->GetPlayer(*me, playerGUID))
							{
                                player->KilledMonsterCredit(me->GetEntry(), 0);

								player->AreaExploredOrEventHappens(QUEST_LET_THE_FIRES_COME_A);
								player->AreaExploredOrEventHappens(QUEST_LET_THE_FIRES_COME_H);
								player->AreaExploredOrEventHappens(QUEST_STOP_THE_FIRES_A);
								player->AreaExploredOrEventHappens(QUEST_STOP_THE_FIRES_H);
							}

                            events.ScheduleEvent(EVENT_FIRE_GROW_FIRE, 12000);
                        }
                    }
					else
					{
                        if (Aura* fireSize = me->GetAura(SPELL_FIRE_SIZE_STACK))
                        {
                            if (fireSize->GetStackAmount() < 10)
                            {
                                me->RemoveAura(fireSize);
                                me->RemoveAurasDueToSpell(SPELL_FIRE_VISUAL_BUFF);
                                me->DespawnAfterAction(1*IN_MILLISECONDS);
                            }
							else fireSize->ModStackAmount(-10);
                        }
                    }
                    break;
                case EVENT_FIRE_GROW_FIRE:
                    if (FireEffigy)
                    {
                        if (GameObject* effigy = me->FindGameobjectWithDistance(GO_FIRE_EFFIGY, 0.5f))
                            effigy->SetGoState(GO_STATE_ACTIVE);
                    }
					else
					{
						if (Off)
							break;

                        if (Aura* fireSize = me->GetAura(SPELL_FIRE_SIZE_STACK))
                            if (fireSize->GetStackAmount() == 255)
								break;

                        if (!me->HasAura(SPELL_FIRE_STARTING_SIZE))
                            me->CastSpell(me, SPELL_FIRE_STARTING_SIZE, true);

                        if (!me->HasAura(SPELL_FIRE_VISUAL_BUFF))
                            me->CastSpell(me, SPELL_FIRE_VISUAL_BUFF, true);

                        me->CastSpell(me, SPELL_FIRE_SIZE_STACK, true);
                        events.ScheduleEvent(EVENT_FIRE_GROW_FIRE, urand(1000,2500));
                    }
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_halloween_fireAI(creature);
    }
};

class npc_shade_horseman : public CreatureScript
{
public:
    npc_shade_horseman() : CreatureScript("npc_shade_horseman") { }

    struct npc_shade_horsemanAI : public QuantumBasicAI
    {
        npc_shade_horsemanAI(Creature* creature) : QuantumBasicAI(creature), fires(creature) {}

        SummonList fires;
        EventMap events;

        bool moving;
        bool pointReached;
        bool allFiresSet;
        bool firesOut;

        uint32 wpCount;
        std::list<uint64> playerList;

        void Reset()
        {
            moving = true;
            pointReached = true;
            allFiresSet = false;
            firesOut = false;
            wpCount = 0;
            playerList.clear();
            events.Reset();

            me->Mount(MOUNT_HORSEMAN);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->AddUnitMovementFlag(MOVEMENTFLAG_ON_TRANSPORT);
			me->SetCanFly(true);
			me->SetDisableGravity(true);
			me->SetSpeed(MOVE_RUN, 2.0f, true);

            events.ScheduleEvent(EVENT_HORSEMAN_LAUGHS, 5000);
            events.ScheduleEvent(EVENT_HORSEMAN_CONFLAGRATION, 7000);
            events.ScheduleEvent(EVENT_FIRE_FAIL, 600000);
        }

        void JustSummoned(Creature* summon)
        {
            if (!summon)
                return;

            summon->SetCreatorGUID(me->GetGUID());
            fires.Summon(summon);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            if (!summon)
                return;

            fires.Despawn(summon);
        }

        void JustDied(Unit* killer)
        {
			me->PlayDirectSound(SAY_HORSEMAN_DEATH);

            if (killer && killer->GetAreaId() == me->GetAreaId())
                killer->SummonGameObject(GO_LARGE_JACK_O_LANTERN, me->GetPositionX(), me->GetPositionY(), killer->GetPositionZ()+1.0f, me->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 7*DAY);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (moving && id < FIRE_NODES_PER_AREA)
            {
                if (id == 0 && !allFiresSet)
                {
                    sLog->outBasic("HEADLESS HORSEMAN BUG TRACKING Area %u.", me->GetAreaId());
                    me->PlayDirectSound(SAY_HORSEMAN_SPAWN);
                }

                if (!allFiresSet)
                {
                    const Position pos = GetPositionsForArea()[wpCount];
                    if (pos.IsPositionValid())
                        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_FIRE_CREATE_NODE, true);
                }

                if (id+1 == FIRE_NODES_PER_AREA)
                {
                    allFiresSet = true;
                    wpCount = 0;
                }
                else
                    ++wpCount;

                pointReached = true;
            }
			else if (id == FIRE_NODES_PER_AREA && firesOut)
            {
				me->SetCanFly(false);
				me->SetDisableGravity(false);

                me->RemoveMount();
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_ON_TRANSPORT);
                events.ScheduleEvent(EVENT_HORSEMAN_CLEAVE, urand(5000, 10000));
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (!firesOut)
            {
                const Position pos = GetPositionsForArea()[wpCount];

                if (!pos.IsPositionValid())
                    return;

                if (pointReached)
                {
                    pointReached = false;
                    me->GetMotionMaster()->MovePoint(wpCount, pos.GetPositionX(), pos.GetPositionY(), GetZForArea());
                }

                if (allFiresSet && fires.size() <= (uint32)(FIRE_NODES_PER_AREA*0.3f) && wpCount != FIRE_NODES_PER_AREA+1)
                {
                    me->PlayDirectSound(SAY_HORSEMAN_FIRES_OUT);
                    wpCount = FIRE_NODES_PER_AREA;
                    me->GetMotionMaster()->MoveIdle();
                    me->GetMotionMaster()->MovePoint(FIRE_NODES_PER_AREA, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
                    fires.DespawnAll();
                    firesOut = true;
                    // Credit quest to players
                    if (!playerList.empty())
                        for (std::list<uint64>::const_iterator i = playerList.begin();i != playerList.end(); ++i)
                            if (Player* player = me->GetPlayer(*me, *i))
                                player->CastSpell(player, SPELL_QUESTS_CREDITS, true);
                    return;
                }

                switch(events.ExecuteEvent())
                {
                    case EVENT_FIRE_FAIL:
                        fires.DespawnAll();
                        me->PlayDirectSound(SAY_HORSEMAN_FIRES_FAIL);
                        wpCount = FIRE_NODES_PER_AREA+1;
                        me->GetMotionMaster()->MovePoint(wpCount, pos.GetPositionX(), pos.GetPositionY(), GetZForArea());
                        if (!playerList.empty())
                        {
                            for (std::list<uint64>::const_iterator i = playerList.begin();i != playerList.end(); ++i)
                            {
                                Player* player = me->GetPlayer(*me, *i);
                                if (player)
                                {
                                    uint32 questId = player->GetTeam() == ALLIANCE ? QUEST_LET_THE_FIRES_COME_A : QUEST_LET_THE_FIRES_COME_H;
                                    if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                                        player->FailQuest(questId);
                                }
                            }
                        }
                        me->DespawnAfterAction(10*IN_MILLISECONDS);
                        break;
					case EVENT_HORSEMAN_LAUGHS:
                        me->PlayDirectSound(SAY_HORSEMAN_LAUGHS);
                        events.ScheduleEvent(EVENT_HORSEMAN_LAUGHS, 8000);
                        break;
                    case EVENT_HORSEMAN_CONFLAGRATION:
						if (Player* player = me->FindPlayerWithDistance(300.0f))
							DoCast(player, RAND(SPELL_HORSEMAN_CONFLAGRATION_1, SPELL_HORSEMAN_CONFLAGRATION_2, SPELL_HORSEMAN_JACK_O_LANTERN), true);

                        events.ScheduleEvent(EVENT_HORSEMAN_CONFLAGRATION, 10000);
                        break;
                }
            }
			else
			{
                switch(events.ExecuteEvent())
                {
                    case EVENT_HORSEMAN_CLEAVE:
                        if (Unit* victim = me->GetVictim())
                            me->CastSpell(victim, SPELL_HORSEMAN_CLEAVE, true);
                        events.ScheduleEvent(EVENT_HORSEMAN_CLEAVE, 5000);
                        return;
                    case EVENT_HORSEMAN_CONFLAGRATION:
						if (Player* player = me->FindPlayerWithDistance(300.0f))
							DoCast(player, RAND(SPELL_HORSEMAN_CONFLAGRATION_1, SPELL_HORSEMAN_CONFLAGRATION_2, SPELL_HORSEMAN_JACK_O_LANTERN), true);

                        events.ScheduleEvent(EVENT_HORSEMAN_CONFLAGRATION, urand(5000, 10000));
                        break;
                }
                DoMeleeAttackIfReady();
            }
        }

        const Position* GetPositionsForArea()
        {
            switch (me->GetAreaId())
            {
                case 87: // GoldShire
                    return FireNodesGoldShire;
                case 362: // Razor Hill
                case 2337:
                case 14:
                    return FireNodesRazorHill;
            }
            return NULL;
        }

        float GetZForArea()
        {
            switch (me->GetAreaId())
            {
                case 87: //GoldShire
                    return 77.6f;
                case 362: // Razor Hill
                case 2337:
                case 14:
                    return 40.0f;
            }
            return 0.0f;
        }

        void SetGUID(uint64 guid, int32 id)
        {
            if (id == EVENT_FIRE_HIT_BY_BUCKET)
            {
                playerList.push_back(guid);
                playerList.unique();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shade_horsemanAI(creature);
    }
};

class item_water_bucket : public ItemScript
{
public:
	item_water_bucket() : ItemScript("item_water_bucket") {}

	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
	{
		if (Creature* dummy = player->SummonCreature(NPC_FIRE_DUMMY, targets.GetDst()->GetPositionX(), targets.GetDst()->GetPositionY(), targets.GetDst()->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 500))
		{
			std::list<Creature*> firesList;
			Trinity::AllCreaturesOfEntryInRange checker(dummy, NPC_HEADLESS_FIRE, 15.0f);
			Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(dummy, firesList, checker);
			player->VisitNearbyObject(15.0f, searcher);

			if (firesList.empty())
			{
				Creature* fire = dummy->FindCreatureWithDistance(NPC_HEADLESS_FIRE, 15.0f, true);

				if (fire && fire->IsAlive())
					fire->AI()->SetGUID(player->GetGUID(), EVENT_FIRE_HIT_BY_BUCKET);

				else if (Player* friendPlr = dummy->SelectNearestPlayer(15.0f))
				{
					if (friendPlr->IsFriendlyTo(player) && friendPlr->IsAlive())
						player->CastSpell(friendPlr, SPELL_CREATE_WATER_BUCKET, true);
				}
				else
					return false;
			}

			for (std::list<Creature*>::const_iterator i = firesList.begin(); i != firesList.end(); ++i)
			{
				if ((*i) && (*i)->IsAlive())
					(*i)->AI()->SetGUID(player->GetGUID(), EVENT_FIRE_HIT_BY_BUCKET);
			}
		}
		return false;
	}
};

#define SAY_GIFT "Take this comforting gift into the new year."

enum WinterWeilFeast
{
    SPELL_MISTLETOE_DEBUFF         = 26218,
    SPELL_CREATE_MISTLETOE         = 26206,
    SPELL_CREATE_FRESH_HOLLY       = 26207,
    SPELL_CREATE_SNOWFLAKES        = 45036,

	ITEM_GOBLIN_WEATHER_MACHINE    = 35227,
	ITEM_PERPETUAL_PURPLE_FIREWORK = 49703,
	SURPRISE_MENU                  = 0,
};

class npc_winter_reveler : public CreatureScript
{
public:
	npc_winter_reveler() : CreatureScript("npc_winter_reveler") { }

	struct npc_winter_revelerAI : public QuantumBasicAI
	{
		npc_winter_revelerAI(Creature* creature) : QuantumBasicAI(creature) {}

		void ReceiveEmote(Player* player, uint32 emote)
		{
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (player->HasAura(SPELL_MISTLETOE_DEBUFF))
				return;

			if (!IsHolidayActive(HOLIDAY_FEAST_OF_WINTER_VEIL))
				return;

			if (emote == TEXT_EMOTE_KISS)
			{
				DoCast(player, RAND(SPELL_CREATE_MISTLETOE, SPELL_CREATE_FRESH_HOLLY, SPELL_CREATE_SNOWFLAKES), true);
				DoCast(player, SPELL_MISTLETOE_DEBUFF, true);
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_winter_revelerAI(creature);
	}
};

class npc_greatfather_winter : public CreatureScript
{
public:
	npc_greatfather_winter() : CreatureScript("npc_greatfather_winter") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->HasAura(SPELL_MISTLETOE_DEBUFF))
			player->CLOSE_GOSSIP_MENU();

		player->PlayerTalkClass->ClearMenus();
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "What do you want get in the new year?", GOSSIP_SENDER_MAIN, SURPRISE_MENU);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
			case SURPRISE_MENU:
				player->CLOSE_GOSSIP_MENU();
				creature->MonsterSay(SAY_GIFT, LANG_UNIVERSAL, 0);
				creature->CastSpell(player, SPELL_MISTLETOE_DEBUFF, true);
				creature->CastSpell(player,(RAND(SPELL_CREATE_MISTLETOE, SPELL_CREATE_FRESH_HOLLY, SPELL_CREATE_SNOWFLAKES), true));
				break;
			default:
				break;
		}
		return true;
	}
};

enum CommonersText
{
	// Human Commoner
	SAY_HUMAN_COMMONER_WINTER_VEIL_1   = -1300001,
	SAY_HUMAN_COMMONER_WINTER_VEIL_2   = -1300002,
	SAY_HUMAN_COMMONER_WINTER_VEIL_3   = -1300003,
	SAY_HUMAN_COMMONER_WINTER_VEIL_4   = -1300004,
	// Draenei Commoner
	SAY_DRAENEI_COMMONER_WINTER_VEIL_1 = -1300005,
	SAY_DRAENEI_COMMONER_WINTER_VEIL_2 = -1300006,
	SAY_DRAENEI_COMMONER_WINTER_VEIL_3 = -1300007,
	SAY_DRAENEI_COMMONER_WINTER_VEIL_4 = -1300008,
	// Gnome Commoner
	SAY_GNOME_COMMONER_WINTER_VEIL_1   = -1300009,
	SAY_GNOME_COMMONER_WINTER_VEIL_2   = -1300010,
	SAY_GNOME_COMMONER_WINTER_VEIL_3   = -1300011,
	SAY_GNOME_COMMONER_WINTER_VEIL_4   = -1300012,
	// Night Elf Commoner
	SAY_N_ELF_COMMONER_WINTER_VEIL_1   = -1300013,
	SAY_N_ELF_COMMONER_WINTER_VEIL_2   = -1300014,
	SAY_N_ELF_COMMONER_WINTER_VEIL_3   = -1300015,
	SAY_N_ELF_COMMONER_WINTER_VEIL_4   = -1300016,
	// Orc Commoner
	SAY_ORC_COMMONER_WINTER_VEIL_1     = -1300017,
	SAY_ORC_COMMONER_WINTER_VEIL_2     = -1300018,
	// Tauren Commoner
	SAY_TAUREN_COMMONER_WINTER_VEIL_1  = -1300019,
	SAY_TAUREN_COMMONER_WINTER_VEIL_2  = -1300020,
	SAY_TAUREN_COMMONER_WINTER_VEIL_3  = -1300021,
	SAY_TAUREN_COMMONER_WINTER_VEIL_4  = -1300022,
	// Troll Commoner
	SAY_TROLL_COMMONER_WINTER_VEIL_1   = -1300023,
	SAY_TROLL_COMMONER_WINTER_VEIL_2   = -1300024,
	SAY_TROLL_COMMONER_WINTER_VEIL_3   = -1300025,
	// Dwarf Commoner
	SAY_DWARF_COMMONER_WINTER_VEIL_1   = -1300026,
	SAY_DWARF_COMMONER_WINTER_VEIL_2   = -1300027,
	SAY_DWARF_COMMONER_WINTER_VEIL_3   = -1300028,
	// Spells
	SPELL_CORSAIR_COSTUME              = 51926,
	// Gossip Entries
	GOSSIP_MENU_HUMAN_COMMONER         = 7898,
	GOSSIP_MENU_BLOOD_ELF_COMMONER     = 7921,
	GOSSIP_MENU_DRAENEI_COMMONER       = 7922,
	GOSSIP_MENU_DWARF_COMMONER         = 7923,
	GOSSIP_MENU_ORC_COMMONER           = 7925,
	GOSSIP_MENU_TAUREN_COMMONER        = 7926,
	GOSSIP_MENU_FORSAKEN_COMMONER      = 7927,
	GOSSIP_MENU_GNOME_COMMONER         = 7931,
	GOSSIP_MENU_TROLL_COMMONER         = 7935,
	GOSSIP_MENU_GOBLIN_COMMONER        = 8064,
	GOSSIP_MENU_NIGHT_ELF_COMMONER     = 10250,
};

enum SeasonalEvent
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class npc_human_commoner : public CreatureScript
{
public:
    npc_human_commoner() : CreatureScript("npc_human_commoner") {}

    struct npc_human_commonerAI : public QuantumBasicAI
    {
        npc_human_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_HUMAN_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_HUMAN_COMMONER_WINTER_VEIL_1, SAY_HUMAN_COMMONER_WINTER_VEIL_2, SAY_HUMAN_COMMONER_WINTER_VEIL_3, SAY_HUMAN_COMMONER_WINTER_VEIL_4), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_human_commonerAI(creature);
    }
};

class npc_draenei_commoner : public CreatureScript
{
public:
    npc_draenei_commoner() : CreatureScript("npc_draenei_commoner") {}

    struct npc_draenei_commonerAI : public QuantumBasicAI
    {
        npc_draenei_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_DRAENEI_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_DRAENEI_COMMONER_WINTER_VEIL_1, SAY_DRAENEI_COMMONER_WINTER_VEIL_2, SAY_DRAENEI_COMMONER_WINTER_VEIL_3, SAY_DRAENEI_COMMONER_WINTER_VEIL_4), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_draenei_commonerAI(creature);
    }
};

class npc_gnome_commoner : public CreatureScript
{
public:
    npc_gnome_commoner() : CreatureScript("npc_gnome_commoner") {}

    struct npc_gnome_commonerAI : public QuantumBasicAI
    {
        npc_gnome_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_GNOME_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_GNOME_COMMONER_WINTER_VEIL_1, SAY_GNOME_COMMONER_WINTER_VEIL_2, SAY_GNOME_COMMONER_WINTER_VEIL_3, SAY_GNOME_COMMONER_WINTER_VEIL_4), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gnome_commonerAI(creature);
    }
};

class npc_night_elf_commoner : public CreatureScript
{
public:
    npc_night_elf_commoner() : CreatureScript("npc_night_elf_commoner") {}

    struct npc_night_elf_commonerAI : public QuantumBasicAI
    {
        npc_night_elf_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_NIGHT_ELF_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_N_ELF_COMMONER_WINTER_VEIL_1, SAY_N_ELF_COMMONER_WINTER_VEIL_2, SAY_N_ELF_COMMONER_WINTER_VEIL_3, SAY_N_ELF_COMMONER_WINTER_VEIL_4), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_night_elf_commonerAI(creature);
    }
};

class npc_orc_commoner : public CreatureScript
{
public:
    npc_orc_commoner() : CreatureScript("npc_orc_commoner") {}

    struct npc_orc_commonerAI : public QuantumBasicAI
    {
        npc_orc_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_ORC_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_ORC_COMMONER_WINTER_VEIL_1, SAY_ORC_COMMONER_WINTER_VEIL_2), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orc_commonerAI(creature);
    }
};

class npc_tauren_commoner : public CreatureScript
{
public:
    npc_tauren_commoner() : CreatureScript("npc_tauren_commoner") {}

    struct npc_tauren_commonerAI : public QuantumBasicAI
    {
        npc_tauren_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_TAUREN_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_TAUREN_COMMONER_WINTER_VEIL_1, SAY_TAUREN_COMMONER_WINTER_VEIL_2, SAY_TAUREN_COMMONER_WINTER_VEIL_3, SAY_TAUREN_COMMONER_WINTER_VEIL_4), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tauren_commonerAI(creature);
    }
};

class npc_troll_commoner : public CreatureScript
{
public:
    npc_troll_commoner() : CreatureScript("npc_troll_commoner") {}

    struct npc_troll_commonerAI : public QuantumBasicAI
    {
        npc_troll_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_TROLL_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_TROLL_COMMONER_WINTER_VEIL_1, SAY_TROLL_COMMONER_WINTER_VEIL_2, SAY_TROLL_COMMONER_WINTER_VEIL_3), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_troll_commonerAI(creature);
    }
};

class npc_dwarf_commoner : public CreatureScript
{
public:
    npc_dwarf_commoner() : CreatureScript("npc_dwarf_commoner") {}

    struct npc_dwarf_commonerAI : public QuantumBasicAI
    {
        npc_dwarf_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

        void Reset()
		{
			SayTimer = 500;
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_DWARF_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive() && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
			{
				DoSendQuantumText(RAND(SAY_DWARF_COMMONER_WINTER_VEIL_1, SAY_DWARF_COMMONER_WINTER_VEIL_2, SAY_DWARF_COMMONER_WINTER_VEIL_3), me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dwarf_commonerAI(creature);
    }
};

class npc_blood_elf_commoner : public CreatureScript
{
public:
    npc_blood_elf_commoner() : CreatureScript("npc_blood_elf_commoner") {}

    struct npc_blood_elf_commonerAI : public QuantumBasicAI
    {
        npc_blood_elf_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset(){}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_BLOOD_ELF_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blood_elf_commonerAI(creature);
    }
};

class npc_forsaken_commoner : public CreatureScript
{
public:
    npc_forsaken_commoner() : CreatureScript("npc_forsaken_commoner") {}

    struct npc_forsaken_commonerAI : public QuantumBasicAI
    {
        npc_forsaken_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset(){}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_FORSAKEN_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_commonerAI(creature);
    }
};

class npc_goblin_commoner : public CreatureScript
{
public:
    npc_goblin_commoner() : CreatureScript("npc_goblin_commoner") {}

    struct npc_goblin_commonerAI : public QuantumBasicAI
    {
        npc_goblin_commonerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset(){}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_GOBLIN_COMMONER && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goblin_commonerAI(creature);
    }
};

enum TorchTossingBunny
{
    SPELL_TARGET_INDICATOR = 45723,
};

class npc_torch_tossing_bunny : public CreatureScript
{
    public:
        npc_torch_tossing_bunny() : CreatureScript("npc_torch_tossing_bunny") { }

        struct npc_torch_tossing_bunnyAI : public QuantumBasicAI
        {
            npc_torch_tossing_bunnyAI(Creature* creature) : QuantumBasicAI(creature) { }

            void Reset()
            {
                TargetTimer = urand(5000, 7000);
            }

			uint32 TargetTimer;

            void UpdateAI(uint32 const diff)
            {
				// Out of Combat Timer
                if (TargetTimer <= diff && !me->IsInCombatActive())
                {
					DoCast(SPELL_TARGET_INDICATOR);
					TargetTimer = urand(5000, 7000);
                }
                else TargetTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_torch_tossing_bunnyAI(creature);
        }
};

enum MidsummerFestival
{
	SPELL_FLAME_BREATH_EATER = 46332,
	SPELL_NPC_JUGGLE_TORCH   = 46322,
	SPELL_FLAME_BREATH_AURA  = 29402,
};

class npc_master_flame_eater : public CreatureScript
{
public:
    npc_master_flame_eater() : CreatureScript("npc_master_flame_eater") {}

    struct npc_master_flame_eaterAI : public QuantumBasicAI
    {
        npc_master_flame_eaterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BreathTimer;

        void Reset()
		{
			BreathTimer = 500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (BreathTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_FLAME_BREATH_EATER);
				BreathTimer = urand(6000, 7000);
			}
			else BreathTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_flame_eaterAI(creature);
    }
};

class npc_midsummer_fireeater : public CreatureScript
{
public:
    npc_midsummer_fireeater() : CreatureScript("npc_midsummer_fireeater") {}

    struct npc_midsummer_fireeaterAI : public QuantumBasicAI
    {
        npc_midsummer_fireeaterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JuggleTimer;

        void Reset()
		{
			JuggleTimer = 2000;

			DoCast(me, SPELL_FLAME_BREATH_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (JuggleTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_NPC_JUGGLE_TORCH);
				JuggleTimer = 20000;
			}
			else JuggleTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_midsummer_fireeaterAI(creature);
    }
};

enum FestivalScorchling
{
	SPELL_GROW                     = 47114,
	SPELL_COMPLETE_QUEST_INCENSE_A = 46825,
	SPELL_COMPLETE_QUEST_INCENSE_H = 46826,

	ITEM_ID_SUMMER_INCENSE         = 35725,
	NPC_RAGNAROS                   = 26502,
	QUEST_INCENSE_FOR_FESTIVAL_A   = 11964,
	QUEST_INCENSE_FOR_FESTIVAL_H   = 11966,
};

#define SAY_PART_1  "Thank you again, $N, for this delectable incense."
#define SAY_PART_2  "Devours the incense. It's ravenous!"
#define SAY_PART_3  "So good! So packed with energy!"
#define SAY_PART_4  "It has everything a growing scorchling needs!"
#define SAY_PART_5  "I can feel the power SURGING within me!"
#define SAY_PART_6  "Bellows with laughter!"
#define SAY_PART_7  "Now! Finally! Our plans can take effect!"
#define SAY_PART_8  "KNEEL, LITTLE MORTAL! KNEEL BEFORE THE MIGHT OF THE HERALD OF RAGNAROS!"
#define SAY_PART_9  "YOU WILL ALL PERISH IN FLAMES!"
#define SAY_PART_10 "Blinks..."
#define SAY_PART_11 "Ah. I was merely jesting..."

#define START_OPTION "Feed the Scorchling"

class npc_festival_scorchling : public CreatureScript
{
public:
    npc_festival_scorchling() : CreatureScript("npc_festival_scorchling") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

        if (player->HasItemCount(ITEM_ID_SUMMER_INCENSE, 1) && player->GetQuestStatus(QUEST_INCENSE_FOR_FESTIVAL_A) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_INCENSE_FOR_FESTIVAL_H) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, START_OPTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
	}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
		if (action == GOSSIP_ACTION_INFO_DEF+1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_festival_scorchling::npc_festival_scorchlingAI, creature->AI())->StartEvent(player);
		}
		return true;
	}

	bool OnQuestComplete(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_INCENSE_FOR_FESTIVAL_A || quest->GetQuestId() == QUEST_INCENSE_FOR_FESTIVAL_H)
            CAST_AI(npc_festival_scorchling::npc_festival_scorchlingAI, creature->AI())->StartEvent(player);

        return true;
	}

    struct npc_festival_scorchlingAI : public QuantumBasicAI
    {
        npc_festival_scorchlingAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
        }

        uint64 EventStarter;

        bool EventStarted;

        uint32 Part;

        uint32 PartTimer;

        void Reset()
        {
            EventStarter = 0;
            EventStarted = false;
            me->SetUInt32Value(UNIT_NPC_FLAGS, 3);
        }

        void MoveInLineOfSight(Unit* /*who*/){}

        void StartEvent(Player* Starter)
        {
            Starter->DestroyItemCount(ITEM_ID_SUMMER_INCENSE, 1, true);
            EventStarted = true;
            Part = 0;
            EventStarter = Starter->GetGUID();
            me->SetUInt32Value(UNIT_NPC_FLAGS, 0);

            PartTimer = 0;
        }

        void SayPart(uint32 Stage)
        {
            switch (Stage)
            {
			case 1:
                me->MonsterSay(SAY_PART_1, LANG_UNIVERSAL, EventStarter);
                PartTimer = 2000;
                break;
            case 2:
                me->MonsterTextEmote(SAY_PART_2, NULL, false);
                PartTimer = 1000;
                break;
            case 3:
                me->MonsterSay(SAY_PART_3, LANG_UNIVERSAL, 0);
                PartTimer = 2500;
                break;
            case 4:
                me->MonsterSay(SAY_PART_4, LANG_UNIVERSAL, 0);
                PartTimer = 2500;
                break;
            case 5:
                me->MonsterSay(SAY_PART_5, LANG_UNIVERSAL, 0);
                PartTimer = 2500;
                break;
            case 6:
                me->MonsterTextEmote(SAY_PART_6, NULL, false);
                PartTimer = 500;
                break;
            case 7:
                me->MonsterSay(SAY_PART_7, LANG_UNIVERSAL, 0);
                PartTimer = 2500;
                break;
            case 8:
                float x, y, z;
                me->GetPosition(x, y, z);
                DoSpawnCreature(NPC_RAGNAROS, 0, 0, 0, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 5000);
                me->MonsterYell(SAY_PART_8, LANG_UNIVERSAL, 0);
                PartTimer = 2500;
                break;
            case 9:
                me->MonsterYell(SAY_PART_9, LANG_UNIVERSAL, 0);
                PartTimer = 2500;
                break;
            case 10:
                me->MonsterTextEmote(SAY_PART_10, NULL, false);
                me->RemoveAurasDueToSpell(SPELL_GROW);
                PartTimer = 2000;
                break;
            case 11:
                me->MonsterSay(SAY_PART_11, LANG_UNIVERSAL, 0);

				if (Player* player = Unit::GetPlayer(*me, EventStarter))
				{
					if (player->GetQuestStatus(QUEST_INCENSE_FOR_FESTIVAL_A) == QUEST_STATUS_INCOMPLETE)
						player->CastSpell(player, SPELL_COMPLETE_QUEST_INCENSE_A, true);

					if (player->GetQuestStatus(QUEST_INCENSE_FOR_FESTIVAL_H) == QUEST_STATUS_INCOMPLETE)
						player->CastSpell(player, SPELL_COMPLETE_QUEST_INCENSE_H, true);
				}

                PartTimer = 9999000;
                return;
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (EventStarted)
            {
                if (PartTimer <= diff)
                {
					SayPart(Part);

					if (Part > 1 && Part < 8)
						DoCast(me, SPELL_GROW, true);

					Part++;

                    if (Part > 11)
                    {
						Reset();
						return;
                    }
                }
				else PartTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            me->AI()->EnterEvadeMode();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_festival_scorchlingAI(creature);
    }
};

#define SAY_DIALOG_1 "These stones should be the last of them. Our coordination with Neptulon's forces will be impeccable."
#define SAY_DIALOG_2 "Yes. The Tidehunter will be pleased at this development. The Firelord's hold will weaken."
#define SAY_DIALOG_3 "And your own preparations? Will the Frost Lord have a path to the portal?"
#define SAY_DIALOG_4 "Skar'this has informed us well. We have worked our way into the slave pens and await your cryomancerss."
#define SAY_DIALOG_5 "The ritual in Coilfang will bring Ahune through once he is fully prepared, and the resulting clash between Firelord and Frostlord will rend the foundations of this world. Our ultimate goals are in reach at last..."

enum QuestInnoncentDisguise
{
	SPELL_FROSTBOLT               = 9672,
	SPELL_FROST_ARMOR             = 18100,
	SPELL_CRAB_DISGUISE           = 46337,
	SPELL_QUEST_DISGUISE_COMPLETE = 46375,

	NPC_HERETIC_EMISSARY          = 25951,
	QUEST_INNCOENT_DISGUISE       = 11891,
};

class npc_ice_caller_briatha : public CreatureScript
{
public:
	npc_ice_caller_briatha() : CreatureScript("npc_ice_caller_briatha") {}

	struct npc_ice_caller_briathaAI : public QuantumBasicAI
	{
		npc_ice_caller_briathaAI(Creature* creature) : QuantumBasicAI(creature)
		{
			Reset();
		}

		uint32 Part;
		uint32 PartTimer;
		uint32 FrostboltTimer;

		uint64 EventStarter;

		bool EventStarted;

		void Reset()
		{
			EventStarter = 0;

			FrostboltTimer = 500;

			EventStarted = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!EventStarted)
			{
				if (EventStarted || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 20.0f)
					&& who->ToPlayer()->GetQuestStatus(QUEST_INNCOENT_DISGUISE) == QUEST_STATUS_COMPLETE
					|| who->ToPlayer()->GetQuestStatus(QUEST_INNCOENT_DISGUISE) == QUEST_STATUS_REWARDED)
					return;

				if (EventStarted == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 20.0f) && who->HasAura(SPELL_CRAB_DISGUISE)
					&& who->ToPlayer()->GetQuestStatus(QUEST_INNCOENT_DISGUISE) == QUEST_STATUS_INCOMPLETE)
					StartEvent(who->ToPlayer());
			}
		}

		void StartEvent(Player* Starter)
		{
			EventStarted = true;
			Part = 1;
			EventStarter = Starter->GetGUID();

			PartTimer = 0;
		}

		void SayPart(uint32 Stage)
		{
			switch (Stage)
			{
			    case 1:
					me->MonsterSay(SAY_DIALOG_1, LANG_UNIVERSAL, EventStarter);
					PartTimer = 5000;
					break;
				case 2:
					if (Creature* Emmissary = GetClosestCreatureWithEntry(me, NPC_HERETIC_EMISSARY, 30.0f))
						Emmissary->MonsterSay(SAY_DIALOG_2, LANG_UNIVERSAL, 0);

					PartTimer = 5000;
					break;
				case 3:
					me->MonsterSay(SAY_DIALOG_3, LANG_UNIVERSAL, 0);
					PartTimer = 5000;
					break;
				case 4:
					if (Creature* Emmissary = GetClosestCreatureWithEntry(me, NPC_HERETIC_EMISSARY, 30.0f))
						Emmissary->MonsterSay(SAY_DIALOG_4, LANG_UNIVERSAL, 0);

					PartTimer = 5000;
				break;
				case 5:
					me->MonsterSay(SAY_DIALOG_5, LANG_UNIVERSAL, 0);
					PartTimer = 10000;
					break;
				case 6:
					if (Player* player = Unit::GetPlayer(*me, EventStarter))
						player->CastSpell(player, SPELL_QUEST_DISGUISE_COMPLETE, true);

					PartTimer = 500;
					break;
			}
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(const uint32 diff)
		{
			// Out of Combat
			if (!me->HasAura(SPELL_FROST_ARMOR) && !me->IsInCombatActive())
				DoCast(me, SPELL_FROST_ARMOR);

			if (PartTimer <= diff && EventStarted)
			{
				SayPart(Part);
				Part++;

				if (Part > 6)
				{
					Reset();
					return;
				}
			}
			else PartTimer -= diff;

			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_FROST_ARMOR))
				DoCast(me, SPELL_FROST_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
	    return new npc_ice_caller_briathaAI(creature);
	}
};

void AddSC_world_season_events()
{
	new npc_elder_lunar_festival();
	new npc_lunar_festival_harbinger();
	new npc_omen();
	new npc_giant_spotlight();
	new npc_minion_of_omen();
	new boss_prince_tenris_mirkblood();
	new npc_self_turning_and_oscillating_utility_target();
	new npc_wild_wolpertinger();
	new npc_halloween_orphan_matron();
	new npc_halloween_fire();
	new npc_shade_horseman();
	new item_water_bucket();
	new npc_winter_reveler();
	new npc_greatfather_winter();
	new npc_human_commoner();
	new npc_draenei_commoner();
	new npc_gnome_commoner();
	new npc_night_elf_commoner();
	new npc_orc_commoner();
	new npc_tauren_commoner();
	new npc_troll_commoner();
	new npc_dwarf_commoner();
	new npc_blood_elf_commoner();
	new npc_forsaken_commoner();
	new npc_goblin_commoner();
	new npc_torch_tossing_bunny();
	new npc_master_flame_eater();
	new npc_midsummer_fireeater();
	new npc_festival_scorchling();
	new npc_ice_caller_briatha();
}