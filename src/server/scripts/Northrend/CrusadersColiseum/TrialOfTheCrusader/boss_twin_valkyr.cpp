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
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "trial_of_the_crusader.h"

enum Texts
{
    SAY_AGGRO           = -1649040,
    SAY_DEATH           = -1649041,
    SAY_BERSERK         = -1649042,
    EMOTE_SHIELD        = -1649043,
    SAY_SHIELD          = -1649044,
    SAY_SLAY_1          = -1649045,
    SAY_SLAY_2          = -1649046,
    EMOTE_LIGHT_VORTEX  = -1649047,
    SAY_LIGHT_VORTEX    = -1649048,
    EMOTE_DARK_VORTEX   = -1649049,
    SAY_DARK_VORTEX     = -1649050,
};

enum Equipment
{
    EQUIP_MAIN_1         = 9423,
    EQUIP_MAIN_2         = 37377,
};

enum Spells
{
    SPELL_LIGHT_TWIN_SPIKE      = 66075,
    SPELL_LIGHT_SURGE           = 65766,
    SPELL_LIGHT_SHIELD          = 65858,
    SPELL_LIGHT_TWIN_PACT       = 65876,
    SPELL_LIGHT_VORTEX          = 66046,
    SPELL_LIGHT_TOUCH           = 67297,
    SPELL_LIGHT_ESSENCE         = 65686,
    SPELL_EMPOWERED_LIGHT       = 65748,
    SPELL_TWIN_EMPATHY_LIGHT    = 66133,
    SPELL_UNLEASHED_LIGHT       = 65795,
    SPELL_DARK_TWIN_SPIKE       = 66069,
    SPELL_DARK_SURGE            = 65768,
    SPELL_DARK_SHIELD           = 65874,
    SPELL_DARK_TWIN_PACT        = 65875,
    SPELL_DARK_VORTEX           = 66058,
    SPELL_DARK_TOUCH            = 67282,
    SPELL_DARK_ESSENCE          = 65684,
    SPELL_EMPOWERED_DARK        = 65724,
    SPELL_TWIN_EMPATHY_DARK     = 66132,
    SPELL_UNLEASHED_DARK        = 65808,
    SPELL_CONTROLLER_PERIODIC   = 66149,
    SPELL_POWER_TWINS           = 65879,
    SPELL_BERSERK               = 64238,
    SPELL_POWERING_UP           = 67590,
    SPELL_SURGE_OF_SPEED        = 65828,
	// More Researh New Spells By Crispi
	SPELL_BULLET_CONTROLLER_PERIODIC_1             = 68396,
	SPELL_BULLET_CONTROLLER_PERIODIC_TRIGGER_LIGHT = 66152,
	SPELL_BULLET_CONTROLLER_PERIODIC_TRIGGER_DARK  = 66153,
};

#define SPELL_DARK_ESSENCE_HELPER RAID_MODE<uint32>(65684, 67176, 67177, 67178)
#define SPELL_LIGHT_ESSENCE_HELPER RAID_MODE<uint32>(65686, 67222, 67223, 67224)
#define SPELL_POWERING_UP_HELPER RAID_MODE<uint32>(67590, 67602, 67603, 67604)
#define SPELL_UNLEASHED_DARK_HELPER RAID_MODE<uint32>(65808, 67172, 67173, 67174)
#define SPELL_UNLEASHED_LIGHT_HELPER RAID_MODE<uint32>(65795, 67238, 67239, 67240)
#define SPELL_DARK_VORTEX_HELPER RAID_MODE<uint32>(66059, 67155, 67156, 67157)
#define SPELL_LIGHT_VORTEX_HELPER RAID_MODE<uint32>(66048, 67203, 67204, 67205)

enum Actions
{
    ACTION_VORTEX,
    ACTION_PACT
};

class OrbsDespawner : public BasicEvent
{
    public:
        explicit OrbsDespawner(Creature* creature) : creature(creature) {}

        bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
        {
            Trinity::CreatureWorker<OrbsDespawner> worker(creature, *this);
            creature->VisitNearbyGridObject(5000.0f, worker);
            return true;
        }

		Creature* creature;

        void operator()(Creature* creature) const
        {
            switch (creature->GetEntry())
            {
                case NPC_BULLET_DARK:
                case NPC_BULLET_LIGHT:
                    creature->DespawnAfterAction();
                    return;
                default:
                    return;
            }
        }
};

struct boss_twin_baseAI : public QuantumBasicAI
{
    boss_twin_baseAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    SummonList Summons;

    AuraStateType  AuraState;

    uint8 Stage;
    bool IsBerserk;

    uint32 Weapon;
    uint32 SpecialAbilityTimer;
    uint32 SpikeTimer;
    uint32 TouchTimer;
    uint32 BerserkTimer;

    int32 VortexSay;
    int32 VortexEmote;
    uint32 SisterNpcId;
    uint32 MyEmphatySpellId;
    uint32 OtherEssenceSpellId;
    uint32 SurgeSpellId;
    uint32 VortexSpellId;
    uint32 ShieldSpellId;
    uint32 TwinPactSpellId;
    uint32 SpikeSpellId;
    uint32 TouchSpellId;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
        me->ModifyAuraState(AuraState, true);
        /* Uncomment this once that they are flying above the ground
        me->SetLevitate(true);
        me->SetFlying(true); */
        IsBerserk = false;

        SpecialAbilityTimer = MINUTE*IN_MILLISECONDS;
        SpikeTimer = 20*IN_MILLISECONDS;
        TouchTimer = urand(10, 15)*IN_MILLISECONDS;
        BerserkTimer = IsHeroic() ? 6*MINUTE*IN_MILLISECONDS : 10*MINUTE*IN_MILLISECONDS;

		DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

		me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

        Summons.DespawnAll();
    }

    void JustReachedHome()
    {
		instance->SetData(TYPE_VALKIRIES, FAIL);

        Summons.DespawnAll();
        me->DespawnAfterAction();
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != WAYPOINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case 1:
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_AGGRESSIVE);
                break;
        }
    }

    void KilledUnit(Unit* who)
    {
        if (who->GetTypeId() == TYPE_ID_PLAYER)
			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
    }

    void JustSummoned(Creature* summoned)
    {
        Summons.Summon(summoned);
    }

    void SummonedCreatureDespawn(Creature* summoned)
    {
        switch (summoned->GetEntry())
        {
            case NPC_LIGHT_ESSENCE:
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIGHT_ESSENCE_HELPER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_POWERING_UP_HELPER);
                break;
            case NPC_DARK_ESSENCE:
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DARK_ESSENCE_HELPER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_POWERING_UP_HELPER);
                break;
            case NPC_BULLET_CONTROLLER:
                me->m_Events.AddEvent(new OrbsDespawner(me), me->m_Events.CalculateTime(100));
                break;
        }
        Summons.Despawn(summoned);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoSendQuantumText(SAY_DEATH, me);

		if (Creature* pSister = GetSister())
		{
			if (!pSister->IsAlive())
			{
				me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
				pSister->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(TYPE_VALKIRIES, DONE);
				Summons.DespawnAll();
			}
			else
			{
				me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
				instance->SetData(TYPE_VALKIRIES, SPECIAL);
            }
        }
        Summons.DespawnAll();
    }

    Creature* GetSister()
    {
        return Unit::GetCreature((*me), instance->GetData64(SisterNpcId));
    }

    void EnterToBattle(Unit* /*who*/)
    {
        me->SetInCombatWithZone();

		if (Creature* pSister = GetSister())
		{
			me->AddAura(MyEmphatySpellId, pSister);
			pSister->SetInCombatWithZone();

            instance->SetData(TYPE_VALKIRIES, IN_PROGRESS);
        }

        DoSendQuantumText(SAY_AGGRO, me);
        DoCast(me, SurgeSpellId);
    }

    void DoAction(const int32 action)
    {
        switch (action)
        {
            case ACTION_VORTEX:
                Stage = me->GetEntry() == NPC_FJOLA_LIGHTBANE ? 2 : 1;
                break;
            case ACTION_PACT:
                Stage = me->GetEntry() == NPC_FJOLA_LIGHTBANE ? 1 : 2;
                break;
        }
    }

    void EnableDualWield(bool mode = true)
    {
        SetEquipmentSlots(false, Weapon, mode ? Weapon : int32(EQUIP_UNEQUIP), EQUIP_UNEQUIP);
        me->SetCanDualWield(mode);
        me->UpdateDamagePhysical(mode ? OFF_ATTACK : BASE_ATTACK);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!instance || !UpdateVictim())
            return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

        switch (Stage)
        {
            case 0:
                break;
            case 1: // Vortex
                if (SpecialAbilityTimer <= diff)
                {
                    if (Creature* pSister = GetSister())
                        pSister->AI()->DoAction(ACTION_VORTEX);

                    DoSendQuantumText(VortexEmote, me);
                    DoSendQuantumText(VortexSay, me);
                    DoCastAOE(VortexSpellId);
                    Stage = 0;
                    SpecialAbilityTimer = MINUTE*IN_MILLISECONDS;
                }
                else SpecialAbilityTimer -= diff;
                break;
            case 2: // Shield+Pact
                if (SpecialAbilityTimer <= diff)
                {
                    DoSendQuantumText(EMOTE_SHIELD, me);
                    DoSendQuantumText(SAY_SHIELD, me);
                    if (Creature* pSister = GetSister())
                    {
                        pSister->AI()->DoAction(ACTION_PACT);
                        pSister->CastSpell(pSister, SPELL_POWER_TWINS, false);
                    }
                    DoCast(me, ShieldSpellId);
                    DoCast(me, TwinPactSpellId);
                    Stage = 0;
                    SpecialAbilityTimer = MINUTE*IN_MILLISECONDS;
                }
                else SpecialAbilityTimer -= diff;
                break;
            default:
                break;
        }

        if (SpikeTimer <= diff)
        {
            DoCastVictim(SpikeSpellId);
            SpikeTimer = 20*IN_MILLISECONDS;
        }
        else SpikeTimer -= diff;

        if (TouchTimer <= diff && IsHeroic())
        {
            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 200, true, OtherEssenceSpellId))
			{
                me->CastCustomSpell(TouchSpellId, SPELLVALUE_MAX_TARGETS, 1, target, false);
				TouchTimer = urand(10, 15)*IN_MILLISECONDS;
			}
        }
        else TouchTimer -= diff;

        if (!IsBerserk && BerserkTimer <= diff)
        {
            DoCast(me, SPELL_BERSERK);
            DoSendQuantumText(SAY_BERSERK, me);
            IsBerserk = true;
        }
        else BerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

class boss_fjola_lightbane : public CreatureScript
{
public:
    boss_fjola_lightbane() : CreatureScript("boss_fjola_lightbane") { }

    struct boss_fjola_lightbaneAI : public boss_twin_baseAI
    {
        boss_fjola_lightbaneAI(Creature* creature) : boss_twin_baseAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
		{
            boss_twin_baseAI::Reset();
            SetEquipmentSlots(false, EQUIP_MAIN_1, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
            Stage = 0;
            Weapon = EQUIP_MAIN_1;
            AuraState = AURA_STATE_UNKNOWN22;
            VortexEmote = EMOTE_LIGHT_VORTEX;
            VortexSay = SAY_LIGHT_VORTEX;
            SisterNpcId = NPC_EYDIS_DARKBANE;
            MyEmphatySpellId = SPELL_TWIN_EMPATHY_DARK;
            OtherEssenceSpellId = SPELL_DARK_ESSENCE_HELPER;
            SurgeSpellId = SPELL_LIGHT_SURGE;
            VortexSpellId = SPELL_LIGHT_VORTEX;
            ShieldSpellId = SPELL_LIGHT_SHIELD;
            TwinPactSpellId = SPELL_LIGHT_TWIN_PACT;
            TouchSpellId = SPELL_LIGHT_TOUCH;
            SpikeSpellId = SPELL_LIGHT_TWIN_SPIKE;
			me->ModifyAuraState(AURA_STATE_UNKNOWN22, true);

			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT,  EVENT_START_TWINS_FIGHT);
        }

        void EnterToBattle(Unit* who)
        {
			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT,  EVENT_START_TWINS_FIGHT);

            me->SummonCreature(NPC_BULLET_CONTROLLER, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
            boss_twin_baseAI::EnterToBattle(who);
        }

        void EnterEvadeMode()
        {
            instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
            boss_twin_baseAI::EnterEvadeMode();
        }

        void JustReachedHome()
        {
			instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));

            boss_twin_baseAI::JustReachedHome();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fjola_lightbaneAI(creature);
    }
};

class boss_eydis_darkbane : public CreatureScript
{
public:
    boss_eydis_darkbane() : CreatureScript("boss_eydis_darkbane") { }

    struct boss_eydis_darkbaneAI : public boss_twin_baseAI
    {
        boss_eydis_darkbaneAI(Creature* creature) : boss_twin_baseAI(creature) {}

        void Reset()
		{
            boss_twin_baseAI::Reset();
            SetEquipmentSlots(false, EQUIP_MAIN_2, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
            Stage = 1;
            Weapon = EQUIP_MAIN_2;
            AuraState = AURA_STATE_UNKNOWN19;
            VortexEmote = EMOTE_DARK_VORTEX;
            VortexSay = SAY_DARK_VORTEX;
            SisterNpcId = NPC_FJOLA_LIGHTBANE;
            MyEmphatySpellId = SPELL_TWIN_EMPATHY_LIGHT;
            OtherEssenceSpellId = SPELL_LIGHT_ESSENCE_HELPER;
            SurgeSpellId = SPELL_DARK_SURGE;
            VortexSpellId = SPELL_DARK_VORTEX;
            ShieldSpellId = SPELL_DARK_SHIELD;
            TwinPactSpellId = SPELL_DARK_TWIN_PACT;
            TouchSpellId = SPELL_DARK_TOUCH;
            SpikeSpellId = SPELL_DARK_TWIN_SPIKE;
			me->ModifyAuraState(AURA_STATE_UNKNOWN19, true);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_eydis_darkbaneAI(creature);
    }
};

#define ESSENCE_REMOVE 0
#define ESSENCE_APPLY 1

class npc_essence_of_twin : public CreatureScript
{
    public:
        npc_essence_of_twin() : CreatureScript("npc_essence_of_twin") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            player->RemoveAurasDueToSpell(creature->GetAI()->GetData(ESSENCE_REMOVE));
            player->CastSpell(player, creature->GetAI()->GetData(ESSENCE_APPLY), true);
            player->CLOSE_GOSSIP_MENU();
            return true;
		}

        struct npc_essence_of_twinAI : public QuantumBasicAI
        {
            npc_essence_of_twinAI(Creature* creature) : QuantumBasicAI(creature) { }

            uint32 GetData(uint32 data)
            {
                uint32 spellReturned = 0;
                switch (me->GetEntry())
                {
                    case NPC_LIGHT_ESSENCE:
                        spellReturned = (data == ESSENCE_REMOVE) ? SPELL_DARK_ESSENCE_HELPER : SPELL_LIGHT_ESSENCE_HELPER;
                        break;
                    case NPC_DARK_ESSENCE:
                        spellReturned = (data == ESSENCE_REMOVE) ? SPELL_LIGHT_ESSENCE_HELPER : SPELL_DARK_ESSENCE_HELPER;
                        break;
                    default:
                        break;
                }

                return spellReturned;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_essence_of_twinAI(creature);
        };
};

struct mob_unleashed_ballAI : public QuantumBasicAI
{
    mob_unleashed_ballAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    uint32 RangeCheckTimer;

    void MoveToNextPoint()
    {
        float x0 = ToCCommonLoc[1].GetPositionX(), y0 = ToCCommonLoc[1].GetPositionY(), r = 47.0f;
        float y = y0;
        float x = frand(x0 - r, x0 + r);
        float sq = pow(r, 2) - pow(x - x0, 2);
        float rt = sqrtf(fabs(sq));
        if (urand(0, 1))
            y = y0 + rt;
        else
            y = y0 - rt;

		me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
    }

    void Reset()
    {
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
        me->SetDisableGravity(true);
        me->SetCanFly(true);
        SetCombatMovement(false);
        MoveToNextPoint();
		RangeCheckTimer = 500;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

		switch (id)
        {
            case 0:
				if (urand(0, 3) == 0)
				{
                    MoveToNextPoint();
				}
                else
				{
					me->DisappearAndDie();
				}
				break;
        }
    }
};

class npc_unleashed_dark : public CreatureScript
{
public:
    npc_unleashed_dark() : CreatureScript("npc_unleashed_dark") { }

    struct npc_unleashed_darkAI : public mob_unleashed_ballAI
    {
        npc_unleashed_darkAI(Creature* creature) : mob_unleashed_ballAI(creature) {}

        void UpdateAI(const uint32 diff)
        {
            if (RangeCheckTimer <= diff)
            {
                if (me->SelectNearestPlayer(3.0f))
				{
					DoCastAOE(SPELL_UNLEASHED_DARK_HELPER);
					me->GetMotionMaster()->MoveIdle();
					me->DespawnAfterAction(1*IN_MILLISECONDS);
				}
				RangeCheckTimer = 0.5*IN_MILLISECONDS;
            }
            else RangeCheckTimer -= diff;
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_UNLEASHED_DARK_HELPER)
            {
				if (target->HasAura(SPELL_DARK_ESSENCE_HELPER))
				{
					// need to do the things in this order, else players might have 100 charges of Powering Up without anything happening
					Aura* aura = target->GetAura(SPELL_POWERING_UP_HELPER);
					if (aura)
					{
						aura->ModStackAmount(int(spell->Effects[EFFECT_0].CalcValue() * 0.001) - 1);
						target->CastSpell(target, SPELL_POWERING_UP_HELPER, true);
					}
					else
					{
						target->CastSpell(target, SPELL_POWERING_UP_HELPER, true);

						if (Aura* temp = target->GetAura(SPELL_POWERING_UP_HELPER))
							temp->ModStackAmount(int(spell->Effects[EFFECT_0].CalcValue() * 0.001) - 1);
					}
				}
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unleashed_darkAI(creature);
    }
};

class npc_unleashed_light : public CreatureScript
{
public:
    npc_unleashed_light() : CreatureScript("npc_unleashed_light") { }

    struct npc_unleashed_lightAI : public mob_unleashed_ballAI
    {
        npc_unleashed_lightAI(Creature* creature) : mob_unleashed_ballAI(creature) {}

        void UpdateAI(const uint32 diff)
        {
            if (RangeCheckTimer <= diff)
            {
                if (me->SelectNearestPlayer(3.0f))
				{
					DoCastAOE(SPELL_UNLEASHED_LIGHT_HELPER);
					me->GetMotionMaster()->MoveIdle();
					me->DespawnAfterAction(1*IN_MILLISECONDS);
				}
				RangeCheckTimer = 0.5*IN_MILLISECONDS;
            }
            else RangeCheckTimer -= diff;
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_UNLEASHED_LIGHT_HELPER)
            {
                if (target->HasAura(SPELL_LIGHT_ESSENCE_HELPER))
				{
					// need to do the things in this order, else players might have 100 charges of Powering Up without anything happening
					Aura* aura = target->GetAura(SPELL_POWERING_UP_HELPER);
					if (aura)
					{
						aura->ModStackAmount(int(spell->Effects[EFFECT_0].CalcValue() * 0.001) - 1);
						target->CastSpell(target, SPELL_POWERING_UP_HELPER, true);
					}
					else
					{
						target->CastSpell(target, SPELL_POWERING_UP_HELPER, true);

						if (Aura* temp = target->GetAura(SPELL_POWERING_UP_HELPER))
						{
							temp->ModStackAmount(int(spell->Effects[EFFECT_0].CalcValue() * 0.001) - 1);
						}
					}
				}
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unleashed_lightAI(creature);
    }
};

class npc_bullet_controller : public CreatureScript
{
public:
    npc_bullet_controller() : CreatureScript("npc_bullet_controller") { }

    struct npc_bullet_controllerAI : public QuantumBasicAI
    {
        npc_bullet_controllerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
			SetCombatMovement(false);
        }

        void Reset()
        {
            DoCastAOE(SPELL_CONTROLLER_PERIODIC);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            UpdateVictim();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bullet_controllerAI(creature);
    }
};

class spell_powering_up : public SpellScriptLoader
{
    public:
        spell_powering_up() : SpellScriptLoader("spell_powering_up") { }

        class spell_powering_up_SpellScript : public SpellScript
        {
            public:
                PrepareSpellScript(spell_powering_up_SpellScript)

            uint32 spellId;
            uint32 poweringUp;

            bool Load()
            {
                spellId = sSpellMgr->GetSpellIdForDifficulty(SPELL_SURGE_OF_SPEED, GetCaster());
                if (!sSpellMgr->GetSpellInfo(spellId))
                    return false;

                poweringUp = sSpellMgr->GetSpellIdForDifficulty(SPELL_POWERING_UP, GetCaster());
                if (!sSpellMgr->GetSpellInfo(poweringUp))
                    return false;

                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    if (Aura* aura = target->GetAura(poweringUp))
                    {
                        if (aura->GetStackAmount() >= 100)
                        {
                            if (target->GetDummyAuraEffect(SPELL_FAMILY_GENERIC, 2206, EFFECT_1))
                                target->CastSpell(target, SPELL_EMPOWERED_DARK, true);

                            if (target->GetDummyAuraEffect(SPELL_FAMILY_GENERIC, 2845, EFFECT_1))
                                target->CastSpell(target, SPELL_EMPOWERED_LIGHT, true);

							target->RemoveAurasDueToSpell(poweringUp);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_powering_up_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_powering_up_SpellScript();
        }
};

class spell_valkyr_essences : public SpellScriptLoader
{
    public:
        spell_valkyr_essences() : SpellScriptLoader("spell_valkyr_essences") { }

        class spell_valkyr_essences_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_valkyr_essences_AuraScript);

            uint32 spellId;

            bool Load()
            {
                spellId = sSpellMgr->GetSpellIdForDifficulty(SPELL_SURGE_OF_SPEED, GetCaster());
                if (!sSpellMgr->GetSpellInfo(spellId))
                    return false;
                return true;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & /*absorbAmount*/)
            {
                if (Unit* owner = GetUnitOwner())
                {
                    if ((dmgInfo.GetSchoolMask() == SPELL_SCHOOL_MASK_SHADOW && owner->GetDummyAuraEffect(SPELL_FAMILY_GENERIC, 2206, EFFECT_1)) ||    // dark damage
                        (dmgInfo.GetSchoolMask() == SPELL_SCHOOL_MASK_FIRE && owner->GetDummyAuraEffect(SPELL_FAMILY_GENERIC, 2845, EFFECT_1)))        // light damage
                    {
                        if (urand(0, 99) < 5)
                            GetTarget()->CastSpell(GetTarget(), spellId, true);
                    }
                }
            }

            void Register()
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_valkyr_essences_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_valkyr_essences_AuraScript();
        }
};

class spell_power_of_the_twins : public SpellScriptLoader
{
    public:
        spell_power_of_the_twins() : SpellScriptLoader("spell_power_of_the_twins") { }

        class spell_power_of_the_twins_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_power_of_the_twins_AuraScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* Valk = ObjectAccessor::GetCreature(*GetCaster(), instance->GetData64(GetCaster()->GetEntry())))
                        CAST_AI(boss_twin_baseAI, Valk->AI())->EnableDualWield(true);
                }
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                {
                    if (Creature* Valk = ObjectAccessor::GetCreature(*GetCaster(), instance->GetData64(GetCaster()->GetEntry())))
                        CAST_AI(boss_twin_baseAI, Valk->AI())->EnableDualWield(false);
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_power_of_the_twins_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_power_of_the_twins_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_power_of_the_twins_AuraScript();
        }
};

class spell_twin_vortex : public SpellScriptLoader
{
    public:
        spell_twin_vortex() : SpellScriptLoader("spell_twin_vortex") {}

        class spell_twin_vortex_SpellScript : public SpellScript
        {
            public:
                PrepareSpellScript(spell_twin_vortex_SpellScript);

            uint32 vortexId;
            uint32 poweringUp;

            bool Load()
            {
                vortexId = sSpellMgr->GetSpellIdForDifficulty(SPELL_DARK_VORTEX, GetCaster());
                if (!sSpellMgr->GetSpellInfo(vortexId))
                    return false;

                poweringUp = sSpellMgr->GetSpellIdForDifficulty(SPELL_POWERING_UP, GetCaster());
                if (!sSpellMgr->GetSpellInfo(poweringUp))
                    return false;

                return true;
            }

            void HandleDealDamage(SpellEffIndex effIndex)
            {
                uint32 buffValue = uint32(GetSpellInfo()->Effects[effIndex].CalcValue()) * 0.001 - 1;

                if (Unit* target = GetHitUnit())
                {
                    if ((GetSpellInfo()->GetSchoolMask() == SPELL_SCHOOL_MASK_SHADOW && target->GetDummyAuraEffect(SPELL_FAMILY_GENERIC, 2206, EFFECT_1)) || // dark damage
                        (GetSpellInfo()->GetSchoolMask() == SPELL_SCHOOL_MASK_FIRE && target->GetDummyAuraEffect(SPELL_FAMILY_GENERIC, 2845, EFFECT_1)))   // light damage
                    {
                        Aura* aura = target->GetAura(poweringUp);
                        if (aura)
                        {
                            aura->ModStackAmount(buffValue);
                            target->CastSpell(target, poweringUp, true);
                        }
                        else
                        {
                            target->CastSpell(target, poweringUp, true);
                            if (Aura* temp = target->GetAura(poweringUp))
                                temp->ModStackAmount(buffValue);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_twin_vortex_SpellScript::HandleDealDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_twin_vortex_SpellScript();
        }
};

void AddSC_boss_twin_valkyr()
{
    new boss_fjola_lightbane();
    new boss_eydis_darkbane();
    new npc_unleashed_light();
    new npc_unleashed_dark();
    new npc_essence_of_twin();
    new npc_bullet_controller();
    new spell_powering_up();
    new spell_valkyr_essences();
    new spell_power_of_the_twins();
	new spell_twin_vortex();
}