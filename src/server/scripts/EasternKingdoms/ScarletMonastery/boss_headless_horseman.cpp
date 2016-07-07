/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "LFGMgr.h"
#include "QuantumCreature.h"
#include "QuantumSystemText.h"
#include "scarlet_monastery.h"

enum Texts
{
	SAY_SLAY            = -1595048,
	SAY_AGGRO           = -1595049,
	SAY_FAIL            = -1595050,
	SAY_CONFLAGRATION_1 = -1595051,
	SAY_CONFLAGRATION_2 = -1595052,
	SAY_PUMPKINS        = -1595053,
	SAY_DEATH           = -1595054,
	SAY_HEAD_OVER_HERE  = -1595055,
	EMOTE_LAUGHS        = -1595056,
};

enum Sounds
{
	SOUND_LOST_HEAD = 12568,
	SOUND_PHASE     = 12567,
};

enum Spells
{
    SPELL_SUMMON_PUMPKIN          = 42394,
    SPELL_IMMUNED                 = 42556,
    SPELL_BODY_REGEN              = 42403,
    SPELL_CONFUSED                = 43105,
    SPELL_HEAL_BODY               = 43306,
    SPELL_CLEAVE                  = 42587,
    SPELL_WHIRLWIND               = 43116,
    SPELL_CONFLAGRATION           = 42380,
	SPELL_BODY_FLAME              = 42074,
	SPELL_HEAD_FLAME              = 42971,
    SPELL_FLYING_HEAD             = 42399,
    SPELL_HEAD_VISUAL             = 42413,
    SPELL_HEAD_LANDS              = 42400,
	SPELL_HEAD_IS_DEAD            = 42428, 
    SPELL_RHYME_BIG               = 42909,
	// Summons
	SPELL_SUMMON_PULSED_PUMPKIN   = 42277,
	SPELL_PUMPKIN_AURA            = 42280,
	SPELL_PUMPKIN_AURA_GREEN      = 42294,
	SPELL_PUMPKIN_HEAD_VISUAL     = 44016,
	SPELL_PUMPKIN_BLAST           = 50066,
	SPELL_SPROUTING               = 42281,
	SPELL_SPROUT_BODY             = 42285,
	SPELL_SQUASH_SOUL             = 42514,
	SPELL_THRASH                  = 3417,
	//SPELL_CREATE_PUMPKIN_TREATS   = 42754,
};

enum Actions
{
    ACTION_SWITCH_PHASE = 1,
};

static Position FlightPos[] =
{
    {1754.00f, 1346.00f, 17.50f},
    {1765.00f, 1347.00f, 19.00f},
    {1784.00f, 1346.80f, 25.40f},
    {1803.30f, 1347.60f, 33.00f},
    {1824.00f, 1350.00f, 42.60f},
    {1838.80f, 1353.20f, 49.80f},
    {1852.00f, 1357.60f, 55.70f},
    {1861.30f, 1364.00f, 59.40f},
    {1866.30f, 1374.80f, 61.70f},
    {1864.00f, 1387.30f, 63.20f},
    {1854.80f, 1399.40f, 64.10f},
    {1844.00f, 1406.90f, 64.10f},
    {1824.30f, 1411.40f, 63.30f},
    {1801.00f, 1412.30f, 60.40f},
    {1782.00f, 1410.10f, 55.50f},
    {1770.50f, 1405.20f, 50.30f},
    {1765.20f, 1400.70f, 46.60f},
    {1761.40f, 1393.40f, 41.70f},
    {1759.10f, 1386.70f, 36.60f},
    {1757.80f, 1378.20f, 29.00f},
    {1758.00f, 1367.00f, 19.51f},
};

static char const* Text[] =
{
	"Rise, rider...",
    "Your time has come...",
    "Tonight is your life...",
    "For ever taken!"
};

class boss_headless_horseman : public CreatureScript
{
    public:
        boss_headless_horseman() : CreatureScript("boss_headless_horseman") { }

        struct boss_headless_horsemanAI : public QuantumBasicAI
        {
            boss_headless_horsemanAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

			bool WpReached;
			bool Intro;

			SummonList Summons;

            uint8 Phase;
            uint8 WpCount;
            uint8 IntroCount;

            uint32 IntroTimer;
            uint32 LaughTimer;
            uint32 CleaveTimer;
            uint32 SummonTimer;
            uint32 ConflagrationTimer;

            void Reset()
            {
				IntroTimer = 1*IN_MILLISECONDS;
                LaughTimer = 3*IN_MILLISECONDS;
                CleaveTimer = 5*IN_MILLISECONDS;
				SummonTimer = 1*IN_MILLISECONDS;
                ConflagrationTimer = 8*IN_MILLISECONDS;

				WpCount = 0;
                IntroCount = 0;
                WpReached = false;
				Intro = false;
                Phase = 0;

                Summons.DespawnAll();

				me->SetCorpseDelay(75);
                me->SetVisible(false);

				me->SummonGameObject(GO_LOOSELY_TURNED_SOUL, 1766.65f, 1349.3f, 17.55f, 0.04f, 0, 0, 0, 0, 1*DAY);

				// Flags
				me->SetReactState(REACT_PASSIVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
				me->SetCanFly(true);
				me->SetSpeed(MOVE_FLIGHT, 1.5f, true);

				DoCast(me, SPELL_HEAD_VISUAL, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE || id != WpCount)
                    return;

				if (id == 1)
					DoSendQuantumText(SAY_AGGRO, me);

                if (id < 20)
                {
                    ++WpCount;
                    WpReached = true;
                }
                else // Srart attacking
                {
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
					me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
					me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_DOWN);
					me->SetCanFly(false);

                    DoZoneInCombat(me, 150.0f);

                    if (me->GetVictim())
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                }
            }

			void EnterToBattle(Unit* /*who*/){}

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_SLAY, me);
            }

			void EnterEvadeMode()
			{
				QuantumBasicAI::EnterEvadeMode();
				DoSendQuantumText(SAY_FAIL, me);
			}

			void JustDied(Unit* /*killer*/)
            {
				DoSendQuantumText(SAY_DEATH, me);

                Summons.DespawnAll();

                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
					Map::PlayerList const& players = me->GetMap()->GetPlayers();
					if (!players.isEmpty())
					{
						for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
						{
							if (Player* player = i->getSource())
							{
								if (player->IsAtGroupRewardDistance(me))
									sLFGMgr->RewardDungeonDoneFor(285, player);
							}
						}
					}
                }

                DoCast(me, SPELL_HEAD_FLAME, true);
				DoCast(me, SPELL_BODY_FLAME, true);
                me->SummonCreature(NPC_SIR_THOMAS, 1762.863f, 1345.217f, 17.9f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1*HOUR*IN_MILLISECONDS);
            }

			void JustSummoned(Creature* summon)
			{
				switch (summon->GetEntry())
				{
                    case NPC_PUMPKIN_FIEND:
					case NPC_PULSING_PUMPKIN:
						Summons.Summon(summon);
						Summons.DoZoneInCombat();
						break;
					default:
						break;
				}
			}

			void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (Phase > 3)
                {
                    me->RemoveAllAuras();
                    return;
                }

                if (me->HasAura(SPELL_BODY_REGEN))
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth())
                {
                    damage = me->GetHealth() - 1;

                    DoCast(me, SPELL_IMMUNED, true);
                    DoCast(me, SPELL_BODY_REGEN, true);
                    DoCast(me, SPELL_CONFUSED, true);

                    Position randomPos;
                    me->GetRandomNearPosition(randomPos, 20.0f);

                    if (Creature* head = me->SummonCreature(NPC_HEAD_OF_HORSEMAN, randomPos))
                    {
                        head->AI()->SetData(0, Phase);

                        switch (Phase)
                        {
                            case 2:
								head->SetHealth(uint32(head->GetMaxHealth() * 2 / 3));
								break;
                            case 3:
								head->SetHealth(uint32(head->GetMaxHealth() / 3));
								break;
                        }
                    }

                    me->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
                }
            }

			void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_SWITCH_PHASE:
						DoPlaySoundToSet(me, SOUND_PHASE);
                        me->RemoveAllAuras();
                        DoCast(me, SPELL_HEAL_BODY, true);
                        DoCast(me, SPELL_HEAD_VISUAL, true);

                        ++Phase;
                        if (Phase > 3)
                            me->DealDamage(me, me->GetHealth());
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
				if (!UpdateVictim())
					return;

                if (Phase == 0)
                {
                    if (IntroTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100.0f, true))
                        {
                            if (IntroCount < 4)
                                target->ToPlayer()->Say(Text[IntroCount], 0);
                            else
                            {
                                DoCast(me, SPELL_RHYME_BIG, true);
                                target->ToPlayer()->Say(Text[IntroCount], 0);
                                target->HandleEmoteCommand(ANIM_EMOTE_SHOUT);
                                Phase = 1;
                                WpReached = true;
                                me->SetVisible(true);
                            }
                        }
                        IntroTimer = 3*IN_MILLISECONDS;
                        ++IntroCount;
                    }
                    else IntroTimer -= diff;
                    return;
                }

                if (WpReached)
                {
                    WpReached = false;
                    me->GetMotionMaster()->MovePoint(WpCount, FlightPos[WpCount]);
                }

                if (me->HasAura(SPELL_BODY_REGEN))
                {
                    if (me->IsFullHealth())
                    {
                        me->RemoveAllAuras();
                        DoCast(me, SPELL_HEAD_VISUAL, true);

                        if (Creature* head = me->FindCreatureWithDistance(NPC_HEAD_OF_HORSEMAN, 250.0f, true))
                        {
                            head->SetFullHealth();
                            head->RemoveAllAuras();
                            head->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            head->DespawnAfterAction(3*IN_MILLISECONDS);
                            head->CastSpell(me, SPELL_FLYING_HEAD, true);
                        }
                    }
                    else
						if (!me->HasAura(SPELL_WHIRLWIND) && me->GetHealthPct() > 10.0f)
							DoCast(me, SPELL_WHIRLWIND, true);

                    return;
                }

                if (LaughTimer <= diff)
                {
                    DoSendQuantumText(EMOTE_LAUGHS, me);
					me->PlayDirectSound(RAND(11965, 11975, 11976));
                    LaughTimer = 11000*IN_MILLISECONDS;
                }
                else LaughTimer -= diff;

                if (me->HasReactState(REACT_PASSIVE))
                    return;

                if (CleaveTimer <= diff)
                {
                    DoCastVictim(SPELL_CLEAVE);
                    CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
                }
                else CleaveTimer -= diff;

				if (Phase == 2)
				{
					if (ConflagrationTimer <= diff)
					{
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoSendQuantumText(RAND(SAY_CONFLAGRATION_1, SAY_CONFLAGRATION_2), me);
							DoCast(target, SPELL_CONFLAGRATION);
							ConflagrationTimer = 8*IN_MILLISECONDS;
						}
					}
					else ConflagrationTimer -= diff;
				}

				if (Phase == 3)
				{
					if (SummonTimer <= diff)
					{
						// Say
						DoSendQuantumText(SAY_PUMPKINS, me);
						// Visual
						DoCast(me, SPELL_SUMMON_PUMPKIN, true);
						// Pulsed
						DoCast(me, SPELL_SUMMON_PULSED_PUMPKIN, true);
						DoCast(me, SPELL_SUMMON_PULSED_PUMPKIN, true);
						DoCast(me, SPELL_SUMMON_PULSED_PUMPKIN, true);
						DoCast(me, SPELL_SUMMON_PULSED_PUMPKIN, true);
						// Fiend
						me->SummonCreature(NPC_PUMPKIN_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
						me->SummonCreature(NPC_PUMPKIN_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
						me->SummonCreature(NPC_PUMPKIN_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
						me->SummonCreature(NPC_PUMPKIN_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
						SummonTimer = 20*IN_MILLISECONDS;
					}
					else SummonTimer -= diff;
				}

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_headless_horsemanAI(creature);
        }
};

class npc_head_of_horseman : public CreatureScript
{
    public:
        npc_head_of_horseman() : CreatureScript("npc_head_of_horseman") { }

        struct npc_horseman_headAI : public QuantumBasicAI
        {
            npc_horseman_headAI(Creature* creature) : QuantumBasicAI(creature)
			{
				me->SetDisplayId(21908);
				DoSendQuantumText(SAY_HEAD_OVER_HERE, me);
				DoCast(me, SPELL_HEAD_VISUAL, true);
                DoCast(me, SPELL_HEAD_LANDS, true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
				me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MoveRandom(20.0f);
                Despawn = false;
			}

			uint8 Phase;

            bool Despawn;

			void Reset()
			{
				me->PlayDirectSound(SOUND_LOST_HEAD);
			}

            void SetData(uint32 /*type*/, uint32 data)
            {
                Phase = data;
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (Despawn)
                {
                    damage = 0;
                    return;
                }

                int32 healthPct;

                switch (Phase)
                {
                    case 1:
						healthPct = 66;
						break;
                    case 2:
						healthPct = 33;
						break;
                    default:
						healthPct = 1;
						break;
                }

                if (me->HealthBelowPctDamaged(healthPct, damage) || damage >= me->GetHealth())
                {
                    damage = 0;

                    me->RemoveAllAuras();
					DoCast(me, SPELL_HEAD_IS_DEAD, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->DespawnAfterAction(2*IN_MILLISECONDS);

                    Despawn = true;

                    if (me->ToTempSummon())
					{
                        if (Unit* horseman = me->ToTempSummon()->GetSummoner())
                        {
                            if (Phase < 3)
                                DoCast(horseman, SPELL_FLYING_HEAD, true);

                            horseman->ToCreature()->AI()->DoAction(ACTION_SWITCH_PHASE);
                        }
					}
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_horseman_headAI(creature);
        }
};

class npc_pumpkin_fiend : public CreatureScript
{
public:
	npc_pumpkin_fiend() : CreatureScript("npc_pumpkin_fiend") {}

    struct npc_pumpkin_fiendAI : public QuantumBasicAI
    {
        npc_pumpkin_fiendAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SquashSoulTimer;

		void Reset()
        {
			SquashSoulTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_PUMPKIN_AURA_GREEN, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SquashSoulTimer <= diff)
			{
				DoCastVictim(SPELL_SQUASH_SOUL);
				SquashSoulTimer = 4*IN_MILLISECONDS;
			}
			else SquashSoulTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pumpkin_fiendAI(creature);
    }
};

class npc_pulsing_pumpkin : public CreatureScript
{
public:
	npc_pulsing_pumpkin() : CreatureScript("npc_pulsing_pumpkin") {}

    struct npc_pulsing_pumpkinAI : public QuantumBasicAI
    {
        npc_pulsing_pumpkinAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SquashSoulTimer;

		bool Sprouted;

		void Reset()
        {
			SquashSoulTimer = 2*IN_MILLISECONDS;

			Sprouted = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_PUMPKIN_HEAD_VISUAL, true);
			DoCast(me, SPELL_SPROUTING, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

		void DamageTaken(Unit* /*doneBy*/, uint32 &amount)
        {
            if (amount >= me->GetHealth())
            {
                amount = 0;

				//DoCast(me, SPELL_SPROUT_BODY, true);
				DoCast(me, SPELL_PUMPKIN_AURA, true);
				me->RemoveAurasDueToSpell(SPELL_PUMPKIN_HEAD_VISUAL);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
				Sprouted = true;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SquashSoulTimer <= diff)
			{
				DoCastVictim(SPELL_SQUASH_SOUL);
				SquashSoulTimer = 4*IN_MILLISECONDS;
			}
			else SquashSoulTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pulsing_pumpkinAI(creature);
    }
};

class npc_pumpkin_soldier : public CreatureScript
{
public:
	npc_pumpkin_soldier() : CreatureScript("npc_pumpkin_soldier") {}

    struct npc_pumpkin_soldierAI : public QuantumBasicAI
    {
        npc_pumpkin_soldierAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PumpkinBlastTimer;

		void Reset()
        {
			PumpkinBlastTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PumpkinBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PUMPKIN_BLAST);
					PumpkinBlastTimer = 3*IN_MILLISECONDS;
				}
			}
			else PumpkinBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pumpkin_soldierAI(creature);
    }
};

class go_loosely_turned_soil : public GameObjectScript
{
public:
	go_loosely_turned_soil() : GameObjectScript("go_loosely_turned_soil") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (go->GetGoType() == GAMEOBJECT_TYPE_QUEST_GIVER)
		{
			player->PrepareQuestMenu(go->GetGUID());
			player->SendPreparedQuest(go->GetGUID());
		}

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_CALL_HEADLESS_HORSEMAN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
		{
			if (Unit* horseman = go->GetOwner())
			{
				horseman->ToCreature()->AI()->AttackStart(player);
				horseman->AddThreat(player, 1.0f);
				go->Delete();
			}
		}
		return true;
	}
};  

void AddSC_boss_headless_horseman()
{
    new boss_headless_horseman();
    new npc_head_of_horseman();
	new npc_pumpkin_fiend();
	new npc_pulsing_pumpkin();
	new npc_pumpkin_soldier();
    new go_loosely_turned_soil();
}