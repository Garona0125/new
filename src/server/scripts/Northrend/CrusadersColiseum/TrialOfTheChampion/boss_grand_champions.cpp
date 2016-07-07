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
#include "Vehicle.h"
#include "trial_of_the_champion.h"

enum Spells // 67870
{
	SPELL_VISUAL_SHIELD_1         = 63130,
    SPELL_VISUAL_SHIELD_2         = 63131,
    SPELL_VISUAL_SHIELD_3         = 63132,
	SPELL_THROW_VISUAL            = 45827,
	SPELL_CHARGE                  = 68284,
	SPELL_SHIELD_BREAKER          = 62575,
	SPELL_SHIELD_BREAKER_VISUAL   = 45815,
	SPELL_DEFEND                  = 66482,
	SPELL_THRUST                  = 62544,
    SPELL_KNEEL                   = 68442,
    SPELL_MORTAL_STRIKE           = 68783,
    SPELL_MORTAL_STRIKE_H         = 68784,
    SPELL_BLADESTORM              = 63784,
    SPELL_INTERCEPT               = 67540,
    SPELL_ROLLING_THROW           = 47115,
    SPELL_FIREBALL                = 66042,
    SPELL_FIREBALL_H              = 68310,
    SPELL_BLAST_WAVE              = 66044,
    SPELL_BLAST_WAVE_H            = 68312,
    SPELL_HASTE                   = 66045,
    SPELL_POLYMORPH               = 66043,
    SPELL_POLYMORPH_H             = 68311,
    SPELL_CHAIN_LIGHTNING         = 67529,
    SPELL_CHAIN_LIGHTNING_H       = 68319,
    SPELL_EARTH_SHIELD            = 67530,
    SPELL_HEALING_WAVE            = 67528,
    SPELL_HEALING_WAVE_H          = 68318,
    SPELL_HEX_OF_MENDING          = 67534,
    SPELL_HEX_OF_MENDING_HEAL     = 67535,
    SPELL_DISENGAGE               = 68339,
    SPELL_LIGHTNING_ARROWS        = 66085,
    SPELL_LIGHTNING_ARROWS_DAMAGE = 66095,
    SPELL_LIGHTNING_ARROWS_VISUAL = 66083,
    SPELL_MULTI_SHOT              = 66081,
    SPELL_SHOOT                   = 65868,
    SPELL_SHOOT_H                 = 67988,
    SPELL_DEADLY_POISON           = 67711,
    SPELL_EVISCERATE              = 67709,
    SPELL_EVISCERATE_H            = 68317,
    SPELL_FAN_OF_KNIVES           = 67706,
    SPELL_POISON_BOTTLE           = 67701,
    SPELL_DUAL_WIELD              = 42459,
};

Unit* GetRangedTarget(float minRange, float maxRange, WorldObject* source)
{
    Map* map = source->GetMap();
    if (map && map->IsDungeon())
    {
        std::list<Unit*> targets;
        Map::PlayerList const& Players = map->GetPlayers();
        for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
		{
            if (Player* player = itr->getSource())
            {
                if (player->IsGameMaster() || !player->IsAlive() || source->GetDistance(player) < minRange || source->GetDistance(player) > maxRange)
                    continue;

                targets.push_back(player);
            }
		}

        if (!targets.empty())
        {
            Unit* target = Quantum::DataPackets::SelectRandomContainerElement(targets);

            if (Unit* vehicle = target->GetVehicleBase())
                return vehicle;

            return target;
        }
    }

    return NULL;
}

struct npc_mounted_championAI : QuantumBasicAI
{
    npc_mounted_championAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;

    uint32 ChargeTimer;
    uint32 ShieldBreakerTimer;
    uint32 BuffTimer;

    bool Defeated;

    void Reset()
    {
        if (Defeated)
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

		DoCast(me, SPELL_UNIT_DETECTION_ALL);
		me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

        me->Mount(GetMountId());
        ChargeTimer = urand(1000, 5000);
        ShieldBreakerTimer = 8000;
        BuffTimer = urand(4000, 5000);
        Defeated = false;
    }

    uint32 GetMountId()
    {
        switch (me->GetEntry())
        {
			// Alliance
            case NPC_STORMWIND_CHAMPION:
				return 28912;
            case NPC_IRONFORGE_CHAMPION:
				return 29258;
            case NPC_GNOMEREGAN_CHAMPION:
				return 28571;
            case NPC_DARNASSUS_CHAMPION:
				return 29256;
            case NPC_EXODAR_CHAMPION:
				return 29255;
            case NPC_JACOB:
				return 29284;
            case NPC_AMBROSE:
				return 28571;
            case NPC_COLOSOS:
				return 29255;
            case NPC_JAELYNE:
				return 9991;
            case NPC_LANA:
				return 2787;
			// Horde
            case NPC_ORGRIMMAR_CHAMPION:
				return 29260;
            case NPC_THUNDER_BLUFF_CHAMPION:
				return 29259;
            case NPC_UNDERCITY_CHAMPION:
				return 29257;
            case NPC_SENJIN_CHAMPION:
				return 29261;
            case NPC_SILVERMOON_CHAMPION:
				return 29262;
            case NPC_MOKRA:
				return 29879;
            case NPC_ERESSEA:
				return 28607;
            case NPC_RUNOK:
				return 29880;
            case NPC_ZULTORE:
				return 29261;
            case NPC_VISCERI:
				return 10718;
        }
        return 0;
    }

    void DamageTaken(Unit* /*attacker*/, uint32 & damage)
    {
        if (!isGrandChampion())
            return;

        if (Defeated)
        {
            damage = 0;
            return;
        }

        if (damage >= me->GetHealth())
        {
            damage = 0;
            Defeated = true;
            me->GetMotionMaster()->MoveIdle();
            me->RemoveMount();
            me->RemoveAurasDueToSpell(SPELL_DEFEND);
            me->CastSpell(me, SPELL_KNEEL, true);
            me->SetTarget(0);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
    }

    uint32 GetData(uint32 type)
    {
        if (type == DATA_CHAMPION_DEFEATED)
            return Defeated ? 1 : 0;

        return 0;
    }
	
	void EnterEvadeMode()
	{
		if (GetRangedTarget(0.0f , 200.0f, me))
		{
			AggroAllPlayers(me);
			return;
		}
		
		QuantumBasicAI::EnterEvadeMode();
	}

    void EnterToBattle(Unit* /*who*/)
    {
		DoCastSpellDefend();

        if (instance->GetData(MINION_ENCOUNTER) != IN_PROGRESS)
            instance->SetData(MINION_ENCOUNTER, IN_PROGRESS);
    }

	void DoCastSpellDefend()
	{
		for (uint8 i = 0; i < 3; ++i)
		{
			DoCast(me, SPELL_DEFEND, true);
			DoCast(me, SPELL_VISUAL_SHIELD_1, true);
		}
	}

    void JustReachedHome()
    {
        if (instance->GetData(MINION_ENCOUNTER) == IN_PROGRESS)
            instance->SetData(MINION_ENCOUNTER, FAIL);
    }

    bool isGrandChampion()
    {
		return (me->GetEntry() == NPC_MOKRA || me->GetEntry() == NPC_JACOB ||
			me->GetEntry() == NPC_ERESSEA || me->GetEntry() == NPC_AMBROSE ||
			me->GetEntry() == NPC_RUNOK || me->GetEntry() == NPC_COLOSOS ||
			me->GetEntry() == NPC_ZULTORE || me->GetEntry() == NPC_JAELYNE ||
			me->GetEntry() == NPC_VISCERI || me->GetEntry() == NPC_LANA);
    }

    bool isInMountedGauntlet()
    {
        if (Creature* announcer = me->GetCreature(*me, instance->GetData64(DATA_ANNOUNCER)))
        {
            if (announcer->AI()->GetData(EVENT_INTRO) == IN_PROGRESS || announcer->AI()->GetData(EVENT_WAVES) == IN_PROGRESS)
                return true;
        }
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Defeated)
            return;

        if (BuffTimer <= diff)
        {
            Aura* defend = me->GetAura(SPELL_DEFEND);
            if (!defend || defend->GetStackAmount() < 3)
            {
                DoCast(me, SPELL_DEFEND, true);
				DoCast(me, SPELL_VISUAL_SHIELD_1, true);
                BuffTimer = urand(4000, 5000);
            }
			else BuffTimer = urand(1000, 2000);
        }
		else BuffTimer -= diff;

        if (ChargeTimer <= diff)
        {
            if (Unit* target = GetRangedTarget(8.0f, 25.0f, me))
            {
                me->AddThreat(target, 5.0f);
                DoCast(target, SPELL_CHARGE, true);
				ChargeTimer = 7000;
			}
        }
		else ChargeTimer -= diff;

        if (ShieldBreakerTimer <= diff)
        {
            if (Unit* target = GetRangedTarget(10.0f, 30.0f, me))
			{
                DoCast(target, SPELL_SHIELD_BREAKER, true);
				ShieldBreakerTimer = 9000;
			}
        }
		else ShieldBreakerTimer -= diff;

		// Use Thrust instead of melee attack
        if (me->IsAttackReady() && me->IsWithinMeleeRange(me->GetVictim()))
        {
            me->AddUnitState(UNIT_STATE_ON_VEHICLE);
            DoCastVictim(SPELL_THRUST);
            me->ResetAttackTimer();
            me->ClearUnitState(UNIT_STATE_ON_VEHICLE);
        }
    }
};

class generic_vehicleAI_toc5 : public CreatureScript
{
public:
    generic_vehicleAI_toc5() : CreatureScript("generic_vehicleAI_toc5") { }

    struct generic_vehicleAI_toc5AI : public npc_mounted_championAI
    {
        generic_vehicleAI_toc5AI(Creature* creature) : npc_mounted_championAI(creature) {}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new generic_vehicleAI_toc5AI(creature);
    }
};

class boss_warrior_toc5 : public CreatureScript
{
public:
    boss_warrior_toc5() : CreatureScript("boss_warrior_toc5") { }

    // Marshal Jacob Alerius && Mokra the Skullcrusher || Warrior
    struct boss_warrior_toc5AI : public npc_mounted_championAI
    {
        boss_warrior_toc5AI(Creature* creature) : npc_mounted_championAI(creature) {}

        uint32 BladeStormTimer;
        uint32 InterceptTimer;
        uint32 MortalStrikeTimer;

        bool Defeated;

        void Reset()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::Reset();
                return;
            }

            Defeated = false;
            BladeStormTimer = urand(15000, 20000);
            InterceptTimer = 7000;
            MortalStrikeTimer = urand(8000, 12000);
            me->SetWalk(false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::DamageTaken(NULL, damage);
                return;
            }

            if (Defeated)
            {
                damage = 0;
                return;
            }

            // Prevent damage from finishing hit and mark creature as Defeated
            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                BindPlayersToInstance(me);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            // Knee at home position after being Defeated
            if (type == POINT_MOTION_TYPE && id == 1)
                me->CastSpell(me, SPELL_KNEEL, true);
        }

        uint32 GetData(uint32 type)
        {
            if (isInMountedGauntlet())
                return npc_mounted_championAI::GetData(type);

            // Used by Announcer on periodic check of the bosses state
            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void EnterToBattle(Unit* who)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::EnterToBattle(who);
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) != IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);
			}
        }

        void JustReachedHome()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::JustReachedHome();
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) == IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, FAIL);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::UpdateAI(diff);
                return;
            }

            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (InterceptTimer <= diff)
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (me->GetMap()->IsDungeon() && !players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        Player* player = itr->getSource();
                        if (player && !player->IsGameMaster() && me->IsInRange(player, 8.0f, 25.0f, false))
                        {
                            me->AddThreat(player, 5.0f);
                            DoCast(player, SPELL_INTERCEPT);
                            break;
						}
					}
				}
				InterceptTimer = 7000;
            }
			else InterceptTimer -= diff;

            if (BladeStormTimer <= diff)
            {
                DoCastVictim(SPELL_BLADESTORM);
                BladeStormTimer = urand(15000, 20000);
            }
			else BladeStormTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = urand(8000, 12000);
            }
			else MortalStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_warrior_toc5AI(creature);
    }
};

class boss_mage_toc5 : public CreatureScript
{
public:
    boss_mage_toc5() : CreatureScript("boss_mage_toc5") { }

    struct boss_mage_toc5AI : public npc_mounted_championAI
    {
        boss_mage_toc5AI(Creature* creature) : npc_mounted_championAI(creature) {}

        uint32 FireBallTimer;
        uint32 BlastWaveTimer;
        uint32 HasteTimer;
        uint32 PolymorphTimer;
        bool Defeated;

        void Reset()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::Reset();
                return;
            }

            Defeated = false;
            FireBallTimer = 2000;
            PolymorphTimer = 8000;
            BlastWaveTimer = 12000;
            HasteTimer = 22000;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetWalk(false);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::DamageTaken(NULL, damage);
                return;
            }

            if (Defeated)
            {
                damage = 0;
                return;
            }

            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                BindPlayersToInstance(me);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
			{
                me->CastSpell(me, SPELL_KNEEL, true);
			}
        }

        uint32 GetData(uint32 type)
        {
            if (isInMountedGauntlet())
                return npc_mounted_championAI::GetData(type);

            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void EnterToBattle(Unit* who)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::EnterToBattle(who);
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) != IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);
			}
        }

        void JustReachedHome()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::JustReachedHome();
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) == IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, FAIL);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::UpdateAI(diff);
                return;
            }

            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (PolymorphTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    DoCast(target, SPELL_POLYMORPH);
                PolymorphTimer = 8000;
            }
			else PolymorphTimer -= diff;

            if (BlastWaveTimer <= diff)
            {
                DoCastAOE(SPELL_BLAST_WAVE, false);
                BlastWaveTimer = 13000;
            }
			else BlastWaveTimer -= diff;

            if (HasteTimer <= diff)
            {
                me->InterruptNonMeleeSpells(true);

                DoCast(me, SPELL_HASTE);
                HasteTimer = 22000;
            }
			else HasteTimer -= diff;

            if (FireBallTimer <= diff)
            {
                DoCastVictim(SPELL_FIREBALL);
                FireBallTimer = 2600;
            }
			else FireBallTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_mage_toc5AI(creature);
    }
};

class boss_shaman_toc5 : public CreatureScript
{
public:
    boss_shaman_toc5() : CreatureScript("boss_shaman_toc5") { }

    // Colosos && Runok Wildmane || Shaman
    struct boss_shaman_toc5AI : public npc_mounted_championAI
    {
        boss_shaman_toc5AI(Creature* creature) : npc_mounted_championAI(creature) {}

        uint32 ChainLightningTimer;
        uint32 EarthShieldTimer;
        uint32 HealingWaveTimer;
        uint32 HexMendingTimer;
        bool Defeated;

        void Reset()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::Reset();
                return;
            }

            Defeated = false;
            ChainLightningTimer = 5000;
            HealingWaveTimer = 12000;
            EarthShieldTimer = urand(30000, 35000);
            HexMendingTimer = urand(1000, 5000);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetWalk(false);
        }

        void EnterToBattle(Unit* who)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::EnterToBattle(who);
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) != IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);
			}

            DoCast(me, SPELL_EARTH_SHIELD);
        }

        void JustReachedHome()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::JustReachedHome();
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) == IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, FAIL);
			}
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::DamageTaken(NULL, damage);
                return;
            }

            if (Defeated)
            {
                damage = 0;
                return;
            }

            // Prevent damage from finishing hit and mark creature as Defeated
            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                BindPlayersToInstance(me);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            // Knee at home position after being Defeated
            if (type == POINT_MOTION_TYPE && id == 1)
                me->CastSpell(me, SPELL_KNEEL, true);
        }

        uint32 GetData(uint32 type)
        {
            if (isInMountedGauntlet())
                return npc_mounted_championAI::GetData(type);

            // Used by Announcer on periodic check of the bosses state
            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::UpdateAI(diff);
                return;
            }

            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (ChainLightningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    DoCast(target, SPELL_CHAIN_LIGHTNING);

                ChainLightningTimer = 8000;
            }
			else ChainLightningTimer -= diff;

            if (HealingWaveTimer <= diff)
            {
                bool chance = urand(0, 1);

                if (!chance)
                {
                    if (Unit* friendUnit = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
                    {
                        DoCast(friendUnit, SPELL_HEALING_WAVE);
                        HealingWaveTimer = 5000;
                    }
                }
				else
                {
                    DoCast(me, SPELL_HEALING_WAVE);
                    HealingWaveTimer = 8000;
                }
            }
			else HealingWaveTimer -= diff;

            if (EarthShieldTimer <= diff)
            {
                Unit* friendUnit = SelectTargetHpFriendly(HEAL_PERCENT_GREATER);

                if (friendUnit && friendUnit->HealthBelowPct(HEALTH_PERCENT_60) && !friendUnit->HasAura(SPELL_EARTH_SHIELD))
                {
                    DoCast(friendUnit, SPELL_EARTH_SHIELD);
                    EarthShieldTimer = urand(30000, 35000);
                }
				else if (!me->HasAura(SPELL_EARTH_SHIELD))
                {
                    DoCast(me, SPELL_EARTH_SHIELD);
                    EarthShieldTimer = urand(30000, 35000);
                }
				else EarthShieldTimer = urand(1500, 3500);
            }
			else EarthShieldTimer -= diff;

            if (HexMendingTimer <= diff)
            {
                DoCastVictim(SPELL_HEX_OF_MENDING, true);
                HexMendingTimer = urand(12000, 15000);
            }
			else HexMendingTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_shaman_toc5AI(creature);
    }
};

class boss_hunter_toc5 : public CreatureScript
{
public:
    boss_hunter_toc5() : CreatureScript("boss_hunter_toc5") { }

    // Jaelyne Evensong && Zul'tore || Hunter
    struct boss_hunter_toc5AI : public npc_mounted_championAI
    {
        boss_hunter_toc5AI(Creature* creature) : npc_mounted_championAI(creature) {}

        uint32 MultiShotTimer;
        uint32 LightningArrowsTimer;
        uint32 DisengageTimer;

        bool LigthingArrows;
        bool Defeated;

        void Reset()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::Reset();
                return;
            }

            if (Defeated)
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            Defeated = false;
            LigthingArrows = false;
            MultiShotTimer = 2000;
            DisengageTimer = 3000;
            LightningArrowsTimer = 7000;
            me->SetSheath(SHEATH_STATE_RANGED);
            me->SetStatFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, IsHeroic() ? 5000.0f : 3000.0f);
            me->SetStatFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, IsHeroic() ? 6000.0f : 4000.0f);
            me->SetWalk(false);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::DamageTaken(NULL, damage);
                return;
            }

            if (Defeated)
            {
                damage = 0;
                return;
            }

            // Prevent damage from finishing hit and mark creature as Defeated
            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                BindPlayersToInstance(me);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            // Knee at home position after being Defeated
            if (type == POINT_MOTION_TYPE && id == 1)
                me->CastSpell(me, SPELL_KNEEL, true);
        }

        uint32 GetData(uint32 type)
        {
            if (isInMountedGauntlet())
                return npc_mounted_championAI::GetData(type);

            // Used by Announcer on periodic check of the bosses state
            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void EnterToBattle(Unit* who)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::EnterToBattle(who);
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) != IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);
			}
        }

        void JustReachedHome()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::JustReachedHome();
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) == IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, FAIL);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::UpdateAI(diff);
                return;
            }

            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->GetDistance(me->GetVictim()) >= 30.0f)
                me->GetMotionMaster()->MoveChase(me->GetVictim(), 25.0f);

            if (DisengageTimer <= diff)
            {
                if (me->GetDistance(me->GetVictim()) <= 3.0f)
                {
                    DoCast(SPELL_DISENGAGE);
                    DisengageTimer = 7000;
                }
                else DisengageTimer = 1000;
            }
			else DisengageTimer -= diff;

            if (LightningArrowsTimer <= diff && !me->HasAura(SPELL_LIGHTNING_ARROWS))
            {
                DoCastAOE(SPELL_LIGHTNING_ARROWS_VISUAL, false);
                LightningArrowsTimer = 14000;
                LigthingArrows = true;
                return;
            }
			else LightningArrowsTimer -= diff;

            // Trigger the aura after the visual storm
            if (LigthingArrows && !me->HasAura(SPELL_LIGHTNING_ARROWS_VISUAL))
            {
                DoCast(SPELL_LIGHTNING_ARROWS);
                LigthingArrows = false;
            }

            if (MultiShotTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    if (target->IsInRange(me, 5.0f, 35.0f))
                    {
                        DoCast(target, SPELL_MULTI_SHOT);
                        MultiShotTimer = 6000;
                    }
					else MultiShotTimer = 1000;
				}
            }
			else MultiShotTimer -= diff;

            // Shoot instead of meele swing
            if (me->IsAttackReady())
            {
                if (Aura* lArrows = me->GetAura(SPELL_LIGHTNING_ARROWS))
                {
                    DoCastVictim(SPELL_LIGHTNING_ARROWS_DAMAGE, true);
                    lArrows->ModCharges(-1);
                }
                DoCastVictim(SPELL_SHOOT, true);
                me->ResetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_hunter_toc5AI(creature);
    }
};

class boss_rouge_toc5 : public CreatureScript
{
public:
    boss_rouge_toc5() : CreatureScript("boss_rouge_toc5") { }

    // Lana Stouthammer Evensong && Deathstalker Visceri || Rouge
    struct boss_rouge_toc5AI : public npc_mounted_championAI
    {
        boss_rouge_toc5AI(Creature* creature) : npc_mounted_championAI(creature) {}

        uint32 EviscerateTimer;
        uint32 FanKivesTimer;
        uint32 PosionBottleTimer;

        bool Defeated;

        void Reset()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::Reset();
                return;
            }

            if (Defeated)
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            Defeated = false;
            EviscerateTimer = 8000;
            FanKivesTimer = 14000;
            PosionBottleTimer = 19000;

            // Settings for offhand attack
            DoCast(me, SPELL_DUAL_WIELD, true);
            me->SetAttackTime(OFF_ATTACK, 1500);
            me->SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, IsHeroic() ? 5000.0f : 3000.0f);
            me->SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, IsHeroic() ? 6000.0f : 4000.0f);
            me->SetWalk(false);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::DamageTaken(NULL, damage);
                return;
            }

            if (Defeated)
            {
                damage = 0;
                return;
            }

            // Prevent damage from finishing hit and mark creature as Defeated
            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                BindPlayersToInstance(me);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            // Knee at home position after being Defeated
            if (type == POINT_MOTION_TYPE && id == 1)
                me->CastSpell(me, SPELL_KNEEL, true);
        }

        uint32 GetData(uint32 type)
        {
            if (isInMountedGauntlet())
                return npc_mounted_championAI::GetData(type);

            // Used by Announcer on periodic check of the bosses state
            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void EnterToBattle(Unit* who)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::EnterToBattle(who);
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) != IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);
			}

            DoCast(me, SPELL_DEADLY_POISON);
        }

        void JustReachedHome()
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::JustReachedHome();
                return;
            }

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_GRAND_CHAMPIONS) == IN_PROGRESS)
                    instance->SetData(BOSS_GRAND_CHAMPIONS, FAIL);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (isInMountedGauntlet())
            {
                npc_mounted_championAI::UpdateAI(diff);
                return;
            }

            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (EviscerateTimer <= diff)
            {
                DoCastVictim(SPELL_EVISCERATE);
                EviscerateTimer = 8000;
            }
			else EviscerateTimer -= diff;

            if (FanKivesTimer <= diff)
            {
                DoCastAOE(SPELL_FAN_OF_KNIVES, false);
                FanKivesTimer = 14000;
            }
			else FanKivesTimer -= diff;

            if (PosionBottleTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_POISON_BOTTLE);
					PosionBottleTimer = 19000;
				}
            }
			else PosionBottleTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_rouge_toc5AI(creature);
    }
};

class spell_toc5_ride_mount : public SpellScriptLoader
{
    public:
        spell_toc5_ride_mount() : SpellScriptLoader("spell_toc5_ride_mount") {}

        class spell_toc5_ride_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toc5_ride_mount_SpellScript);

            SpellCastResult CheckRequirement()
            {

                if (GetCaster()->GetUInt32Value(PLAYER_VISIBLE_ITEM_16_ENTRY_ID) == 46106 || GetCaster()->GetUInt32Value(PLAYER_VISIBLE_ITEM_16_ENTRY_ID) == 46069 || GetCaster()->GetUInt32Value(PLAYER_VISIBLE_ITEM_16_ENTRY_ID) == 46070)
                {
                    GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                    return SPELL_CAST_OK;
                }
				else
				{
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_toc5_ride_mount_SpellScript::CheckRequirement);
            }
        };

        class spell_toc5_ride_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_toc5_ride_mount_AuraScript);

            void HandleOnEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->RemoveAurasDueToSpell(SPELL_DEFEND);

                if (Unit* caster = GetCaster())
                {
                    caster->RemoveAurasDueToSpell(SPELL_DEFEND);
                    for (uint8 i = 0; i<3; i++)
                        caster->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 +i);
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_toc5_ride_mount_AuraScript::HandleOnEffect, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_toc5_ride_mount_AuraScript::HandleOnEffect, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_toc5_ride_mount_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_toc5_ride_mount_SpellScript();
        }
};

class player_hex_mendingAI : public PlayerAI
{
    public:
        player_hex_mendingAI(Player* player) : PlayerAI(player) {}

        void HealReceived(Unit* healer, uint32 & addHealth)
        {
            PlayerAI::HealReceived(healer, addHealth);
            me->CastCustomSpell(SPELL_HEX_OF_MENDING_HEAL, SPELLVALUE_BASE_POINT0, int32(addHealth*2.0f), me, true);
        }

        void UpdateAI(const uint32 /*diff*/) { }

    private:
        uint64 casterGUID;
};

class spell_toc5_hex_mending : public SpellScriptLoader
{
    public:
        spell_toc5_hex_mending() : SpellScriptLoader("spell_toc5_hex_mending") { }

        class spell_toc5_hex_mending_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_toc5_hex_mending_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                oldAI = GetTarget()->GetAI();
                GetTarget()->SetAI(new player_hex_mendingAI(GetTarget()->ToPlayer()));
                oldAIState = GetTarget()->IsAIEnabled;
                GetTarget()->IsAIEnabled = true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                delete GetTarget()->GetAI();
                GetTarget()->SetAI(oldAI);
                GetTarget()->IsAIEnabled = oldAIState;
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_toc5_hex_mending_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_toc5_hex_mending_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

            UnitAI* oldAI;
            bool oldAIState;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_toc5_hex_mending_AuraScript();
        }
};

void AddSC_boss_grand_champions()
{
    new generic_vehicleAI_toc5();
    new boss_warrior_toc5();
    new boss_mage_toc5();
    new boss_shaman_toc5();
    new boss_hunter_toc5();
    new boss_rouge_toc5();
    new spell_toc5_ride_mount();
    new spell_toc5_hex_mending();
}