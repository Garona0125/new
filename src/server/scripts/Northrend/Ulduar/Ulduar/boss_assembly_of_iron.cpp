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
    SAY_STEELBREAKER_AGGRO                      = -1603020,
    SAY_STEELBREAKER_SLAY_1                     = -1603021,
    SAY_STEELBREAKER_SLAY_2                     = -1603022,
    SAY_STEELBREAKER_POWER                      = -1603023,
    SAY_STEELBREAKER_DEATH_1                    = -1603024,
    SAY_STEELBREAKER_DEATH_2                    = -1603025,
    SAY_STEELBREAKER_BERSERK                    = -1603026,

    SAY_MOLGEIM_AGGRO                           = -1603030,
    SAY_MOLGEIM_SLAY_1                          = -1603031,
    SAY_MOLGEIM_SLAY_2                          = -1603032,
    SAY_MOLGEIM_RUNE_DEATH                      = -1603033,
    SAY_MOLGEIM_SUMMON                          = -1603034,
    SAY_MOLGEIM_DEATH_1                         = -1603035,
    SAY_MOLGEIM_DEATH_2                         = -1603036,
    SAY_MOLGEIM_BERSERK                         = -1603037,

    SAY_BRUNDIR_AGGRO                           = -1603040,
    SAY_BRUNDIR_SLAY_1                          = -1603041,
    SAY_BRUNDIR_SLAY_2                          = -1603042,
    SAY_BRUNDIR_SPECIAL                         = -1603043,
    SAY_BRUNDIR_FLIGHT                          = -1603044,
    SAY_BRUNDIR_DEATH_1                         = -1603045,
    SAY_BRUNDIR_DEATH_2                         = -1603046,
    SAY_BRUNDIR_BERSERK                         = -1603047,
	EMOTE_OVERLOAD                              = -1613229,
	// After Open Archivum Door
	// You\'ve defeated the Iron Council and unlocked the Archivum! Well done lads! sound id 15827
};

enum Spells
{
    // Any boss
    SPELL_SUPERCHARGE                   = 61920,
    SPELL_BERSERK                       = 47008, // Hard enrage, don't know the correct ID.
    SPELL_CREDIT_MARKER                 = 65195, // spell_dbc
    SPELL_IRON_BOOT_FLASK               = 58501,
    // Steelbreaker
    SPELL_HIGH_VOLTAGE_10               = 61890,
    SPELL_HIGH_VOLTAGE_25               = 63498,
    SPELL_FUSION_PUNCH_10               = 61903,
    SPELL_FUSION_PUNCH_25               = 63493,
    SPELL_STATIC_DISRUPTION_10          = 61911,
    SPELL_STATIC_DISRUPTION_25          = 63495,
	SPELL_STATIC_DISRUPTION_PROC_10     = 61912,
	SPELL_STATIC_DISRUPTION_PROC_25     = 63494,
	SPELL_OVERWHELMING_POWER_10         = 64637,
    SPELL_OVERWHELMING_POWER_25         = 61888,
    SPELL_MELTDOWN                      = 61889,
    SPELL_ELECTRICAL_CHARGE             = 61900,
    SPELL_ELECTRICAL_CHARGE_TRIGGER     = 61901,
    SPELL_ELECTRICAL_CHARGE_TRIGGERED   = 61902,
    // Runemaster Molgeim
    SPELL_SHIELD_OF_RUNES_10            = 62274,
	SPELL_SHIELD_OF_RUNES_25            = 63489,
    SPELL_SHIELD_OF_RUNES_BUFF          = 62277,
    SPELL_SHIELD_OF_RUNES_BUFF_25       = 63967,
    SPELL_SUMMON_RUNE_OF_POWER          = 63513,
    SPELL_RUNE_OF_POWER                 = 61974,
    SPELL_RUNE_OF_DEATH                 = 62269,
    SPELL_RUNE_OF_SUMMONING             = 62273,
    SPELL_RUNE_OF_SUMMONING_VIS         = 62019,
    SPELL_RUNE_OF_SUMMONING_SUMMON      = 62020,
    SPELL_LIGHTNING_BLAST_10            = 62054,
    SPELL_LIGHTNING_BLAST_25            = 63491,
    // Stormcaller Brundir
    SPELL_CHAIN_LIGHTNING_10            = 61879,
    SPELL_CHAIN_LIGHTNING_25            = 63479,
    SPELL_OVERLOAD_10                   = 61869,
    SPELL_OVERLOAD_25                   = 63481,
    SPELL_LIGHTNING_WHIRL_10            = 61915,
    SPELL_LIGHTNING_WHIRL_25            = 63483,
    SPELL_LIGHTNING_WHIRL_DMG_10        = 61916,
    SPELL_LIGHTNING_WHIRL_DMG_25        = 63482,
    SPELL_LIGHTNING_TENDRILS_10         = 61887,
    SPELL_LIGHTNING_TENDRILS_25         = 63486,
	SPELL_LIGHTNING_TENDRILS_VISUAL     = 61883,
    SPELL_STORMSHIELD                   = 64187,
	SPELL_LIGHTNING_BEAM_CHANNEL        = 45537,
	// Elementals
	SPELL_ELEMENTAL_PASSIVE_10          = 62052,
	SPELL_ELEMENTAL_PASSIVE_25          = 63492,
};

enum Events
{
    EVENT_UPDATEPHASE              = 1,
    EVENT_ENRAGE                   = 2,
    // Steelbreaker
    EVENT_FUSION_PUNCH             = 3,
    EVENT_STATIC_DISRUPTION        = 4,
    EVENT_OVERWHELMING_POWER       = 5,
    // Molgeim
    EVENT_RUNE_OF_POWER            = 6,
    EVENT_SHIELD_OF_RUNES          = 7,
    EVENT_RUNE_OF_DEATH            = 8,
    EVENT_RUNE_OF_SUMMONING        = 9,
    EVENT_LIGHTNING_BLAST          = 10,
    // Brundir
    EVENT_CHAIN_LIGHTNING          = 11,
    EVENT_OVERLOAD                 = 12,
    EVENT_LIGHTNING_WHIRL          = 13,
    EVENT_LIGHTNING_TENDRILS_START = 14,
    EVENT_LIGHTNING_TENDRILS_END   = 15,
    EVENT_THREAT_WIPE              = 16,
    EVENT_STORMSHIELD              = 17,
};

enum MovePoints
{
    POINT_FLY   = 1,
    POINT_LAND  = 2,
    POINT_CHASE = 3,
};

enum Data
{
    DATA_I_CHOOSE_YOU = 1,
    DATA_CANT_DO_THAT = 2,
};

bool IsEncounterComplete(InstanceScript* instance, Creature* me)
{
   if (!instance || !me)
        return false;

    for (uint8 i = 0; i < 3; ++i)
    {
        uint64 guid = instance->GetData64(DATA_STEELBREAKER + i);
        if (!guid)
            return false;

        if (Creature* boss = Unit::GetCreature(*me, guid))
        {
            if (boss->IsAlive())
                return false;
        }
        else
            return false;
    }
    return true;
}

void RespawnEncounter(InstanceScript* instance, Creature* me)
{
    for (uint8 i = 0; i < 3; ++i)
    {
        uint64 guid = instance->GetData64(DATA_STEELBREAKER + i);
        if (!guid)
            continue;

        if (Creature* boss = Unit::GetCreature(*me, guid))
        {
            if (!boss->IsAlive())
            {
                boss->Respawn();
                boss->GetMotionMaster()->MoveTargetedHome();
            }
        }
    }
}

void StartEncounter(InstanceScript* instance, Creature* me, Unit* /*target*/)
{
    if (instance->GetBossState(DATA_ASSEMBLY_OF_IRON) == IN_PROGRESS)
        return; // Prevent recursive calls

    instance->SetBossState(DATA_ASSEMBLY_OF_IRON, IN_PROGRESS);

    for (uint8 i = 0; i < 3; ++i)
    {
        uint64 guid = instance->GetData64(DATA_STEELBREAKER + i);
        if (!guid)
            continue;

        if (Creature* boss = Unit::GetCreature(*me, guid))
		{
			boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            boss->SetInCombatWithZone();
		}
    }
}

bool UpdateSupercharge(Creature* target)
{
    if (Aura* Supercharge = target->GetAura(SPELL_SUPERCHARGE))
    {
        Supercharge->ModStackAmount(1);
        if (UnitAI* AI = target->GetAI())
        {
            AI->DoAction(EVENT_UPDATEPHASE);
            return true;
        }
    }
    return false;
}

class boss_steelbreaker : public CreatureScript
{
    public:
        boss_steelbreaker() : CreatureScript("boss_steelbreaker") { }

        struct boss_steelbreakerAI : public BossAI
        {
            boss_steelbreakerAI(Creature* creature) : BossAI(creature, DATA_ASSEMBLY_OF_IRON) {}

			uint32 Phase;

            void Reset()
            {
                _Reset();
                Phase = 0;
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                me->RemoveAllAuras();
                me->RemoveLootMode(LOOT_MODE_DEFAULT);

                if (instance)
					RespawnEncounter(instance, me);
            }

            void EnterToBattle(Unit* who)
            {
                me->SetActive(true);
                StartEncounter(instance, me, who);
                DoSendQuantumText(SAY_STEELBREAKER_AGGRO, me);
                DoZoneInCombat();
                DoCast(me, RAID_MODE(SPELL_HIGH_VOLTAGE_10, SPELL_HIGH_VOLTAGE_25));
                events.ScheduleEvent(EVENT_ENRAGE, 900000);
                events.ScheduleEvent(EVENT_FUSION_PUNCH, 15000);
                DoAction(EVENT_UPDATEPHASE);
            }

			void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_STEELBREAKER_SLAY_1, SAY_STEELBREAKER_SLAY_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(RAND(SAY_STEELBREAKER_DEATH_1, SAY_STEELBREAKER_DEATH_2), me);

                if (IsEncounterComplete(instance, me) && instance)
                {
                    _JustDied();
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CREDIT_MARKER);
                }
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_I_CHOOSE_YOU)
                    return (Phase >= 3) ? 1 : 0;

                return 0;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case EVENT_UPDATEPHASE:
                        events.SetPhase(++Phase);
                        if (Phase == 2)
                            events.ScheduleEvent(EVENT_STATIC_DISRUPTION, 30000);
                        if (Phase == 3)
                        {
                            me->ResetLootMode();
                            DoCast(me, SPELL_ELECTRICAL_CHARGE, true);
                            events.ScheduleEvent(EVENT_OVERWHELMING_POWER, 3000);
                        }
						break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    bool HasSupercharge = false;

                    if (Creature* Brundir = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_BRUNDIR) : 0))
                    {
                        if (Brundir->IsAlive())
                        {
                            Brundir->SetFullHealth();
                            HasSupercharge = UpdateSupercharge(Brundir);
                        }
                    }

                    if (Creature* Molgeim = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MOLGEIM) : 0))
                    {
                        if (Molgeim->IsAlive())
                        {
                            Molgeim->SetFullHealth();
                            HasSupercharge = UpdateSupercharge(Molgeim);
                        }
                    }

                    if (!HasSupercharge)
                        DoCast(me, SPELL_SUPERCHARGE, true);
                }
            }

            void SpellHit(Unit* /*from*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_SUPERCHARGE:
                        DoAction(EVENT_UPDATEPHASE);
                        break;
                    case SPELL_ELECTRICAL_CHARGE_TRIGGERED:
                        if (!me->IsInCombatActive())
                            me->RemoveAurasDueToSpell(SPELL_ELECTRICAL_CHARGE_TRIGGERED);
                        break;
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_MELTDOWN && target && target->ToCreature())
                    target->CastSpell(me, SPELL_ELECTRICAL_CHARGE_TRIGGER, true);
            }

            // try to prefer ranged targets
            Unit* GetDisruptionTarget()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    std::list<Player*> playerList;
                    Map::PlayerList const& Players = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                    {
                        if (Player* player = itr->getSource())
                        {
                            if (player->IsDead() || player->IsGameMaster())
                                continue;

                            float Distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            if (Distance < 15.0f || Distance > 100.0f)
                                continue;

                            playerList.push_back(player);
                        }
                    }

                    if (playerList.empty())
                        return NULL;

                    return Quantum::DataPackets::SelectRandomContainerElement(playerList);
                }
                else
                    return NULL;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENRAGE:
                            DoSendQuantumText(SAY_STEELBREAKER_BERSERK, me);
                            DoCast(SPELL_BERSERK);
                            break;
                        case EVENT_FUSION_PUNCH:
                            DoCastVictim(RAID_MODE<uint32>(SPELL_FUSION_PUNCH_10, SPELL_FUSION_PUNCH_25));
                            events.ScheduleEvent(EVENT_FUSION_PUNCH, urand(13000, 22000));
                            break;
                        case EVENT_STATIC_DISRUPTION:
                        {
                            Unit* target = GetDisruptionTarget();
                            if (!target)
                                target = SelectTarget(TARGET_RANDOM, 0, 100.0f, true);
                            if (target)
                                DoCast(target, RAID_MODE<uint32>(SPELL_STATIC_DISRUPTION_10, SPELL_STATIC_DISRUPTION_25));
                            events.ScheduleEvent(EVENT_STATIC_DISRUPTION, 20000);
                            break;
                        }
                        case EVENT_OVERWHELMING_POWER:
                            if (me->GetVictim() && !me->GetVictim()->HasAura(RAID_MODE<uint32>(SPELL_OVERWHELMING_POWER_10, SPELL_OVERWHELMING_POWER_25)))
                            {
                                DoSendQuantumText(SAY_STEELBREAKER_POWER, me);
                                DoCastVictim(RAID_MODE<uint32>(SPELL_OVERWHELMING_POWER_10, SPELL_OVERWHELMING_POWER_25));
                            }
                            events.ScheduleEvent(EVENT_OVERWHELMING_POWER, 2000);
                            break;
                    }
                }
				DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_steelbreakerAI(creature);
        }
};

class boss_runemaster_molgeim : public CreatureScript
{
    public:
        boss_runemaster_molgeim() : CreatureScript("boss_runemaster_molgeim") { }

        struct boss_runemaster_molgeimAI : public BossAI
        {
            boss_runemaster_molgeimAI(Creature* creature) : BossAI(creature, DATA_ASSEMBLY_OF_IRON){}

			uint32 Phase;

            void Reset()
            {
                _Reset();
                Phase = 0;
				me->RemoveAllAuras();
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                me->RemoveLootMode(LOOT_MODE_DEFAULT);
                if (instance)
                    RespawnEncounter(instance, me);
            }

            void EnterToBattle(Unit* who)
            {
                me->SetActive(true);
                StartEncounter(instance, me, who);
				DoSendQuantumText(SAY_MOLGEIM_AGGRO, me);
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_ENRAGE, 900000);
                events.ScheduleEvent(EVENT_SHIELD_OF_RUNES, 27000);
                events.ScheduleEvent(EVENT_RUNE_OF_POWER, 60000);
                DoAction(EVENT_UPDATEPHASE);
            }

			void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_MOLGEIM_SLAY_1, SAY_MOLGEIM_SLAY_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(RAND(SAY_MOLGEIM_DEATH_1, SAY_MOLGEIM_DEATH_2), me);

                if (IsEncounterComplete(instance, me) && instance)
                {
                    _JustDied();

                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CREDIT_MARKER);
                }
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_I_CHOOSE_YOU)
                    return (Phase >= 3) ? 1 : 0;

                return 0;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case EVENT_UPDATEPHASE:
                        events.SetPhase(++Phase);
                        if (Phase == 2)
                            events.ScheduleEvent(EVENT_RUNE_OF_DEATH, 30000);
                        if (Phase == 3)
                        {
                            me->ResetLootMode();
                            events.ScheduleEvent(EVENT_RUNE_OF_SUMMONING, urand(20000, 30000));
                        }
						break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    bool HasSupercharge = false;

                    if (Creature* Steelbreaker = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_STEELBREAKER) : 0))
                    {
                        if (Steelbreaker->IsAlive())
                        {
                            Steelbreaker->SetFullHealth();
                            HasSupercharge = UpdateSupercharge(Steelbreaker);
                        }
                    }

                    if (Creature* Brundir = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_BRUNDIR) : 0))
                    {
                        if (Brundir->IsAlive())
                        {
                            Brundir->SetFullHealth();
                            HasSupercharge = UpdateSupercharge(Brundir);
                        }
                    }

                    if (!HasSupercharge)
                        DoCast(me, SPELL_SUPERCHARGE, true);
                }
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            void SpellHit(Unit* /*from*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_SUPERCHARGE)
                    DoAction(EVENT_UPDATEPHASE);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENRAGE:
                            DoSendQuantumText(SAY_MOLGEIM_BERSERK, me);
                            DoCast(SPELL_BERSERK);
                            break;
                        case EVENT_RUNE_OF_POWER:
                        {
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
								DoCast(target, SPELL_SUMMON_RUNE_OF_POWER);

                            events.ScheduleEvent(EVENT_RUNE_OF_POWER, 60000);
                            break;
                        }
                        case EVENT_SHIELD_OF_RUNES:
                            DoCast(me, RAID_MODE(SPELL_SHIELD_OF_RUNES_10, SPELL_SHIELD_OF_RUNES_25));
                            events.ScheduleEvent(EVENT_SHIELD_OF_RUNES, urand(27000, 34000));
                            break;
                        case EVENT_RUNE_OF_DEATH:
							DoSendQuantumText(SAY_MOLGEIM_RUNE_DEATH, me);
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_RUNE_OF_DEATH);
							}
                            events.ScheduleEvent(EVENT_RUNE_OF_DEATH, urand(30000, 40000));
                            break;
                        case EVENT_RUNE_OF_SUMMONING:
                            DoSendQuantumText(SAY_MOLGEIM_SUMMON, me);
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_RUNE_OF_SUMMONING);
							}
                            events.ScheduleEvent(EVENT_RUNE_OF_SUMMONING, urand(35000, 45000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_runemaster_molgeimAI(creature);
        }
};

class boss_stormcaller_brundir : public CreatureScript
{
    public:
        boss_stormcaller_brundir() : CreatureScript("boss_stormcaller_brundir") { }

        struct boss_stormcaller_brundirAI : public BossAI
        {
            boss_stormcaller_brundirAI(Creature* creature) : BossAI(creature, DATA_ASSEMBLY_OF_IRON){}

			uint32 Phase;
            bool ForceLand;
            bool CouldNotDoThat;

            void Reset()
            {
                _Reset();
                Phase = 0;
                ForceLand = false;
                CouldNotDoThat = true;
				me->RemoveAllAuras();
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                me->RemoveLootMode(LOOT_MODE_DEFAULT);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
                if (instance)
                    RespawnEncounter(instance, me);

                me->SetSpeed(MOVE_RUN, 1.42857f);
                if (me->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY))
                {
                    me->SetDisableGravity(false);
                    me->SendMovementFlagUpdate();
                }

				LightningBeam();
            }

			void JustReachedHome()
			{
				LightningBeam();
			}

            void EnterToBattle(Unit* who)
            {
                me->SetActive(true);
                StartEncounter(instance, me, who);
                DoSendQuantumText(SAY_BRUNDIR_AGGRO, me);
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_ENRAGE, 900000);
                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(9000, 17000), 1);
                events.ScheduleEvent(EVENT_OVERLOAD, urand(60000, 80000), 1);
                DoAction(EVENT_UPDATEPHASE);
            }

			void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_BRUNDIR_SLAY_1, SAY_BRUNDIR_SLAY_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(RAND(SAY_BRUNDIR_DEATH_1, SAY_BRUNDIR_DEATH_2), me);

                if (IsEncounterComplete(instance, me) && instance)
                {
                    _JustDied();

                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CREDIT_MARKER);
                }
            }

			void LightningBeam()
			{
				if (Creature* Steelbreaker = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_STEELBREAKER) : 0))
					DoCast(Steelbreaker, SPELL_LIGHTNING_BEAM_CHANNEL, true);
			}

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_I_CHOOSE_YOU:
                        return (Phase >= 3) ? 1 : 0;
                    case DATA_CANT_DO_THAT:
                        return CouldNotDoThat ? 1 : 0;
                }

                return 0;
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_CHAIN_LIGHTNING_10:
                    case SPELL_CHAIN_LIGHTNING_25:
                    case SPELL_LIGHTNING_WHIRL_DMG_10:
                    case SPELL_LIGHTNING_WHIRL_DMG_25:
						CouldNotDoThat = false;
						break;
				}
			}

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case EVENT_UPDATEPHASE:
                        events.SetPhase(++Phase);
                        if (Phase == 2)
                            events.ScheduleEvent(EVENT_LIGHTNING_WHIRL, urand(20000, 40000), 1);
                        if (Phase == 3)
                        {
                            me->ResetLootMode();
                            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                            DoCast(me, SPELL_STORMSHIELD, true);
                            events.ScheduleEvent(EVENT_LIGHTNING_TENDRILS_START, urand(40000, 80000));
                        }
						break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    // do not die flying
                    if (me->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY))
                    {
                        damage = me->GetHealth() - 1;

                        if (!ForceLand)
                        {
                            me->GetMotionMaster()->MovePoint(POINT_LAND, me->GetPositionX(), me->GetPositionY(), 427.28f);
                            me->RemoveAurasDueToSpell(RAID_MODE(SPELL_LIGHTNING_TENDRILS_10, SPELL_LIGHTNING_TENDRILS_25));
							me->RemoveAurasDueToSpell(SPELL_LIGHTNING_TENDRILS_VISUAL);
                            events.CancelEvent(EVENT_LIGHTNING_TENDRILS_END);
                            events.CancelEvent(EVENT_THREAT_WIPE);
                            ForceLand = true;
                        }
                        return;
                    }

                    bool HasSupercharge = false;

                    if (Creature* Steelbreaker = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_STEELBREAKER) : 0))
                    {
                        if (Steelbreaker->IsAlive())
                        {
                            Steelbreaker->SetFullHealth();
                            HasSupercharge = UpdateSupercharge(Steelbreaker);
                        }
                    }

                    if (Creature* Molgeim = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MOLGEIM) : 0))
                    {
                        if (Molgeim->IsAlive())
                        {
                            Molgeim->SetFullHealth();
                            HasSupercharge = UpdateSupercharge(Molgeim);
                        }
                    }

                    if (!HasSupercharge)
                        DoCast(me, SPELL_SUPERCHARGE, true);
                }
            }

            void SpellHit(Unit* /*from*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_SUPERCHARGE)
                    DoAction(EVENT_UPDATEPHASE);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (id)
                {
                    case POINT_FLY:
                    {
						DoSendQuantumText(SAY_BRUNDIR_FLIGHT, me);
                        DoCast(RAID_MODE(SPELL_LIGHTNING_TENDRILS_10, SPELL_LIGHTNING_TENDRILS_25));
						DoCast(SPELL_LIGHTNING_TENDRILS_VISUAL);
                        events.ScheduleEvent(EVENT_LIGHTNING_TENDRILS_END, 30000);
                        events.ScheduleEvent(EVENT_THREAT_WIPE, 0);
                        break;
                    }
                    case POINT_LAND:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        me->SetSpeed(MOVE_RUN, 1.42857f);
                        me->SetDisableGravity(false);
                        me->SendMovementFlagUpdate();
                        if (me->GetVictim())
							me->GetMotionMaster()->MoveChase(me->GetVictim());
						break;
                    }
                    case POINT_CHASE:
                    {
                        if (Unit* target = me->GetVictim())
                            me->GetMotionMaster()->MovePoint(POINT_CHASE, target->GetPositionX(), target->GetPositionY(), 435.0f);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENRAGE:
                            DoSendQuantumText(SAY_BRUNDIR_BERSERK, me);
                            DoCast(SPELL_BERSERK);
                            break;
                        case EVENT_CHAIN_LIGHTNING:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, RAID_MODE(SPELL_CHAIN_LIGHTNING_10, SPELL_CHAIN_LIGHTNING_25));
							}
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(3000, 5000), 1);
                            break;
                        case EVENT_OVERLOAD:
                            if (!me->HasUnitState(UNIT_STATE_STUNNED))
							{
								DoSendQuantumText(EMOTE_OVERLOAD, me);
							}
                            DoCast(RAID_MODE(SPELL_OVERLOAD_10, SPELL_OVERLOAD_25));
                            events.ScheduleEvent(EVENT_OVERLOAD, urand(60000, 80000), 1);
                            break;
                        case EVENT_LIGHTNING_WHIRL:
                            DoCast(RAID_MODE(SPELL_LIGHTNING_WHIRL_10, SPELL_LIGHTNING_WHIRL_25));
                            events.ScheduleEvent(EVENT_LIGHTNING_WHIRL, urand(20000, 40000), 1);
                            break;
                        case EVENT_THREAT_WIPE:
                            DoResetThreat();
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                me->AddThreat(target, 99999.9f);
                                me->GetMotionMaster()->MovePoint(POINT_CHASE, target->GetPositionX(), target->GetPositionY(), 435.0f);
                            }
                            events.ScheduleEvent(EVENT_THREAT_WIPE, 5000);
                            break;
                        case EVENT_LIGHTNING_TENDRILS_START:
							me->SetSpeed(MOVE_RUN, 0.7f);
                            me->SetReactState(REACT_PASSIVE);
                            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                            me->SetDisableGravity(false);
                            me->SendMovementFlagUpdate();
                            me->GetMotionMaster()->MovePoint(POINT_FLY, me->GetPositionX(), me->GetPositionY(), 435.0f);
                            events.DelayEvents(35000, 1);
                            break;
                        case EVENT_LIGHTNING_TENDRILS_END:
                            me->GetMotionMaster()->MovePoint(POINT_LAND, me->GetPositionX(), me->GetPositionY(), 427.28f);
                            me->RemoveAurasDueToSpell(RAID_MODE(SPELL_LIGHTNING_TENDRILS_10, SPELL_LIGHTNING_TENDRILS_25));
							me->RemoveAurasDueToSpell(SPELL_LIGHTNING_TENDRILS_VISUAL);
                            events.ScheduleEvent(EVENT_LIGHTNING_TENDRILS_START, urand(40000, 80000));
                            events.CancelEvent(EVENT_THREAT_WIPE);
                            break;
                    }
                }

                if (!me->HasAura(RAID_MODE(SPELL_LIGHTNING_TENDRILS_10, SPELL_LIGHTNING_TENDRILS_25)))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_stormcaller_brundirAI(creature);
        }
};

class npc_rune_of_power : public CreatureScript
{
public:
	npc_rune_of_power() : CreatureScript("npc_rune_of_power") { }

	struct npc_rune_of_powerAI : public QuantumBasicAI
	{
		npc_rune_of_powerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetInCombatWithZone();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			me->SetCurrentFaction(FACTION_HOSTILE);
			DoCast(SPELL_RUNE_OF_POWER);
			me->DespawnAfterAction(1*MINUTE*IN_MILLISECONDS);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_rune_of_powerAI(creature);
	}
};

class npc_lightning_elemental : public CreatureScript
{
    public:
        npc_lightning_elemental() : CreatureScript("npc_lightning_elemental") { }

        struct npc_lightning_elementalAI : public QuantumBasicAI
        {
            npc_lightning_elementalAI(Creature* creature) : QuantumBasicAI(creature)
            {
				Reset();
            }

			void Reset()
			{
				DoCast(me, RAID_MODE(SPELL_ELEMENTAL_PASSIVE_10, SPELL_ELEMENTAL_PASSIVE_25));

                me->SetInCombatWithZone();

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    me->AddThreat(target, 99999.9f);
                    AttackStart(target);
                }
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;

				DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_lightning_elementalAI(creature);
        }
};

class npc_rune_of_summoning : public CreatureScript
{
    public:
        npc_rune_of_summoning() : CreatureScript("npc_rune_of_summoning") { }

        struct npc_rune_of_summoningAI : public QuantumBasicAI
        {
            npc_rune_of_summoningAI(Creature* creature) : QuantumBasicAI(creature)
            {
                SummonCount = 0;
                SummonTimer = 5000;
                instance = creature->GetInstanceScript();
                me->AddAura(SPELL_RUNE_OF_SUMMONING_VIS, me);
            }

			InstanceScript* instance;
			uint32 SummonCount;
			uint32 SummonTimer;

            void JustSummoned(Creature* summon)
            {
                if (Creature* Molgeim = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MOLGEIM) : 0))
                    Molgeim->AI()->JustSummoned(summon);
            }

            void SummonLightningElemental()
            {
                me->CastSpell(me, SPELL_RUNE_OF_SUMMONING_SUMMON, false);
                if (++SummonCount == 10)
                    me->DespawnAfterAction();
                else
                    SummonTimer = 2000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (SummonTimer <= diff)
                    SummonLightningElemental();
                else
                    SummonTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_rune_of_summoningAI(creature);
        }
};

class spell_steelbreaker_static_disruption : public SpellScriptLoader
{
    public:
        spell_steelbreaker_static_disruption() : SpellScriptLoader("spell_steelbreaker_static_disruption") { }

        class spell_steelbreaker_static_disruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_steelbreaker_static_disruption_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_STATIC_DISRUPTION_PROC_10))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_STATIC_DISRUPTION_PROC_25))
                    return false;
                return true;
            }

            void HandleTriggerMissile(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();
                if (caster && target)
                {
                    uint32 id = uint32(caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL ? SPELL_STATIC_DISRUPTION_PROC_10 : SPELL_STATIC_DISRUPTION_PROC_25);
                    caster->CastSpell(target, id, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_steelbreaker_static_disruption_SpellScript::HandleTriggerMissile, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_steelbreaker_static_disruption_SpellScript();
        }
};

class spell_steelbreaker_electrical_charge : public SpellScriptLoader
{
    public:
        spell_steelbreaker_electrical_charge() : SpellScriptLoader("spell_steelbreaker_electrical_charge") { }

        class spell_steelbreaker_electrical_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_steelbreaker_electrical_charge_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                Unit* caster = GetCaster();
                if (target && target->ToPlayer() && caster && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                    target->CastSpell(caster, GetSpellInfo()->Effects[EFFECT_0].CalcValue(), true);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_steelbreaker_electrical_charge_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_steelbreaker_electrical_charge_AuraScript();
        }
};

class spell_assembly_rune_of_summoning : public SpellScriptLoader
{
    public:
        spell_assembly_rune_of_summoning() : SpellScriptLoader("spell_assembly_rune_of_summoning") { }

        class spell_assembly_rune_of_summoning_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_assembly_rune_of_summoning_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RUNE_OF_SUMMONING_SUMMON))
                    return false;
                return true;
            }

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(GetTarget(), SPELL_RUNE_OF_SUMMONING_SUMMON, true, NULL, aurEff, GetTarget()->IsSummon() ? GetTarget()->ToTempSummon()->GetSummonerGUID() : 0);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (TempSummon* summon = GetTarget()->ToTempSummon())
                    summon->DespawnAfterAction(1);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_assembly_rune_of_summoning_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectRemove += AuraEffectRemoveFn(spell_assembly_rune_of_summoning_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_assembly_rune_of_summoning_AuraScript();
        }
};

class spell_shield_of_runes : public SpellScriptLoader
{
    public:
        spell_shield_of_runes() : SpellScriptLoader("spell_shield_of_runes") { }

        class spell_shield_of_runes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shield_of_runes_AuraScript);

            void OnAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                uint32 damage = dmgInfo.GetDamage();

                if (absorbAmount > damage)
                    return;

                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_SHIELD_OF_RUNES_BUFF, true);
            }

            void Register()
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_shield_of_runes_AuraScript::OnAbsorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_shield_of_runes_AuraScript();
        }
};

class achievement_i_choose_you : public AchievementCriteriaScript
{
public:
	achievement_i_choose_you() : AchievementCriteriaScript("achievement_i_choose_you") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* boss = target->ToCreature())
			if (boss->AI()->GetData(DATA_I_CHOOSE_YOU))
				return true;

		return false;
	}
};

class achievement_but_i_am_on_your_side : public AchievementCriteriaScript
{
public:
	achievement_but_i_am_on_your_side() : AchievementCriteriaScript("achievement_but_i_am_on_your_side") { }

	bool OnCheck(Player* player, Unit* target)
	{
		if (!target || !player)
			return false;

		if (Creature* boss = target->ToCreature())
			if (boss->AI()->GetData(DATA_I_CHOOSE_YOU) && player->HasAura(SPELL_IRON_BOOT_FLASK))
				return true;

		return false;
	}
};

class achievement_cant_do_that_while_stunned : public AchievementCriteriaScript
{
public:
	achievement_cant_do_that_while_stunned() : AchievementCriteriaScript("achievement_cant_do_that_while_stunned") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* boss = target->ToCreature())
			if (boss->AI()->GetData(DATA_I_CHOOSE_YOU))
				if (InstanceScript* instance = boss->GetInstanceScript())
					if (Creature* brundir = ObjectAccessor::GetCreature(*boss, instance->GetData64(DATA_BRUNDIR)))
						if (brundir->AI()->GetData(DATA_CANT_DO_THAT))
							return true;

		return false;
	}
};

void AddSC_boss_assembly_of_iron()
{
    new boss_steelbreaker();
    new boss_runemaster_molgeim();
    new boss_stormcaller_brundir();
    new npc_lightning_elemental();
    new npc_rune_of_summoning();
    new npc_rune_of_power();
	new spell_steelbreaker_static_disruption();
    new spell_steelbreaker_electrical_charge();
	new spell_assembly_rune_of_summoning();
    new spell_shield_of_runes();
    new achievement_i_choose_you();
    new achievement_but_i_am_on_your_side();
    new achievement_cant_do_that_while_stunned();
}