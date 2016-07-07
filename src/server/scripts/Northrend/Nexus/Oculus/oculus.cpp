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
#include "SpellScript.h"
#include "oculus.h"

#define GOSSIP_ITEM_DRAKES         "So where do we go from here?"
#define GOSSIP_ITEM_BELGARISTRASZ1 "I want to fly on the wings of the Red Flight"
#define GOSSIP_ITEM_BELGARISTRASZ2 "What abilities do Ruby Drakes have?"
#define GOSSIP_ITEM_VERDISA1       "I want to fly on the wings of the Green Flight"
#define GOSSIP_ITEM_VERDISA2       "What abilities do Emerald Drakes have?"
#define GOSSIP_ITEM_ETERNOS1       "I want to fly on the wings of the Bronze Flight"
#define GOSSIP_ITEM_ETERNOS2       "What abilities do Amber Drakes have?"

#define HAS_ESSENCE(a) ((a)->HasItemCount(ITEM_ID_EMERALD_ESSENCE, 1) || (a)->HasItemCount(ITEM_ID_AMBER_ESSENCE, 1) || (a)->HasItemCount(ITEM_ID_RUBY_ESSENCE, 1))

enum Gossips
{
    GOSSIP_TEXTID_DRAKES                    = 13267,
    GOSSIP_TEXTID_BELGARISTRASZ1            = 12916,
    GOSSIP_TEXTID_BELGARISTRASZ2            = 13466,
    GOSSIP_TEXTID_BELGARISTRASZ3            = 13254,

    GOSSIP_TEXTID_VERDISA1                  = 1,
    GOSSIP_TEXTID_VERDISA2                  = 1,
    GOSSIP_TEXTID_VERDISA3                  = 13258,
    GOSSIP_TEXTID_ETERNOS1                  = 1,
    GOSSIP_TEXTID_ETERNOS2                  = 1,
    GOSSIP_TEXTID_ETERNOS3                  = 13256,
};

enum Says
{
    SAY_VAROS          = 0,
    SAY_UROM           = 1,
};

class npc_oculus_drake : public CreatureScript
{
    public:
        npc_oculus_drake() : CreatureScript("npc_oculus_drake") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_DRAKOS_EVENT) == DONE)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DRAKES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DRAKES, creature->GetGUID());
                }
            }
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (creature->GetEntry())
            {
                case NPC_VERDISA:
                    switch (action)
                    {
                        case GOSSIP_ACTION_INFO_DEF + 1:
                            if (!HAS_ESSENCE(player))
                            {
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VERDISA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VERDISA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VERDISA1, creature->GetGUID());
                            }
                            else
                            {
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VERDISA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VERDISA2, creature->GetGUID());
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 2:
                        {
                            player->AddItem(ITEM_ID_EMERALD_ESSENCE, 1);
                            player->CLOSE_GOSSIP_MENU();
                            break;
                        }
                        case GOSSIP_ACTION_INFO_DEF + 3:
                            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VERDISA3, creature->GetGUID());
                            break;
                    }
                    break;
                case NPC_BELGARISTRASZ:
                    switch (action)
                    {
                        case GOSSIP_ACTION_INFO_DEF + 1:
                            if (!HAS_ESSENCE(player))
                            {
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BELGARISTRASZ1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BELGARISTRASZ2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BELGARISTRASZ1, creature->GetGUID());
                            }
                            else
                            {
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BELGARISTRASZ2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BELGARISTRASZ2, creature->GetGUID());
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 2:
                        {
                            player->AddItem(ITEM_ID_RUBY_ESSENCE, 1);
                            player->CLOSE_GOSSIP_MENU();
                            break;
                        }
                        case GOSSIP_ACTION_INFO_DEF + 3:
                            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BELGARISTRASZ3, creature->GetGUID());
                            break;
                    }
                    break;
                case NPC_ETERNOS:
                    switch (action)
                    {
                        case GOSSIP_ACTION_INFO_DEF + 1:
                            if (!HAS_ESSENCE(player))
                            {
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ETERNOS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ETERNOS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ETERNOS1, creature->GetGUID());
                            }
                            else
                            {
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ETERNOS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ETERNOS2, creature->GetGUID());
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 2:
                        {
                            player->AddItem(ITEM_ID_AMBER_ESSENCE, 1);
                            player->CLOSE_GOSSIP_MENU();
                            break;
                        }
                        case GOSSIP_ACTION_INFO_DEF + 3:
                            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ETERNOS3, creature->GetGUID());
                            break;
                    }
                    break;
            }
            return true;
        }
};

enum DrakeTexts
{
    SAY_DRAKES_TAKEOFF                = 0,
    WHISPER_DRAKES_WELCOME            = 1,
    WHISPER_DRAKES_ABILITIES          = 2,
    WHISPER_DRAKES_SPECIAL            = 3,
    WHISPER_DRAKES_LOWHEALTH          = 4,
};

enum DrakeSpells
{
	SPELL_RIDE_RUBY_DRAKE_QUE           = 49463,          // Apply Aura: Periodic Trigger, Interval: 3 seconds --> 49464
    SPELL_RUBY_DRAKE_SADDLE             = 49464,          // Allows you to ride on the back of an Amber Drake. --> Dummy
    SPELL_RUBY_SEARING_WRATH            = 50232,          // (60 yds) - Instant - Breathes a stream of fire at an enemy dragon, dealing 6800 to 9200 Fire damage and then jumping to additional dragons within 30 yards. Each jump increases the damage by 50%. Affects up to 5 total targets
    SPELL_RUBY_EVASIVE_AURA             = 50248,          // Instant - Allows the Ruby Drake to generate Evasive Charges when hit by hostile attacks and spells.
    SPELL_RUBY_EVASIVE_CHARGES          = 50241,
    SPELL_RUBY_EVASIVE_MANEUVERS        = 50240,          // Instant - 5 sec. cooldown - Allows your drake to dodge all incoming attacks and spells. Requires Evasive Charges to use. Each attack or spell dodged while this ability is active burns one Evasive Charge. Lasts 30 sec. or until all charges are exhausted.
    SPELL_RUBY_MARTYR                   = 50253,          // Instant - 10 sec. cooldown - Redirect all harmful spells cast at friendly drakes to yourself for 10 sec.
    SPELL_RIDE_AMBER_DRAKE_QUE          = 49459,          // Apply Aura: Periodic Trigger, Interval: 3 seconds --> 49460
    SPELL_AMBER_DRAKE_SADDLE            = 49460,          // Allows you to ride on the back of an Amber Drake. --> Dummy
    SPELL_AMBER_SHOCK_CHARGE            = 49836,
    SPELL_AMBER_SHOCK_LANCE             = 49840,          // (60 yds) - Instant - Deals 4822 to 5602 Arcane damage and detonates all Shock Charges on an enemy dragon. Damage is increased by 6525 for each detonated.
    SPELL_AMBER_STOP_TIME               = 49838,          // Instant - 1 min cooldown - Halts the passage of time, freezing all enemy dragons in place for 10 sec. This attack applies 5 Shock Charges to each affected target.
    SPELL_AMBER_TEMPORAL_RIFT           = 49592,          // (60 yds) - Channeled - Channels a temporal rift on an enemy dragon for 10 sec. While trapped in the rift, all damage done to the target is increased by 100%. In addition, for every 15, 000 damage done to a target affected by Temporal Rift, 1 Shock Charge is generated.
    SPELL_RIDE_EMERALD_DRAKE_QUE        = 49427,         // Apply Aura: Periodic Trigger, Interval: 3 seconds --> 49346
    SPELL_EMERALD_DRAKE_SADDLE          = 49346,         // Allows you to ride on the back of an Amber Drake. --> Dummy
    SPELL_EMERALD_LEECHING_POISON       = 50328,         // (60 yds) - Instant - Poisons the enemy dragon, leeching 1300 to the caster every 2 sec. for 12 sec. Stacks up to 3 times.
    SPELL_EMERALD_TOUCH_THE_NIGHTMARE   = 50341,         // (60 yds) - Instant - Consumes 30% of the caster's max health to inflict 25, 000 nature damage to an enemy dragon and reduce the damage it deals by 25% for 30 sec.
    SPELL_EMERALD_DREAM_FUNNEL          = 50344,         // (60 yds) - Channeled - Transfers 5% of the caster's max health to a friendly drake every second for 10 seconds as long as the caster channels.
    // Misc
    POINT_LAND                          = 2,
    POINT_TAKE_OFF                      = 3,
};

enum DrakeEvents
{
    EVENT_WELCOME          = 1,
    EVENT_ABILITIES        = 2,
    EVENT_SPECIAL_ATTACK   = 3,
    EVENT_LOW_HEALTH       = 4,
    EVENT_RESET_LOW_HEALTH = 5,
    EVENT_TAKE_OFF         = 6,
};

class npc_ruby_emerald_amber_drake : public CreatureScript
{
    public:
        npc_ruby_emerald_amber_drake() : CreatureScript("npc_ruby_emerald_amber_drake") { }

        struct npc_ruby_emerald_amber_drakeAI : public VehicleAI
        {
            npc_ruby_emerald_amber_drakeAI(Creature* creature) : VehicleAI(creature)
            {
                Initialize();
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            EventMap events;

            bool HealthWarning;

            void Initialize()
            {
                HealthWarning = true;
            }

            void Reset()
            {
                events.Reset();
                Initialize();

				me->SetPowerType(POWER_ENERGY);
				me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
            }

            void IsSummonedBy(Unit* summoner)
            {
                if (instance->GetBossState(DATA_EREGOS) == IN_PROGRESS)
				{
                    if (Creature* eregos = me->FindCreatureWithDistance(NPC_LEY_GUARDIAN_EREGOS, 500.0f, true))
                        eregos->AI()->EnterEvadeMode();
				}

                me->SetFacingToObject(summoner);

                switch (me->GetEntry())
                {
                    case NPC_RUBY_DRAKE_VEHICLE:
                        me->CastSpell(summoner, SPELL_RIDE_RUBY_DRAKE_QUE);
                        break;
                    case NPC_EMERALD_DRAKE_VEHICLE:
                        me->CastSpell(summoner, SPELL_RIDE_EMERALD_DRAKE_QUE);
                        break;
                    case NPC_AMBER_DRAKE_VEHICLE:
                        me->CastSpell(summoner, SPELL_RIDE_AMBER_DRAKE_QUE);
                        break;
                    default:
                        return;
                }

				Position position;
				me->GetPosition(&position);
                me->GetMotionMaster()->MovePoint(POINT_LAND, position);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE && id == POINT_LAND)
                    me->SetDisableGravity(false); // Needed this for proper animation after spawn, the summon in air fall to ground bug leave no other option for now, if this isn't used the drake will only walk on move.
            }

            void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
            {
                if (passenger->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                if (apply)
                {
                    if (instance->GetBossState(DATA_VAROS) != DONE)
                        events.ScheduleEvent(EVENT_WELCOME, 10*IN_MILLISECONDS);

                    else if (instance->GetBossState(DATA_UROM) == DONE)
                        events.ScheduleEvent(EVENT_SPECIAL_ATTACK, 10*IN_MILLISECONDS);
                }
                else
                {
                    events.Reset();
                    events.ScheduleEvent(EVENT_TAKE_OFF, 2*IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (HealthWarning)
                {
                    if (me->GetHealthPct() <= 40.0f)
                        events.ScheduleEvent(EVENT_LOW_HEALTH, 0);
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WELCOME:
							if (Unit* summoner = me->ToTempSummon()->GetSummoner())
								Talk(WHISPER_DRAKES_WELCOME);
                            events.ScheduleEvent(EVENT_ABILITIES, 5*IN_MILLISECONDS);
                            break;
                        case EVENT_ABILITIES:
                            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                Talk(WHISPER_DRAKES_ABILITIES);
                            break;
                        case EVENT_SPECIAL_ATTACK:
							if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                Talk(WHISPER_DRAKES_SPECIAL);
                            break;
                        case EVENT_LOW_HEALTH:
							if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                                Talk(WHISPER_DRAKES_LOWHEALTH);
                            HealthWarning = false;
                            events.ScheduleEvent(EVENT_RESET_LOW_HEALTH, 25000);
                            break;
                        case EVENT_RESET_LOW_HEALTH:
                            HealthWarning = true;
                            break;
                        case EVENT_TAKE_OFF:
                        {
                            me->DespawnAfterAction(2.5*IN_MILLISECONDS);
                            me->SetOrientation(2.5f);
                            me->SetSpeed(MOVE_FLIGHT, 1.0f, true);
                            Talk(SAY_DRAKES_TAKEOFF);
							Position position;
							me->GetPosition(&position);
                            Position offset = {10.0f, 10.0f, 12.0f, 0.0f};
                            position.RelocateOffset(offset);
                            me->SetDisableGravity(true);
                            me->GetMotionMaster()->MovePoint(POINT_TAKE_OFF, position);
                            break;
                        }
                        default:
                            break;
                    }
                }
            };
        };

		CreatureAI* GetAI(Creature* creature) const
        {
			return new npc_ruby_emerald_amber_drakeAI(creature);
        }
};

class npc_image_belgaristrasz : public CreatureScript
{
public:
    npc_image_belgaristrasz() : CreatureScript("npc_image_belgaristrasz") { }

    struct npc_image_belgaristraszAI : public QuantumBasicAI
    {
        npc_image_belgaristraszAI(Creature* creature) : QuantumBasicAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (summoner->GetEntry() == NPC_VAROS_CLOUDSTRIDER)
            {
               Talk(SAY_VAROS);
               me->DespawnAfterAction(2*MINUTE*IN_MILLISECONDS);
            }

            if (summoner->GetEntry() == NPC_LORD_MAGE_UROM)
            {
               Talk(SAY_UROM);
               me->DespawnAfterAction(2*MINUTE*IN_MILLISECONDS);
            }
        }            
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_image_belgaristraszAI(creature);
    }
};

class spell_amber_drake_shock_lance : public SpellScriptLoader
{
    public:
        spell_amber_drake_shock_lance() : SpellScriptLoader("spell_amber_drake_shock_lance") { }

        class spell_amber_drake_shock_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_amber_drake_shock_lance_SpellScript);

            void RecalculateDamage()
            {
                if (Aura* charge = GetHitUnit()->GetAura(SPELL_SHOCK_CHARGE))
                {
                    SetHitDamage(6525 * charge->GetStackAmount() + GetHitDamage());
                    charge->Remove();
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_amber_drake_shock_lance_SpellScript::RecalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_amber_drake_shock_lance_SpellScript();
        }
};

class IsNoValidDrake
{
public:
	bool operator() (Unit* unit)
	{
		if (unit->ToCreature())
		{
			switch (unit->ToCreature()->GetEntry())
			{
			    case NPC_AZURE_RING_GUARDIAN:
				case NPC_LEY_GUARDIAN_EREGOS:
				case NPC_GREATER_LEY_WHELP:
					return false;
				default:
					break;
			}
		}
		return true;
	}
};

class spell_amber_drake_stop_time : public SpellScriptLoader
{
    public:
        spell_amber_drake_stop_time() : SpellScriptLoader("spell_amber_drake_stop_time") { }

        class spell_amber_drake_stop_time_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_amber_drake_stop_time_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (IsNoValidDrake());
            }

            void HandleStun(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit())
                    GetHitUnit()->CastCustomSpell(SPELL_SHOCK_CHARGE, SPELLVALUE_AURA_STACK, 5, GetHitUnit(), true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_amber_drake_stop_time_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_amber_drake_stop_time_SpellScript::HandleStun, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_amber_drake_stop_time_SpellScript();
        }
};

class spell_amber_drake_temporal_rift : public SpellScriptLoader
{
    public:
        spell_amber_drake_temporal_rift() : SpellScriptLoader("spell_amber_drake_temporal_rift") { }

        class spell_amber_drake_temporal_rift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_amber_drake_temporal_rift_AuraScript);

			uint32 TargetHealth;
            uint32 Damage;

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget())
                    TargetHealth = GetTarget()->GetHealth();

                Damage = 0;
            }

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                Damage += TargetHealth - GetTarget()->GetHealth();

                for (; Damage >= 15000; Damage -= 15000)
                    GetTarget()->CastSpell(GetTarget(), SPELL_SHOCK_CHARGE, true);

                TargetHealth = GetTarget()->GetHealth();
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_amber_drake_temporal_rift_AuraScript::OnApply, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_amber_drake_temporal_rift_AuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_amber_drake_temporal_rift_AuraScript();
        }
};

class spell_oculus_ride_ruby_emerald_amber_drake_que : public SpellScriptLoader
{
    public:
        spell_oculus_ride_ruby_emerald_amber_drake_que() : SpellScriptLoader("spell_oculus_ride_ruby_emerald_amber_drake_que") { }

        class spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript);

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                    GetTarget()->CastSpell(caster, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript();
        }
};

class spell_oculus_touch_the_nightmare : public SpellScriptLoader
{
    public:
        spell_oculus_touch_the_nightmare() : SpellScriptLoader("spell_oculus_touch_the_nightmare") { }

        class spell_oculus_touch_the_nightmare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_oculus_touch_the_nightmare_SpellScript);

            void HandleDamageCalc(SpellEffIndex /*effIndex*/)
            {
                SetHitDamage(int32(GetCaster()->CountPctFromMaxHealth(HEALTH_PERCENT_30)));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_oculus_touch_the_nightmare_SpellScript::HandleDamageCalc, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_oculus_touch_the_nightmare_SpellScript();
        }
};

class spell_oculus_dream_funnel : public SpellScriptLoader
{
    public:
        spell_oculus_dream_funnel() : SpellScriptLoader("spell_oculus_dream_funnel") { }

        class spell_oculus_dream_funnel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_dream_funnel_AuraScript);

            void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                    amount = int32(caster->CountPctFromMaxHealth(HEALTH_PERCENT_5));

                canBeRecalculated = false;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_oculus_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_oculus_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_oculus_dream_funnel_AuraScript();
        }
};

void AddSC_oculus()
{
    new npc_oculus_drake();
    new npc_ruby_emerald_amber_drake();
    new npc_image_belgaristrasz();
	new spell_oculus_ride_ruby_emerald_amber_drake_que();
    new spell_amber_drake_shock_lance();
    new spell_amber_drake_stop_time();
    new spell_amber_drake_temporal_rift();
	new spell_oculus_touch_the_nightmare();
	new spell_oculus_dream_funnel();
}