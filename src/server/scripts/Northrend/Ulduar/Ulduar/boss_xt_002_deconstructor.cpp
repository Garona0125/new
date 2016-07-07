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
#include "Vehicle.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO                                 = -1603300,
    SAY_HEART_OPENED                          = -1603301,
    SAY_HEART_CLOSED                          = -1603302,
    SAY_TYMPANIC_TANTRUM                      = -1603303,
    SAY_SLAY_1                                = -1603304,
    SAY_SLAY_2                                = -1603305,
    SAY_BERSERK                               = -1603306,
    SAY_DEATH                                 = -1603307,
    SAY_SUMMON                                = -1603308,
	EMOTE_TYMPANIC_TANTRUM                    = -1613233,
	EMOTE_HEART                               = -1613234,
	EMOTE_REPAIR                              = -1613235,
};

enum Spells
{
    SPELL_TYMPANIC_TANTRUM                   = 62776,
    SPELL_SEARING_LIGHT_10                   = 63018,
    SPELL_SEARING_LIGHT_25                   = 65121,
    SPELL_GRAVITY_BOMB_10                    = 63024,
    SPELL_GRAVITY_BOMB_25                    = 64234,
    SPELL_HEARTBREAK_10                      = 65737,
    SPELL_HEARTBREAK_25                      = 64193,
    SPELL_HEART_EXPOSED                      = 62789,
    SPELL_ENRAGE                             = 26662,
    SPELL_VOID_ZONE_10                       = 64203,
    SPELL_VOID_ZONE_25                       = 64235,
    SPELL_CONSUMPTION_10                     = 64208,
    SPELL_CONSUMPTION_25                     = 64206,
    SPELL_STATIC_CHARGED_10                  = 64227,
    SPELL_STATIC_CHARGED_25                  = 64236,
    SPELL_SHOCK                              = 64230,
    SPELL_EXPOSED_HEART                      = 63849,
    SPELL_HEART_RIDE_XT002                   = 63852,
    SPELL_ARCING_SMASH                       = 8374,
    SPELL_TRAMPLE                            = 5568,
    SPELL_UPPERCUT                           = 10966,
    SPELL_BOOM                               = 62834,
    SPELL_AURA_BOOMBOT                       = 65032,
    SPELL_HEAL_XT002                         = 62832,
    SPELL_ACHIEVEMENT_CREDIT_NERF_SCRAPBOTS  = 65037,
};

enum Actions
{
    ACTION_XT_002_ENTER_HARD_MODE         = 1,
    ACTION_XT_002_REACHED                 = 2,
};

enum Data
{
    DATA_TRANSFERED_HEALTH,
    DATA_HARD_MODE,
    DATA_HEALTH_RECOVERED,
    DATA_GRAVITY_BOMB_CASUALTY,
};

enum StartAchievementEvent
{
	ACHIEV_TIMED_START_EVENT = 21027,
};

#define SPAWN_Z        412
#define LR_X           796
#define LR_Y           -94
#define LL_X           796
#define LL_Y           57
#define UR_X           890
#define UR_Y           -82
#define UL_X           894
#define UL_Y           62

class boss_xt_002_deconstructor : public CreatureScript
{
    public:
        boss_xt_002_deconstructor() : CreatureScript("boss_xt_002_deconstructor") { }

        struct boss_xt_002_deconstructor_AI : public BossAI
        {
            boss_xt_002_deconstructor_AI(Creature* creature) : BossAI(creature, DATA_XT002_DECONSTRUCTOR) {}

			uint32 SearingLightTimer;
            uint32 GravityBombTimer;
            uint32 TympanicTantrumTimer;
            uint32 HeartPhaseTimer;
            uint32 SpawnAddTimer;
            uint32 EnrageTimer;

            uint8 Phase;
            uint8 HeartExposed;
            uint32 TransferHealth;

            bool Enraged;
            bool EnterHardMode;
            bool HardMode;
            bool HealthRecovered;
            bool GravityBombCasualty;

            void Reset()
            {
                _Reset();
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
                me->ResetLootMode();

				me->SetPowerType(POWER_ENERGY);
				me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                SearingLightTimer = 10*IN_MILLISECONDS;
                GravityBombTimer = 20*IN_MILLISECONDS;
                HeartPhaseTimer = 30*IN_MILLISECONDS;
                SpawnAddTimer = 12*IN_MILLISECONDS;
                EnrageTimer = 10*MINUTE*IN_MILLISECONDS;
                TympanicTantrumTimer = RAID_MODE(35000, 50000); // DBM

                Enraged = false;
                HardMode = false;
                EnterHardMode = false;
                HealthRecovered = false;
                GravityBombCasualty = false;

                Phase = 1;
                HeartExposed = 0;

                if (instance)
                    instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				_EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);

                if (instance)
                    instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
            }

			void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                me->RemoveAllAuras();

				_JustDied();

                DoSendQuantumText(SAY_DEATH, me);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE);

				if (instance)
					instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_NERF_SCRAPBOTS_10, ACHIEVEMENT_NERF_SCRAPBOTS_25));
            }

            void JustSummoned(Creature* summoned)
            {
                summons.Summon(summoned);
                DoZoneInCombat(summoned);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_XT_002_ENTER_HARD_MODE:
                        if (!HardMode)
                        {
                            HardMode = true;
                            me->AddLootMode(LOOT_MODE_HARD_MODE_1);
                            // Enter hard mode
                            EnterHardMode = true;
                            // set max health
                            me->SetFullHealth();
                            // Get his heartbreak buff
                            me->CastSpell(me, RAID_MODE(SPELL_HEARTBREAK_10, SPELL_HEARTBREAK_25), true);
                        }
                        break;
                    case ACTION_XT_002_REACHED:
                        HealthRecovered = true;
                        break;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_HARD_MODE:
                        return HardMode ? 1 : 0;
                    case DATA_HEALTH_RECOVERED:
                        return HealthRecovered ? 1 : 0;
                    case DATA_GRAVITY_BOMB_CASUALTY:
                        return GravityBombCasualty ? 1 : 0;
                }

                return 0;
            }

            void SetData(uint32 id, uint32 data)
            {
                switch (id)
                {
                    case DATA_TRANSFERED_HEALTH:
                        TransferHealth = data;
                        break;
                    case DATA_GRAVITY_BOMB_CASUALTY:
                        GravityBombCasualty = (data > 0) ? true : false;
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (EnterHardMode)
                {
                    SetPhaseOne();
                    EnterHardMode = false;
                }

                // Handles spell casting. These spells only occur during phase 1 or hard mode
                if (Phase == 1 || HardMode)
                {
                    if (SearingLightTimer <= diff)
                    {
                        if (!me->HasAura(SPELL_TYMPANIC_TANTRUM))
                            DoCast(me, RAID_MODE(SPELL_SEARING_LIGHT_10, SPELL_SEARING_LIGHT_25), true);

                        SearingLightTimer = 20*IN_MILLISECONDS;
                    }
                    else SearingLightTimer -= diff;

                    if (GravityBombTimer <= diff)
                    {
                        if (!me->HasAura(SPELL_TYMPANIC_TANTRUM))
                            DoCast(me, RAID_MODE(SPELL_GRAVITY_BOMB_10, SPELL_GRAVITY_BOMB_25), true);

                        GravityBombTimer = 20*IN_MILLISECONDS;
                    }
                    else GravityBombTimer -= diff;

                    if (TympanicTantrumTimer <= diff)
                    {
                        DoSendQuantumText(SAY_TYMPANIC_TANTRUM, me);
                        DoSendQuantumText(EMOTE_TYMPANIC_TANTRUM, me);
                        DoCast(SPELL_TYMPANIC_TANTRUM);
                        TympanicTantrumTimer = 1*MINUTE*IN_MILLISECONDS;
                    }
                    else TympanicTantrumTimer -= diff;
                }

                if (!HardMode)
                {
                    if (Phase == 1)
                    {
                        if (HealthBelowPct(100 - (HeartExposed + 1) * 25))
                            exposeHeart();

                        DoMeleeAttackIfReady();
                    }
                    else
                    {
                        // Start summoning adds
                        if (SpawnAddTimer <= diff)
                        {
                            DoSendQuantumText(SAY_SUMMON, me);

                            // Spawn Pummeller
                            switch (rand() % 4)
                            {
                                case 0:
									me->SummonCreature(NPC_XM_024_PUMMELLER, LR_X, LR_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
									break;
                                case 1:
									me->SummonCreature(NPC_XM_024_PUMMELLER, LL_X, LL_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
									break;
                                case 2:
									me->SummonCreature(NPC_XM_024_PUMMELLER, UR_X, UR_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
									break;
                                case 3:
									me->SummonCreature(NPC_XM_024_PUMMELLER, UL_X, UL_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
									break;
                            }

                            // Spawn 5 Scrapbots
                            for (int8 n = 0; n < 5; ++n)
                            {
                                // Some randomes are added so they wont spawn in a pile
                                switch (rand() % 4)
                                {
                                    case 0:
										me->SummonCreature(NPC_XS_013_SCRAPBOT, float(irand(LR_X - 3, LR_X + 3)), float(irand(LR_Y - 3, LR_Y + 3)), SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                    case 1:
										me->SummonCreature(NPC_XS_013_SCRAPBOT, float(irand(LL_X - 3, LL_X + 3)), float(irand(LL_Y - 3, LL_Y + 3)), SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                    case 2:
										me->SummonCreature(NPC_XS_013_SCRAPBOT, float(irand(UR_X - 3, UR_X + 3)), float(irand(UR_Y - 3, UR_Y + 3)), SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                    case 3:
										me->SummonCreature(NPC_XS_013_SCRAPBOT, float(irand(UL_X - 3, UL_X + 3)), float(irand(UL_Y - 3, UL_Y + 3)), SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                }
                            }

                            // Spawn 3 Bombs
                            for (int8 n = 0; n < 3; ++n)
                            {
                                switch (rand() % 4)
                                {
                                    case 0:
										me->SummonCreature(NPC_XE_321_BOOMBOT, LR_X, LR_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                    case 1:
										me->SummonCreature(NPC_XE_321_BOOMBOT, LL_X, LL_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                    case 2:
										me->SummonCreature(NPC_XE_321_BOOMBOT, UR_X, UR_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                    case 3:
										me->SummonCreature(NPC_XE_321_BOOMBOT, UL_X, UL_Y, SPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
										break;
                                }
                            }

                            SpawnAddTimer = 12*IN_MILLISECONDS;
                        }
                        else SpawnAddTimer -= diff;

                        // Is the phase over?
                        if (HeartPhaseTimer <= diff)
                        {
                            DoSendQuantumText(SAY_HEART_CLOSED, me);
                            SetPhaseOne();
                        }
                        else HeartPhaseTimer -= diff;
                    }
                }
                else DoMeleeAttackIfReady();

                if (!Enraged)
                {
                    if (EnrageTimer <= diff)
                    {
                        DoSendQuantumText(SAY_BERSERK, me);
                        DoCast(me, SPELL_ENRAGE, true);
                        Enraged = true;
                    }
                    else EnrageTimer -= diff;
                }
            }

            // NOT BLIZZLIKE ... Heart is Spawned the whole Encounter ... VX - 002 is a vehicle
            void exposeHeart()
            {
                me->GetMotionMaster()->MoveIdle();
                me->SetStandState(UNIT_STAND_STATE_SUBMERGED);

                // Remove if still active?
                me->RemoveAurasDueToSpell(SPELL_TYMPANIC_TANTRUM);

                // Make untargetable
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE);

                // Summon the heart npc
                if (Creature* heart = me->SummonCreature(NPC_XT_002_HEART, *me, TEMPSUMMON_TIMED_DESPAWN, 30*IN_MILLISECONDS))
                {
                    heart->EnterVehicle(me, 1);
                    heart->ClearUnitState(UNIT_STATE_ON_VEHICLE); // Hack
                    heart->SetInCombatWithZone();
                    heart->CastSpell(heart, SPELL_EXPOSED_HEART, true);
                }

                // Start "end of phase 2 timer"
                HeartPhaseTimer = 30*IN_MILLISECONDS;

                // Phase 2 has offically started
                Phase = 2;
                ++HeartExposed;

                // Reset the add spawning timer
                SpawnAddTimer = 12*IN_MILLISECONDS;

                DoSendQuantumText(SAY_HEART_OPENED, me);
                DoSendQuantumText(EMOTE_HEART, me);
            }

            void SetPhaseOne()
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->GetMotionMaster()->MoveChase(me->GetVictim());

                SearingLightTimer = 15*IN_MILLISECONDS;
                GravityBombTimer = 25*IN_MILLISECONDS;
                TympanicTantrumTimer = 60*IN_MILLISECONDS;
                SpawnAddTimer = 12*IN_MILLISECONDS;

                if (!HardMode)
                {
                    me->ModifyHealth(-((int32)TransferHealth));
                    me->LowerPlayerDamageReq(TransferHealth);
                }

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE);
                Phase = 1;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_xt_002_deconstructor_AI(creature);
        }
};

class npc_xt002_heart : public CreatureScript
{
    public:
        npc_xt002_heart() : CreatureScript("npc_xt002_heart") { }

        struct npc_xt002_heartAI : public QuantumBasicAI
        {
            npc_xt002_heartAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                instance = creature->GetInstanceScript();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

			InstanceScript* instance;

            void JustDied(Unit* /*killer*/)
            {
                if (instance)
				{
                    if (Creature* XT002 = me->GetCreature(*me, instance ? instance->GetData64(DATA_XT002_DECONSTRUCTOR) : 0))
					{
                        if (XT002->AI())
                            XT002->AI()->DoAction(ACTION_XT_002_ENTER_HARD_MODE);
					}
				}

                //removes the aura
                me->RemoveAurasDueToSpell(SPELL_EXPOSED_HEART);
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (Creature* XT002 = me->GetCreature(*me, instance ? instance->GetData64(DATA_XT002_DECONSTRUCTOR) : 0))
				{
                    if (XT002->AI())
                    {
                        uint32 health = me->GetHealth();
                        if (health <= damage)
                            health = 0;
                        else
                            health -= damage;
                        XT002->AI()->SetData(DATA_TRANSFERED_HEALTH, me->GetMaxHealth() - health);
                    }
				}
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!me->HasAura(SPELL_EXPOSED_HEART))
                    me->AddAura(SPELL_EXPOSED_HEART, me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_xt002_heartAI(creature);
        }
};

class npc_scrapbot : public CreatureScript
{
    public:
        npc_scrapbot() : CreatureScript("npc_scrapbot") { }

        struct npc_scrapbotAI : public QuantumBasicAI
        {
            npc_scrapbotAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
                NearCheckTimer = 5*IN_MILLISECONDS;
                Casted = false;
            }

			InstanceScript* instance;
            uint32 NearCheckTimer;
            bool Casted;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);

                if (Creature* XT002 = me->GetCreature(*me, instance ? instance->GetData64(DATA_XT002_DECONSTRUCTOR) : 0))
                    me->GetMotionMaster()->MoveFollow(XT002, 1, float(2*M_PI*rand_norm()));
            }

            void UpdateAI(uint32 const diff)
            {
                if (NearCheckTimer <= diff)
                {
                    if (Creature* xt002 = me->GetCreature(*me, instance ? instance->GetData64(DATA_XT002_DECONSTRUCTOR) : 0))
					{
                        if (!Casted && xt002->IsAlive())
						{
                            if (me->GetDistance2d(xt002) <= 2)
                            {
                                Casted = true;
								DoSendQuantumText(EMOTE_REPAIR, xt002);
                                xt002->CastSpell(xt002, SPELL_HEAL_XT002, true);
                                xt002->AI()->DoAction(ACTION_XT_002_REACHED);
                                me->DespawnAfterAction(0.5*IN_MILLISECONDS);
                            }
						}
					}

                    NearCheckTimer = 1*IN_MILLISECONDS;
                }
                else NearCheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_scrapbotAI(creature);
        }
};

class npc_pummeller : public CreatureScript
{
    public:
        npc_pummeller() : CreatureScript("npc_pummeller") {}

        struct npc_pummellerAI : public QuantumBasicAI
        {
            npc_pummellerAI(Creature* creature) : QuantumBasicAI(creature){}

			uint32 ArcingSmashTimer;
            uint32 TrampleTimer;
            uint32 UppercutTimer;

            void Reset()
            {
                ArcingSmashTimer = 7*IN_MILLISECONDS;
                TrampleTimer = 2*IN_MILLISECONDS;
                UppercutTimer = 10*IN_MILLISECONDS;

                if (Player* target = me->SelectNearestPlayer(500.0f))
                    AttackStart(target);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->IsWithinMeleeRange(me->GetVictim()))
                {
                    if (ArcingSmashTimer <= diff)
                    {
                        DoCastVictim(SPELL_ARCING_SMASH);
                        ArcingSmashTimer = 7*IN_MILLISECONDS;
                    }
                    else ArcingSmashTimer -= diff;

                    if (TrampleTimer <= diff)
                    {
                        DoCastVictim(SPELL_TRAMPLE);
                        TrampleTimer = 2*IN_MILLISECONDS;
                    }
                    else TrampleTimer -= diff;

                    if (UppercutTimer <= diff)
                    {
                        DoCastVictim(SPELL_UPPERCUT);
                        UppercutTimer = 10*IN_MILLISECONDS;
                    }
                    else UppercutTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pummellerAI(creature);
        }
};

class BoomEvent : public BasicEvent
{
    public:
        BoomEvent(Creature* me) : _me(me) {}

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _me->CastSpell(_me, SPELL_BOOM, false);
            return true;
        }

    private:
        Creature* _me;
};

class npc_boombot : public CreatureScript
{
    public:
        npc_boombot() : CreatureScript("npc_boombot") { }

        struct npc_boombotAI : public QuantumBasicAI
        {
            npc_boombotAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
			bool Boomed;

            void Reset()
            {
                Boomed = false;

                DoCast(SPELL_AURA_BOOMBOT); // For achievement
                me->SetFloatValue(UNIT_FIELD_MIN_DAMAGE, 15000.0f);
                me->SetFloatValue(UNIT_FIELD_MAX_DAMAGE, 18000.0f);
                me->SetDisplayId(19139);
                me->SetSpeed(MOVE_RUN, 0.5f, true);

                if (Creature* xt002 = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_XT002_DECONSTRUCTOR) : 0))
                    me->GetMotionMaster()->MoveFollow(xt002, 0.0f, 0.0f);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage)
            {
                if (damage >= (me->GetHealth() - me->GetMaxHealth() * 0.5f) && !Boomed)
                {
                    Boomed = true;

                    WorldPacket data(SMSG_SPELLINSTAKILLLOG, 8+8+4);
                    data << uint64(me->GetGUID());
                    data << uint64(me->GetGUID());
                    data << uint32(SPELL_BOOM);
                    me->SendMessageToSet(&data, false);

                    me->DealDamage(me, me->GetHealth(), NULL, NODAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                    damage = 0;

                    me->m_Events.AddEvent(new BoomEvent(me), me->m_Events.CalculateTime(1*IN_MILLISECONDS));
                }
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_boombotAI(creature);
        }
};

class npc_void_zone : public CreatureScript
{
    public:
        npc_void_zone() : CreatureScript("npc_void_zone") { }

        struct npc_void_zoneAI : public QuantumBasicAI
        {
            npc_void_zoneAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                me->SetReactState(REACT_PASSIVE);
            }

			uint32 ConsumptionTimer;

            void Reset()
            {
                ConsumptionTimer = 3*IN_MILLISECONDS;
            }

            void UpdateAI(const uint32 diff)
            {
                if (ConsumptionTimer <= diff)
                {
                    int32 dmg = RAID_MODE(5000, 7500);
                    me->CastCustomSpell(me, RAID_MODE(SPELL_CONSUMPTION_10, SPELL_CONSUMPTION_25), &dmg, 0, 0, false);
                    ConsumptionTimer = 3*IN_MILLISECONDS;
                }
                else ConsumptionTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_void_zoneAI(creature);
        }
};

class npc_life_spark : public CreatureScript
{
    public:
        npc_life_spark() : CreatureScript("npc_life_spark") { }

        struct npc_life_sparkAI : public QuantumBasicAI
        {
            npc_life_sparkAI(Creature* creature) : QuantumBasicAI(creature){}

			uint32 ShockTimer;

            void Reset()
            {
                DoCast(me, RAID_MODE(SPELL_STATIC_CHARGED_10, SPELL_STATIC_CHARGED_25));
                ShockTimer = 0; // first one is immediate.
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (ShockTimer <= diff)
                {
                    if (me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        DoCastVictim(SPELL_SHOCK);
                        ShockTimer = 12*IN_MILLISECONDS;
                    }
                }
                else ShockTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_life_sparkAI(creature);
        }
};

class BombTargetSelector : public std::unary_function<Unit *, bool>
{
    public:
        BombTargetSelector(Creature* me, const Unit* victim) : _me(me), _victim(victim) {}

        bool operator() (Unit* target)
        {
            if (target == _victim && _me->getThreatManager().getThreatList().size() > 1)
                return true;

            return false;
        }

        Creature* _me;
        Unit const* _victim;
};

class spell_xt002_searing_light : public SpellScriptLoader
{
    public:
        spell_xt002_searing_light() : SpellScriptLoader("spell_xt002_searing_light") { }

        class spell_xt002_searing_light_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_searing_light_targeting_SpellScript);

            bool Load()
            {
                target = NULL;
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (BombTargetSelector(GetCaster()->ToCreature(), GetCaster()->GetVictim()));

                if (unitList.empty())
                    return;

                target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                unitList.clear();
                unitList.push_back(target);
            }

            void SetTarget(std::list<Unit*>& unitList)
            {
                unitList.clear();
                if (target)
                    unitList.push_back(target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_xt002_searing_light_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_xt002_searing_light_targeting_SpellScript::SetTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }

            Unit* target;
        };

        class spell_xt002_searing_light_spawn_life_spark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xt002_searing_light_spawn_life_spark_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetOwner()->ToPlayer())
				{
                    if (Unit* xt002 = GetCaster())
					{
                        if (xt002->HasAura(aurEff->GetAmount())) // Heartbreak aura indicating hard mode
                            xt002->SummonCreature(NPC_LIFE_SPARK, *player, TEMPSUMMON_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS);
					}
				}
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xt002_searing_light_spawn_life_spark_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_xt002_searing_light_targeting_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_xt002_searing_light_spawn_life_spark_AuraScript();
        }
};

class spell_xt002_gravity_bomb : public SpellScriptLoader
{
    public:
        spell_xt002_gravity_bomb() : SpellScriptLoader("spell_xt002_gravity_bomb") { }

        class spell_xt002_gravity_bomb_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_gravity_bomb_targeting_SpellScript);

            bool Load()
            {
                target = NULL;
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (BombTargetSelector(GetCaster()->ToCreature(), GetCaster()->GetVictim()));

                if (unitList.empty())
                    return;

                target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                unitList.clear();
                unitList.push_back(target);
            }

            void SetTarget(std::list<Unit*>& unitList)
            {
                unitList.clear();
                if (target)
                    unitList.push_back(target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_xt002_gravity_bomb_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_xt002_gravity_bomb_targeting_SpellScript::SetTarget, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
            }

            Unit* target;
        };

        class spell_xt002_gravity_bomb_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xt002_gravity_bomb_aura_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetOwner()->ToPlayer())
				{
                    if (Unit* xt002 = GetCaster())
					{
                        if (xt002->HasAura(aurEff->GetAmount())) // Heartbreak aura indicating hard mode
                            xt002->SummonCreature(NPC_VOID_ZONE, *player, TEMPSUMMON_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS);
					}
				}
            }

            void OnPeriodic(AuraEffect const* aurEff)
            {
                Unit* xt002 = GetCaster();
                if (!xt002)
                    return;

                Unit* owner = GetOwner()->ToUnit();
                if (!owner)
                    return;

                if (aurEff->GetAmount() >= int32(owner->GetHealth()))
				{
                    if (xt002->GetAI())
                        xt002->GetAI()->SetData(DATA_GRAVITY_BOMB_CASUALTY, 1);
				}
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xt002_gravity_bomb_aura_AuraScript::OnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_xt002_gravity_bomb_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_xt002_gravity_bomb_targeting_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_xt002_gravity_bomb_aura_AuraScript();
        }
};

class spell_xt002_gravity_bomb_damage : public SpellScriptLoader
{
    public:
        spell_xt002_gravity_bomb_damage() : SpellScriptLoader("spell_xt002_gravity_bomb_damage") { }

        class spell_xt002_gravity_bomb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_gravity_bomb_damage_SpellScript);

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (GetHitDamage() >= int32(GetHitUnit()->GetHealth()))
				{
                    if (caster->GetAI())
                        caster->GetAI()->SetData(DATA_GRAVITY_BOMB_CASUALTY, 1);
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_xt002_gravity_bomb_damage_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_xt002_gravity_bomb_damage_SpellScript();
        }
};

class spell_xt002_tympanic_tantrum : public SpellScriptLoader
{
    public:
        spell_xt002_tympanic_tantrum() : SpellScriptLoader("spell_xt002_tympanic_tantrum") { }

        class spell_xt002_tympanic_tantrum_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_tympanic_tantrum_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
				unitList.remove_if(PlayerOrPetCheck());
			}

			void RecalculateDamage()
			{
				SetHitDamage(GetHitUnit()->CountPctFromMaxHealth(GetHitDamage()));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_xt002_tympanic_tantrum_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_xt002_tympanic_tantrum_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
				OnHit += SpellHitFn(spell_xt002_tympanic_tantrum_SpellScript::RecalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_xt002_tympanic_tantrum_SpellScript();
        }
};

class achievement_nerf_engineering : public AchievementCriteriaScript
{
    public:
        achievement_nerf_engineering() : AchievementCriteriaScript("achievement_nerf_engineering") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target || !target->GetAI())
                return false;

            return !(target->GetAI()->GetData(DATA_HEALTH_RECOVERED));
        }
};

class achievement_heartbreaker : public AchievementCriteriaScript
{
    public:
        achievement_heartbreaker() : AchievementCriteriaScript("achievement_heartbreaker") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target || !target->GetAI())
                return false;

            return target->GetAI()->GetData(DATA_HARD_MODE);
        }
};

class achievement_nerf_gravity_bombs : public AchievementCriteriaScript
{
    public:
        achievement_nerf_gravity_bombs() : AchievementCriteriaScript("achievement_nerf_gravity_bombs") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target || !target->GetAI())
                return false;

            return !(target->GetAI()->GetData(DATA_GRAVITY_BOMB_CASUALTY));
        }
};

void AddSC_boss_xt_002_deconstructor()
{
    new boss_xt_002_deconstructor();
    new npc_xt002_heart();
    new npc_scrapbot();
    new npc_pummeller();
    new npc_boombot();
    new npc_void_zone();
    new npc_life_spark();
    new spell_xt002_searing_light();
    new spell_xt002_gravity_bomb();
    new spell_xt002_gravity_bomb_damage();
    new spell_xt002_tympanic_tantrum();
    new achievement_nerf_engineering();
    new achievement_heartbreaker();
    new achievement_nerf_gravity_bombs();
}