/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_INTRO_1                 = -1659118,
    SAY_INTRO_2                 = -1659119,
    SAY_KELESETH_INVOCATION     = -1659120,
    EMOTE_KELESETH_INVOCATION   = -1659121,
    SAY_KELESETH_SPECIAL        = -1659122,
    SAY_KELESETH_KILL_1         = -1659123,
	SAY_KELESETH_KILL_2         = -1659124,
    EMOTE_KELESETH_BERSERK      = -1659125,
    SAY_KELESETH_DEATH          = -1659126,
    SAY_TALDARAM_INVOCATION     = -1659127,
    EMOTE_TALDARAM_INVOCATION   = -1659128,
    SAY_TALDARAM_SPECIAL        = -1659129,
    EMOTE_TALDARAM_FLAME        = -1659130,
    SAY_TALDARAM_KILL_1         = -1659131,
	SAY_TALDARAM_KILL_2         = -1659132,
    EMOTE_TALDARAM_BERSERK      = -1659133,
    EMOTE_TALDARAM_DEATH        = -1659134,
    SAY_VALANAR_INVOCATION      = -1659135,
    EMOTE_VALANAR_INVOCATION    = -1659136,
    SAY_VALANAR_SPECIAL         = -1659137,
    EMOTE_VALANAR_SHOCK_VORTEX  = -1659138,
    SAY_VALANAR_KILL_1          = -1659139,
	SAY_VALANAR_KILL_2          = -1659140,
    SAY_VALANAR_BERSERK         = -1659141,
    SAY_VALANAR_DEATH           = -1659142,
};

enum Spells
{
    SPELL_FEIGN_DEATH                   = 71598,
    SPELL_OOC_INVOCATION_VISUAL         = 70934,
    SPELL_INVOCATION_VISUAL_ACTIVE      = 71596,
    // Heroic mode
    SPELL_SHADOW_PRISON                 = 72998,
    SPELL_SHADOW_PRISON_DAMAGE          = 72999,
    SPELL_SHADOW_PRISON_DUMMY           = 73001,
    // Prince Keleseth
    SPELL_INVOCATION_OF_BLOOD_KELESETH  = 70981,
    SPELL_SHADOW_RESONANCE              = 71943,
    SPELL_SHADOW_LANCE_10N              = 71405,
	SPELL_SHADOW_LANCE_25N              = 72804,
	SPELL_SHADOW_LANCE_10H              = 72805,
	SPELL_SHADOW_LANCE_25H              = 72806,
    SPELL_EMPOWERED_SHADOW_LANCE_10N    = 71815,
	SPELL_EMPOWERED_SHADOW_LANCE_25N    = 72809,
	SPELL_EMPOWERED_SHADOW_LANCE_10H    = 72810,
	SPELL_EMPOWERED_SHADOW_LANCE_25H    = 72811,
    // Dark Nucleus
    SPELL_SHADOW_RESONANCE_AURA         = 72980,
    SPELL_SHADOW_RESONANCE_RESIST       = 71822,
    // Prince Taldaram
    SPELL_INVOCATION_OF_BLOOD_TALDARAM  = 70982,
    SPELL_GLITTERING_SPARKS             = 71806,
    SPELL_CONJURE_FLAME                 = 71718,
    SPELL_CONJURE_EMPOWERED_FLAME       = 72040,
    // Ball of Flame
    SPELL_FLAME_SPHERE_SPAWN_EFFECT     = 55891, // cast from creature_template_addon (needed cast before entering world)
    SPELL_BALL_OF_FLAMES_VISUAL         = 71706,
    SPELL_BALL_OF_FLAMES                = 71714,
    SPELL_FLAMES                        = 71393,
    SPELL_FLAME_SPHERE_DEATH_EFFECT     = 55947,
    // Ball of Inferno Flame
    SPELL_BALL_OF_FLAMES_PROC           = 71756,
    SPELL_BALL_OF_FLAMES_PERIODIC       = 71709,
    // Prince Valanar
    SPELL_INVOCATION_OF_BLOOD_VALANAR   = 70952,
    SPELL_KINETIC_BOMB_TARGET           = 72053,
    SPELL_KINETIC_BOMB                  = 72080,
    SPELL_SHOCK_VORTEX                  = 72037,
    SPELL_EMPOWERED_SHOCK_VORTEX        = 72039,
    // Kinetic Bomb
    SPELL_UNSTABLE                      = 72059,
    SPELL_KINETIC_BOMB_VISUAL           = 72054,
    SPELL_KINETIC_BOMB_EXPLOSION        = 72052,
    SPELL_KINETIC_BOMB_KNOCKBACK        = 72087,
    // Shock Vortex
    SPELL_SHOCK_VORTEX_PERIODIC         = 71945,
    SPELL_SHOCK_VORTEX_DUMMY            = 72633,
};

enum Events
{
    EVENT_INTRO_1               = 1,
    EVENT_INTRO_2               = 2,
    EVENT_INVOCATION_OF_BLOOD   = 3,
    EVENT_BERSERK               = 4,
    // Keleseth
    EVENT_SHADOW_RESONANCE      = 5,
    EVENT_SHADOW_LANCE          = 6,
    // Taldaram
    EVENT_GLITTERING_SPARKS     = 7,
    EVENT_CONJURE_FLAME         = 8,
    // Valanar
    EVENT_KINETIC_BOMB          = 9,
    EVENT_SHOCK_VORTEX          = 10,
    EVENT_BOMB_DESPAWN          = 11,
    EVENT_CONTINUE_FALLING      = 12,
	EVENT_CHECK_BOMB_TIMER      = 13,
};

enum Actions
{
    ACTION_STAND_UP             = 1,
    ACTION_CAST_INVOCATION      = 2,
    ACTION_REMOVE_INVOCATION    = 3,
    ACTION_KINETIC_BOMB_JUMP    = 4,
    ACTION_FLAME_BALL_CHASE     = 5,
};

enum Points
{
    POINT_INTRO_DESPAWN         = 380040,
    POINT_KINETIC_BOMB_IMPACT   = 384540,
};

enum Displays
{
    DISPLAY_KINETIC_BOMB        = 31095,
};

class StandUpEvent : public BasicEvent
{
    public:
        StandUpEvent(Creature& owner) : BasicEvent(), owner(owner) { }

        bool Execute(uint64 /*eventTime*/, uint32 /*diff*/)
        {
            owner.HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
            return true;
        }

    private:
        Creature& owner;
};

Position const introFinalPos = {4660.490f, 2769.200f, 430.0000f, 0.000000f};
Position const triggerPos    = {4680.231f, 2769.134f, 379.9256f, 3.121708f};
Position const triggerEndPos = {4680.180f, 2769.150f, 365.5000f, 3.121708f};

class boss_blood_council_controller : public CreatureScript
{
    public:
        boss_blood_council_controller() : CreatureScript("boss_blood_council_controller") { }

        struct boss_blood_council_controllerAI : public BossAI
        {
            boss_blood_council_controllerAI(Creature* creature) : BossAI(creature, DATA_BLOOD_PRINCE_COUNCIL){}

            void Reset()
            {
                events.Reset();
                me->SetReactState(REACT_PASSIVE);
                InvocationStage = 0;
                ResetCounter = 0;

                instance->SetBossState(DATA_BLOOD_PRINCE_COUNCIL, NOT_STARTED);
            }

            void EnterToBattle(Unit* who)
            {
                if (instance->GetBossState(DATA_BLOOD_PRINCE_COUNCIL) == IN_PROGRESS)
                    return;

                if (!instance->CheckRequiredBosses(DATA_BLOOD_PRINCE_COUNCIL, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                instance->SetBossState(DATA_BLOOD_PRINCE_COUNCIL, IN_PROGRESS);

                DoCast(me, SPELL_INVOCATION_OF_BLOOD_VALANAR);

                if (Creature* keleseth = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_KELESETH_GUID)))
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, keleseth);
                    DoZoneInCombat(keleseth);
                }

                if (Creature* taldaram = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_TALDARAM_GUID)))
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, taldaram);
                    DoZoneInCombat(taldaram);
                }

                if (Creature* valanar = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_VALANAR_GUID)))
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, valanar);
                    DoZoneInCombat(valanar);
                }

                events.ScheduleEvent(EVENT_INVOCATION_OF_BLOOD, 46500);

                InvocationOrder[0] = InvocationData(instance->GetData64(DATA_PRINCE_VALANAR_GUID), SPELL_INVOCATION_OF_BLOOD_VALANAR, EMOTE_VALANAR_INVOCATION, 71070);
                if (urand(0, 1))
                {
                    InvocationOrder[1] = InvocationData(instance->GetData64(DATA_PRINCE_TALDARAM_GUID), SPELL_INVOCATION_OF_BLOOD_TALDARAM, EMOTE_TALDARAM_INVOCATION, 71081);
                    InvocationOrder[2] = InvocationData(instance->GetData64(DATA_PRINCE_KELESETH_GUID), SPELL_INVOCATION_OF_BLOOD_KELESETH, EMOTE_KELESETH_INVOCATION, 71080);
                }
                else
                {
                    InvocationOrder[1] = InvocationData(instance->GetData64(DATA_PRINCE_KELESETH_GUID), SPELL_INVOCATION_OF_BLOOD_KELESETH, EMOTE_KELESETH_INVOCATION, 71080);
                    InvocationOrder[2] = InvocationData(instance->GetData64(DATA_PRINCE_TALDARAM_GUID), SPELL_INVOCATION_OF_BLOOD_TALDARAM, EMOTE_TALDARAM_INVOCATION, 71081);
                }
            }

            void SetData(uint32 /*type*/, uint32 data)
            {
                ResetCounter += uint8(data);
                if (ResetCounter == 3)
                    EnterEvadeMode();
            }

            void JustReachedHome()
            {
                ResetCounter = 0;
                if (Creature* keleseth = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_KELESETH_GUID)))
                    keleseth->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (Creature* taldaram = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_TALDARAM_GUID)))
                    taldaram->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (Creature* valanar = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_VALANAR_GUID)))
                    valanar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void JustDied(Unit* killer)
            {
                _JustDied();

                // kill all prices
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (++InvocationStage == 3)
                        InvocationStage = 0;

                    if (Creature* prince = ObjectAccessor::GetCreature(*me, InvocationOrder[InvocationStage].guid))
                    {
                        // make sure looting is allowed
                        if (me->IsDamageEnoughForLootingAndReward())
                            prince->LowerPlayerDamageReq(prince->GetMaxHealth());

                        killer->Kill(prince);
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
                        case EVENT_INVOCATION_OF_BLOOD:
                        {
                            Creature* oldPrince = ObjectAccessor::GetCreature(*me, InvocationOrder[InvocationStage].guid);
                            if (++InvocationStage == 3)
                                InvocationStage = 0;

                            Creature* newPrince = ObjectAccessor::GetCreature(*me, InvocationOrder[InvocationStage].guid);
                            if (oldPrince)
                            {
                                oldPrince->AI()->DoAction(ACTION_REMOVE_INVOCATION);
                                if (newPrince)
                                    oldPrince->CastSpell(newPrince, InvocationOrder[InvocationStage].visualSpell, true);
                            }

                            if (newPrince)
                            {
                                newPrince->SetHealth(me->GetHealth());
								DoSendQuantumText(InvocationOrder[InvocationStage].textId, newPrince);
                            }

                            DoCast(me, InvocationOrder[InvocationStage].spellId);
                            events.ScheduleEvent(EVENT_INVOCATION_OF_BLOOD, 46500);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

        private:
            struct InvocationData
            {
                uint64 guid;
                uint32 spellId;
                uint32 textId;
                uint32 visualSpell;

                InvocationData(uint64 Guid, uint32 SpellId, uint32 TextId, uint32 VisualSpell)
                {
                    guid = Guid;
                    spellId = SpellId;
                    textId = TextId;
                    visualSpell = VisualSpell;
                }

                InvocationData() : guid(0), spellId(0), textId(0), visualSpell(0) { }
            }

			InvocationOrder[3];

            uint32 InvocationStage;
            uint32 ResetCounter;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_blood_council_controllerAI>(creature);
        }
};

class boss_prince_keleseth_icc : public CreatureScript
{
    public:
        boss_prince_keleseth_icc() : CreatureScript("boss_prince_keleseth_icc") { }

        struct boss_prince_kelesethAI : public BossAI
        {
            boss_prince_kelesethAI(Creature* creature) : BossAI(creature, DATA_BLOOD_PRINCE_COUNCIL)
            {
                IsEmpowered = false;
                SpawnHealth = creature->GetMaxHealth();
            }

            void InitializeAI()
            {
                if (CreatureData const* data = sObjectMgr->GetCreatureData(me->GetDBTableGUIDLow()))
                    if (data->curhealth)
                        SpawnHealth = data->curhealth;

                if (!me->IsDead())
                    JustRespawned();

                me->SetReactState(REACT_DEFENSIVE);
            }

            void Reset()
            {
                events.Reset();
                summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                IsEmpowered = false;
                me->SetHealth(SpawnHealth);
                instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(true));
                me->SetReactState(REACT_DEFENSIVE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BLOOD_PRINCES_CONTROL)))
                    DoZoneInCombat(controller);

                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_SHADOW_RESONANCE, urand(10000, 15000));
                events.ScheduleEvent(EVENT_SHADOW_LANCE, 2000);

                if (IsHeroic())
                {
                    me->AddAura(SPELL_SHADOW_PRISON, me);
                    DoCast(me, SPELL_SHADOW_PRISON_DUMMY);
                }
            }

            void JustDied(Unit* killer)
            {
                events.Reset();
                summons.DespawnAll();

                DoSendQuantumText(SAY_KELESETH_DEATH, me);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void JustReachedHome()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetHealth(SpawnHealth);

                IsEmpowered = false;

                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BLOOD_PRINCES_CONTROL)))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    controller->AI()->SetData(0, 1);
                }
            }

            void JustRespawned()
            {
                DoCast(me, SPELL_FEIGN_DEATH);
                me->SetHealth(SpawnHealth);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_INVOCATION_OF_BLOOD_KELESETH)
                    DoAction(ACTION_CAST_INVOCATION);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                Position pos;
                me->GetPosition(&pos);
                float maxRange = me->GetDistance2d(summon);
                float angle = me->GetAngle(summon);
                me->MovePositionToFirstCollision(pos, maxRange, angle);
				summon->Relocate(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
				summon->SendMovementFlagUpdate();
                summon->ToTempSummon()->SetTempSummonType(TEMPSUMMON_CORPSE_DESPAWN);
            }

            void DamageDealt(Unit* /*target*/, uint32& damage, DamageEffectType damageType)
            {
                if (damageType != SPELL_DIRECT_DAMAGE)
                    return;

                if (damage > RAID_MODE<uint32>(23000, 25000, 23000, 25000))
                    instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(false));
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (!IsEmpowered)
                {
                    me->AddThreat(attacker, float(damage));
                    damage = 0;
                }
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_KELESETH_KILL_1, SAY_KELESETH_KILL_2), me);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_STAND_UP:
                        me->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                        me->ForceValuesUpdateAtIndex(UNIT_NPC_FLAGS);   // was in sniff. don't ask why
                        me->m_Events.AddEvent(new StandUpEvent(*me), me->m_Events.CalculateTime(1000));
                        break;
                    case ACTION_CAST_INVOCATION:
                        DoSendQuantumText(SAY_KELESETH_INVOCATION, me);
                        DoCast(me, SPELL_INVOCATION_VISUAL_ACTIVE, true);
                        IsEmpowered = true;
                        break;
                    case ACTION_REMOVE_INVOCATION:
                        me->SetHealth(SpawnHealth);
                        me->RemoveAurasDueToSpell(SPELL_INVOCATION_VISUAL_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_KELESETH);
                        IsEmpowered = false;
                        break;
                    default:
                        break;
                }
            }

            bool CheckRoom()
            {
                if (!CheckBoundary(me))
                {
                    EnterEvadeMode();
                    if (Creature* taldaram = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_TALDARAM_GUID)))
                        taldaram->AI()->EnterEvadeMode();

                    if (Creature* valanar = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_VALANAR_GUID)))
                        valanar->AI()->EnterEvadeMode();

                    return false;
                }

                return true;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !CheckRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            DoSendQuantumText(EMOTE_KELESETH_BERSERK, me);
                            break;
                        case EVENT_SHADOW_RESONANCE:
                            DoSendQuantumText(SAY_KELESETH_SPECIAL, me);
                            DoCast(me, SPELL_SHADOW_RESONANCE);
                            events.ScheduleEvent(EVENT_SHADOW_RESONANCE, urand(10000, 15000));
                            break;
                        case EVENT_SHADOW_LANCE:
							if (me->GetVictim())
                            {
                                if (!me->IsWithinLOS(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ()) && me->GetDistance(me->GetVictim()) < 150.0f)
                                {
                                    me->Relocate(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ());
                                    me->SendMovementFlagUpdate();
                                }
                            }

                            if (IsEmpowered)
                                DoCast(RAID_MODE(SPELL_EMPOWERED_SHADOW_LANCE_10N, SPELL_EMPOWERED_SHADOW_LANCE_25N, SPELL_EMPOWERED_SHADOW_LANCE_10H, SPELL_EMPOWERED_SHADOW_LANCE_25H));
                            else
                                DoCast(RAID_MODE(SPELL_SHADOW_LANCE_10N, SPELL_SHADOW_LANCE_25N, SPELL_SHADOW_LANCE_10H, SPELL_SHADOW_LANCE_25H));
                            events.ScheduleEvent(EVENT_SHADOW_LANCE, 2000);
                            break;
                        default:
                            break;
                    }
                }
				// does not melee
            }

        private:
            uint32 SpawnHealth;
            bool IsEmpowered;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_prince_kelesethAI>(creature);
        }
};

class boss_prince_taldaram_icc : public CreatureScript
{
    public:
        boss_prince_taldaram_icc() : CreatureScript("boss_prince_taldaram_icc") { }

        struct boss_prince_taldaramAI : public BossAI
        {
            boss_prince_taldaramAI(Creature* creature) : BossAI(creature, DATA_BLOOD_PRINCE_COUNCIL)
            {
                IsEmpowered = false;
                SpawnHealth = creature->GetMaxHealth();
            }

			uint32 SpawnHealth;
            bool IsEmpowered;

            void InitializeAI()
            {
                if (CreatureData const* data = sObjectMgr->GetCreatureData(me->GetDBTableGUIDLow()))
                    if (data->curhealth)
                        SpawnHealth = data->curhealth;

                if (!me->IsDead())
                    JustRespawned();

                me->SetReactState(REACT_DEFENSIVE);
            }

            void Reset()
            {
                events.Reset();
                summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                IsEmpowered = false;
                me->SetHealth(SpawnHealth);
                instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(true));
                me->SetReactState(REACT_DEFENSIVE);
            }

            void MoveInLineOfSight(Unit* /*who*/) {}

            void EnterToBattle(Unit* /*who*/)
            {
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BLOOD_PRINCES_CONTROL)))
                    DoZoneInCombat(controller);

                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_GLITTERING_SPARKS, urand(12000, 15000));
                events.ScheduleEvent(EVENT_CONJURE_FLAME, 20000);
                if (IsHeroic())
                    me->AddAura(SPELL_SHADOW_PRISON, me);
            }

            void JustDied(Unit* killer)
            {
                events.Reset();
                summons.DespawnAll();

                DoSendQuantumText(EMOTE_TALDARAM_DEATH, me);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void JustReachedHome()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SetHealth(SpawnHealth);
                IsEmpowered = false;
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BLOOD_PRINCES_CONTROL)))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    controller->AI()->SetData(0, 1);
                }
            }

            void JustRespawned()
            {
                DoCast(me, SPELL_FEIGN_DEATH);
                me->SetHealth(SpawnHealth);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_INVOCATION_OF_BLOOD_TALDARAM)
                    DoAction(ACTION_CAST_INVOCATION);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                Unit* target = SelectTarget(TARGET_RANDOM, 1, -10.0f, true); // first try at distance
                if (!target)
                    target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true);     // too bad for you raiders, its going to boom

                if (summon->GetEntry() == NPC_BALL_OF_INFERNO_FLAME && target)
					DoSendQuantumText(EMOTE_TALDARAM_FLAME, target);

                if (target)
                    summon->AI()->SetGUID(target->GetGUID());
            }

            void DamageDealt(Unit* /*target*/, uint32& damage, DamageEffectType damageType)
            {
                if (damageType != SPELL_DIRECT_DAMAGE)
                    return;

                if (damage > RAID_MODE<uint32>(23000, 25000, 23000, 25000))
                    instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(false));
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (!IsEmpowered)
                {
                    me->AddThreat(attacker, float(damage));
                    damage = 0;
                }
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_TALDARAM_KILL_1, SAY_TALDARAM_KILL_2), me);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_STAND_UP:
                        me->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                        me->ForceValuesUpdateAtIndex(UNIT_NPC_FLAGS);
                        me->m_Events.AddEvent(new StandUpEvent(*me), me->m_Events.CalculateTime(1000));
                        break;
                    case ACTION_CAST_INVOCATION:
                        DoSendQuantumText(SAY_TALDARAM_INVOCATION, me);
                        DoCast(me, SPELL_INVOCATION_VISUAL_ACTIVE, true);
                        IsEmpowered = true;
                        break;
                    case ACTION_REMOVE_INVOCATION:
                        me->SetHealth(SpawnHealth);
                        me->RemoveAurasDueToSpell(SPELL_INVOCATION_VISUAL_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_TALDARAM);
                        IsEmpowered = false;
                        break;
                    default:
                        break;
                }
            }

            bool CheckRoom()
            {
                if (!CheckBoundary(me))
                {
                    EnterEvadeMode();
                    if (Creature* keleseth = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_KELESETH_GUID)))
                        keleseth->AI()->EnterEvadeMode();

                    if (Creature* valanar = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_VALANAR_GUID)))
                        valanar->AI()->EnterEvadeMode();

                    return false;
                }

                return true;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !CheckRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            DoSendQuantumText(EMOTE_TALDARAM_BERSERK, me);
                            break;
                        case EVENT_GLITTERING_SPARKS:
                            DoCastVictim(SPELL_GLITTERING_SPARKS);
                            events.ScheduleEvent(EVENT_GLITTERING_SPARKS, urand(15000, 50000));
                            break;
                        case EVENT_CONJURE_FLAME:
                            if (IsEmpowered)
                            {
                                DoCast(me, SPELL_CONJURE_EMPOWERED_FLAME);
                                events.ScheduleEvent(EVENT_CONJURE_FLAME, urand(15000, 25000));
                            }
                            else
                            {
                                DoCast(me, SPELL_CONJURE_FLAME);
                                events.ScheduleEvent(EVENT_CONJURE_FLAME, urand(20000, 30000));
                            }
                            DoSendQuantumText(SAY_TALDARAM_SPECIAL, me);
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
            return GetIcecrownCitadelAI<boss_prince_taldaramAI>(creature);
        }
};

class boss_prince_valanar_icc : public CreatureScript
{
    public:
        boss_prince_valanar_icc() : CreatureScript("boss_prince_valanar_icc") { }

        struct boss_prince_valanarAI : public BossAI
        {
            boss_prince_valanarAI(Creature* creature) : BossAI(creature, DATA_BLOOD_PRINCE_COUNCIL)
            {
                IsEmpowered = false;
                SpawnHealth = creature->GetMaxHealth();
            }

			uint32 SpawnHealth;
            bool IsEmpowered;

            void InitializeAI()
            {
                if (CreatureData const* data = sObjectMgr->GetCreatureData(me->GetDBTableGUIDLow()))
                    if (data->curhealth)
                        SpawnHealth = data->curhealth;

                if (!me->IsDead())
                    JustRespawned();

                me->SetReactState(REACT_DEFENSIVE);
            }

            void Reset()
            {
                events.Reset();
                summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                IsEmpowered = false;
                me->SetHealth(me->GetMaxHealth());
                instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(true));
                me->SetReactState(REACT_DEFENSIVE);
            }

            void MoveInLineOfSight(Unit* /*who*/){}

            void EnterToBattle(Unit* /*who*/)
            {
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BLOOD_PRINCES_CONTROL)))
                    DoZoneInCombat(controller);

                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_KINETIC_BOMB, urand(18000, 24000));
                events.ScheduleEvent(EVENT_SHOCK_VORTEX, urand(15000, 20000));
                if (IsHeroic())
                    me->AddAura(SPELL_SHADOW_PRISON, me);
            }

            void JustDied(Unit* killer)
            {
                events.Reset();
                summons.DespawnAll();

                DoSendQuantumText(SAY_VALANAR_DEATH, me);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void JustReachedHome()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SetHealth(me->GetMaxHealth());
                IsEmpowered = false;
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BLOOD_PRINCES_CONTROL)))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    controller->AI()->SetData(0, 1);
                }
            }

            void JustRespawned()
            {
                DoCast(me, SPELL_FEIGN_DEATH);
                me->SetHealth(SpawnHealth);
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_KINETIC_BOMB_TARGET:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->CastSpell(summon, SPELL_KINETIC_BOMB, true, 0, 0, me->GetGUID());
                        break;
                    case NPC_KINETIC_BOMB:
                    {
                        float x, y, z;
                        summon->GetPosition(x, y, z);
                        float ground_Z = summon->GetMap()->GetHeight(summon->GetPhaseMask(), x, y, z, true, 500.0f);
                        summon->GetMotionMaster()->MovePoint(POINT_KINETIC_BOMB_IMPACT, x, y, ground_Z);
                        summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    case NPC_SHOCK_VORTEX:
                        summon->CastSpell(summon, SPELL_SHOCK_VORTEX_DUMMY, true);
                        summon->CastSpell(summon, SPELL_SHOCK_VORTEX_PERIODIC, true);
                        break;
                    default:
                        break;
                }
                summons.Summon(summon);

                if (me->IsInCombatActive())
                    DoZoneInCombat(summon);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_INVOCATION_OF_BLOOD_VALANAR)
                    DoAction(ACTION_CAST_INVOCATION);
            }

            void DamageDealt(Unit* /*target*/, uint32& damage, DamageEffectType damageType)
            {
                if (damageType != SPELL_DIRECT_DAMAGE)
                    return;

                if (damage > RAID_MODE<uint32>(23000, 25000, 23000, 25000))
                    instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(false));
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (!IsEmpowered)
                {
                    me->AddThreat(attacker, float(damage));
                    damage = 0;
                }
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_VALANAR_KILL_1, SAY_VALANAR_KILL_2), me);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_STAND_UP:
                        me->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                        me->ForceValuesUpdateAtIndex(UNIT_NPC_FLAGS);   // was in sniff. don't ask why
                        me->m_Events.AddEvent(new StandUpEvent(*me), me->m_Events.CalculateTime(1000));
                        break;
                    case ACTION_CAST_INVOCATION:
						DoSendQuantumText(SAY_VALANAR_INVOCATION, me);
                        DoCast(me, SPELL_INVOCATION_VISUAL_ACTIVE, true);
                        IsEmpowered = true;
                        break;
                    case ACTION_REMOVE_INVOCATION:
                        me->SetHealth(SpawnHealth);
                        me->RemoveAurasDueToSpell(SPELL_INVOCATION_VISUAL_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_VALANAR);
                        IsEmpowered = false;
                        break;
                    default:
                        break;
                }
            }

            bool CheckRoom()
            {
                if (!CheckBoundary(me))
                {
                    EnterEvadeMode();
                    if (Creature* keleseth = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_KELESETH_GUID)))
                        keleseth->AI()->EnterEvadeMode();

                    if (Creature* taldaram = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_TALDARAM_GUID)))
                        taldaram->AI()->EnterEvadeMode();

                    return false;
                }
                return true;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !CheckRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            DoSendQuantumText(SAY_VALANAR_BERSERK, me);
                            break;
                        case EVENT_KINETIC_BOMB:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
                            {
                                DoCast(target, SPELL_KINETIC_BOMB_TARGET);
                                DoSendQuantumText(SAY_VALANAR_SPECIAL, me);
                            }
                            events.ScheduleEvent(EVENT_KINETIC_BOMB, urand(18000, 24000));
                            break;
                        case EVENT_SHOCK_VORTEX:
                            if (IsEmpowered)
                            {
                                DoCast(me, SPELL_EMPOWERED_SHOCK_VORTEX);
                                DoSendQuantumText(EMOTE_VALANAR_SHOCK_VORTEX, me);
                                events.ScheduleEvent(EVENT_SHOCK_VORTEX, 30000);
                            }
                            else
                            {
								if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
									DoCast(target, SPELL_SHOCK_VORTEX);
								events.ScheduleEvent(EVENT_SHOCK_VORTEX, urand(18000, 23000));
                            }
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
            return GetIcecrownCitadelAI<boss_prince_valanarAI>(creature);
        }
};

class npc_blood_queen_lana_thel : public CreatureScript
{
    public:
        npc_blood_queen_lana_thel() : CreatureScript("npc_blood_queen_lana_thel") { }

        struct npc_blood_queen_lana_thelAI : public QuantumBasicAI
        {
            npc_blood_queen_lana_thelAI(Creature* creature) : QuantumBasicAI(creature)
            {
                _introDone = false;
                instance = creature->GetInstanceScript();
            }

            void Reset()
            {
                _events.Reset();
				me->SetDisableGravity(true);
                if (instance->GetBossState(DATA_BLOOD_PRINCE_COUNCIL) == DONE)
                {
                    me->SetVisible(false);
                    _introDone = true;
                }
                else
                    me->SetVisible(true);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (_introDone)
                    return;

                if (!me->IsWithinDistInMap(who, 35.0f))
                    return;

                _introDone = true;
				DoSendQuantumText(SAY_INTRO_1, me);
                _events.SetPhase(1);
                _events.ScheduleEvent(EVENT_INTRO_1, 14000);
                // summon a visual trigger
                if (Creature* summon = DoSummon(NPC_FLOATING_TRIGGER, triggerPos, 15000, TEMPSUMMON_TIMED_DESPAWN))
                {
                    summon->CastSpell(summon, SPELL_OOC_INVOCATION_VISUAL, true);
                    summon->SetSpeed(MOVE_FLIGHT, 0.15f, true);
                    summon->GetMotionMaster()->MovePoint(0, triggerEndPos);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE && id == POINT_INTRO_DESPAWN)
                    me->SetVisible(false);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!_events.GetPhaseMask())
                    return;

                _events.Update(diff);

                if (_events.ExecuteEvent() == EVENT_INTRO_1)
                {
					DoSendQuantumText(SAY_INTRO_2, me);
                    me->GetMotionMaster()->MovePoint(POINT_INTRO_DESPAWN, introFinalPos);
                    _events.Reset();

                    // remove Feign Death from princes
                    if (Creature* keleseth = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_KELESETH_GUID)))
                        keleseth->AI()->DoAction(ACTION_STAND_UP);

                    if (Creature* taldaram = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_TALDARAM_GUID)))
                        taldaram->AI()->DoAction(ACTION_STAND_UP);

                    if (Creature* valanar = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PRINCE_VALANAR_GUID)))
                    {
                        valanar->AI()->DoAction(ACTION_STAND_UP);
                        valanar->SetHealth(valanar->GetMaxHealth());
                    }
                }
            }

        private:
            EventMap _events;
            InstanceScript* instance;
            bool _introDone;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_blood_queen_lana_thelAI>(creature);
        }
};

class npc_ball_of_flame : public CreatureScript
{
    public:
        npc_ball_of_flame() : CreatureScript("npc_ball_of_flame") { }

        struct npc_ball_of_flameAI : public QuantumBasicAI
        {
            npc_ball_of_flameAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                DespawnTimer = 0;
            }

            void Reset()
            {
                me->CastSpell(me, SPELL_BALL_OF_FLAMES_VISUAL, true);
                if (me->GetEntry() == NPC_BALL_OF_INFERNO_FLAME)
                {
                    me->CastSpell(me, SPELL_BALL_OF_FLAMES_PROC, true);
                    me->CastSpell(me, SPELL_BALL_OF_FLAMES_PERIODIC, true);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == CHASE_MOTION_TYPE && id == GUID_LOPART(ChaseGUID) && ChaseGUID)
                {
                    me->RemoveAurasDueToSpell(SPELL_BALL_OF_FLAMES_PERIODIC);
                    DoCast(me, SPELL_FLAMES);
                    DespawnTimer = 1000;
                    ChaseGUID = 0;
                }
            }

            void SetGUID(uint64 guid, int32 /*type*/)
            {
                ChaseGUID = guid;
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_FLAME_BALL_CHASE)
				{
                    if (Player* target = ObjectAccessor::GetPlayer(*me, ChaseGUID))
                    {
                        // need to clear states now because this call is before AuraEffect is fully removed
                        me->ClearUnitState(UNIT_STATE_CASTING | UNIT_STATE_STUNNED);
                        if (target && me->Attack(target, true))
						{
                            me->GetMotionMaster()->MoveChase(target, 1.0f);
						}
                    }
				}
            }

            void DamageDealt(Unit* /*target*/, uint32& damage, DamageEffectType damageType)
            {
                if (damageType != SPELL_DIRECT_DAMAGE)
                    return;

                if (damage > RAID_MODE<uint32>(23000, 25000, 23000, 25000))
                    instance->SetData(DATA_ORB_WHISPERER_ACHIEVEMENT, uint32(false));
            }

            void UpdateAI(uint32 const diff)
            {
                if (!DespawnTimer)
                    return;

                if (DespawnTimer <= diff)
                {
                    DespawnTimer = 0;
                    DoCast(me, SPELL_FLAME_SPHERE_DEATH_EFFECT);
                }
                else
                    DespawnTimer -= diff;
            }

        private:
            uint64 ChaseGUID;
            InstanceScript* instance;
            uint32 DespawnTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_ball_of_flameAI>(creature);
        }
};

class npc_kinetic_bomb : public CreatureScript
{
    public:
        npc_kinetic_bomb() : CreatureScript("npc_kinetic_bomb") { }

        struct npc_kinetic_bombAI : public QuantumBasicAI
        {
            npc_kinetic_bombAI(Creature* creature) : QuantumBasicAI(creature) { }

			EventMap events;
            float _x;
            float _y;
            float _groundZ;
            bool isExploding;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CHECK_BOMB_TIMER, 500);
                me->SetDisplayId(DISPLAY_KINETIC_BOMB);
                me->CastSpell(me, SPELL_UNSTABLE, true);
                me->CastSpell(me, SPELL_KINETIC_BOMB_VISUAL, true);
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_FLIGHT, IsHeroic() ? 0.3f : 0.15f, true);
                me->GetPosition(_x, _y, _groundZ);
                _groundZ = me->GetMap()->GetHeight(me->GetPhaseMask(), _x, _y, _groundZ, true, 500.0f);
                isExploding = false;
            }

            void DoAction(int32 const action)
            {
                if (action == SPELL_KINETIC_BOMB_EXPLOSION)
                {
                    isExploding = true;
                    me->CastSpell(me, SPELL_KINETIC_BOMB_EXPLOSION, true);
                    me->RemoveAurasDueToSpell(SPELL_KINETIC_BOMB_VISUAL);
                    events.ScheduleEvent(EVENT_BOMB_DESPAWN, 1000);
                }
                else if (action == ACTION_KINETIC_BOMB_JUMP)
                {
                    // Only begin moving up if we are not already moving up
                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != EFFECT_MOTION_TYPE)
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(_x, _y, 384.0f, 1.0f, 16.0f, 2);
                    }
                }
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != EFFECT_MOTION_TYPE)
                    return;

                if (point == 2)
                {
                    events.CancelEvent(EVENT_CONTINUE_FALLING);
                    events.ScheduleEvent(EVENT_CONTINUE_FALLING, 100);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BOMB_DESPAWN:
                            me->DespawnAfterAction();
                            break;
                        case EVENT_CONTINUE_FALLING:
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(POINT_KINETIC_BOMB_IMPACT, _x, _y, _groundZ);
                            break;
                        case EVENT_CHECK_BOMB_TIMER:
                        {
                            // Bomb exploding triggered, no need for further checking here anymore
                            if (isExploding)
                                break;

                            // Despawn after 1 minute
                            if (!me->HasAura(SPELL_UNSTABLE))
                                me->DespawnAfterAction();

                            // Explode if z coordinate is below a specific value - z coordinate is depending on the x position of the spawn, room has two levels
                            if (me->GetPositionZ() < (me->GetPositionX() < 4660.0f ? 361.4f : 364.3f))
                                DoAction(SPELL_KINETIC_BOMB_EXPLOSION);

                            events.ScheduleEvent(EVENT_CHECK_BOMB_TIMER, 500);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_kinetic_bombAI>(creature);
        }
};

class npc_dark_nucleus : public CreatureScript
{
    public:
        npc_dark_nucleus() : CreatureScript("npc_dark_nucleus") { }

        struct npc_dark_nucleusAI : public QuantumBasicAI
        {
            npc_dark_nucleusAI(Creature* creature) : QuantumBasicAI(creature)
            {
                LockedTarget = false;
                TargetAuraCheck = 0;
            }

			uint32 TargetAuraCheck;
            bool LockedTarget;

            void Reset()
            {
                me->SetReactState(REACT_DEFENSIVE);
                me->CastSpell(me, SPELL_SHADOW_RESONANCE_AURA, true);
            }

            void EnterToBattle(Unit* who)
            {
                TargetAuraCheck = 1000;
                if (me->GetDistance(who) >= 15.0f)
                {
                    DoStartMovement(who);
                    return;
                }

                DoCast(who, SPELL_SHADOW_RESONANCE_RESIST);
                me->ClearUnitState(UNIT_STATE_CASTING);
            }

            void MoveInLineOfSight(Unit* who)
            {
                QuantumBasicAI::MoveInLineOfSight(who);
            }

            void DamageTaken(Unit* attacker, uint32& /*damage*/)
            {
                if (attacker == me)
                    return;

                me->DeleteThreatList();
                me->AddThreat(attacker, 500000000.0f);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (TargetAuraCheck <= diff)
                {
                    TargetAuraCheck = 1000;
                    if (Unit* victim = me->GetVictim())
                        if (me->GetDistance(victim) < 15.0f &&
                            !victim->HasAura(SPELL_SHADOW_RESONANCE_RESIST, me->GetGUID()))
                        {
                            DoCast(victim, SPELL_SHADOW_RESONANCE_RESIST);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                        }
                        else
                            MoveInLineOfSight(me->GetVictim());
                }
                else TargetAuraCheck -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_dark_nucleusAI>(creature);
        }
};

class spell_taldaram_glittering_sparks : public SpellScriptLoader
{
    public:
        spell_taldaram_glittering_sparks() : SpellScriptLoader("spell_taldaram_glittering_sparks") { }

        class spell_taldaram_glittering_sparks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_taldaram_glittering_sparks_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_taldaram_glittering_sparks_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_taldaram_glittering_sparks_SpellScript();
        }
};

class spell_taldaram_summon_flame_ball : public SpellScriptLoader
{
    public:
        spell_taldaram_summon_flame_ball() : SpellScriptLoader("spell_taldaram_summon_flame_ball") { }

        class spell_taldaram_summon_flame_ball_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_taldaram_summon_flame_ball_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_taldaram_summon_flame_ball_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_taldaram_summon_flame_ball_SpellScript();
        }
};

class spell_taldaram_flame_ball_visual : public SpellScriptLoader
{
    public:
        spell_taldaram_flame_ball_visual() : SpellScriptLoader("spell_taldaram_flame_ball_visual") { }

        class spell_flame_ball_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_flame_ball_visual_AuraScript);

            bool Load()
            {
                if (GetCaster()->GetEntry() == NPC_BALL_OF_FLAME || GetCaster()->GetEntry() == NPC_BALL_OF_INFERNO_FLAME)
                    return true;
                return false;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* target = GetTarget()->ToCreature();
                if (!target)
                    return;

                // SPELL_FLAME_SPHERE_SPAWN_EFFECT
                if (GetSpellInfo()->Id == SPELL_FLAME_SPHERE_SPAWN_EFFECT)
                {
                    target->CastSpell(target, SPELL_BALL_OF_FLAMES, true);
                    target->AI()->DoAction(ACTION_FLAME_BALL_CHASE);
                }
                else // SPELL_FLAME_SPHERE_DEATH_EFFECT
                    target->DespawnAfterAction();
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_flame_ball_visual_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_flame_ball_visual_AuraScript();
        }
};

class spell_taldaram_ball_of_inferno_flame : public SpellScriptLoader
{
    public:
        spell_taldaram_ball_of_inferno_flame() : SpellScriptLoader("spell_taldaram_ball_of_inferno_flame") { }

        class spell_taldaram_ball_of_inferno_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_taldaram_ball_of_inferno_flame_SpellScript);

            void ModAuraStack()
            {
                if (Aura* aur = GetHitAura())
                    aur->SetStackAmount(uint8(GetSpellInfo()->StackAmount));
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_taldaram_ball_of_inferno_flame_SpellScript::ModAuraStack);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_taldaram_ball_of_inferno_flame_SpellScript();
        }
};

class spell_valanar_kinetic_bomb : public SpellScriptLoader
{
    public:
        spell_valanar_kinetic_bomb() : SpellScriptLoader("spell_valanar_kinetic_bomb") { }

        class spell_valanar_kinetic_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_valanar_kinetic_bomb_SpellScript);

            void ChangeSummonPos(SpellEffIndex /*effIndex*/)
            {
                WorldLocation summonPos = *GetExplTargetDest();
                Position offset = {0.0f, 0.0f, 20.0f, 0.0f};
                summonPos.RelocateOffset(offset);
                SetExplTargetDest(summonPos);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_valanar_kinetic_bomb_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        class spell_valanar_kinetic_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_valanar_kinetic_bomb_AuraScript);

            void HandleDummyTick(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();
                if (target->GetTypeId() != TYPE_ID_UNIT)
                    return;

                if (Creature* bomb = target->FindCreatureWithDistance(NPC_KINETIC_BOMB, 1.0f, true))
                {
                    bomb->CastSpell(bomb, SPELL_KINETIC_BOMB_EXPLOSION, true);
                    bomb->RemoveAurasDueToSpell(SPELL_KINETIC_BOMB_VISUAL);
                    target->RemoveAura(GetAura());
                    bomb->AI()->DoAction(SPELL_KINETIC_BOMB_EXPLOSION);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_valanar_kinetic_bomb_AuraScript::HandleDummyTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_valanar_kinetic_bomb_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_valanar_kinetic_bomb_AuraScript();
        }
};

class spell_valanar_kinetic_bomb_knockback : public SpellScriptLoader
{
    public:
        spell_valanar_kinetic_bomb_knockback() : SpellScriptLoader("spell_valanar_kinetic_bomb_knockback") { }

        class spell_valanar_kinetic_bomb_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_valanar_kinetic_bomb_knockback_SpellScript);

            void KnockIntoAir()
            {
                if (Creature* target = GetHitCreature())
                    target->AI()->DoAction(ACTION_KINETIC_BOMB_JUMP);
            }

            void Register()
            {
                BeforeHit += SpellHitFn(spell_valanar_kinetic_bomb_knockback_SpellScript::KnockIntoAir);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_valanar_kinetic_bomb_knockback_SpellScript();
        }
};

class spell_valanar_kinetic_bomb_absorb : public SpellScriptLoader
{
    public:
        spell_valanar_kinetic_bomb_absorb() : SpellScriptLoader("spell_valanar_kinetic_bomb_absorb") { }

        class spell_valanar_kinetic_bomb_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_valanar_kinetic_bomb_absorb_AuraScript);

            void OnAbsorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                absorbAmount = CalculatePctN(dmgInfo.GetDamage(), aurEff->GetAmount());
                RoundToInterval<uint32>(absorbAmount, 0, dmgInfo.GetDamage());
                dmgInfo.AbsorbDamage(absorbAmount);
            }

            void Register()
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_valanar_kinetic_bomb_absorb_AuraScript::OnAbsorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_valanar_kinetic_bomb_absorb_AuraScript();
        }
};

class spell_blood_council_shadow_prison : public SpellScriptLoader
{
    public:
        spell_blood_council_shadow_prison() : SpellScriptLoader("spell_blood_council_shadow_prison") { }

        class spell_blood_council_shadow_prison_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_council_shadow_prison_AuraScript);

            void HandleDummyTick(AuraEffect const* aurEff)
            {
                // Shadow prison should only target players
				if (GetTarget()->GetTypeId() == TYPE_ID_PLAYER && GetTarget()->isMoving())
                    GetTarget()->CastSpell(GetTarget(), SPELL_SHADOW_PRISON_DAMAGE, true, 0, aurEff);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_blood_council_shadow_prison_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_blood_council_shadow_prison_AuraScript();
        }
};

class spell_blood_council_shadow_prison_damage : public SpellScriptLoader
{
    public:
        spell_blood_council_shadow_prison_damage() : SpellScriptLoader("spell_blood_council_shadow_prison_damage") { }

        class spell_blood_council_shadow_prison_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_council_shadow_prison_SpellScript);

            void AddExtraDamage()
            {
                if (Aura* aur = GetHitUnit()->GetAura(GetSpellInfo()->Id))
                    if (AuraEffect const* eff = aur->GetEffect(EFFECT_1))
                        SetHitDamage(GetHitDamage() + eff->GetAmount());
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_blood_council_shadow_prison_SpellScript::AddExtraDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_blood_council_shadow_prison_SpellScript();
        }
};

void AddSC_boss_blood_prince_council()
{
    new boss_blood_council_controller();
    new boss_prince_keleseth_icc();
    new boss_prince_taldaram_icc();
    new boss_prince_valanar_icc();
    new npc_blood_queen_lana_thel();
    new npc_ball_of_flame();
    new npc_kinetic_bomb();
    new npc_dark_nucleus();
    new spell_taldaram_glittering_sparks();
    new spell_taldaram_summon_flame_ball();
    new spell_taldaram_flame_ball_visual();
    new spell_taldaram_ball_of_inferno_flame();
    new spell_valanar_kinetic_bomb();
    new spell_valanar_kinetic_bomb_knockback();
    new spell_valanar_kinetic_bomb_absorb();
    new spell_blood_council_shadow_prison();
    new spell_blood_council_shadow_prison_damage();
}