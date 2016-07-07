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
#include "trial_of_the_crusader.h"

enum Yells
{
    SAY_AGGRO               = -1649031,
    SAY_DEATH               = -1649032,
    EMOTE_INCINERATE        = -1649033,
    SAY_INCINERATE          = -1649034,
    EMOTE_LEGION_FLAME      = -1649035,
    EMOTE_NETHER_PORTAL     = -1649036,
    SAY_NETHER_PORTAL       = -1649037,
    EMOTE_INFERNAL_ERUPTION = -1649038,
    SAY_INFERNAL_ERUPTION   = -1649039,
	SAY_SLAY_1              = -1649111,
	SAY_SLAY_2              = -1649112,
	SAY_BERSERK             = -1649113,
};

enum Equipment
{
    EQUIP_MAIN    = 47266,
    EQUIP_OFFHAND = 46996,
    EQUIP_RANGED  = 47267,
    EQUIP_DONE    = EQUIP_NO_CHANGE,
};

enum BossSpells
{
	SPELL_FEL_FIREBALL_10N         = 66532,
	SPELL_FEL_FIREBALL_25N         = 66963,
	SPELL_FEL_FIREBALL_10H         = 66964,
	SPELL_FEL_FIREBALL_25H         = 66965,
	SPELL_FEL_LIGHTING_10N         = 66528,
	SPELL_FEL_LIGHTING_25N         = 67029,
	SPELL_FEL_LIGHTING_10H         = 67030,
	SPELL_FEL_LIGHTING_25H         = 67031,
	SPELL_INCINERATE_FLESH_10N     = 66237,
	SPELL_INCINERATE_FLESH_25N     = 67049,
	SPELL_INCINERATE_FLESH_10H     = 67050,
	SPELL_INCINERATE_FLESH_25H     = 67051,
	SPELL_NETHER_POWER_10N         = 66228,
	SPELL_NETHER_POWER_25N         = 67106,
	SPELL_NETHER_POWER_10H         = 67107,
	SPELL_NETHER_POWER_25H         = 67108,
    SPELL_LEGION_FLAME             = 66197,
    SPELL_LEGION_FLAME_EFFECT      = 66201,
	SPELL_TOUCH_OF_JARAXXUS        = 66209,
    SPELL_INFERNAL_ERUPTION        = 66258,
    SPELL_INFERNAL_ERUPTION_EFFECT = 66252,
    SPELL_NETHER_PORTAL            = 66269,
    SPELL_NETHER_PORTAL_EFFECT     = 66263,
    SPELL_BERSERK                  = 64238,
    // Mistress of Pain spells
    SPELL_SHIVAN_SLASH_10N         = 66378,
	SPELL_SHIVAN_SLASH_25N         = 67097,
	SPELL_SHIVAN_SLASH_10H         = 67098,
	SPELL_SHIVAN_SLASH_25H         = 67099,
    SPELL_SPINNING_STRIKE          = 66283,
	SPELL_MISTRESS_KISS            = 66336,
	// Felflame Infernal
    SPELL_FEL_INFERNO_10N          = 66495,
	SPELL_FEL_INFERNO_25N          = 67045,
	SPELL_FEL_INFERNO_10H          = 67046,
	SPELL_FEL_INFERNO_25H          = 67047,
    SPELL_FEL_STREAK               = 66494,
	SPELL_LORD_HITTIN              = 66326,
};

class boss_lord_jaraxxus : public CreatureScript
{
public:
    boss_lord_jaraxxus() : CreatureScript("boss_lord_jaraxxus") { }

    struct boss_lord_jaraxxusAI : public QuantumBasicAI
    {
        boss_lord_jaraxxusAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint32 FelFireballTimer;
        uint32 FelLightningTimer;
        uint32 IncinerateFleshTimer;
        uint32 NetherPowerTimer;
        uint32 LegionFlameTimer;
		uint32 TouchOfJaraxxusTimer;
        uint32 SummonNetherPortalTimer;
        uint32 SummonInfernalEruptionTimer;
		uint32 BerserkTimer;

        void Reset()
        {
			instance->SetData(TYPE_JARAXXUS, NOT_STARTED);

            SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);
            FelFireballTimer = 5*IN_MILLISECONDS;
            FelLightningTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            IncinerateFleshTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
            NetherPowerTimer = 40*IN_MILLISECONDS;
            LegionFlameTimer = 30*IN_MILLISECONDS;
			TouchOfJaraxxusTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            SummonNetherPortalTimer = 1*MINUTE*IN_MILLISECONDS;
            SummonInfernalEruptionTimer = 2*MINUTE*IN_MILLISECONDS;
			BerserkTimer = 10*MINUTE*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            Summons.DespawnAll();
        }

        void JustReachedHome()
        {
			instance->SetData(TYPE_JARAXXUS, FAIL);

            DoCast(me, SPELL_JARAXXUS_CHAINS);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetReactState(REACT_PASSIVE);
        }

        void KilledUnit(Unit* who)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

		void CleanJaraxxusAuras()
		{
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_FIREBALL_10N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_FIREBALL_25N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_FIREBALL_10H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_FIREBALL_25H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INCINERATE_FLESH_10N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INCINERATE_FLESH_25N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INCINERATE_FLESH_10H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INCINERATE_FLESH_25H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_INFERNO_10N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_INFERNO_25N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_INFERNO_10H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_INFERNO_25H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LEGION_FLAME);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MISTRESS_KISS);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEL_STREAK);
		}

        void JustDied(Unit* /*killer*/)
        {
			CleanJaraxxusAuras();
            Summons.DespawnAll();
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(TYPE_JARAXXUS, DONE);
        }

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->SetInCombatWithZone();

			instance->SetData(TYPE_JARAXXUS, IN_PROGRESS);

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            DoSendQuantumText(SAY_AGGRO, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (SummonInfernalEruptionTimer <= diff)
            {
                DoSendQuantumText(EMOTE_INFERNAL_ERUPTION, me);
                DoSendQuantumText(SAY_INFERNAL_ERUPTION, me);
                DoCast(SPELL_INFERNAL_ERUPTION);
                SummonInfernalEruptionTimer = 2*MINUTE*IN_MILLISECONDS;
            }
			else SummonInfernalEruptionTimer -= diff;

            if (SummonNetherPortalTimer <= diff)
            {
                DoSendQuantumText(EMOTE_NETHER_PORTAL, me);
                DoSendQuantumText(SAY_NETHER_PORTAL, me);
                DoCast(SPELL_NETHER_PORTAL);
                SummonNetherPortalTimer = 2*MINUTE*IN_MILLISECONDS;
            }
			else SummonNetherPortalTimer -= diff;

            if (FelFireballTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_FEL_FIREBALL_10N, SPELL_FEL_FIREBALL_25N, SPELL_FEL_FIREBALL_10H, SPELL_FEL_FIREBALL_25H));
                FelFireballTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
			else FelFireballTimer -= diff;

            if (FelLightningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_FEL_LIGHTING_10N, SPELL_FEL_LIGHTING_25N, SPELL_FEL_LIGHTING_10H, SPELL_FEL_LIGHTING_25H));
					FelLightningTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
				}
            }
			else FelLightningTimer -= diff;

            if (IncinerateFleshTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0, true))
                {
                    DoSendQuantumText(EMOTE_INCINERATE, me, target);
                    DoSendQuantumText(SAY_INCINERATE, me);
					DoCast(target, RAID_MODE(SPELL_INCINERATE_FLESH_10N, SPELL_INCINERATE_FLESH_25N, SPELL_INCINERATE_FLESH_10H, SPELL_INCINERATE_FLESH_25H));
					IncinerateFleshTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
				}
            }
			else IncinerateFleshTimer -= diff;

            if (NetherPowerTimer <= diff)
            {
				me->CastCustomSpell(RAID_MODE(SPELL_NETHER_POWER_10N, SPELL_NETHER_POWER_25N, SPELL_NETHER_POWER_10H, SPELL_NETHER_POWER_25H), SPELLVALUE_AURA_STACK, RAID_MODE<uint32>(5, 10, 5,10), me, true);
                NetherPowerTimer = 40*IN_MILLISECONDS;
            }
			else NetherPowerTimer -= diff;

            if (LegionFlameTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0, true))
                {
                    DoSendQuantumText(EMOTE_LEGION_FLAME, me, target);
                    DoCast(target, SPELL_LEGION_FLAME);
					LegionFlameTimer = 30*IN_MILLISECONDS;
				}
            }
			else LegionFlameTimer -= diff;

			if (GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC && TouchOfJaraxxusTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0, true))
				{
					me->CastCustomSpell(SPELL_TOUCH_OF_JARAXXUS, SPELLVALUE_MAX_TARGETS, 1);
					TouchOfJaraxxusTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
				}
			}
			else TouchOfJaraxxusTimer -= diff;

			if (BerserkTimer <= diff && !me->HasAura(SPELL_BERSERK))
            {
				DoCast(me, SPELL_BERSERK);
				DoSendQuantumText(SAY_BERSERK, me);
            }
			else BerserkTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lord_jaraxxusAI(creature);
    }
};

class npc_legion_flame : public CreatureScript
{
public:
    npc_legion_flame() : CreatureScript("npc_legion_flame") { }

    struct npc_legion_flameAI : public QuantumBasicAI
    {
        npc_legion_flameAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			SetCombatMovement(false);
			Reset();
        }

        InstanceScript* instance;

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetInCombatWithZone();
            DoCast(SPELL_LEGION_FLAME_EFFECT);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            UpdateVictim();

            if (instance->GetData(TYPE_JARAXXUS) != IN_PROGRESS)
                me->DespawnAfterAction();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_legion_flameAI(creature);
    }
};

class npc_infernal_volcano : public CreatureScript
{
public:
    npc_infernal_volcano() : CreatureScript("npc_infernal_volcano") { }

    struct npc_infernal_volcanoAI : public QuantumBasicAI
    {
        npc_infernal_volcanoAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        SummonList Summons;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);

            if (!IsHeroic())
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
            else
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);

            Summons.DespawnAll();
        }

        void IsSummonedBy(Unit* /*summoner*/)
        {
            DoCast(SPELL_INFERNAL_ERUPTION_EFFECT);
        }

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
            summoned->SetCorpseDelay(0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnAfterAction();
        }

		void UpdateAI(uint32 const diff) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infernal_volcanoAI(creature);
    }
};

class npc_fel_infernal : public CreatureScript
{
public:
    npc_fel_infernal() : CreatureScript("npc_fel_infernal") { }

    struct npc_fel_infernalAI : public QuantumBasicAI
    {
        npc_fel_infernalAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;
        uint32 FelStreakTimer;

        void Reset()
        {
            FelStreakTimer = 30*IN_MILLISECONDS;
            me->SetInCombatWithZone();
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_FEL_STREAK)
				DoCastAOE(RAID_MODE(SPELL_FEL_INFERNO_10N, SPELL_FEL_INFERNO_25N, SPELL_FEL_INFERNO_10H, SPELL_FEL_INFERNO_25H));
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (instance->GetData(TYPE_JARAXXUS) != IN_PROGRESS)
                me->DespawnAfterAction();

            if (FelStreakTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_FEL_STREAK);
					FelStreakTimer = 30*IN_MILLISECONDS;
				}
            }
			else FelStreakTimer -= diff;

            if (instance->GetData(TYPE_JARAXXUS) != IN_PROGRESS)
                me->DespawnAfterAction();

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fel_infernalAI(creature);
    }
};

class npc_nether_portal : public CreatureScript
{
public:
    npc_nether_portal() : CreatureScript("npc_nether_portal") { }

    struct npc_nether_portalAI : public QuantumBasicAI
    {
        npc_nether_portalAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        SummonList Summons;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);

            if (!IsHeroic())
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
            else
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);

            Summons.DespawnAll();
        }

        void IsSummonedBy(Unit* /*summoner*/)
        {
            DoCast(SPELL_NETHER_PORTAL_EFFECT);
        }

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
            summoned->SetCorpseDelay(0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnAfterAction();
        }

		void UpdateAI(uint32 const diff) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nether_portalAI(creature);
    }
};

class npc_mistress_of_pain : public CreatureScript
{
public:
    npc_mistress_of_pain() : CreatureScript("npc_mistress_of_pain") { }

    struct npc_mistress_of_painAI : public QuantumBasicAI
    {
        npc_mistress_of_painAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			instance->SetData(DATA_MISTRESS_OF_PAIN_COUNT, INCREASE);
            Reset();
        }

        InstanceScript* instance;

        uint32 ShivanSlashTimer;
        uint32 SpinningStrikeTimer;
        uint32 MistressKissTimer;

        void Reset()
        {
            ShivanSlashTimer = 30*IN_MILLISECONDS;
            SpinningStrikeTimer = 30*IN_MILLISECONDS;
            MistressKissTimer = 15*IN_MILLISECONDS;
            me->SetInCombatWithZone();
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_MISTRESS_OF_PAIN_COUNT, DECREASE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetData(TYPE_JARAXXUS) != IN_PROGRESS)
            {
                me->DespawnAfterAction();
                return;
            }

            if (!UpdateVictim())
                return;

            if (ShivanSlashTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_SHIVAN_SLASH_10N, SPELL_SHIVAN_SLASH_25N, SPELL_SHIVAN_SLASH_10H, SPELL_SHIVAN_SLASH_25H));
                ShivanSlashTimer = 30*IN_MILLISECONDS;
            }
			else ShivanSlashTimer -= diff;

            if (SpinningStrikeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0, true))
				{
                    DoCast(target, SPELL_SPINNING_STRIKE);
					SpinningStrikeTimer = 30*IN_MILLISECONDS;
				}
            }
			else SpinningStrikeTimer -= diff;

            if (IsHeroic() && MistressKissTimer <= diff)
            {
				DoCast(me, SPELL_MISTRESS_KISS);
				MistressKissTimer = 30*IN_MILLISECONDS;
            }
			else MistressKissTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mistress_of_painAI(creature);
    }
};

enum MistressKiss
{
    SPELL_MISTRESS_KISS_DAMAGE_SILENCE = 66359,
};

class spell_mistress_kiss : public SpellScriptLoader
{
    public:
        spell_mistress_kiss() : SpellScriptLoader("spell_mistress_kiss") { }

        class spell_mistress_kiss_AuraScript : public AuraScript
        {
			PrepareAuraScript(spell_mistress_kiss_AuraScript);

            bool Load()
            {
                if (GetCaster())
                    if (sSpellMgr->GetSpellIdForDifficulty(SPELL_MISTRESS_KISS_DAMAGE_SILENCE, GetCaster()))
                        return true;
                return false;
            }

            void HandleDummyTick(AuraEffect const* /*aurEff*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetTarget();
                if (caster && target)
                {
                    if (target->HasUnitState(UNIT_STATE_CASTING))
                    {
                        caster->CastSpell(target, SPELL_MISTRESS_KISS_DAMAGE_SILENCE, true);
                        target->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mistress_kiss_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mistress_kiss_AuraScript();
        }
};

enum MistressKissDebuff
{
    SPELL_MISTRESS_KISS_DEBUFF = 66334,
};

class spell_mistress_kiss_area : public SpellScriptLoader
{
    public:
        spell_mistress_kiss_area() : SpellScriptLoader("spell_mistress_kiss_area") {}

        class spell_mistress_kiss_area_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mistress_kiss_area_SpellScript);

            bool Load()
            {
                if (GetCaster())
                    if (sSpellMgr->GetSpellIdForDifficulty(SPELL_MISTRESS_KISS_DEBUFF, GetCaster()))
                        return true;
                return false;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (caster && target)
                    caster->CastSpell(target, SPELL_MISTRESS_KISS_DEBUFF, true);
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                // get a list of players with mana
                std::list<Unit*> _unitList;

                for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)

					if ((*itr)->ToUnit()->GetPowerType() == POWER_MANA)
						_unitList.push_back(*itr);

                if (Unit* _target = Quantum::DataPackets::SelectRandomContainerElement(_unitList))
                {
                    unitList.clear();
                    unitList.push_back(_target);
                    if (Unit* caster = GetCaster())
                        caster->CastSpell(_target->ToUnit(), SPELL_MISTRESS_KISS_DEBUFF, true);
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_mistress_kiss_area_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mistress_kiss_area_SpellScript();
        }
};

void AddSC_boss_jaraxxus()
{
    new boss_lord_jaraxxus();
    new npc_legion_flame();
    new npc_infernal_volcano();
    new npc_fel_infernal();
    new npc_nether_portal();
    new npc_mistress_of_pain();
	new spell_mistress_kiss();
	new spell_mistress_kiss_area();
}