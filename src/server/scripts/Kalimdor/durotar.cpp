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
#include "Vehicle.h"
#include "SpellScript.h"

enum LazyPeonYells
{
    SAY_SPELL_HIT                   = 0, //Ow! OK, I''ll get back to work, $N!'
};

enum LazyPeon
{
    QUEST_LAZY_PEONS  = 5441,

    GO_LUMBERPILE     = 175784,

    SPELL_BUFF_SLEEP  = 17743,
    SPELL_AWAKEN_PEON = 19938,
};

class npc_lazy_peon : public CreatureScript
{
public:
    npc_lazy_peon() : CreatureScript("npc_lazy_peon") { }

    struct npc_lazy_peonAI : public QuantumBasicAI
    {
        npc_lazy_peonAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;

        uint32 RebuffTimer;
        bool work;

        void Reset ()
        {
            PlayerGUID = 0;
            RebuffTimer = 0;
            work = false;
        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            if (id == 1)
                work = true;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_AWAKEN_PEON && caster->GetTypeId() == TYPE_ID_PLAYER && CAST_PLR(caster)->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
            {
                caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
				Talk(SAY_SPELL_HIT, caster->GetGUID());
                me->RemoveAllAuras();

                if (GameObject* Lumberpile = me->FindGameobjectWithDistance(GO_LUMBERPILE, 20.0f))
                    me->GetMotionMaster()->MovePoint(1, Lumberpile->GetPositionX()-1, Lumberpile->GetPositionY(), Lumberpile->GetPositionZ());
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (work == true)
				me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);

            if (RebuffTimer <= diff)
            {
                DoCast(me, SPELL_BUFF_SLEEP);
                RebuffTimer = 300000;                 //Rebuff agian in 5 minutes
            }
            else RebuffTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lazy_peonAI(creature);
    }
};

enum Texts
{
    SAY_MATRIARCH_AGGRO     = 0,
    SAY_VOLUNTEER_START     = 0,
    SAY_VOLUNTEER_END       = 1,
};

enum Spells
{
    SPELL_SUMMON_MATRIARCH              = 75187,
    SPELL_NO_SUMMON_AURA                = 75213,
    SPELL_DETECT_INVIS                  = 75180,
    SPELL_SUMMON_ZENTABRA_TRIGGER       = 75212,
    SPELL_POUNCE                        = 61184,
    SPELL_FURIOUS_BITE                  = 75164,
    SPELL_SUMMON_ZENTABRA               = 75181,
    SPELL_SPIRIT_OF_THE_TIGER_RIDER     = 75166,
    SPELL_EJECT_PASSENGERS              = 50630,
    SPELL_VOLUNTEER_AURA                = 75076,
    SPELL_PETACT_AURA                   = 74071,
    SPELL_QUEST_CREDIT                  = 75106,
    SPELL_MOUNTING_CHECK                = 75420,
    SPELL_TURNIN                        = 73953,
    SPELL_AOE_TURNIN                    = 75107,
    SPELL_MOTIVATE_1                    = 75088,
    SPELL_MOTIVATE_2                    = 75086,
};

enum Creatures
{
    NPC_TIGER_VEHICLE                   = 40305,
    NPC_URUZIN                          = 40253,
    NPC_VOLUNTEER_1                     = 40264,
    NPC_VOLUNTEER_2                     = 40260,
    NPC_CITIZEN_1                       = 40256,
    NPC_CITIZEN_2                       = 40257,
};

enum Events
{
    EVENT_CHECK_SUMMON_AURA             = 1,
    EVENT_POUNCE                        = 2,
    EVENT_NOSUMMON                      = 3,
};

enum Points
{
    POINT_URUZIN                        = 4026400,
};

class npc_tiger_matriarch_credit : public CreatureScript
{
    public:
        npc_tiger_matriarch_credit() : CreatureScript("npc_tiger_matriarch_credit") { }

        struct npc_tiger_matriarch_creditAI : public QuantumBasicAI
        {
           npc_tiger_matriarch_creditAI(Creature* creature) : QuantumBasicAI(creature)
           {
			   SetCombatMovement(false);
               events.ScheduleEvent(EVENT_CHECK_SUMMON_AURA, 2000);
           }

		   EventMap events;

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_CHECK_SUMMON_AURA)
                {
                    std::list<Creature*> tigers;
                    GetCreatureListWithEntryInGrid(tigers, me, NPC_TIGER_VEHICLE, 15.0f);
                    if (!tigers.empty())
                    {
                        for (std::list<Creature*>::iterator itr = tigers.begin(); itr != tigers.end(); ++itr)
                        {
                            if (!(*itr)->IsSummon())
                                continue;

                            if (Unit* summoner = (*itr)->ToTempSummon()->GetSummoner())
							{
                                if (!summoner->HasAura(SPELL_NO_SUMMON_AURA) && !summoner->HasAura(SPELL_SUMMON_ZENTABRA_TRIGGER) && !summoner->IsInCombatActive())
                                {
                                    me->AddAura(SPELL_NO_SUMMON_AURA, summoner);
                                    me->AddAura(SPELL_DETECT_INVIS, summoner);
                                    summoner->CastSpell(summoner, SPELL_SUMMON_MATRIARCH, true);
                                    Talk(SAY_MATRIARCH_AGGRO, summoner->GetGUID());
								}
							}
						}
                    }

                    events.ScheduleEvent(EVENT_CHECK_SUMMON_AURA, 5000);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tiger_matriarch_creditAI(creature);
        }
};

class npc_tiger_matriarch : public CreatureScript
{
    public:
        npc_tiger_matriarch() : CreatureScript("npc_tiger_matriarch") {}

        struct npc_tiger_matriarchAI : public QuantumBasicAI
        {
            npc_tiger_matriarchAI(Creature* creature) : QuantumBasicAI(creature), tigerGuid(0) {}

			EventMap events;
            uint64 tigerGuid;

            void EnterToBattle(Unit* /*who*/)
            {
                events.Reset();
                events.ScheduleEvent(EVENT_POUNCE, 100);
                events.ScheduleEvent(EVENT_NOSUMMON, 50000);
            }

            void IsSummonedBy(Unit* summoner)
            {
                if (summoner->GetTypeId() != TYPE_ID_PLAYER || !summoner->GetVehicle())
					return;

                tigerGuid = summoner->GetVehicle()->GetBase()->GetGUID();
                if (Unit* tiger = ObjectAccessor::GetUnit(*me, tigerGuid))
                {
                    me->AddThreat(tiger, 500000.0f);
                    DoCast(me, SPELL_FURIOUS_BITE);
                }
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() != TYPE_ID_UNIT || !victim->IsSummon())
                    return;

                if (Unit* vehSummoner = victim->ToTempSummon()->GetSummoner())
                {
                    vehSummoner->RemoveAurasDueToSpell(SPELL_NO_SUMMON_AURA);
                    vehSummoner->RemoveAurasDueToSpell(SPELL_DETECT_INVIS);
                    vehSummoner->RemoveAurasDueToSpell(SPELL_SPIRIT_OF_THE_TIGER_RIDER);
                    vehSummoner->RemoveAurasDueToSpell(SPELL_SUMMON_ZENTABRA_TRIGGER);
                }
                me->DespawnAfterAction();
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (!attacker->IsSummon())
                    return;

                if (HealthBelowPct(20))
                {
                    damage = 0;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    if (Unit* vehSummoner = attacker->ToTempSummon()->GetSummoner())
                    {
                        vehSummoner->AddAura(SPELL_SUMMON_ZENTABRA_TRIGGER, vehSummoner);
                        vehSummoner->CastSpell(vehSummoner, SPELL_SUMMON_ZENTABRA, true);
                        attacker->CastSpell(attacker, SPELL_EJECT_PASSENGERS, true);
                        vehSummoner->RemoveAurasDueToSpell(SPELL_NO_SUMMON_AURA);
                        vehSummoner->RemoveAurasDueToSpell(SPELL_DETECT_INVIS);
                        vehSummoner->RemoveAurasDueToSpell(SPELL_SPIRIT_OF_THE_TIGER_RIDER);
                        vehSummoner->RemoveAurasDueToSpell(SPELL_SUMMON_ZENTABRA_TRIGGER);
                    }
                    me->DespawnAfterAction();
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (!tigerGuid)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_POUNCE:
                            DoCastVictim(SPELL_POUNCE);
                            events.ScheduleEvent(EVENT_POUNCE, 30000);
                            break;
                        case EVENT_NOSUMMON: // Reapply SPELL_NO_SUMMON_AURA
                            if (Unit* tiger = ObjectAccessor::GetUnit(*me, tigerGuid))
                            {
                                if (tiger->IsSummon())
                                    if (Unit* vehSummoner = tiger->ToTempSummon()->GetSummoner())
                                        me->AddAura(SPELL_NO_SUMMON_AURA, vehSummoner);
                            }
                            events.ScheduleEvent(EVENT_NOSUMMON, 50000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tiger_matriarchAI(creature);
        }
};

uint32 const trollmodel[] =
{
	11665, 11734, 11750, 12037,
	12038, 12042, 12049, 12849,
	13529, 14759, 15570, 15701,
	15702, 1882,  1897, 1976,
	2025, 27286, 2734, 2735,
	4084, 4085, 4087, 4089,
	4231, 4357,4358, 4360,
	4361, 4362, 4363, 4370,
	4532, 4537, 4540, 4610,
	6839, 7037, 9767, 9768,
};

class npc_troll_volunteer : public CreatureScript
{
    public:
        npc_troll_volunteer() : CreatureScript("npc_troll_volunteer") { }

        struct npc_troll_volunteerAI : public QuantumBasicAI
        {
            npc_troll_volunteerAI(Creature* creature) : QuantumBasicAI(creature) {}

			uint32 MountModel;

            bool Complete;

            void InitializeAI()
            {
                if (me->IsDead() || !me->GetOwner())
                    return;

                Reset();

                switch (urand(0, 3))
                {
                    case 0:
                        MountModel = 6471;
                        break;
                    case 1:
                        MountModel = 6473;
                        break;
                    case 2:
                        MountModel = 6469;
                        break;
                    default:
                        MountModel = 6472;
                        break;
                }
                me->SetDisplayId(trollmodel[urand(0, 39)]);
                if (Player* player = me->GetOwner()->ToPlayer())
                    me->GetMotionMaster()->MoveFollow(player, 5.0f, float(rand_norm() + 1.0f) * M_PI / 3.0f * 4.0f);
            }

            void Reset()
            {
                Complete = false;
                me->AddAura(SPELL_VOLUNTEER_AURA, me);
                me->AddAura(SPELL_MOUNTING_CHECK, me);
                DoCast(me, SPELL_PETACT_AURA);
                me->SetReactState(REACT_PASSIVE);
                Talk(SAY_VOLUNTEER_START);
            }

            // This is needed for mount check aura to know what MountModel the npc got stored
            uint32 GetMountId()
            {
                return MountModel;
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id == POINT_URUZIN)
                    me->DespawnAfterAction();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (!Complete && spell->Id == SPELL_AOE_TURNIN && caster->GetEntry() == NPC_URUZIN)
                {
                    Complete = true;
                    DoCast(me, SPELL_TURNIN);
                    DoCast(me, SPELL_QUEST_CREDIT);
                    me->RemoveAurasDueToSpell(SPELL_MOUNTING_CHECK);
                    me->RemoveMount();
                    Talk(SAY_VOLUNTEER_END);
                    me->GetMotionMaster()->MovePoint(POINT_URUZIN, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_troll_volunteerAI(creature);
        }
};

typedef npc_troll_volunteer::npc_troll_volunteerAI VolunteerAI;

class spell_mount_check : public SpellScriptLoader
{
    public:
        spell_mount_check() : SpellScriptLoader("spell_mount_check") {}

        class spell_mount_check_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mount_check_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNTING_CHECK))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();
                Unit* owner = target->GetOwner();

                if (!owner)
                    return;

                if (owner->IsMounted() && !target->IsMounted())
                {
                    if (Creature* volunteer = target->ToCreature())
					{
                        if (uint32 mountid = CAST_AI(npc_troll_volunteer::npc_troll_volunteerAI, volunteer->AI())->GetMountId())
                            target->Mount(mountid);
					}
                }
                else if (!owner->IsMounted() && target->IsMounted())
                    target->RemoveMount();

                target->SetSpeed(MOVE_RUN, owner->GetSpeedRate(MOVE_RUN));
                target->SetSpeed(MOVE_WALK, owner->GetSpeedRate(MOVE_WALK));
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mount_check_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mount_check_AuraScript();
        }
};

class spell_voljin_war_drums : public SpellScriptLoader
{
    public:
        spell_voljin_war_drums() : SpellScriptLoader("spell_voljin_war_drums") {}

        class spell_voljin_war_drums_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_voljin_war_drums_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MOTIVATE_1))
                    return false;

                if (!sSpellMgr->GetSpellInfo(SPELL_MOTIVATE_2))
                    return false;
               return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    uint32 motivate = 0;
                    if (target->GetEntry() == NPC_CITIZEN_1)
                        motivate = SPELL_MOTIVATE_1;

                    else if (target->GetEntry() == NPC_CITIZEN_2)
                        motivate = SPELL_MOTIVATE_2;

                    if (motivate)
                        caster->CastSpell(target, motivate, false);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_voljin_war_drums_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_voljin_war_drums_SpellScript();
        }
};

enum VoodooSpells
{
    SPELL_BREW      = 16712, // Special Brew
    SPELL_GHOSTLY   = 16713, // Ghostly
    SPELL_HEX1      = 16707, // Hex
    SPELL_HEX2      = 16708, // Hex
    SPELL_HEX3      = 16709, // Hex
    SPELL_GROW      = 16711, // Grow
    SPELL_LAUNCH    = 16716 // Launch (Whee!)
};

class spell_voodoo : public SpellScriptLoader
{
    public:
        spell_voodoo() : SpellScriptLoader("spell_voodoo") {}

        class spell_voodoo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_voodoo_SpellScript)

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BREW) || !sSpellMgr->GetSpellInfo(SPELL_GHOSTLY) ||
					!sSpellMgr->GetSpellInfo(SPELL_HEX1) || !sSpellMgr->GetSpellInfo(SPELL_HEX2) ||
                    !sSpellMgr->GetSpellInfo(SPELL_HEX3) || !sSpellMgr->GetSpellInfo(SPELL_GROW) ||
                    !sSpellMgr->GetSpellInfo(SPELL_LAUNCH))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 spellid = RAND(SPELL_BREW, SPELL_GHOSTLY, RAND(SPELL_HEX1, SPELL_HEX2, SPELL_HEX3), SPELL_GROW, SPELL_LAUNCH);

                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, spellid, false);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_voodoo_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_voodoo_SpellScript();
        }
};

void AddSC_durotar()
{
    new npc_lazy_peon();
    new npc_tiger_matriarch_credit();
    new npc_tiger_matriarch();
    new npc_troll_volunteer();
    new spell_mount_check();
    new spell_voljin_war_drums();
    new spell_voodoo();
}