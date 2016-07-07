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
    SAY_AGGRO                                   = -1603180,
    SAY_AGGRO_WITH_ELDER                        = -1603181,
    SAY_SLAY_1                                  = -1603182,
    SAY_SLAY_2                                  = -1603183,
    SAY_DEATH                                   = -1603184,
    SAY_BERSERK                                 = -1603185,
    SAY_SUMMON_CONSERVATOR                      = -1603186,
    SAY_SUMMON_TRIO                             = -1603187,
    SAY_SUMMON_LASHERS                          = -1603188,
    EMOTE_ALLIES                                = -1603105,
    EMOTE_GIFT                                  = -1603106,
	EMOTE_GROUND_TREMOR                         = -1603107,
	EMOTE_IRON_ROOTS                            = -1603108,
    // Elder Brightleaf
    SAY_BRIGHTLEAF_AGGRO                        = -1603190,
    SAY_BRIGHTLEAF_SLAY_1                       = -1603191,
    SAY_BRIGHTLEAF_SLAY_2                       = -1603192,
    SAY_BRIGHTLEAF_DEATH                        = -1603193,
    // Elder Ironbranch
    SAY_IRONBRANCH_AGGRO                        = -1603194,
    SAY_IRONBRANCH_SLAY_1                       = -1603195,
    SAY_IRONBRANCH_SLAY_2                       = -1603196,
    SAY_IRONBRANCH_DEATH                        = -1603197,
    // Elder Stonebark
    SAY_STONEBARK_AGGRO                         = -1603198,
    SAY_STONEBARK_SLAY_1                        = -1603199,
    SAY_STONEBARK_SLAY_2                        = -1603200,
    SAY_STONEBARK_DEATH                         = -1603201,
};

enum Events
{
    ACHIEV_CON_SPEED_ATORY_START_EVENT          = 21597,
	ACHIEV_LUMBERJACKED                         = 21686,
    SPELL_ACHIEVEMENT_CHECK                     = 65074,
    SPELL_LUMBERJACKED_ACHIEVEMENT_CHECK        = 65296,
};

enum Spells
{
    // Freya
    SPELL_BERSERK                               = 47008,
    SPELL_TOUCH_OF_EONAR_10                     = 62528,
    SPELL_TOUCH_OF_EONAR_25                     = 62892,
    SPELL_ATTUNED_TO_NATURE                     = 62519,
    SPELL_SUNBEAM_10                            = 62623,
    SPELL_SUNBEAM_25                            = 62872,
    SPELL_NATURE_BOMB_VISUAL                    = 64648,
    SPELL_NATURE_BOMB_SUMMON                    = 64606,
	SPELL_SUMMON_ANCIENT_CONSERVATOR            = 62685,
	SPELL_SUMMON_TRIO                           = 62686,
	SPELL_SUMMON_LASHERS                        = 62687,
	SPELL_SUMMON_WAVE_10_MOB                    = 62688,
    SPELL_LIFEBINDERS_GIFT_TRIGGER_MISSILE_1    = 62572,
    SPELL_LIFEBINDERS_GIFT_SUMMON_1             = 62568,
    SPELL_LIFEBINDERS_GIFT_TRIGGER_MISSILE_2    = 62870,
    SPELL_LIFEBINDERS_GIFT_SUMMON_2             = 62869,
    SPELL_LIFEBINDERS_GIFT_VISUAL               = 62579,
    // Hardmode
    SPELL_STONEBARK_ESSENCE                     = 62483,
    SPELL_IRONBRANCH_ESSENCE                    = 62484,
    SPELL_BRIGHTLEAF_ESSENCE                    = 62485,
    SPELL_FREYA_UNSTABLE_ENERGY_10              = 62451,
    SPELL_FREYA_UNSTABLE_ENERGY_25              = 62865,
    SPELL_FREYA_IRON_ROOTS_10                   = 62438,
    SPELL_FREYA_IRON_ROOTS_25                   = 62861,
    SPELL_FREYA_GROUND_TREMOR_10                = 62437,
    SPELL_FREYA_GROUND_TREMOR_25                = 62859,
    //Nature Bomb
    SPELL_NATURE_BOMB_VISUAL_OBJECT             = 64600,
    SPELL_NATURE_BOMB_EXPLOSION_10              = 64587,
    SPELL_NATURE_BOMB_EXPLOSION_25              = 64650,
    // Freya Adds
    SPELL_ATTUNED_TO_NATURE_REMOVE_2            = 62524,
    SPELL_ATTUNED_TO_NATURE_REMOVE_10           = 62525,
    SPELL_ATTUNED_TO_NATURE_REMOVE_25           = 62521,
    // Eonar's Gift
    SPELL_LIFEBINDERS_GIFT_10                   = 62584,
    SPELL_LIFEBINDERS_GIFT_25                   = 64185,
    SPELL_PHEROMONES                            = 62619,
    // Freya Chests
	SPELL_FREYA_CHEST_RAID_10_0                 = 62950, // 194324
    SPELL_FREYA_CHEST_RAID_10_1                 = 62952, // 194325
    SPELL_FREYA_CHEST_RAID_10_2                 = 62953, // 194326
    SPELL_FREYA_CHEST_RAID_10_3                 = 62954, // 194327
    SPELL_FREYA_CHEST_RAID_25_0                 = 62955, // 194328
    SPELL_FREYA_CHEST_RAID_25_1                 = 62956, // 194329
    SPELL_FREYA_CHEST_RAID_25_2                 = 62957, // 194330
    SPELL_FREYA_CHEST_RAID_25_3                 = 62958, // 194331
    // Detonating lasher
    SPELL_DETONATE_10                           = 62598,
    SPELL_DETONATE_25                           = 62937,
    SPELL_FLAME_LASH                            = 62608,
    // Ancient Water Spirit
    SPELL_TIDAL_WAVE_10                         = 62653,
    SPELL_TIDAL_WAVE_25                         = 62935,
    SPELL_WATER_REVIVE_VISUAL                   = 62849,
    // Storm Lasher
    SPELL_LIGHTNING_LASH_10                     = 62648,
    SPELL_LIGHTNING_LASH_25                     = 62939,
    SPELL_STORMBOLT_10                          = 62649,
    SPELL_STORMBOLT_25                          = 62938,
    SPELL_STORM_REVIVE_VISUAL                   = 62851,
	SPELL_STORM_LASHER_VISUAL                   = 62639,
    // Snaplasher
    SPELL_HARDENED_BARK_10                      = 62664,
    SPELL_HARDENED_BARK_25                      = 64191,
    SPELL_LASHER_REVIVE_VISUAL                  = 62848,
    // Ancient Conservator
    SPELL_CONSERVATOR_GRIP                      = 62532,
    SPELL_NATURES_FURY_10                       = 62589,
    SPELL_NATURES_FURY_25                       = 63571,
    // Healthy Spore
    SPELL_HEALTHY_SPORE_VISUAL                  = 62538,
    SPELL_POTENT_PHEROMONES                     = 62541,
    SPELL_GROW                                  = 62559,
    // Elders
    SPELL_DRAINED_OF_POWER                      = 62467,
    // Elder Brightleaf
    // All spells are triggered
    SPELL_SOLAR_FLARE_10                        = 62240, // Target Amount is affected by stackamount of Brightleaf flux
    SPELL_SOLAR_FLARE_25                        = 62920, // Target Amount is affected by stackamount of Brightleaf flux
    SPELL_UNSTABLE_ENERGY_10                    = 62217, // Remove Unstable Sun Beam Buff
    SPELL_UNSTABLE_ENERGY_25                    = 62922,
    SPELL_PHOTOSYNTHESIS                        = 62209, // While standing in Beam
    SPELL_BRIGHTLEAF_FLUX                       = 62262, // script effect needs to trigger 62239
    SPELL_BRIGHTLEAF_FLUX_BUFF                  = 62239, // randomstack 1-8
    SPELL_FLUX_PLUS                             = 62251, // randomspell with flux
    SPELL_FLUX_MINUS                            = 62252, // randomspell with flux
    SPELL_UNSTABLE_SUN_BEAM_SUMMON              = 62207, // 62921, 62221, 64088
    SPELL_UNSTABLE_SUN_BEAM_PERIODIC            = 62211, // Triggers the Beam, triggers 62243 and 62216
	SPELL_UNSTABLE_SUN_BEAM_VISUAL              = 62216,
    SPELL_UNSTABLE_SUN_BEAM_TRIGGERD            = 62243,
    //Elder Ironbranch
    //only impale is not triggered
    SPELL_IMPALE_10                             = 62310,
    SPELL_IMPALE_25                             = 62928,
    SPELL_THORN_SWARM_10                        = 62285, // Need Target fix ?
    SPELL_THORN_SWARM_25                        = 62931,
    SPELL_IRON_ROOTS_10                         = 62283,
    SPELL_IRON_ROOTS_25                         = 62930,
    // Elder Stonebark
    //every spell is not triggered
    SPELL_FISTS_OF_STONE                        = 62344,
    SPELL_GROUND_TREMOR_10                      = 62325,
    SPELL_GROUND_TREMOR_25                      = 62932,
    SPELL_PETRIFIED_BARK_10                     = 62337,
    SPELL_PETRIFIED_BARK_25                     = 62933,
};

enum Phase
{
    PHASE_SPAWNING     = 1,
    PHASE_NOT_SPAWNING = 2,
};

const uint32 WaveSpells[3] =
{
    SPELL_SUMMON_ANCIENT_CONSERVATOR,
	SPELL_SUMMON_TRIO,
	SPELL_SUMMON_LASHERS,
};

enum Actions
{
    ACTION_ELEMENTAL_DEAD       = 1,
};

enum Data
{
    DATA_KNOCK_ON_WOOD          = 1,
};

class boss_freya : public CreatureScript
{
public:
    boss_freya() : CreatureScript("boss_freya") { }

    struct boss_freyaAI : public QuantumBasicAI
    {
        boss_freyaAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();

			EncounterFinished = (instance->GetBossState(DATA_FREYA) == DONE);
        }

        InstanceScript* instance;

        uint32 SpawnWaves[3];
        uint32 WaveCount;

        uint32 WaveTimer;
        uint32 BerserkTimer;
        uint32 LifebindersGiftTimer;
        uint32 SunbeamTimer;
        uint32 GroundTremorTimer;
        uint32 IronRootsTimer;
        uint32 UnstableEnergyTimer;
        uint32 NaturalBombTimer;
        uint32 InFightAggroCheckTimer;
		uint32 ChestSpell;

        uint32 ReviveTimer;
        uint8 ReviveCount;
		uint8 EldersCount;
        std::set<uint64> elementalList;

		bool EncounterFinished;
        bool IsElderBrightleafAlive;
        bool IsElderIronbranchAlive;
        bool IsElderStonebarkAlive;

        void Reset()
        {
            if (EncounterFinished)
                me->SetCurrentFaction(FACTION_FRIENDLY);

            else
            {
				instance->SetBossState(DATA_FREYA, NOT_STARTED);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                ReviveCount = 0;
				EldersCount = 0;

                ReviveTimer = 15000;
                elementalList.clear();

				IronRootsDespawn();

                WaveCount = 0;
                WaveTimer = 10000;
                GroundTremorTimer = 30000;
                IronRootsTimer = 22000;
                SunbeamTimer = urand(20000, 30000);
                UnstableEnergyTimer = 15000;
                BerserkTimer = 600000;
                LifebindersGiftTimer = 30000;
                NaturalBombTimer = 60000;
                InFightAggroCheckTimer = 5000;

                IsElderBrightleafAlive = IsElderIronbranchAlive = IsElderStonebarkAlive = false;

                Creature* Elder[3];
                for (uint8 n = 0; n < 3; ++n)
                {
                    Elder[n] = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ELDER_BRIGHTLEAF + n));
                    if (Elder[n] && Elder[n]->IsAlive())
                    {
                        Elder[n]->ResetLootMode();
                        Elder[n]->AI()->EnterEvadeMode();
                    }
                }
            }
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
							player->AddItem(ITEM_ID_HEART_OF_FREYA, 1);
					}
				}
			}
		}

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

		void DamageTaken(Unit* /*attacker*/, uint32 &amount)
        {
            if (amount >= me->GetHealth())
            {
                amount = 0;
                EncounterIsDone();
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_KNOCK_ON_WOOD:
                    return GetEldersCount();
            }
            return 0;
        }

        void DoAction(const int32 action)
        {
            switch (action)
            {
                case ACTION_ELEMENTAL_DEAD:
                    if (!ReviveCount)
                        ReviveTimer = 15000;
                    ++ReviveCount;
                    break;
                default:
                    break;
            }
        }

        void ReviveElementals()
        {
            if (ReviveCount >= elementalList.size())
                elementalList.clear();
            else
            {
                if (!elementalList.empty())
				{
                    for (std::set<uint64>::const_iterator itr = elementalList.begin(); itr != elementalList.end(); ++itr)
					{
                        if (Creature* creature = me->GetCreature(*me, *itr))
                        {
                            if (creature->IsAlive())
                                creature->SetFullHealth();
                            else
                                creature->Respawn();

                            switch (creature->GetEntry())
                            {
                                case NPC_SNAPLASHER:
                                    creature->CastSpell(creature, SPELL_LASHER_REVIVE_VISUAL, true);
                                    break;
                                case NPC_STORM_LASHER:
                                    creature->CastSpell(creature, SPELL_STORM_REVIVE_VISUAL, true);
                                    break;
                                case NPC_ANCIENT_WATER_SPIRIT:
                                    creature->CastSpell(creature, SPELL_WATER_REVIVE_VISUAL, true);
                                    break;
                            }
                        }
					}
				}
            }

            ReviveCount = 0;
        }

		void IronRootsDespawn()
		{
			std::list<Creature*> RootCountList;
			me->GetCreatureListWithEntryInGrid(RootCountList, NPC_IRON_ROOTS, 350.0f);
			me->GetCreatureListWithEntryInGrid(RootCountList, NPC_STENGTHENED_IRON_ROOTS, 350.0f);

			if (!RootCountList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = RootCountList.begin(); itr != RootCountList.end(); ++itr)
				{
					if (Creature* root = *itr)
						me->Kill(root);
				}
			}
		}

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_SNAPLASHER:
                case NPC_STORM_LASHER:
                case NPC_ANCIENT_WATER_SPIRIT:
                    elementalList.insert(summon->GetGUID());
					break;
                default:
                    break;
            }
        }

        void InitSpawnWaves()
        {
             memset(&SpawnWaves, 0, sizeof(SpawnWaves));
             uint32 i_rand = urand(0, 2);
             switch (i_rand)
             {
                 case 0:
                     SpawnWaves[0] = i_rand;
                     if (urand(0, 1) == 0)
                     {
                         SpawnWaves[1] = 1;
                         SpawnWaves[2] = 2;
                     }
					 else
                     {
                         SpawnWaves[1] = 2;
                         SpawnWaves[2] = 1;
                     }
                     break;
                 case 1:
                     SpawnWaves[0] = i_rand;
                     if (urand(0, 1) == 0)
                     {
                          SpawnWaves[1] = 0;
                          SpawnWaves[2] = 2;
                     }
					 else
                     {
                          SpawnWaves[1] = 2;
                          SpawnWaves[2] = 0;
                     }
                     break;
                 case 2:
                     SpawnWaves[0] = i_rand;
                     if (urand(0, 1) == 0)
                     {
                          SpawnWaves[1] = 0;
                          SpawnWaves[2] = 1;
                     }
					 else
                     {
                          SpawnWaves[1] = 1;
                          SpawnWaves[2] = 0;
                     }
                     break;
             }
        }

		void DoSummonWave()
		{
			uint32 spawntype = WaveCount % 3;
			DoSendQuantumText(EMOTE_ALLIES, me);
			DoSendQuantumText(RAND(SAY_SUMMON_CONSERVATOR, SAY_SUMMON_TRIO, SAY_SUMMON_LASHERS), me);
			DoCast(WaveSpells[SpawnWaves[spawntype]]);
			++WaveCount;
		}

        uint32 GetEldersCount()
        {
            uint32 i = 0;
            if (IsElderBrightleafAlive)
			{
				i++;
				EldersCount++;
			}
            if (IsElderIronbranchAlive)
			{
				i++;
				EldersCount++;
			}
            if (IsElderStonebarkAlive)
			{
				i++;
				EldersCount++;
			}
            return i;
        }

        void EncounterIsDone()
        {
            if (EncounterFinished)
                return;

            EncounterFinished = true;
			me->RemoveAllAuras();
			IronRootsDespawn();
			//QuestReward();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            DoSendQuantumText(SAY_DEATH, me);
			me->SummonCreature(NPC_UNFLUENCE_TENTACLE, 2346.51f, -48.7069f, 425.718f, 3.19981f, TEMPSUMMON_TIMED_DESPAWN, 35000);

            if (!instance)
                return;

            instance->SetBossState(DATA_FREYA, DONE);

			switch (EldersCount)
			{
                case 0:
                    ChestSpell = RAID_MODE(SPELL_FREYA_CHEST_RAID_10_0, SPELL_FREYA_CHEST_RAID_25_0);
                    break;
                case 1:
                    ChestSpell = RAID_MODE(SPELL_FREYA_CHEST_RAID_10_1, SPELL_FREYA_CHEST_RAID_25_1);
                    break;
                case 2:
                    ChestSpell = RAID_MODE(SPELL_FREYA_CHEST_RAID_10_2, SPELL_FREYA_CHEST_RAID_25_2);
                    break;
                case 3:
                    ChestSpell = RAID_MODE(SPELL_FREYA_CHEST_RAID_10_3, SPELL_FREYA_CHEST_RAID_25_3);
                    break;
            }

			DoCast(me, ChestSpell, true);

            EnterEvadeMode();
            me->DespawnAfterAction(10*IN_MILLISECONDS);
            DoCast(me, SPELL_ACHIEVEMENT_CHECK, true);

			instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_CON_SPEED_ATORY_10, ACHIEVEMENT_CON_SPEED_ATORY_25));
			instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_LUMBERJACKED_10, ACHIEVEMENT_LUMBERJACKED_25));
			instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_DEFORESTATION_10, ACHIEVEMENT_DEFORESTATION_25));
			instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_GETTING_BACK_TO_NATURE_10, ACHIEVEMENT_GETTING_BACK_TO_NATURE_25));

            Creature* Elder[3];
            for (uint8 n = 0; n < 3; ++n)
            {
                Elder[n] = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ELDER_BRIGHTLEAF + n));
                if (Elder[n] && Elder[n]->IsAlive())
                    Elder[n]->DespawnAfterAction(2*IN_MILLISECONDS);
            }
        }

        void JustReachedHome()
        {
            me->SetActive(false);
			Reset();
        }

        void EnterToBattle(Unit* who)
        {
            me->SetActive(true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_AGGRO, me);

            // Add Attuned to Nature and Touch of Eonar
            DoCast(RAID_MODE(SPELL_TOUCH_OF_EONAR_10, SPELL_TOUCH_OF_EONAR_25));
            me->CastCustomSpell(SPELL_ATTUNED_TO_NATURE, SPELLVALUE_AURA_STACK, 150, me, true);

            if (instance)
            {
                InitSpawnWaves();
                instance->SetBossState(DATA_FREYA, IN_PROGRESS);

                Creature* Elder[3];
                for (uint8 n = 0; n < 3; ++n)
                {
                    Elder[n] = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ELDER_BRIGHTLEAF + n));
                    if (Elder[n] && Elder[n]->IsAlive())
                    {
                        me->AddAura(SPELL_DRAINED_OF_POWER, Elder[n]);
                        Elder[n]->RemoveLootMode(LOOT_MODE_DEFAULT);
                        Elder[n]->AI()->AttackStart(who);
                        Elder[n]->AddThreat(who, 250.0f);
                        Elder[n]->SetInCombatWith(who);
                    }
                }

                if (Elder[0]->IsAlive())
                {
                    Elder[0]->CastSpell(me, SPELL_BRIGHTLEAF_ESSENCE, true);
                    IsElderBrightleafAlive = true;
                }

                if (Elder[1]->IsAlive())
                {
                    Elder[1]->CastSpell(me, SPELL_IRONBRANCH_ESSENCE, true);
                    IsElderIronbranchAlive = true;
                }

                if (Elder[2]->IsAlive())
                {
                    Elder[2]->CastSpell(me, SPELL_STONEBARK_ESSENCE, true);
                    IsElderStonebarkAlive = true;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (InFightAggroCheckTimer <= diff)
            {
                if (me->GetVictim() && me->GetVictim()->ToPlayer())
				{
                    DoAttackerGroupInCombat(me->GetVictim()->ToPlayer());
					InFightAggroCheckTimer = 5000;
				}
            }
			else InFightAggroCheckTimer -= diff;

            if (ReviveCount)
            {
				if (ReviveTimer <= diff)
					ReviveElementals();
				else
					ReviveTimer -= diff;
            }

            if (WaveCount < 6)
            {
                if (WaveTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoSummonWave();
                        WaveTimer = 60000;
                    }
                }
                else WaveTimer -= diff;
            }
            else
            {
                if (NaturalBombTimer <= diff)
                {
                    std::list<Player*> PlayerList = me->GetNearestPlayersList(500.0f);
                    Quantum::DataPackets::RandomResizeList<Player*>(PlayerList, uint32(Is25ManRaid() ? urand(10, 15) : urand(4, 6)));
                    for (std::list<Player*>::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                        me->CastSpell(*itr, SPELL_NATURE_BOMB_VISUAL, true);

                    NaturalBombTimer = urand(20000, 30000);
                }
                else NaturalBombTimer -= diff;
            }

            if (BerserkTimer <= diff)
            {
                if (!me->HasAura(SPELL_BERSERK))
                    DoCast(me, SPELL_BERSERK, true);
            }
			else BerserkTimer -= diff;

            // Hardmode Elder Stonebark
            if (IsElderStonebarkAlive)
            {
                if (GroundTremorTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
						DoSendQuantumText(EMOTE_GROUND_TREMOR, me);
                        DoCast(RAID_MODE(SPELL_FREYA_GROUND_TREMOR_10, SPELL_FREYA_GROUND_TREMOR_25));
                        GroundTremorTimer = 30000;
                    }
                    else GroundTremorTimer = 3000;
                }
                else GroundTremorTimer -= diff;	
            }

            // Hardmode Elder Ironbranch
            if (IsElderIronbranchAlive)
            {
                if (IronRootsTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 500, true))
						{
							DoSendQuantumText(EMOTE_IRON_ROOTS, me);
							target->CastSpell(target, RAID_MODE(SPELL_FREYA_IRON_ROOTS_10, SPELL_FREYA_IRON_ROOTS_25), false);
							IronRootsTimer = urand(25000, 30000);
						}
                    }
                    else IronRootsTimer = 3000;
                }
                else IronRootsTimer -= diff;
            }

            // Hardmode Elder Brightleaf
            if (IsElderBrightleafAlive)
            {
                if (UnstableEnergyTimer <= diff)
                {
                    std::list<Unit*> targets;
                    SelectTargetList(targets, RAID_MODE(1, 3), TARGET_RANDOM, 150.0f, true);
                    if (!targets.empty())
                        for (std::list<Unit*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                            me->SummonCreature(NPC_SUNBEAM, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ());

                    UnstableEnergyTimer = urand(25000, 30000);
                }
                else UnstableEnergyTimer -= diff;
            }

            if (LifebindersGiftTimer <= diff)
            {
                DoSendQuantumText(EMOTE_GIFT, me);
                DoCastAOE(RAID_MODE(SPELL_LIFEBINDERS_GIFT_TRIGGER_MISSILE_1, SPELL_LIFEBINDERS_GIFT_TRIGGER_MISSILE_2), true);
                LifebindersGiftTimer = 35000 + urand(2000, 10000);
            }
			else LifebindersGiftTimer -= diff;

            if (SunbeamTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 500, true))
                        DoCast(target, RAID_MODE(SPELL_SUNBEAM_10, SPELL_SUNBEAM_25));
                    else
                        DoCastVictim(RAID_MODE(SPELL_SUNBEAM_10, SPELL_SUNBEAM_25));

                    SunbeamTimer = urand(20000, 30000);
                }
                else SunbeamTimer = 3000;
            }
			else SunbeamTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_freyaAI(creature);
    }
};

class npc_nature_bomb : public CreatureScript
{
public:
    npc_nature_bomb() : CreatureScript("npc_nature_bomb") { }

    struct npc_nature_bombAI : public QuantumBasicAI
    {
        npc_nature_bombAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
			SetCombatMovement(false);
            me->SetReactState(REACT_PASSIVE);
            me->SetDisplayId(23258);
            me->SetCurrentFaction(FACTION_HOSTILE);
        }

		InstanceScript* instance;

        uint32 ExplosionTimer;

        void Reset()
        {
            DoCast(SPELL_NATURE_BOMB_VISUAL_OBJECT);
            ExplosionTimer = 10*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
			if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
				me->DespawnAfterAction(2*IN_MILLISECONDS);

            if (ExplosionTimer <= diff)
            {
                if (GameObject* bomb = me->FindGameobjectWithDistance(GO_NATURE_BOMB, 5.0f))
                    bomb->SetGoState(GO_STATE_ACTIVE);

                DoCast(RAID_MODE(SPELL_NATURE_BOMB_EXPLOSION_10, SPELL_NATURE_BOMB_EXPLOSION_25));
                me->DespawnAfterAction(2*IN_MILLISECONDS);
                ExplosionTimer = 10*IN_MILLISECONDS;
            }
			else ExplosionTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nature_bombAI(creature);
    }
};

class spell_freya_natural_bomb_spell : public SpellScriptLoader
{
public:
    spell_freya_natural_bomb_spell() : SpellScriptLoader("spell_freya_natural_bomb_spell") { }

    class spell_freya_natural_bomb_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_freya_natural_bomb_SpellScript);

        void OnHitEffect()
        {
            if (Unit* target = GetHitUnit())
                target->CastSpell(target, SPELL_NATURE_BOMB_SUMMON, true);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_freya_natural_bomb_SpellScript::OnHitEffect);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_freya_natural_bomb_SpellScript();
    }
};

class spell_attuned_to_nature_remove_spell : public SpellScriptLoader
{
public:
    spell_attuned_to_nature_remove_spell() : SpellScriptLoader("spell_attuned_to_nature_remove") { }

    class spell_attuned_to_nature_remove_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_attuned_to_nature_remove_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || GetCaster()->GetTypeId() != TYPE_ID_UNIT)
                    return;

            if (Unit* unitTarget = GetHitUnit())
            {
                uint8 count = 0;
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_ATTUNED_TO_NATURE_REMOVE_2:
						count = 2;
						break;
                    case SPELL_ATTUNED_TO_NATURE_REMOVE_10:
						count = 10;
						break;
                    case SPELL_ATTUNED_TO_NATURE_REMOVE_25:
						count = 25;
						break;
                }
                if (Aura* aura = unitTarget->GetAura(SPELL_ATTUNED_TO_NATURE, unitTarget->GetGUID()))
                    aura->ModStackAmount(-count);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_attuned_to_nature_remove_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_attuned_to_nature_remove_SpellScript();
    }
};

class spell_summon_wave_effect_10mob_spell : public SpellScriptLoader
{
public:
    spell_summon_wave_effect_10mob_spell() : SpellScriptLoader("spell_summon_wave_effect_10mob") { }

    class spell_summon_wave_effect_10mob_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_wave_effect_10mob_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || GetCaster()->GetTypeId() != TYPE_ID_UNIT)
                    return;

            for (uint8 i = 0; i < 10; i++)
                GetCaster()->CastSpell(GetCaster(), SPELL_SUMMON_LASHERS, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_summon_wave_effect_10mob_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_summon_wave_effect_10mob_SpellScript();
    }
};

class npc_detonating_lasher : public CreatureScript
{
public:
    npc_detonating_lasher() : CreatureScript("npc_detonating_lasher") { }

    struct npc_detonating_lasherAI : public QuantumBasicAI
    {
        npc_detonating_lasherAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 FlameLashTimer;
        uint32 ChangeTargetTimer;

        void Reset()
        {
            FlameLashTimer = urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS);
            ChangeTargetTimer = 7.5*IN_MILLISECONDS;
        }

        void DamageTaken(Unit* /*attacker*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
                if (Creature* freya = me->GetCreature(*me, instance->GetData64(DATA_FREYA)))
                    DoCast(freya, SPELL_ATTUNED_TO_NATURE_REMOVE_2, true);

                me->DespawnAfterAction(15*IN_MILLISECONDS);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoCast(me, RAID_MODE(SPELL_DETONATE_10, SPELL_DETONATE_25), true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                me->DespawnAfterAction(2*IN_MILLISECONDS);

            if (!UpdateVictim())
                return;

            if (FlameLashTimer <= diff)
            {
                DoCastVictim(SPELL_FLAME_LASH);
                FlameLashTimer = urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else FlameLashTimer -= diff;

            if (ChangeTargetTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                {
                    DoResetThreat();
                    me->AddThreat(target, 50000.0f);
					ChangeTargetTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
            }
            else ChangeTargetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_detonating_lasherAI(creature);
    }
};

class npc_ancient_water_spirit : public CreatureScript
{
public:
    npc_ancient_water_spirit() : CreatureScript("npc_ancient_water_spirit") { }

    struct npc_ancient_water_spiritAI : public QuantumBasicAI
    {
        npc_ancient_water_spiritAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            alreadyKilled = false;
        }

        InstanceScript* instance;
        uint32 TidalWaveTimer;
        bool alreadyKilled;

        void Reset()
        {
            me->SetCorpseDelay(20);
            TidalWaveTimer = 20000;
        }

        void DamageTaken(Unit* /*attacker*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
                if (Creature* freya = me->GetCreature(*me, instance->GetData64(DATA_FREYA)))
                {
                    if (!alreadyKilled)
                        DoCast(freya, SPELL_ATTUNED_TO_NATURE_REMOVE_10, true);

                    alreadyKilled = true;
                    freya->AI()->DoAction(ACTION_ELEMENTAL_DEAD);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                me->DespawnAfterAction(2*IN_MILLISECONDS);

            if (!UpdateVictim())
                return;

            if (TidalWaveTimer <= diff)
            {
                DoCast(SelectTarget(TARGET_RANDOM, 0), RAID_MODE(SPELL_TIDAL_WAVE_10, SPELL_TIDAL_WAVE_25));
                TidalWaveTimer = 20*IN_MILLISECONDS;
            }
            else TidalWaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancient_water_spiritAI(creature);
    }
};

class npc_storm_lasher : public CreatureScript
{
public:
    npc_storm_lasher() : CreatureScript("npc_storm_lasher") { }

    struct npc_storm_lasherAI : public QuantumBasicAI
    {
        npc_storm_lasherAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
			alreadyKilled = false;
        }

        InstanceScript* instance;
        uint32 LightningLashTimer;
        uint32 StormboltTimer;

        bool alreadyKilled;

        void Reset()
        {
            me->SetCorpseDelay(20);

            LightningLashTimer = 2000;
            StormboltTimer = 3000;

			DoCast(me, SPELL_STORM_LASHER_VISUAL, true);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
                if (Creature* freya = me->GetCreature(*me, instance->GetData64(DATA_FREYA)))
                {
                    if (!alreadyKilled)
                        DoCast(freya, SPELL_ATTUNED_TO_NATURE_REMOVE_10, true);

                    alreadyKilled = true;
                    freya->AI()->DoAction(ACTION_ELEMENTAL_DEAD);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                me->DespawnAfterAction(2*IN_MILLISECONDS);

            if (!UpdateVictim())
                return;

            if (LightningLashTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_LIGHTNING_LASH_10, SPELL_LIGHTNING_LASH_25));
					LightningLashTimer = 6*IN_MILLISECONDS;
				}
            }
            else LightningLashTimer -= diff;

            if (StormboltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_STORMBOLT_10, SPELL_STORMBOLT_25));
					StormboltTimer = 3*IN_MILLISECONDS;
				}
            }
            else StormboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_storm_lasherAI(creature);
    }
};

class npc_snaplasher : public CreatureScript
{
public:
    npc_snaplasher() : CreatureScript("npc_snaplasher") { }

    struct npc_snaplasherAI : public QuantumBasicAI
    {
        npc_snaplasherAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            alreadyKilled = false;
        }

        InstanceScript* instance;
        bool alreadyKilled;

        void Reset()
        {
            me->SetCorpseDelay(20);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, RAID_MODE(SPELL_HARDENED_BARK_10, SPELL_HARDENED_BARK_25));
        }

        void DamageTaken(Unit* /*attacker*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
                if (Creature* freya = me->GetCreature(*me, instance->GetData64(DATA_FREYA)))
                {
                    if (!alreadyKilled)
                        DoCast(freya, SPELL_ATTUNED_TO_NATURE_REMOVE_10, true);
                    alreadyKilled = true;
                    freya->AI()->DoAction(ACTION_ELEMENTAL_DEAD);
                }
            }
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                me->DespawnAfterAction(2*IN_MILLISECONDS);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snaplasherAI(creature);
    }
};

class npc_ancient_conservator : public CreatureScript
{
    public:
        npc_ancient_conservator() : CreatureScript("npc_ancient_conservator") { }

        struct npc_ancient_conservatorAI : public QuantumBasicAI
        {
            npc_ancient_conservatorAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 NaturesFuryTimer;
            uint32 HealthySporeSpawnTimer;

            void Reset()
            {
                NaturesFuryTimer = 10000;
                HealthySporeSpawnTimer = urand(15000, 23000);
            }

            void EnterToBattle(Unit* who)
            {
				DoCast(who, SPELL_CONSERVATOR_GRIP, true);

                for (uint8 i = 1; i <= 6; ++i)
                {
                    Position pos;
                    me->GetRandomNearPosition(pos, 35.0f);
                    me->SummonCreature(NPC_HEALTHLY_SPORE, pos, TEMPSUMMON_TIMED_DESPAWN, urand(30000, 36000));
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    if (Creature* freya = me->GetCreature(*me, instance->GetData64(DATA_FREYA)))
                        DoCast(freya, SPELL_ATTUNED_TO_NATURE_REMOVE_25, true);

                    me->DespawnAfterAction(15*IN_MILLISECONDS);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                    me->DespawnAfterAction(2*IN_MILLISECONDS);

                if (!UpdateVictim())
                    return;

                if (NaturesFuryTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
					{
                        DoCast(target, RAID_MODE(SPELL_NATURES_FURY_10, SPELL_NATURES_FURY_25));
						NaturesFuryTimer = 15*IN_MILLISECONDS;
					}
                }
                else NaturesFuryTimer -= diff;

                if (HealthySporeSpawnTimer <= diff)
                {
                    Position pos;
                    me->GetRandomNearPosition(pos, 35.0f);
                    me->SummonCreature(NPC_HEALTHLY_SPORE, pos, TEMPSUMMON_TIMED_DESPAWN, 30000);
                    HealthySporeSpawnTimer = urand(2.5*IN_MILLISECONDS, 5*IN_MILLISECONDS);
                }
                else HealthySporeSpawnTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ancient_conservatorAI(creature);
        }
};

class npc_healthy_spore : public CreatureScript
{
    public:
        npc_healthy_spore() : CreatureScript("npc_healthy_spore") { }

        struct npc_healthy_sporeAI : public QuantumBasicAI
        {
            npc_healthy_sporeAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                instance = creature->GetInstanceScript();

                ShrinkTimer = urand(22*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            }

			InstanceScript* instance;

            uint32 ShrinkTimer;

            void Reset()
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetCurrentFaction(FACTION_FRIENDLY);
                DoCast(me, SPELL_HEALTHY_SPORE_VISUAL, true);
                DoCast(me, SPELL_GROW, true);
                DoCast(me, SPELL_POTENT_PHEROMONES, true);
            }

            void UpdateAI(const uint32 diff)
            {
                if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                    me->DisappearAndDie();

                if (ShrinkTimer <= diff)
                {
                    me->RemoveAurasDueToSpell(SPELL_GROW);
                    me->DespawnAfterAction(2*IN_MILLISECONDS);
                    ShrinkTimer = 3*IN_MILLISECONDS;
                }
                else ShrinkTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_healthy_sporeAI(creature);
        }
};

class boss_elder_brightleaf : public CreatureScript
{
public:
    boss_elder_brightleaf() : CreatureScript("boss_elder_brightleaf") { }

    struct boss_elder_brightleafAI : public QuantumBasicAI
    {
        boss_elder_brightleafAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 SolarFlareTimer;
        uint32 FluxTimer;
        uint32 UnstableSunbeamTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            SolarFlareTimer = 10000;
            FluxTimer = 1000;
            UnstableSunbeamTimer = 5000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!me->HasAura(SPELL_DRAINED_OF_POWER))
                DoSendQuantumText(SAY_BRIGHTLEAF_AGGRO, me);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_BRIGHTLEAF_SLAY_1, SAY_BRIGHTLEAF_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_IRONBRANCH_AGGRO, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
                return;

            if (SolarFlareTimer <= diff)
            {
                uint32 target_count = me->GetAuraCount(SPELL_BRIGHTLEAF_FLUX_BUFF);
                me->CastCustomSpell(RAID_MODE(SPELL_SOLAR_FLARE_10, SPELL_SOLAR_FLARE_25),SPELLVALUE_MAX_TARGETS, target_count, me, true);
                //DoCastAOE(RAID_MODE(SPELL_SOLAR_FLARE_10, SPELL_SOLAR_FLARE_25),true);
                SolarFlareTimer = 10000 + urand(1500, 6000);
            }
            else { SolarFlareTimer -= diff; }

            if (UnstableSunbeamTimer <= diff)
            {
                DoCast(me, SPELL_UNSTABLE_SUN_BEAM_SUMMON, true);
                UnstableSunbeamTimer = 30000;
            }
            else UnstableSunbeamTimer -= diff;

            // Workaround for Spellscript
            if (FluxTimer <= diff)
            {
                me->RemoveAurasDueToSpell(SPELL_FLUX_PLUS);
                me->RemoveAurasDueToSpell(SPELL_FLUX_MINUS);

                me->CastSpell(me, SPELL_BRIGHTLEAF_FLUX, true);
                if (!me->HasAura(SPELL_BRIGHTLEAF_FLUX_BUFF))
                    me->CastSpell(me, SPELL_BRIGHTLEAF_FLUX_BUFF, true);
                me->SetAuraStack(SPELL_BRIGHTLEAF_FLUX_BUFF, me, urand(1, 10));
                uint8 flux = rand() %2;
                if (flux == 1)
                    me->AddAura(SPELL_FLUX_PLUS, me);
                else
                    me->AddAura(SPELL_FLUX_MINUS, me);
                FluxTimer = 5000;
            }
			else FluxTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_elder_brightleafAI(creature);
    }
};

class npc_unstable_sun_beam : public CreatureScript
{
public:
    npc_unstable_sun_beam() : CreatureScript("npc_unstable_sun_beam") { }

    struct npc_unstable_sun_beamAI : public QuantumBasicAI
    {
        npc_unstable_sun_beamAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint32 UnstableEnergyTimer;

        void Reset()
        {
            me->CastSpell(me, SPELL_UNSTABLE_SUN_BEAM_PERIODIC, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE );
            me->SetDisplayId(MODEL_ID_INVISIBLE);

            UnstableEnergyTimer = urand(30*IN_MILLISECONDS, 45*IN_MILLISECONDS);
        }

        void MoveInLineOfSight(Unit* mover)
        {
            if (mover && mover->ToCreature() && mover->GetEntry() == NPC_ELDER_BRIGHTLEAF)
			{
                if (me->IsWithinDist2d(mover, 4))
                {
                    if (!mover->HasAura(SPELL_PHOTOSYNTHESIS))
                        me->AddAura(SPELL_PHOTOSYNTHESIS, mover);
                }
				else
                {
                    if (mover->HasAura(SPELL_PHOTOSYNTHESIS))
                        mover->RemoveAurasDueToSpell(SPELL_PHOTOSYNTHESIS);
                }
			}
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            switch (spell->Id)
            {
                case SPELL_UNSTABLE_ENERGY_10:
                case SPELL_UNSTABLE_ENERGY_25:
                    target->RemoveAurasDueToSpell(SPELL_UNSTABLE_SUN_BEAM_TRIGGERD);
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (UnstableEnergyTimer <= diff)
            {
                DoCast(me, RAID_MODE(SPELL_UNSTABLE_ENERGY_10, SPELL_UNSTABLE_ENERGY_25), true);
                me->DespawnAfterAction(2*IN_MILLISECONDS);
                UnstableEnergyTimer = urand(30*IN_MILLISECONDS, 45*IN_MILLISECONDS);
            }
            else
			{
				UnstableEnergyTimer -= diff;
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unstable_sun_beamAI(creature);
    }
};

class boss_elder_ironbranch : public CreatureScript
{
public:
	boss_elder_ironbranch() : CreatureScript("boss_elder_ironbranch") { }

    struct boss_elder_ironbranchAI : public QuantumBasicAI
    {
        boss_elder_ironbranchAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 ImpaleTimer;
        uint32 IronRootsTimer;
        uint32 ThornSwarmTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            ImpaleTimer = 45000;
            IronRootsTimer = 15000;
            ThornSwarmTimer = 2000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!me->HasAura(SPELL_DRAINED_OF_POWER))
                DoSendQuantumText(SAY_IRONBRANCH_AGGRO, me);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_IRONBRANCH_SLAY_1, SAY_IRONBRANCH_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_IRONBRANCH_DEATH, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
                return;

            if (ImpaleTimer <= diff)
            {
                DoSendQuantumText(SAY_IRONBRANCH_DEATH, me);
                DoCastVictim(RAID_MODE(SPELL_IMPALE_10, SPELL_IMPALE_25));
                ImpaleTimer = 20000;
            }
            else ImpaleTimer -= diff;

            if (IronRootsTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM))
				{
                    target->CastSpell(target, RAID_MODE(SPELL_IRON_ROOTS_10, SPELL_IRON_ROOTS_25), true);
					IronRootsTimer = urand(20000, 30000);
				}
            }
            else IronRootsTimer -= diff;

            if (ThornSwarmTimer <= diff)
            {
                DoCast(RAID_MODE(SPELL_THORN_SWARM_10, SPELL_THORN_SWARM_25));
                ThornSwarmTimer = 20000;
            }
            else ThornSwarmTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_elder_ironbranchAI(creature);
    }
};

class boss_elder_stonebark : public CreatureScript
{
public:
	boss_elder_stonebark() : CreatureScript("boss_elder_stonebark") { }

    struct boss_elder_stonebarkAI : public QuantumBasicAI
    {
        boss_elder_stonebarkAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 FistsOfStoneTimer;
        uint32 GroundTremorTimer;
        uint32 PetrifiedBarkTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            FistsOfStoneTimer = 25000;
            GroundTremorTimer = 12000;
            PetrifiedBarkTimer = 17000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!me->HasAura(SPELL_DRAINED_OF_POWER))
                DoSendQuantumText(SAY_STONEBARK_AGGRO, me);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_STONEBARK_SLAY_1, SAY_STONEBARK_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_STONEBARK_DEATH, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
                return;

            if (FistsOfStoneTimer <= diff)
            {
                DoCast(SPELL_FISTS_OF_STONE);
                FistsOfStoneTimer = 45000;
            }
            else FistsOfStoneTimer -= diff;

            if (GroundTremorTimer <= diff)
            {
                DoCast(RAID_MODE(SPELL_GROUND_TREMOR_10, SPELL_GROUND_TREMOR_25));
                GroundTremorTimer = 200000 + urand(3000, 10000);
            }
            else GroundTremorTimer -= diff;

            if (PetrifiedBarkTimer <= diff)
            {
                DoCast(RAID_MODE(SPELL_PETRIFIED_BARK_10, SPELL_PETRIFIED_BARK_25));
                PetrifiedBarkTimer = 40000;
            }
            else PetrifiedBarkTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_elder_stonebarkAI(creature);
    }
};

class npc_eonars_gift : public CreatureScript
{
public:
	npc_eonars_gift() : CreatureScript("npc_eonars_gift") { }

	struct npc_eonars_giftAI : public QuantumBasicAI
    {
        npc_eonars_giftAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 LifebindersGiftTimer;

        void Reset()
        {
            me->SetCurrentFaction(FACTION_HOSTILE);

            DoCast(me, SPELL_GROW, true);
            DoCast(me, SPELL_PHEROMONES, true);
            DoCast(me, SPELL_LIFEBINDERS_GIFT_VISUAL, true);

            LifebindersGiftTimer = 12*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_FREYA) != IN_PROGRESS)
                me->DisappearAndDie();

			if (LifebindersGiftTimer <= diff)
            {
                me->RemoveAurasDueToSpell(SPELL_GROW);
                DoCast(RAID_MODE(SPELL_LIFEBINDERS_GIFT_10, SPELL_LIFEBINDERS_GIFT_25));
                me->DespawnAfterAction(2.5*IN_MILLISECONDS);
                LifebindersGiftTimer = 6*IN_MILLISECONDS;
            }
			else LifebindersGiftTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eonars_giftAI(creature);
    }
};

class npc_sun_beam : public CreatureScript
{
    public:
		npc_sun_beam() : CreatureScript("npc_sun_beam") { }

		struct npc_sun_beamAI : public QuantumBasicAI
        {
            npc_sun_beamAI(Creature* creature) : QuantumBasicAI(creature)
			{
				SetCombatMovement(false);
			}

			uint32 UnstableSunbeamEnergyTimer;

            void Reset()
            {
                UnstableSunbeamEnergyTimer = 1*IN_MILLISECONDS;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetDisplayId(MODEL_ID_INVISIBLE);
                me->SetCurrentFaction(FACTION_HOSTILE);
                me->DespawnAfterAction(12*IN_MILLISECONDS);
                DoCast(me, SPELL_UNSTABLE_SUN_BEAM_VISUAL, true);
            }

            void UpdateAI(const uint32 diff)
            {
                if (UnstableSunbeamEnergyTimer <= diff)
                {
                    DoCast(RAID_MODE(SPELL_FREYA_UNSTABLE_ENERGY_10, SPELL_FREYA_UNSTABLE_ENERGY_25));
                    UnstableSunbeamEnergyTimer = 15*IN_MILLISECONDS;
                }
                else UnstableSunbeamEnergyTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_sun_beamAI(creature);
        }
};

class npc_iron_roots : public CreatureScript
{
    public:
		npc_iron_roots() : CreatureScript("npc_iron_roots") { }

        struct npc_iron_rootsAI : public QuantumBasicAI
        {
            npc_iron_rootsAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                SetImmuneToPushPullEffects(true);
                CheckTimer = 3*IN_MILLISECONDS;
            }

			uint32 CheckTimer;
            uint64 VictimGUID;

            void IsSummonedBy(Unit* summoner)
            {
                VictimGUID = summoner->GetGUID();
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Unit* victim = ObjectAccessor::GetUnit(*me, VictimGUID))
                {
                    victim->RemoveAurasDueToSpell(RAID_MODE(SPELL_IRON_ROOTS_10, SPELL_IRON_ROOTS_25));
                    victim->RemoveAurasDueToSpell(RAID_MODE(SPELL_FREYA_IRON_ROOTS_10, SPELL_FREYA_IRON_ROOTS_25));
                }

                me->DespawnAfterAction(2*IN_MILLISECONDS);
            }

            void UpdateAI(const uint32 diff)
            {
                if (CheckTimer <= diff)
                {
                    Unit* victim = ObjectAccessor::GetUnit(*me, VictimGUID);

                    if (!victim || (!victim->HasAura(RAID_MODE(SPELL_IRON_ROOTS_10, SPELL_IRON_ROOTS_25)) && !victim->HasAura(RAID_MODE(SPELL_FREYA_IRON_ROOTS_10, SPELL_FREYA_IRON_ROOTS_25))))
                        me->DespawnAfterAction(2*IN_MILLISECONDS);

                    CheckTimer = 3*IN_MILLISECONDS;
                }
                else CheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_iron_rootsAI(creature);
        }
};

class IsNoAllyOfNature
{
public:
	bool operator() (Unit* unit)
	{
		if (unit->ToCreature())
		{
            switch (unit->ToCreature()->GetEntry())
			{
                case NPC_DETONATING_LASHER:
				case NPC_ANCIENT_WATER_SPIRIT:
				case NPC_STORM_LASHER:
				case NPC_SNAPLASHER:
				case NPC_ANCIENT_CONSERVATOR:
					return false;
				default:
					break;
			}
		}
		return true;
	}
};

class spell_elder_ironbranchs_essence_targeting : public SpellScriptLoader
{
    public:
        spell_elder_ironbranchs_essence_targeting() : SpellScriptLoader("spell_elder_ironbranchs_essence_targeting") { }

        class spell_elder_ironbranchs_essence_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elder_ironbranchs_essence_targeting_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(IsNoAllyOfNature());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_elder_ironbranchs_essence_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_elder_ironbranchs_essence_targeting_SpellScript();
        }
};

class spell_elder_brightleafs_essence_targeting : public SpellScriptLoader
{
    public:
        spell_elder_brightleafs_essence_targeting() : SpellScriptLoader("spell_elder_brightleafs_essence_targeting") { }

        class spell_elder_brightleafs_essence_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elder_brightleafs_essence_targeting_SpellScript);

            void FilterTargetsEffect0(std::list<Unit*>& unitList)
            {
                unitList.remove_if(IsNoAllyOfNature());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_elder_brightleafs_essence_targeting_SpellScript::FilterTargetsEffect0, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_elder_brightleafs_essence_targeting_SpellScript();
        }
};

class spell_aggregation_pheromones_targeting : public SpellScriptLoader
{
    public:
        spell_aggregation_pheromones_targeting() : SpellScriptLoader("spell_aggregation_pheromones_targeting") { }

        class spell_aggregation_pheromones_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggregation_pheromones_targeting_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                if (unitList.size() < 2)
                    unitList.clear();
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_aggregation_pheromones_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_aggregation_pheromones_targeting_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_aggregation_pheromones_targeting_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_aggregation_pheromones_targeting_SpellScript();
        }
};

class spell_elder_brightleaf_unstable_sun_beam : public SpellScriptLoader
{
    public:
        spell_elder_brightleaf_unstable_sun_beam() : SpellScriptLoader("spell_elder_brightleaf_unstable_sun_beam") { }

        class spell_elder_brightleaf_unstable_sun_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elder_brightleaf_unstable_sun_beam_SpellScript);

            void HandleForceCast(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                uint32 triggered_spell_id = GetSpellInfo()->Effects[effIndex].TriggerSpell;

                if (caster && target && triggered_spell_id)
                    target->CastSpell(target, triggered_spell_id, true, NULL, NULL, caster->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_elder_brightleaf_unstable_sun_beam_SpellScript::HandleForceCast, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
            }

            std::list<Unit*> sharedUnitList;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_elder_brightleaf_unstable_sun_beam_SpellScript();
        }
};

class achievement_knock_on_wood : public AchievementCriteriaScript
{
public:
	achievement_knock_on_wood() : AchievementCriteriaScript("achievement_knock_on_wood") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Freya = target->ToCreature())
			if (Freya->AI()->GetData(DATA_KNOCK_ON_WOOD) >= 1)
				return true;

		return false;
	}
};

class achievement_knock_knock_on_wood : public AchievementCriteriaScript
{
public:
	achievement_knock_knock_on_wood() : AchievementCriteriaScript("achievement_knock_knock_on_wood") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Freya = target->ToCreature())
			if (Freya->AI()->GetData(DATA_KNOCK_ON_WOOD) >= 2)
				return true;

		return false;
	}
};

class achievement_knock_knock_knock_on_wood : public AchievementCriteriaScript
{
public:
	achievement_knock_knock_knock_on_wood() : AchievementCriteriaScript("achievement_knock_knock_knock_on_wood") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Freya = target->ToCreature())
			if (Freya->AI()->GetData(DATA_KNOCK_ON_WOOD) == 3)
				return true;

		return false;
	}
};

void AddSC_boss_freya()
{
    new boss_freya();
    new npc_nature_bomb();
    new spell_freya_natural_bomb_spell();
    new spell_attuned_to_nature_remove_spell();
    new spell_summon_wave_effect_10mob_spell();
    new npc_detonating_lasher();
    new npc_ancient_water_spirit();
    new npc_storm_lasher();
    new npc_snaplasher();
    new npc_ancient_conservator();
    new boss_elder_ironbranch();
    new boss_elder_stonebark();
    new boss_elder_brightleaf();
    new npc_unstable_sun_beam();
    new npc_eonars_gift();
    new npc_healthy_spore();
    new npc_sun_beam();
    new npc_iron_roots();
    new spell_elder_ironbranchs_essence_targeting();
    new spell_elder_brightleafs_essence_targeting();
    new spell_aggregation_pheromones_targeting();
    new spell_elder_brightleaf_unstable_sun_beam();
    new achievement_knock_on_wood();
    new achievement_knock_knock_on_wood();
    new achievement_knock_knock_knock_on_wood();
}