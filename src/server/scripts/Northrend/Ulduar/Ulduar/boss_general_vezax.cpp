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
    SAY_AGGRO                     = -1603290,
    SAY_SLAY_1                    = -1603291,
    SAY_SLAY_2                    = -1603292,
    SAY_SURGE_OF_DARKNESS         = -1603293,
    SAY_DEATH                     = -1603294,
    SAY_BERSERK                   = -1603295,
    SAY_HARDMODE                  = -1603296,
	EMOTE_SPAWN_SARONITE_VAPORS   = -1613243,
	EMOTE_SARONITE_ANIMUS         = -1613244,
	EMOTE_SARONITE_BARRIER        = -1613245,
	EMOTE_SURGE_OF_DARKNESS       = -1613246,
};

enum VezaxSpells
{
	SPELL_SHAMANTIC_RAGE                         = 30823,
    SPELL_BERSERK                                = 47008,
	SPELL_DEATH_GRIP                             = 49560,
    SPELL_AURA_OF_DESPAIR                        = 62692,
    SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR         = 64848,
    SPELL_CORRUPTED_RAGE                         = 68415,
    SPELL_MARK_OF_THE_FACELESS                   = 63276,
    SPELL_MARK_OF_THE_FACELESS_DAMAGE            = 63278,
    SPELL_SARONITE_BARRIER                       = 63364,
    SPELL_SEARING_FLAMES                         = 62661,
    SPELL_SHADOW_CRASH                           = 62660,
    SPELL_SHADOW_CRASH_HIT                       = 62659,
    SPELL_SHADOW_CRASH_AURA                      = 63277,
    SPELL_SURGE_OF_DARKNESS                      = 62662,
    SPELL_SUMMON_SARONITE_VAPORS                 = 63081,
    SPELL_PROFOUND_DARKNESS                      = 63420,
    SPELL_SUMMON_SARONITE_ANIMUS                 = 63145,
    SPELL_VISUAL_SARONITE_ANIMUS                 = 63319,
    SPELL_PROFOUND_OF_DARKNESS                   = 63420,
    SPELL_SARONITE_VAPORS                        = 63323,
    SPELL_SARONITE_VAPOR_AURA                    = 63322,
	SPELL_SARONITE_VAPORS_MANA                   = 63337,
    SPELL_SARONITE_VAPORS_DAMAGE                 = 63338,
};

enum AchievementsData
{
    DATA_SMELL_OF_SARONITE,
    DATA_SHADOWDODGER,
};

enum VezaxActions
{
    ACTION_VAPORS_DIE
};

enum VezaxEvents
{
	EVENT_SHADOW_CRASH          = 1,
	EVENT_SEARING_FLAMES        = 2,
	EVENT_SURGE_OF_DARKNESS     = 3,
	EVENT_MARK_OF_THE_FACELESS  = 4,
	EVENT_SUMMON_SARONITE_VAPOR = 5,
	EVENT_BERSERK               = 6,
};

class boss_general_vezax : public CreatureScript
{
    public:
        boss_general_vezax() : CreatureScript("boss_general_vezax") {}

        struct boss_general_vezaxAI : public BossAI
        {
            boss_general_vezaxAI(Creature* creature) : BossAI(creature, DATA_GENERAL_VEZAX) {}

			bool AnimusSummoned;
			bool ShadowDodger;
			bool NotHardMode;
			bool VaporKilled;

            void Reset()
            {
                _Reset();

                ShadowDodger = true;
                NotHardMode = true;
                VaporKilled = false;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
								player->AddItem(ITEM_ID_SARONITE_OF_VEZAX, 1);
						}
					}
				}
			}

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

				DoSendQuantumText(SAY_AGGRO, me);

                DoCast(me, SPELL_AURA_OF_DESPAIR);

                events.ScheduleEvent(EVENT_SHADOW_CRASH, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SEARING_FLAMES, 12*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_MARK_OF_THE_FACELESS, urand(35*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SUMMON_SARONITE_VAPOR, 30*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SURGE_OF_DARKNESS, 1*MINUTE*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BERSERK, 10*MINUTE*IN_MILLISECONDS);
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
                        case EVENT_SHADOW_CRASH:
							DoCast(me, SPELL_AURA_OF_DESPAIR, true);
                            if (Unit* target = CheckPlayersInRange(RAID_MODE(4, 9), 15.0f, 50.0f))
							{
                                DoCast(target, SPELL_SHADOW_CRASH);
							}
                            events.ScheduleEvent(EVENT_SHADOW_CRASH, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                            return;
                        case EVENT_SEARING_FLAMES:
							DoCast(me, SPELL_AURA_OF_DESPAIR, true);
                            DoCastAOE(SPELL_SEARING_FLAMES);
                            events.ScheduleEvent(EVENT_SEARING_FLAMES, urand(14*IN_MILLISECONDS, 17.5*IN_MILLISECONDS));
                            return;
                        case EVENT_MARK_OF_THE_FACELESS:
							DoCast(me, SPELL_AURA_OF_DESPAIR, true);
                            if (Unit* target = CheckPlayersInRange(RAID_MODE(4, 9), 15.0f, 50.0f))
							{
                                DoCast(target, SPELL_MARK_OF_THE_FACELESS);
							}
                            events.ScheduleEvent(EVENT_MARK_OF_THE_FACELESS, urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                            return;
                        case EVENT_SURGE_OF_DARKNESS:
							DoCast(me, SPELL_AURA_OF_DESPAIR, true);
							DoSendQuantumText(SAY_SURGE_OF_DARKNESS, me);
							DoSendQuantumText(EMOTE_SURGE_OF_DARKNESS, me);
                            DoCast(me, SPELL_SURGE_OF_DARKNESS);
                            events.ScheduleEvent(EVENT_SURGE_OF_DARKNESS, urand(50*IN_MILLISECONDS, 70*IN_MILLISECONDS));
                            return;
                        case EVENT_SUMMON_SARONITE_VAPOR:
							DoCast(me, SPELL_AURA_OF_DESPAIR, true);
                            DoCast(me, SPELL_SUMMON_SARONITE_VAPORS, true);
                            if (summons.size() >= 6)
                            {
                                events.CancelEvent(EVENT_SUMMON_SARONITE_VAPOR);
                                if (!VaporKilled && NotHardMode)
								{
									DoSendQuantumText(EMOTE_SARONITE_ANIMUS, me);
                                    DoCast(SPELL_SUMMON_SARONITE_ANIMUS);
								}
                                events.CancelEvent(EVENT_SUMMON_SARONITE_VAPOR);
                            }
                            else
								events.ScheduleEvent(EVENT_SUMMON_SARONITE_VAPOR, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_BERSERK:
							DoCast(me, SPELL_AURA_OF_DESPAIR, true);
							DoSendQuantumText(SAY_BERSERK, me);
                            DoCast(me, SPELL_BERSERK);
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (target && target->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_SHADOW_CRASH_HIT)
                    ShadowDodger = false;
            }

            void JustSummoned(Creature* summoned)
            {
                summons.Summon(summoned);
                switch (summoned->GetEntry())
                {
                    case NPC_SARONITE_ANIMUS:
                        events.CancelEvent(EVENT_SEARING_FLAMES);
						DoSendQuantumText(SAY_HARDMODE, me);
						DoSendQuantumText(EMOTE_SARONITE_BARRIER, me);
                        me->InterruptNonMeleeSpells(false);
                        DoCast(SPELL_SARONITE_BARRIER);
                        me->AddLootMode(LOOT_MODE_HARD_MODE_1);
                        break;
                    default:
                        break;
                }
                DoZoneInCombat(summoned);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                switch (summon->GetEntry())
                {
                    case NPC_SARONITE_ANIMUS:
                        NotHardMode = false;
                        me->RemoveAurasDueToSpell(SPELL_SARONITE_BARRIER);
                        events.ScheduleEvent(EVENT_SEARING_FLAMES, urand(7*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                        break;
                    default:
                        break;
                }
                summons.Despawn(summon);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();

				DoSendQuantumText(SAY_DEATH, me);

				//QuestReward();

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_OF_DESPAIR);
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_SHADOWDODGER:
                        return ShadowDodger ? 1 : 0;
                    case DATA_SMELL_OF_SARONITE:
                        return summons.size()>=6 && !NotHardMode && !VaporKilled ? 1 : 0;
                    default:
                        break;
                }
                return 0;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {   
                    case ACTION_VAPORS_DIE:
                        VaporKilled = true;
                        break;
                    default:
                        break;
                }
            }

            Unit* CheckPlayersInRange(uint8 playersMin, float rangeMin, float rangeMax)
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    std::list<Player*> PlayerList;
                    Map::PlayerList const& Players = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                    {
                        if (Player* player = itr->getSource())
                        {
                            float distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            if (rangeMin > distance || distance > rangeMax)
                                continue;

                            PlayerList.push_back(player);
                        }
                    }

                    if (PlayerList.empty() || PlayerList.size()<playersMin)
                        return SelectTarget(TARGET_RANDOM, 0, 150.0f, true);

                    return Quantum::DataPackets::SelectRandomContainerElement(PlayerList);
                }
                return 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_general_vezaxAI(creature);
        }
};

class boss_saronite_animus : public CreatureScript
{
public:
	boss_saronite_animus() : CreatureScript("boss_saronite_animus") {}

	struct boss_saronite_animusAI : public QuantumBasicAI
	{
		boss_saronite_animusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = me->GetInstanceScript();
		}

		InstanceScript* instance;

		uint32 ProfoundTimer;

		void Reset()
		{
			ProfoundTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_VISUAL_SARONITE_ANIMUS, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void InitializeAI()
		{
			Reset();
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;
			
			if (ProfoundTimer <= diff)
			{
				DoCastAOE(SPELL_PROFOUND_OF_DARKNESS, true);
				ProfoundTimer = RAID_MODE(7*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else ProfoundTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_saronite_animusAI(creature);
	}
};

class npc_saronite_vapors : public CreatureScript
{
    public:
        npc_saronite_vapors() : CreatureScript("npc_saronite_vapors") {}

        struct npc_saronite_vaporsAI : public QuantumBasicAI
        {
            npc_saronite_vaporsAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = me->GetInstanceScript();
			}

			InstanceScript* instance;

			uint32 RandomMoveTimer;

            void InitializeAI()
            {
				DoSendQuantumText(EMOTE_SPAWN_SARONITE_VAPORS, me);

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_DEATH_GRIP, true); 
                me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                me->SetReactState(REACT_PASSIVE);

                Reset();
            }

            void Reset()
            {
                RandomMoveTimer = urand(3*IN_MILLISECONDS, 4.5*IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (instance->GetBossState(DATA_GENERAL_VEZAX) != IN_PROGRESS)
                    me->DisappearAndDie();
                    
                if (RandomMoveTimer <= diff)
                {
                    me->GetMotionMaster()->MoveRandom(15.0f);
                    RandomMoveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
                }
                else RandomMoveTimer -= diff;
            }

            void DamageTaken(Unit* /*who*/, uint32& damage)
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
					me->SetCurrentFaction(1665);
                    me->SetHealth(me->GetMaxHealth());
                    me->RemoveAllAuras();
                    DoCast(me, SPELL_SARONITE_VAPORS);
                    me->DespawnAfterAction(30*IN_MILLISECONDS);

                    if (Creature* vezax = me->GetCreature(*me, instance->GetData64(DATA_GENERAL_VEZAX)))
                        vezax->AI()->DoAction(ACTION_VAPORS_DIE);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_saronite_vaporsAI(creature);
        }
};

class spell_aura_of_despair_aura : public SpellScriptLoader
{
public:
    spell_aura_of_despair_aura() : SpellScriptLoader("spell_aura_of_despair_aura") { }

    class spell_aura_of_despair_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_aura_of_despair_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_CORRUPTED_RAGE))
                return false;
            return true;
        }

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (GetTarget()->GetTypeId() != TYPE_ID_PLAYER)
                return;

            Player* target = GetTarget()->ToPlayer();

            if (target->GetCurrentClass() == CLASS_SHAMAN && target->HasSpell(SPELL_SHAMANTIC_RAGE))
                target->CastSpell(target, SPELL_CORRUPTED_RAGE, true);

            target->CastSpell(target, SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR, true);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (GetTarget()->GetTypeId() != TYPE_ID_PLAYER)
                return;

            Player* target = GetTarget()->ToPlayer();

            target->RemoveAurasDueToSpell(SPELL_CORRUPTED_RAGE);
            target->RemoveAurasDueToSpell(SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_aura_of_despair_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PREVENT_REGENERATE_POWER, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_aura_of_despair_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PREVENT_REGENERATE_POWER, AURA_EFFECT_HANDLE_REAL);
        }

    };

    AuraScript* GetAuraScript() const
    {
        return new spell_aura_of_despair_AuraScript();
    }
};

class spell_mark_of_the_faceless : public SpellScriptLoader
{
    public:
        spell_mark_of_the_faceless() : SpellScriptLoader("spell_mark_of_the_faceless") {}

        class spell_mark_of_the_faceless_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mark_of_the_faceless_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MARK_OF_THE_FACELESS_DAMAGE))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(SPELL_MARK_OF_THE_FACELESS_DAMAGE, SPELLVALUE_BASE_POINT1, aurEff->GetAmount(), GetTarget(), true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mark_of_the_faceless_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mark_of_the_faceless_AuraScript();
        }
};

class spell_mark_of_the_faceless_drain : public SpellScriptLoader
{
    public:
        spell_mark_of_the_faceless_drain() : SpellScriptLoader("spell_mark_of_the_faceless_drain") { }

        class spell_mark_of_the_faceless_drain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mark_of_the_faceless_drain_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove(GetExplTargetUnit());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_mark_of_the_faceless_drain_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mark_of_the_faceless_drain_SpellScript();
        }
};

class spell_saronite_vapors : public SpellScriptLoader
{
public:
    spell_saronite_vapors() : SpellScriptLoader("spell_saronite_vapors") {}

    class spell_saronite_vapors_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_saronite_vapors_AuraScript);

        bool Validate(SpellInfo const* /*spell*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SARONITE_VAPORS_MANA) || !sSpellMgr->GetSpellInfo(SPELL_SARONITE_VAPORS_DAMAGE))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            Unit* caster = GetCaster();

            if (caster && target)
            {
				// Test
				if (target->HasAura(SPELL_SARONITE_VAPORS) || target->HasAura(SPELL_SARONITE_VAPOR_AURA))
				{
					target->RemoveAurasDueToSpell(SPELL_AURA_OF_DESPAIR);
					target->RemoveAurasDueToSpell(SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR);
				}

                int32 damage = 50 << GetStackAmount();
                target->CastCustomSpell(target, SPELL_SARONITE_VAPORS_DAMAGE, &damage, 0, 0, true, 0, 0, caster->GetGUID());
                damage = damage >> 1;
                target->CastCustomSpell(target, SPELL_SARONITE_VAPORS_MANA, &damage, 0, 0, true);
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_saronite_vapors_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_saronite_vapors_AuraScript();
    }
};

class achievement_shadowdodger : public AchievementCriteriaScript
{
public:
	achievement_shadowdodger(const char* name) : AchievementCriteriaScript(name) {}

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (target)
			if (Creature* Vezax = target->ToCreature())
				if (Vezax->AI()->GetData(DATA_SHADOWDODGER))
					return true;

		return false;
	}
};

class achievement_i_love_the_smell_of_saronite_in_the_morning : public AchievementCriteriaScript
{
public:
	achievement_i_love_the_smell_of_saronite_in_the_morning(const char* name) : AchievementCriteriaScript(name) {}

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (target)
			if (Creature* Vezax = target->ToCreature())
				if (Vezax->AI()->GetData(DATA_SMELL_OF_SARONITE))
					return true;

		return false;
	}
};

void AddSC_boss_general_vezax()
{
    new boss_general_vezax();
    new boss_saronite_animus();
    new npc_saronite_vapors();
    new spell_aura_of_despair_aura();
    new spell_mark_of_the_faceless_drain();
    new spell_mark_of_the_faceless();
    new spell_saronite_vapors();
    new achievement_shadowdodger("achievement_shadowdodger");
    new achievement_shadowdodger("achievement_shadowdodger_25");
    new achievement_i_love_the_smell_of_saronite_in_the_morning("achievement_i_love_the_smell_of_saronite_in_the_morning");
    new achievement_i_love_the_smell_of_saronite_in_the_morning("achievement_i_love_the_smell_of_saronite_in_the_morning_25");
}