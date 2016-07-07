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
#include "gundrak.h"

enum Yells
{
    SAY_AGGRO       = -1604019,
    SAY_TRANSFORM_1 = -1604020,
    SAY_TRANSFORM_2 = -1604021,
    SAY_SUMMON_1    = -1604022,
    SAY_SUMMON_2    = -1604023,
    SAY_SUMMON_3    = -1604024,
    SAY_SLAY_1      = -1604025,
    SAY_SLAY_2      = -1604026,
    SAY_SLAY_3      = -1604027,
    SAY_DEATH       = -1604028,
	EMOTE_IMPALE    = -1604033,
};

enum Spells
{
	SPELL_HEARTH_BEAM_VISUAL      = 54988,
    SPELL_ENRAGE_5N               = 55285,
    SPELL_ENRAGE_5H               = 59828,
    SPELL_IMPALING_CHARGE_5N      = 54956,
    SPELL_IMPALING_CHARGE_5H      = 59827,
	SPELL_IMPALING_CHARGE_VEHICLE = 54958,
    SPELL_STOMP_5N                = 55292,
    SPELL_STOMP_5H                = 59829,
    SPELL_PUNCTURE_5N             = 55276,
    SPELL_PUNCTURE_5H             = 59826,
    SPELL_STAMPEDE                = 55218,
    SPELL_WHIRLING_SLASH_5N       = 55250,
    SPELL_WHIRLING_SLASH_5H       = 59824,
    SPELL_STAMPEDE_DMG_5N         = 55220,
    SPELL_STAMPEDE_DMG_5H         = 59823,
	SPELL_TRANSFORM_RHINO         = 55297,
	SPELL_TRANSFORM_BACK          = 55299,
};

enum Achievements
{
    DATA_SHARE_THE_LOVE = 1,
};

enum CombatPhase
{
    PHASE_TROLL = 1,
    PHASE_RHINO = 2,
};

class boss_gal_darah : public CreatureScript
{
public:
    boss_gal_darah() : CreatureScript("boss_gal_darah") { }

    struct boss_gal_darahAI : public QuantumBasicAI
    {
        boss_gal_darahAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 StampedeTimer;
        uint32 WhirlingSlashTimer;
        uint32 PunctureTimer;
        uint32 EnrageTimer;
        uint32 ImpalingChargeTimer;
        uint32 StompTimer;
        uint32 TransformationTimer;
        std::set<uint64> impaledPlayers;

        CombatPhase Phase;
        uint8 PhaseCounter;
        bool bStartOfTransformation;
        bool bTransformated;

        InstanceScript* instance;

        void Reset()
        {
            StampedeTimer = 10000;
            WhirlingSlashTimer = 20000;
            PunctureTimer = 5000;
            EnrageTimer = 15000;
            ImpalingChargeTimer = 20000;
            StompTimer = 10000;
            TransformationTimer = 6000;
            PhaseCounter = 0;

            impaledPlayers.clear();

            bStartOfTransformation = true;
            bTransformated = false;

			DoCast(me, SPELL_TRANSFORM_BACK, true);

            Phase = PHASE_TROLL;

			instance->SetData(DATA_GAL_DARAH_EVENT, NOT_STARTED);

			DoCastAOE(SPELL_HEARTH_BEAM_VISUAL, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_GAL_DARAH_EVENT, IN_PROGRESS);
		}

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
		}

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_TRANSFORM_BACK)
				me->RemoveAurasDueToSpell(SPELL_TRANSFORM_RHINO);
		}

		void EnterEvadeMode()
        {
            me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
            me->SetReactState(REACT_AGGRESSIVE);
            QuantumBasicAI::EnterEvadeMode();
        }

		void JustReachedHome()
		{
			DoCastAOE(SPELL_HEARTH_BEAM_VISUAL, true);
		}

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_GAL_DARAH_EVENT, DONE);
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_SHARE_THE_LOVE)
                return impaledPlayers.size();

            return 0;
        }

		void SetGUID(uint64 guid, int32 type /*= 0 */)
		{
			if (type == DATA_SHARE_THE_LOVE)
			{
				if (Unit* target = ObjectAccessor::GetUnit(*me, guid))
					DoSendQuantumText(EMOTE_IMPALE, target);
				impaledPlayers.insert(guid);
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (!bTransformated && HealthBelowPct(50)) // transform at least once at 50% health
            {
                bTransformated = true;
                PhaseCounter = 2;
            }

            switch (Phase)
            {
                case PHASE_TROLL:
                    if (PhaseCounter >= 2)
                    {
                        if (TransformationTimer <= diff)
                        {
							Phase = PHASE_RHINO;
							PhaseCounter = 0;
							DoCast(me, SPELL_TRANSFORM_RHINO, true);
                            DoSendQuantumText(SAY_TRANSFORM_1, me);
                            bStartOfTransformation = true;
                            bTransformated = true;
                            me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                            me->SetReactState(REACT_AGGRESSIVE);
							TransformationTimer = 5000;
                        }
                        else
                        {
                            TransformationTimer -= diff;

                            if (bStartOfTransformation)
                            {
                                bStartOfTransformation = false;
                                me->AddUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                                me->SetReactState(REACT_PASSIVE);
                            }
                        }
                    }
                    else
                    {
                        if (StampedeTimer <= diff)
                        {
							DoSendQuantumText(RAND(SAY_SUMMON_1, SAY_SUMMON_2, SAY_SUMMON_3), me);

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
								DoCast(target, SPELL_STAMPEDE, true);
								DoCast(target, DUNGEON_MODE(SPELL_STAMPEDE_DMG_5N, SPELL_STAMPEDE_DMG_5H), true);
								StampedeTimer = urand(10000, 15000);
							}
                        }
						else StampedeTimer -= diff;

                        if (WhirlingSlashTimer <= diff)
                        {
                            DoCastAOE(DUNGEON_MODE(SPELL_WHIRLING_SLASH_5N, SPELL_WHIRLING_SLASH_5H));
                            WhirlingSlashTimer = urand(18000, 22000);
                            ++PhaseCounter;
                        }
						else WhirlingSlashTimer -= diff;
                    }
                    break;
                case PHASE_RHINO:
                    if (PhaseCounter >= 2)
                    {
                        if (TransformationTimer <= diff)
                        {
							Phase = PHASE_TROLL;
							PhaseCounter = 0;
							DoCast(me, SPELL_TRANSFORM_BACK, true);
                            DoSendQuantumText(SAY_TRANSFORM_2, me);
                            bStartOfTransformation = true;
                            me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                            me->SetReactState(REACT_AGGRESSIVE);
							TransformationTimer = 6000;
                        }
                        else
                        {
                            TransformationTimer -= diff;

                            if (bStartOfTransformation)
                            {
                                bStartOfTransformation = false;
                                me->AddUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                                me->SetReactState(REACT_PASSIVE);
                            }
                        }
                    }
                    else
                    {
                        if (PunctureTimer <= diff)
                        {
                            DoCastVictim(DUNGEON_MODE(SPELL_PUNCTURE_5N, SPELL_PUNCTURE_5H));
                            PunctureTimer = 15000;
                        }
						else PunctureTimer -= diff;

                        if (EnrageTimer <= diff)
                        {
                            DoCast(me, DUNGEON_MODE(SPELL_ENRAGE_5N, SPELL_ENRAGE_5H));
                            EnrageTimer = 20000;
                        }
						else EnrageTimer -= diff;

                        if (StompTimer <= diff)
                        {
                            DoCastAOE(DUNGEON_MODE(SPELL_STOMP_5N, SPELL_STOMP_5H));
                            StompTimer = urand(10000, 15000);
                        }
						else StompTimer -= diff;

                        if (ImpalingChargeTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                            {
                                DoCast(target, DUNGEON_MODE(SPELL_IMPALING_CHARGE_5N, SPELL_IMPALING_CHARGE_5H));
                                impaledPlayers.insert(target->GetGUID());
                            }
                            ImpalingChargeTimer = 20000;
                            ++PhaseCounter;
                        }
						else ImpalingChargeTimer -= diff;
                    }
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gal_darahAI(creature);
    }
};

class spell_gal_darah_impaling_charge : public SpellScriptLoader
{
    public:
        spell_gal_darah_impaling_charge() : SpellScriptLoader("spell_gal_darah_impaling_charge") { }

        class spell_gal_darah_impaling_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gal_darah_impaling_charge_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_IMPALING_CHARGE_VEHICLE))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetCaster()->GetVehicleKit() && GetCaster()->GetEntry() == NPC_GALDARAH;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    Unit* caster = GetCaster();
                    target->CastSpell(caster, SPELL_IMPALING_CHARGE_VEHICLE, true);
                    caster->ToCreature()->AI()->SetGUID(target->GetGUID(), DATA_SHARE_THE_LOVE);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gal_darah_impaling_charge_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gal_darah_impaling_charge_SpellScript();
        }
};

class achievement_share_the_love : public AchievementCriteriaScript
{
    public:
        achievement_share_the_love() : AchievementCriteriaScript("achievement_share_the_love") { }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* GalDarah = target->ToCreature())
                if (GalDarah->AI()->GetData(DATA_SHARE_THE_LOVE) >= 5)
                    return true;

            return false;
        }
};

void AddSC_boss_gal_darah()
{
    new boss_gal_darah();
	new spell_gal_darah_impaling_charge();
    new achievement_share_the_love();
}