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
    SAY_AGGRO                         = -1603050,
    SAY_SLAY_1                        = -1603051,
    SAY_SLAY_2                        = -1603052,
    SAY_DEATH                         = -1603053,
    SAY_BERSERK                       = -1603054,
	EMOTE_TERRIFYING_SCREECH          = -1613221,
	EMOTE_FERAL_DEFENDER              = -1613222,
	EMOTE_GUARDIAN_SWARM              = -1613223,
};

enum Spells
{
    // Auriaya
    SPELL_TERRIFYING_SCREECH          = 64386,
    SPELL_SENTINEL_BLAST_10           = 64389,
    SPELL_SENTINEL_BLAST_25           = 64678,
    SPELL_SONIC_SCREECH_10            = 64422,
    SPELL_SONIC_SCREECH_25            = 64688,
    SPELL_SUMMON_SWARMING_GUARDIAN    = 64396,
    SPELL_FERAL_DEFENDER_TRIGGER      = 64449,
    SPELL_FERAL_DEFENDER_SUMMON       = 64447,
    SPELL_BERSERK                     = 47008,
    // Feral Defender
    SPELL_FERAL_ESSENCE               = 64455,
    SPELL_FERAL_RUSH_10               = 64496,
    SPELL_FERAL_RUSH_25               = 64674,
    SPELL_FERAL_POUNCE_10             = 64478,
    SPELL_FERAL_POUNCE_25             = 64669,
    SPELL_SLEEPING_FERAL_ESSENCE_10   = 64458,
    SPELL_SLEEPING_FERAL_ESSENCE_25   = 64676,
    // Sanctum Sentry
    SPELL_SAVAGE_POUNCE_10            = 64666,
    SPELL_SAVAGE_POUNCE_25            = 64374,
    SPELL_RIP_FLESH_10                = 64375,
    SPELL_RIP_FLESH_25                = 64667,
    SPELL_STRENGTH_OF_THE_PACK        = 64369,
};

enum Events
{
    EVENT_NONE                        = 1,
    EVENT_TERRIFYING_SCREECH          = 2,
    EVENT_SONIC_SCREECH               = 3,
    EVENT_GUARDIAN_SWARM              = 4,
    EVENT_SENTINEL_BLAST              = 5,
    EVENT_FERAL_DEFENDER_SUMMON       = 6,
    EVENT_FERAL_DEFENDER_RESURRECTION = 7,
    EVENT_BERSERK                     = 8,
};

enum Data
{
    DATA_NINE_LIVES     = 1,
    DATA_CRAZY_CAT_LADY = 2,
};

class boss_auriaya : public CreatureScript
{
    public:
        boss_auriaya() : CreatureScript("boss_auriaya") { }

        struct boss_auriayaAI : public BossAI
        {
            boss_auriayaAI(Creature* creature) : BossAI(creature, DATA_AURIAYA)
            {
				if (SpellInfo* spell = (SpellInfo*)sSpellMgr->GetSpellInfo(RAID_MODE(SPELL_SENTINEL_BLAST_10, SPELL_SENTINEL_BLAST_25)))
					spell->ChannelInterruptFlags &= ~AURA_INTERRUPT_FLAG_TAKE_DAMAGE;
            }

			uint8 DefenderLifeCount;

            bool CrazyCatLady;

            void Reset()
            {
                _Reset();

                CrazyCatLady = true;

                DefenderLifeCount = 9;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                uint8 max = RAID_MODE<uint32>(2, 4);
                for (uint8 i = 0; i < max; ++i)
				{
                    if (Creature* sentry = me->SummonCreature(NPC_SANCTUM_SENTRY, *me, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7000))
                        sentry->GetMotionMaster()->MoveFollow(me, (i < 2) ? 0.5f : 4.0f, M_PI - i - 1.5f);
				}
            }

            void EnterToBattle(Unit* who)
            {
				_EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);

                summons.DoZoneInCombat();

                events.ScheduleEvent(EVENT_TERRIFYING_SCREECH, 40000);
                events.ScheduleEvent(EVENT_SONIC_SCREECH, 60000);
                events.ScheduleEvent(EVENT_GUARDIAN_SWARM, 30000);
                events.ScheduleEvent(EVENT_SENTINEL_BLAST, 20000);
                events.ScheduleEvent(EVENT_FERAL_DEFENDER_SUMMON, 55000);
                events.ScheduleEvent(EVENT_BERSERK, 600000);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);

                _JustDied();
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_NINE_LIVES:
                        return !DefenderLifeCount ? 1 : 0;
                    case DATA_CRAZY_CAT_LADY:
                        return CrazyCatLady ? 1 : 0;
                }
                return 0;
            }

            void MoveInLineOfSight(Unit* /*who*/) {}

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                switch (summon->GetEntry())
                {
                    case NPC_FERAL_DEFENDER:
                        --DefenderLifeCount;
                        me->SummonCreature(NPC_ESSENCE_STALKER, *summon);
                        if (DefenderLifeCount)
                            events.ScheduleEvent(EVENT_FERAL_DEFENDER_RESURRECTION, 30000);
                        break;
                    case NPC_SANCTUM_SENTRY:
                        CrazyCatLady = false;
                        break;
                    default:
                        break;
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
                        case EVENT_TERRIFYING_SCREECH:
							DoSendQuantumText(EMOTE_TERRIFYING_SCREECH, me);
                            DoCast(SPELL_TERRIFYING_SCREECH);
                            events.ScheduleEvent(EVENT_TERRIFYING_SCREECH, 35000);
                            return;
                        case EVENT_SONIC_SCREECH:
                            DoCastVictim(RAID_MODE(SPELL_SONIC_SCREECH_10, SPELL_SONIC_SCREECH_25));
                            events.ScheduleEvent(EVENT_SONIC_SCREECH, 27000);
                            return;
                        case EVENT_GUARDIAN_SWARM:
							DoSendQuantumText(EMOTE_GUARDIAN_SWARM, me);
                            DoCastVictim(SPELL_SUMMON_SWARMING_GUARDIAN);
                            events.ScheduleEvent(EVENT_GUARDIAN_SWARM, 35000);
                            return;
                        case EVENT_SENTINEL_BLAST:
                            DoCastAOE(RAID_MODE(SPELL_SENTINEL_BLAST_10, SPELL_SENTINEL_BLAST_25));
                            events.ScheduleEvent(EVENT_SENTINEL_BLAST, 25000);
                            return;
                        case EVENT_FERAL_DEFENDER_SUMMON:
							DoSendQuantumText(EMOTE_FERAL_DEFENDER, me);
                            DoCast(SPELL_FERAL_DEFENDER_SUMMON);
                            if (Creature* defender = me->FindCreatureWithDistance(NPC_FERAL_DEFENDER, 100.0f, true))
                            {
                                defender->AddAura(SPELL_FERAL_ESSENCE, defender);
                                defender->SetAuraStack(SPELL_FERAL_ESSENCE, defender, DefenderLifeCount);
                            }
                            return;
                        case EVENT_FERAL_DEFENDER_RESURRECTION:
                            if (Creature* defender = me->FindCreatureWithDistance(NPC_FERAL_DEFENDER, 500.0f, false))
                            {
                                defender->Respawn();
                                defender->SetInCombatWithZone();
                                defender->AddAura(SPELL_FERAL_ESSENCE, defender);
                                defender->SetAuraStack(SPELL_FERAL_ESSENCE, defender, DefenderLifeCount);
                            }
                            return;
                        case EVENT_BERSERK:
                            DoSendQuantumText(SAY_BERSERK, me);
                            DoCast(me, SPELL_BERSERK, true);
                            return;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_auriayaAI(creature);
        }
};

class npc_feral_defender: public CreatureScript
{
    public:
        npc_feral_defender() : CreatureScript("npc_feral_defender") { }

        struct npc_feral_defenderAI : public QuantumBasicAI
        {
            npc_feral_defenderAI(Creature* creature) : QuantumBasicAI(creature) {}

			uint32 FeralRushTimer;
            uint32 FeralPounceTimer;

            void Reset()
            {
                FeralRushTimer = 4000;
                FeralPounceTimer = 6000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (FeralRushTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, RAID_MODE(SPELL_FERAL_RUSH_10, SPELL_FERAL_RUSH_25));
						FeralRushTimer = 5000;
					}
                }
                else FeralRushTimer -= diff;

                if (FeralPounceTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, RAID_MODE(SPELL_FERAL_POUNCE_10, SPELL_FERAL_POUNCE_25));
						FeralPounceTimer = 5000;
					}
                }
                else FeralPounceTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_feral_defenderAI(creature);
        }
};

class npc_sanctum_sentry: public CreatureScript
{
    public:
        npc_sanctum_sentry() : CreatureScript("npc_sanctum_sentry") { }

        struct npc_sanctum_sentryAI : public QuantumBasicAI
        {
            npc_sanctum_sentryAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 RipFleshTimer;

            void Reset()
            {
                RipFleshTimer = 10000;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->AddAura(SPELL_STRENGTH_OF_THE_PACK, me);
                DoCast(RAID_MODE(SPELL_SAVAGE_POUNCE_10, SPELL_SAVAGE_POUNCE_25));

                if (me->ToTempSummon())
                {
                    Unit* auriaya = me->ToTempSummon()->GetSummoner();
                    if (auriaya->ToCreature() && !auriaya->IsInCombatActive())
                        auriaya->ToCreature()->SetInCombatWithZone();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (RipFleshTimer <= diff)
                {
                    DoCastVictim(RAID_MODE(SPELL_RIP_FLESH_10, SPELL_RIP_FLESH_25));
                    RipFleshTimer = 10000;
                }
                else RipFleshTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_sanctum_sentryAI(creature);
        }
};

class npc_seeping_essence_stalker: public CreatureScript
{
public:
	npc_seeping_essence_stalker() : CreatureScript("npc_seeping_essence_stalker") {}

	struct npc_seeping_essence_stalkerAI : public QuantumBasicAI
	{
		npc_seeping_essence_stalkerAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
		{
			me->SetDisplayId(MODEL_ID_INVISIBLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE);
			DoCast(RAID_MODE(SPELL_SLEEPING_FERAL_ESSENCE_10, SPELL_SLEEPING_FERAL_ESSENCE_25));
		}

		void UpdateAI(uint32 const /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_seeping_essence_stalkerAI(creature);
	}
};

class SanctumSentryCheck
{
public:
	bool operator() (Unit* unit)
	{
		if (unit->GetEntry() == NPC_SANCTUM_SENTRY)
			return false;

		return true;
	}
};

class spell_auriaya_strenght_of_the_pack : public SpellScriptLoader
{
public:
	spell_auriaya_strenght_of_the_pack() : SpellScriptLoader("spell_auriaya_strenght_of_the_pack") { }

	class spell_auriaya_strenght_of_the_pack_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_auriaya_strenght_of_the_pack_SpellScript);

		void FilterTargets(std::list<Unit*>& unitList)
		{
			unitList.remove_if (SanctumSentryCheck());
		}

		void Register()
		{
			OnUnitTargetSelect += SpellUnitTargetFn(spell_auriaya_strenght_of_the_pack_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_auriaya_strenght_of_the_pack_SpellScript();
	}
};

class spell_auriaya_sentinel_blast : public SpellScriptLoader
{
    public:
        spell_auriaya_sentinel_blast() : SpellScriptLoader("spell_auriaya_sentinel_blast") { }

        class spell_auriaya_sentinel_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_auriaya_sentinel_blast_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(PlayerOrPetCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_auriaya_sentinel_blast_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_auriaya_sentinel_blast_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_auriaya_sentinel_blast_SpellScript();
        }
};

class achievement_nine_lives : public AchievementCriteriaScript
{
public:
	achievement_nine_lives() : AchievementCriteriaScript("achievement_nine_lives") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Auriaya = target->ToCreature())
			if (Auriaya->AI()->GetData(DATA_NINE_LIVES))
				return true;

		return false;
	}
};

class achievement_crazy_cat_lady : public AchievementCriteriaScript
{
public:
	achievement_crazy_cat_lady() : AchievementCriteriaScript("achievement_crazy_cat_lady") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Auriaya = target->ToCreature())
			if (Auriaya->AI()->GetData(DATA_CRAZY_CAT_LADY))
				return true;

		return false;
	}
};

void AddSC_boss_auriaya()
{
    new boss_auriaya();
    new npc_feral_defender();
    new npc_seeping_essence_stalker();
    new npc_sanctum_sentry();
    new spell_auriaya_strenght_of_the_pack();
	new spell_auriaya_sentinel_blast();
    new achievement_nine_lives();
    new achievement_crazy_cat_lady();
}