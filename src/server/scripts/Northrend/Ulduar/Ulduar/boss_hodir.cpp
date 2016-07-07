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
#include "SpellScript.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO                      = -1603210,
    SAY_SLAY_1                     = -1603211,
    SAY_SLAY_2                     = -1603212,
    SAY_FLASH_FREEZE               = -1603213,
    SAY_STALACTITE                 = -1603214,
    SAY_DEATH                      = -1603215,
    SAY_BERSERK                    = -1603216,
    SAY_HARD_MODE_MISSED           = -1603218,
	EMOTE_FLASH_FREEZE             = -1613219,
	EMOTE_FROZEN_BLOWS             = -1613220,
};

enum Spells
{
    SPELL_FROZEN_BLOWS                          = 62478,
    SPELL_FLASH_FREEZE                          = 61968,
    SPELL_FLASH_FREEZE_VISUAL                   = 62148,
    SPELL_BITING_COLD                           = 62038,
    SPELL_BITING_COLD_TRIGGERED                 = 62039,
    SPELL_BITING_COLD_DAMAGE                    = 62188,
    SPELL_FREEZE                                = 62469,
    SPELL_ICICLE                                = 62234,
    SPELL_ICICLE_SNOWDRIFT                      = 62462,
    SPELL_BLOCK_OF_ICE                          = 61969,
    SPELL_BLOCK_OF_ICE_NPC                      = 61990,
    SPELL_FROZEN_KILL                           = 62226,
    SPELL_ICICLE_FALL                           = 62453, // 69428,
    SPELL_FALL_DAMAGE                           = 62236,
    SPELL_FALL_SNOWDRIFT                        = 62460,
	SPELL_CREDIT_MARKER                         = 64899,
    SPELL_BERSERK                               = 47008,
    SPELL_WRATH                                 = 62793,
    SPELL_STARLIGHT                             = 62807,
    SPELL_LAVA_BURST                            = 61924,
    SPELL_STORM_CLOUD_10                        = 65123,
	SPELL_STORM_CLOUD_25                        = 65133,
    SPELL_FIREBALL                              = 61909,
    SPELL_CONJURE_FIRE                          = 62823,
    SPELL_MELT_ICE                              = 64528,
    SPELL_SINGED                                = 62821,
	SPELL_SIGNED_PROC                           = 65280,
    SPELL_SMITE                                 = 61923,
    SPELL_GREATER_HEAL                          = 62809,
    SPELL_DISPEL_MAGIC                          = 63499,
	SPELL_ICE_SHARDS_10                         = 62457,
	SPELL_ICE_SHARDS_25                         = 65370,
	SPELL_SNOW_MOUND                            = 64615,
};

enum Actions
{
	ACTION_FAILED_COOLEST_FRIENDS = 1,
};

enum DataCount
{
	MODE_NPC_HELPER_COUNT_10 = 4,
	MODE_NPC_HELPER_COUNT_25 = 8,
};

enum Events
{
    EVENT_NONE         = 1,
    EVENT_FREEZE       = 2,
    EVENT_FLASH_CAST   = 3,
    EVENT_FLASH_EFFECT = 4,
    EVENT_ICICLE       = 5,
    EVENT_BLOWS        = 6,
    EVENT_RARE_CACHE   = 7,
    EVENT_BERSERK      = 8,
};

struct SummonLocation
{
    float x, y, z, o;
    uint32 entry;
};

SummonLocation addLocations[] =
{
    {1983.75f, -243.36f, 432.767f, 1.57f, 32897}, // Priest 1
    {1999.90f, -230.49f, 432.767f, 1.57f, 33325}, // Druid 1
    {2010.06f, -243.45f, 432.767f, 1.57f, 33328}, // Shaman 1
    {2021.12f, -236.65f, 432.767f, 1.57f, 32893}, // Mage 1
    {2028.10f, -244.66f, 432.767f, 1.57f, 33326}, // Priest 2
    {2014.18f, -232.80f, 432.767f, 1.57f, 32901}, // Druid 2
    {1992.90f, -237.54f, 432.767f, 1.57f, 32900}, // Shaman 2
    {1976.60f, -233.53f, 432.767f, 1.57f, 33327}, // Mage 2
};

class boss_hodir : public CreatureScript
{
    public:
        boss_hodir() : CreatureScript("boss_hodir") { }

        struct boss_hodirAI : public BossAI
        {
            boss_hodirAI(Creature* creature) : BossAI(creature, DATA_HODIR)
            {
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SIGNED_PROC, true);
            }

			uint32 CheckIntenseColdTimer;

            bool MoreThanTwoIntenseCold;
            bool CheeseTheFreeze;
            bool CoolestFriends;
            bool RareCache;

			void Reset()
            {
                _Reset();
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                me->SetReactState(REACT_PASSIVE);
				SpawnHelpers();
			}

			void QuestReward()
			{
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (Is25ManRaid())
						{
							if (player->GetQuestStatus(QUEST_CONQUEROR_OF_ULDUAR_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_CONQUEROR_OF_ULDUAR_H) == QUEST_STATUS_INCOMPLETE)
								player->AddItem(ITEM_ID_SHARD_OF_HODIR, 1);
						}
					}
				}
			}

			void SpawnHelpers()
			{
				// Spawn NPC Helpers
                for (uint8 i = 0; i < RAID_MODE<uint8>(MODE_NPC_HELPER_COUNT_10, MODE_NPC_HELPER_COUNT_25); ++i)
                {
                    if (Creature* helper = me->SummonCreature(addLocations[i].entry, addLocations[i].x, addLocations[i].y, addLocations[i].z, addLocations[i].o))
					{
                        if (Creature* iceBlock = helper->SummonCreature(NPC_FLASH_FREEZE_PRE, addLocations[i].x, addLocations[i].y, addLocations[i].z, addLocations[i].o))
                            helper->AddThreat(me, 5000000.0f);
					}
                }
            }

			void FlashFreeze()
            {
                DoZoneInCombat();
                std::list<HostileReference*> ThreatList = me->getThreatManager().getThreatList();
                for (std::list<HostileReference*>::const_iterator itr = ThreatList.begin(); itr != ThreatList.end(); ++itr)
                {
                    if (Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                    {
                        if (!target->ToPlayer())
                            continue;

                        if (target->HasAura(SPELL_BLOCK_OF_ICE))
                        {
                            DoCast(target, SPELL_FROZEN_KILL);
                            continue;
                        }
                        else
                        {
                            if (GetClosestCreatureWithEntry(target, NPC_ICICLE_TARGET, 5.0f))
                                continue;
                            else if (Creature* iceBlock = target->SummonCreature(NPC_FLASH_FREEZE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 105000))
                            {
                                if (target->GetTypeId() == TYPE_ID_PLAYER)
                                    CheeseTheFreeze = false;
                            }
                        }
                    }
                }
            }

			void EnterToBattle(Unit* who)
            {
                _EnterToBattle();

				if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_AGGRO, me);

                me->SetReactState(REACT_AGGRESSIVE);

                DoCast(me, SPELL_BITING_COLD, false);

                events.ScheduleEvent(EVENT_ICICLE, 2000);
                events.ScheduleEvent(EVENT_FREEZE, 25000);
                events.ScheduleEvent(EVENT_BLOWS, urand(60000, 65000));
                events.ScheduleEvent(EVENT_FLASH_CAST, 50000);
                events.ScheduleEvent(EVENT_RARE_CACHE, 180000);
                events.ScheduleEvent(EVENT_BERSERK, 480000);
                CheckIntenseColdTimer = 2000;

                MoreThanTwoIntenseCold = false;
                CheeseTheFreeze = true;
                CoolestFriends = true;
                RareCache = true;
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_FAILED_COOLEST_FRIENDS:
                        CoolestFriends = false;
                        break;
                }
            }

			void DamageTaken(Unit* /*who*/, uint32& damage)
            {
				if (damage >= me->GetHealth())
				{
					damage = 0;
					//QuestReward();
					DoSendQuantumText(SAY_DEATH, me);
					me->SetCurrentFaction(FACTION_FRIENDLY);
					me->DespawnAfterAction(10*IN_MILLISECONDS);
					EnterEvadeMode();
					me->RemoveAllAuras();
					me->RemoveAllAttackers();
					me->AttackStop();
					me->CombatStop(true);

					if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 500.0f, true))
						trigger->SummonCreature(NPC_UNFLUENCE_TENTACLE, 2000.74f, -221.688f, 432.687f, 4.69535f, TEMPSUMMON_TIMED_DESPAWN, 35000);

					_JustDied();

					if (instance)
					{
						// Kill credit
						instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET_2, SPELL_CREDIT_MARKER);
						// Getting Cold in Here
						if (!MoreThanTwoIntenseCold)
							instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_COLD_IN_HERE_10, ACHIEVEMENT_COLD_IN_HERE_25));
						// Cheese the Freeze
						if (CheeseTheFreeze)
							instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_CHEESE_THE_FREEZE_10, ACHIEVEMENT_CHEESE_THE_FREEZE_25));
						// I Have the Coolest Friends
						if (CoolestFriends)
							instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_COOLEST_FRIENDS_10, ACHIEVEMENT_COOLEST_FRIENDS_25));
						// I Could Say That This Cache Was Rare
						if (RareCache)
						{
							instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_THIS_CACHE_WAS_RARE_10, ACHIEVEMENT_THIS_CACHE_WAS_RARE_25));
							instance->SetData(DATA_HODIR_RARE_CHEST, GO_STATE_READY);
						}
						me->SummonGameObject(RAID_MODE(GO_RARE_CACHE_OF_WINTER_10, GO_RARE_CACHE_OF_WINTER_25), 1966.43f, -203.906f, 432.687f, -0.91f, 0, 0, 1, 1, 604800);
					}
				}
            }

			void EnterEvadeMode()
			{
				_EnterEvadeMode();

				me->GetMotionMaster()->MoveTargetedHome();
				Reset();
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetVictim() && !me->GetVictim()->GetCharmerOrOwnerPlayerOrPlayerItself())
                    me->Kill(me->GetVictim());

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckIntenseColdTimer <= diff && !MoreThanTwoIntenseCold)
                {
                    std::list<HostileReference*> ThreatList = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = ThreatList.begin(); itr != ThreatList.end(); ++itr)
                    {
                        Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        if (!target || target->GetTypeId() != TYPE_ID_PLAYER)
                            continue;

                        Aura* intenseCold = target->GetAura(SPELL_BITING_COLD_TRIGGERED);
                        if (intenseCold && intenseCold->GetStackAmount() > 2)
                        {
							MoreThanTwoIntenseCold = true;
							break;
                        }
                    }
                    CheckIntenseColdTimer = 2000;
                }
                else CheckIntenseColdTimer -= diff;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FREEZE:
                            DoCastAOE(SPELL_FREEZE);
                            events.ScheduleEvent(EVENT_FREEZE, urand(30000, 35000));
                            break;
                        case EVENT_ICICLE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
                                DoCast(target, SPELL_ICICLE);
							}
                            events.ScheduleEvent(EVENT_ICICLE, 2000);
                            break;
                        case EVENT_FLASH_CAST:
                            DoSendQuantumText(SAY_FLASH_FREEZE, me);
                            DoSendQuantumText(EMOTE_FLASH_FREEZE, me);
                            for (uint8 i = 0; i < RAID_MODE(2, 3); ++i)
                            {
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                                    target->CastSpell(target, SPELL_ICICLE_SNOWDRIFT, true);
                            }
                            DoCast(SPELL_FLASH_FREEZE);
                            events.RescheduleEvent(EVENT_ICICLE, 15000);
                            events.ScheduleEvent(EVENT_FLASH_CAST, 50000);
                            events.ScheduleEvent(EVENT_FLASH_EFFECT, 9000);
                            break;
                        case EVENT_FLASH_EFFECT:
                            DoCast(SPELL_FLASH_FREEZE_VISUAL);
                            FlashFreeze();
                            events.CancelEvent(EVENT_FLASH_EFFECT);
                            break;
                        case EVENT_BLOWS:
                            DoSendQuantumText(SAY_STALACTITE, me);
                            DoSendQuantumText(EMOTE_FROZEN_BLOWS, me);
                            DoCast(me, SPELL_FROZEN_BLOWS);
                            events.ScheduleEvent(EVENT_BLOWS, urand(60000, 65000));
                            break;
                        case EVENT_RARE_CACHE:
                            DoSendQuantumText(SAY_HARD_MODE_MISSED, me);
                            RareCache = false;
                            events.CancelEvent(EVENT_RARE_CACHE);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            DoSendQuantumText(SAY_BERSERK, me);
                            events.CancelEvent(EVENT_BERSERK);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_hodirAI(creature);
        }
};

class npc_icicle : public CreatureScript
{
    public:
        npc_icicle() : CreatureScript("npc_icicle") { }

        struct npc_icicleAI : public QuantumBasicAI
        {
            npc_icicleAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(28470);
            }

			uint32 IcicleTimer;

            void Reset()
            {
                IcicleTimer = 2000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (IcicleTimer <= diff)
                {
                    DoCast(me, SPELL_FALL_DAMAGE);
                    DoCast(me, SPELL_ICICLE_FALL);
                    IcicleTimer = 10000;
                }
                else IcicleTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_icicleAI(creature);
        }
};

class npc_icicle_snowdrift : public CreatureScript
{
    public:
        npc_icicle_snowdrift() : CreatureScript("npc_icicle_snowdrift") { }

        struct npc_icicle_snowdriftAI : public QuantumBasicAI
        {
            npc_icicle_snowdriftAI(Creature* creature) : QuantumBasicAI(creature)
			{
				SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(28470);
            }

			uint32 IcicleTimer;

            void Reset()
            {
                IcicleTimer = 2000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (IcicleTimer <= diff)
                {
                    DoCast(me, SPELL_FALL_SNOWDRIFT);
                    DoCast(me, SPELL_ICICLE_FALL);
                    IcicleTimer = 10000;
                }
                else IcicleTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_icicle_snowdriftAI(creature);
        }
};

class npc_snowpacked_icicle : public CreatureScript
{
    public:
        npc_snowpacked_icicle() : CreatureScript("npc_snowpacked_icicle") { }

        struct npc_snowpacked_icicleAI : public QuantumBasicAI
        {
            npc_snowpacked_icicleAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(15880);
            }

			uint32 DespawnTimer;

            void Reset()
            {
                DespawnTimer = 12000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (DespawnTimer <= diff)
                {
                    if (GameObject* snowdrift = me->FindGameobjectWithDistance(GO_SNOWDRIFT, 2.0f))
                        me->RemoveGameObject(snowdrift, true);

                    me->DespawnAfterAction();
                }
                else DespawnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_snowpacked_icicleAI(creature);
        }
};

class npc_hodir_priest : public CreatureScript
{
    public:
        npc_hodir_priest() : CreatureScript("npc_hodir_priest") { }

        struct npc_hodir_priestAI : public QuantumBasicAI
        {
            npc_hodir_priestAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCurrentFaction(FACTION_NONE);
            }

			InstanceScript* instance;

            uint32 HealTimer;
            uint32 DispelTimer;

            void Reset()
            {
                HealTimer = urand(4000, 8000);
                DispelTimer = urand(20000, 30000);
            }

            void AttackStart(Unit* who)
            {
                AttackStartCaster(who, 20.0f);
            }

			void JustDied(Unit* /*killer*/)
            {
                if (Creature* hodir = me->FindCreatureWithDistance(NPC_HODIR, 250.0f, true))
                    hodir->AI()->DoAction(ACTION_FAILED_COOLEST_FRIENDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                if (HealthBelowPct(35))
                    DoCastAOE(SPELL_GREATER_HEAL, true);

                if (HealTimer <= diff)
                {
                    DoCastAOE(SPELL_GREATER_HEAL, true);
                    HealTimer = urand(12000, 14000);
                }
                else HealTimer -= diff;

                if (DispelTimer <= diff)
                {
                    std::list<Player*> players;
                    Trinity::AnyPlayerInObjectRangeCheck checker(me, 30.0f);
                    Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
                    me->VisitNearbyWorldObject(30.0f, searcher);
                    if (!players.empty())
                    {
                        for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
                        {
                            if ((*itr)->HasAura(SPELL_FREEZE))
                            {
                                DoCast((*itr), SPELL_DISPEL_MAGIC, true);
                                DispelTimer = urand(25000, 30000);
                                return;
                            }
                        }
                    }
                    DispelTimer = 5000;
                }
                else DispelTimer -= diff;

                DoSpellAttackIfReady(SPELL_SMITE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hodir_priestAI(creature);
        }
};

class npc_hodir_shaman : public CreatureScript
{
    public:
        npc_hodir_shaman() : CreatureScript("npc_hodir_shaman") { }

        struct npc_hodir_shamanAI : public QuantumBasicAI
        {
            npc_hodir_shamanAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCurrentFaction(FACTION_NONE);
            }

			InstanceScript* instance;

            uint32 StormTimer;

            void Reset()
            {
                StormTimer = urand(15000, 20000);
            }

            void AttackStart(Unit* who)
            {
                AttackStartCaster(who, 20.0f);
            }

			void JustDied(Unit* /*killer*/)
            {
                if (Creature* hodir = me->FindCreatureWithDistance(NPC_HODIR, 250.0f, true))
					hodir->AI()->DoAction(ACTION_FAILED_COOLEST_FRIENDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                if (StormTimer <= diff)
                {
                    std::list<Player*> players;
                    Trinity::AnyPlayerInObjectRangeCheck checker(me, 30.0f);
                    Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
                    me->VisitNearbyWorldObject(30.0f, searcher);
                    if (!players.empty())
                    {
                        std::list<Player*>::iterator itr = players.begin();
						DoCast((*itr), RAID_MODE(SPELL_STORM_CLOUD_10, SPELL_STORM_CLOUD_25));
                    }
                    StormTimer = urand(15000, 20000);
                }
                else StormTimer -= diff;

                DoSpellAttackIfReady(SPELL_LAVA_BURST);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hodir_shamanAI(creature);
        }
};

class npc_hodir_druid : public CreatureScript
{
    public:
        npc_hodir_druid() : CreatureScript("npc_hodir_druid") { }

        struct npc_hodir_druidAI : public QuantumBasicAI
        {
            npc_hodir_druidAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCurrentFaction(FACTION_NONE);
            }

			InstanceScript* instance;

            uint32 StarlightTimer;

            void Reset()
            {
                StarlightTimer = urand(10000, 15000);
            }

            void AttackStart(Unit* who)
            {
                AttackStartCaster(who, 20.0f);
            }

			void JustDied(Unit* /*killer*/)
            {
                if (Creature* hodir = me->FindCreatureWithDistance(NPC_HODIR, 250.0f, true))
                    hodir->AI()->DoAction(ACTION_FAILED_COOLEST_FRIENDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                if (StarlightTimer <= diff)
                {
                    DoCast(me, SPELL_STARLIGHT, true);
                    StarlightTimer = urand(20000, 25000);
                }
                else
                    StarlightTimer -= diff;

                DoSpellAttackIfReady(SPELL_WRATH);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hodir_druidAI(creature);
        }
};

class npc_hodir_mage : public CreatureScript
{
    public:
        npc_hodir_mage() : CreatureScript("npc_hodir_mage") { }

        struct npc_hodir_mageAI : public QuantumBasicAI
        {
            npc_hodir_mageAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

                me->SetCurrentFaction(FACTION_NONE);
            }

			InstanceScript* instance;

            uint32 FireTimer;
            uint32 MeltIceTimer;

            void Reset()
            {
                FireTimer = urand(15000, 20000);
                MeltIceTimer = 5000;
            }

            void AttackStart(Unit* who)
            {
                AttackStartCaster(who, 20.0f);
            }

			void JustDied(Unit* /*killer*/)
            {
                if (Creature* hodir = me->FindCreatureWithDistance(NPC_HODIR, 250.0f, true))
                    hodir->AI()->DoAction(ACTION_FAILED_COOLEST_FRIENDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                if (FireTimer <= diff)
                {
                    DoCast(me, SPELL_CONJURE_FIRE, true);
                    FireTimer = urand(25000, 30000);
                }
                else
                    FireTimer -= diff;

                if (MeltIceTimer <= diff)
                {
                    if (Creature* shard = me->FindCreatureWithDistance(NPC_FLASH_FREEZE, 50.0f, true))
                    {
                        DoCast(shard, SPELL_MELT_ICE, true);
                        MeltIceTimer = urand(5000,10000);
                    }
                    MeltIceTimer = 5000;
                }
                else MeltIceTimer -= diff;

                DoSpellAttackIfReady(SPELL_FIREBALL);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hodir_mageAI(creature);
        }
};

class npc_toasty_fire : public CreatureScript
{
    public:
        npc_toasty_fire() : CreatureScript("npc_toasty_fire") { }

        struct npc_toasty_fireAI : public QuantumBasicAI
        {
            npc_toasty_fireAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(15880);
            }

            void Reset()
            {
                DoCast(me, SPELL_SINGED, true);
            }

			void JustDied(Unit* /*killer*/)
            {
                if (GameObject* fire = me->FindGameobjectWithDistance(GO_TOASTY_FIRE, 4.0f))
					me->RemoveGameObject(fire, true);
            }

            void SpellHit(Unit* /*target*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_BLOCK_OF_ICE || spell->Id == SPELL_ICE_SHARDS_10 || spell->Id == SPELL_ICE_SHARDS_25)
                {
                    if (GameObject* fire = me->FindGameobjectWithDistance(GO_TOASTY_FIRE, 4.0f))
                        me->RemoveGameObject(fire, true);

                    me->DespawnAfterAction();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_toasty_fireAI(creature);
        }
};

class npc_flash_freeze : public CreatureScript
{
    public:
        npc_flash_freeze() : CreatureScript("npc_flash_freeze") { }

        struct npc_flash_freezeAI : public QuantumBasicAI
        {
            npc_flash_freezeAI(Creature* creature) : QuantumBasicAI(creature)
            {
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
				me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                me->SetCurrentFaction(FACTION_HOSTILE);
				me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
				SetCombatMovement(false);
            }

            void Reset()
            {
                if (me->ToTempSummon()->GetSummoner())
                {
                    if (me->GetEntry() == NPC_FLASH_FREEZE_PRE)
                    {
                        DoCast(SPELL_BLOCK_OF_ICE_NPC);
                        me->ToTempSummon()->GetSummoner()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    }
                    else DoCast(SPELL_BLOCK_OF_ICE);

                    me->ToTempSummon()->GetSummoner()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                }
            }

			void JustDied(Unit* /*killer*/)
            {
                if (me->ToTempSummon()->GetSummoner())
                    me->ToTempSummon()->GetSummoner()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
            }

			void DamageTaken(Unit* attacker, uint32 &damage)
            {
                if (me->GetEntry() == NPC_FLASH_FREEZE_PRE)
                {
                    if (attacker && attacker->GetTypeId() == TYPE_ID_PLAYER)
					{
                        if (Creature* hodir = me->FindCreatureWithDistance(NPC_HODIR, 200.0f, true))
						{
                            if (!hodir->IsInCombatActive())
                                hodir->AI()->AttackStart(attacker);
						}
					}

                    me->ToTempSummon()->GetSummoner()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!me->ToTempSummon()->GetSummoner())
                {
                    me->DisappearAndDie();
                    return;
                }

                if (me->ToTempSummon()->GetSummoner()->IsDead())
                {
                    me->ToTempSummon()->GetSummoner()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
                    me->DisappearAndDie();
                    return;
                }

                if (me->GetEntry() == NPC_FLASH_FREEZE_PRE && !me->ToTempSummon()->GetSummoner()->HasAura(SPELL_BLOCK_OF_ICE_NPC))
                {
                    DoCast(SPELL_BLOCK_OF_ICE_NPC);
                    me->ToTempSummon()->GetSummoner()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_flash_freezeAI(creature);
        }
};

class npc_snow_mound : public CreatureScript
{
public:
    npc_snow_mound() : CreatureScript("npc_snow_mound") {}

    struct npc_snow_moundAI : public QuantumBasicAI
    {
        npc_snow_moundAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		uint32 SnowTimer;

		bool Snowed;

        void Reset()
        {
			if (!instance)
				return;

			SnowTimer = 1000;

			Snowed = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SummonGameObject(GO_SNOW_MOUND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), 0, 0, 1, 1, 604800);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (!instance || Snowed || who->GetTypeId() != TYPE_ID_PLAYER || who->ToPlayer()->IsGameMaster() || !who->IsWithinDistInMap(me, 15.0f))
				return;

			if (instance || Snowed == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 15.0f))
			{
				me->SummonCreature(NPC_WINTER_JORMUNGAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000);
				me->SummonCreature(NPC_WINTER_JORMUNGAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000);
				me->SummonCreature(NPC_WINTER_JORMUNGAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000);
				me->SummonCreature(NPC_WINTER_JORMUNGAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000);
				me->SummonCreature(NPC_WINTER_JORMUNGAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000);
				me->SummonCreature(NPC_WINTER_JORMUNGAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000);

				me->DisappearAndDie();

				if (GameObject* mound = me->FindGameobjectWithDistance(GO_SNOW_MOUND, 5.0f))
					mound->Delete();

				Snowed = true;
			}
		}

        void UpdateAI(const uint32 diff)
        {
			if (!instance)
				return;	

			if (SnowTimer <= diff)
			{
				DoCast(me, SPELL_SNOW_MOUND);
				SnowTimer = urand(55000, 60000);
			}
			else SnowTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snow_moundAI(creature);
    }
};

class spell_biting_cold : public SpellScriptLoader
{
    public:
        spell_biting_cold() : SpellScriptLoader("spell_biting_cold") { }

        class spell_biting_cold_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_biting_cold_AuraScript);

            void HandlePeriodicDummy(AuraEffect const* aurEff)
            {
                PreventDefaultAction();

                if (Unit* trigger = GetTarget())
                {
                    if (aurEff->GetSpellInfo()->Id == SPELL_BITING_COLD)
                    {
                        if (trigger->ToPlayer())
						{
                            if (!trigger->HasAura(SPELL_SINGED))
							{
                                trigger->CastSpell(trigger, SPELL_BITING_COLD_TRIGGERED, true, 0, 0, GetCasterGUID());
							}
						}
                    }
                    else if (aurEff->GetSpellInfo()->Id == SPELL_BITING_COLD_TRIGGERED)
                    {
                        int32 damage = trigger->GetAuraCount(SPELL_BITING_COLD_TRIGGERED);
                        damage *= 400;
                        trigger->CastCustomSpell(trigger, SPELL_BITING_COLD_DAMAGE, &damage, NULL, NULL, true, 0, 0, GetCasterGUID());
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_biting_cold_AuraScript::HandlePeriodicDummy, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_biting_cold_AuraScript();
        }
};

class spell_hodir_toasty_fire : public SpellScriptLoader
{
    public:
        spell_hodir_toasty_fire() : SpellScriptLoader("spell_hodir_toasty_fire") { }

        class spell_hodir_toasty_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hodir_toasty_fire_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (!target || !target->ToPlayer())
                    return;

                target->ToPlayer()->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET_2, GetId());
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_hodir_toasty_fire_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_STAT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hodir_toasty_fire_AuraScript();
        }
};

class spell_hodir_starlight : public SpellScriptLoader
{
    public:
        spell_hodir_starlight() : SpellScriptLoader("spell_hodir_starlight") { }

        class spell_hodir_starlight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hodir_starlight_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (!target || !target->ToPlayer())
                    return;

                target->ToPlayer()->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET_2, GetId());
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_hodir_starlight_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hodir_starlight_AuraScript();
        }
};

class achievement_staying_buffed_all_winter : public AchievementCriteriaScript
{
public:
	achievement_staying_buffed_all_winter() : AchievementCriteriaScript("achievement_staying_buffed_all_winter") { }

	bool OnCheck(Player* player, Unit* /*target*/)
	{
		if (player->HasAura(SPELL_SINGED) && player->HasAura(SPELL_STARLIGHT) && (player->HasAura(SPELL_STORM_CLOUD_10) || player->HasAura(SPELL_STORM_CLOUD_25)))
			return true;

		return false;
	}
};

void AddSC_boss_hodir()
{
    new boss_hodir();
    new npc_icicle();
    new npc_icicle_snowdrift();
    new npc_snowpacked_icicle();
    new npc_hodir_priest();
    new npc_hodir_shaman();
    new npc_hodir_druid();
    new npc_hodir_mage();
    new npc_toasty_fire();
    new npc_flash_freeze();
	new npc_snow_mound();
    new spell_biting_cold();
    new spell_hodir_starlight();
    new spell_hodir_toasty_fire();
    new achievement_staying_buffed_all_winter();
}