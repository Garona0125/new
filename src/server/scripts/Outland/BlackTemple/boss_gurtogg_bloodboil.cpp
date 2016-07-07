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
#include "black_temple.h"

#define COORD_Z_HOME 64.0f

enum Says
{
	SAY_AGGRO              = -1564029,
	SAY_SLAY1              = -1564030,
	SAY_SLAY2              = -1564031,
	SAY_SPECIAL1           = -1564032,
	SAY_SPECIAL2           = -1564033,
	SAY_ENRAGE1            = -1564034,
	SAY_ENRAGE2            = -1564035,
	SAY_DEATH              = -1564036,
};

enum Spells
{
	// Phase 1
	SPELL_ARCING_SMASH_1       = 40599,
	SPELL_ACIDIC_WOUND         = 40484, // Trigger Aura
	SPELL_BEWILDERING_STRIKE   = 40491,
	SPELL_EJECT_1              = 40486,
	SPELL_FELBREATH_1          = 40508,
	SPELL_BLOODBOIL            = 42005,
	// Phase 2
	SPELL_ARCING_SMASH_2       = 40457,
	SPELL_EJECT_2              = 40597,
	SPELL_FELBREATH_2          = 40595,
	SPELL_FEL_GEYSER           = 40569,
	SPELL_FEL_GEYSER_DAMAGE    = 40593,
	SPELL_FEL_GEYSER_STUN      = 40591,
	// Fel Rage
	SPELL_INSIGNIFIGANCE       = 40618,
	SPELL_TAUNT_GURTOGG        = 40603,
	SPELL_FEL_RAGE_SELF        = 40594,
	SPELL_FEL_RAGE_1           = 40604,
	SPELL_FEL_RAGE_2           = 40616,
	SPELL_FEL_RAGE_3           = 41625,
	SPELL_FEL_RAGE_SCALE       = 46787,
	SPELL_CHARGE               = 40602,
	SPELL_BERSERK              = 45078,
};

class boss_gurtogg_bloodboil : public CreatureScript
{
public:
    boss_gurtogg_bloodboil() : CreatureScript("boss_gurtogg_bloodboil") { }

    struct boss_gurtogg_bloodboilAI : public QuantumBasicAI
    {
        boss_gurtogg_bloodboilAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
            SetImmuneToPushPullEffects(true);
        }

        InstanceScript* instance;
        uint64 TargetGUID;
        float TargetThreat;

        uint32 BloodboilTimer;
        uint32 BloodboilCount;
        uint32 FelGeyserTimer;
        //uint32 AcidicWoundTimer;
        uint32 BewilderingStrikeTimer;
        uint32 ArcingSmashTimer;
        uint32 FelBreathTimer;
        uint32 EjectTimer;
        uint32 PhaseChangeTimer;
        uint32 EnrageTimer;
        uint32 ChargeTimer;
        bool Phase1;
        uint32 pulseTimer;

        void Reset()
        {
			instance->SetData(DATA_GURTOGGBLOODBOILEVENT, NOT_STARTED);

            TargetGUID = 0;

            TargetThreat = 0;

            BloodboilTimer = 10000;
            BloodboilCount = 0;
            FelGeyserTimer = 1000;
            //AcidicWoundTimer = 6000;
            BewilderingStrikeTimer = 15000;
            ArcingSmashTimer = 19000;
            FelBreathTimer = 25000;
            EjectTimer = 10000;
            PhaseChangeTimer = 65000;
            EnrageTimer = 600000;
            Phase1 = true;
            ChargeTimer = 30000;
            pulseTimer = 10000;

            DoCast(me,SPELL_ACIDIC_WOUND,true);
            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();

            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_GURTOGGBLOODBOILEVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_GURTOGGBLOODBOILEVENT, DONE);

            DoSendQuantumText(SAY_DEATH, me);

            CleanUpAura();
        }

        void CleanUpAura()
        {
            Map *map = me->GetMap();
            if (map->IsDungeon())
            {
                InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
                for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* player = i->getSource())
                    {
                        if (player->IsAlive())
                        {
                            player->RemoveAurasDueToSpell(SPELL_INSIGNIFIGANCE);
                            player->RemoveAurasDueToSpell(SPELL_FEL_RAGE_1);
                            player->RemoveAurasDueToSpell(SPELL_FEL_RAGE_2);
                            player->RemoveAurasDueToSpell(SPELL_FEL_RAGE_3);
                            player->RemoveAurasDueToSpell(SPELL_FEL_RAGE_SCALE);
                        }
                    }
                }
            }
        }

        void RevertThreatOnTarget(uint64 guid)
        {
            Unit* unit = NULL;
            unit = Unit::GetUnit(*me, guid);
            if (unit)
            {
                if (DoGetThreat(unit))
                    DoModifyThreatPercent(unit, -100);

                if (TargetThreat)
                    me->AddThreat(unit, TargetThreat);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
        
            if (Phase1)
            {
                if (pulseTimer <= diff)
                {
                    if (me->GetPositionZ() > (COORD_Z_HOME+15))
                    {
                        EnterEvadeMode();
                        return;
                    }

                    DoAttackerAreaInCombat(me->GetVictim(),50);
                    pulseTimer = 10000;
                }
				else pulseTimer -= diff;
            }

            if (!me->HasAura(SPELL_BERSERK, 0))
            {
                if (EnrageTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK);
                    switch(rand()%2)
                    {
                    case 0:
						DoSendQuantumText(SAY_ENRAGE1, me);
						break;
                    case 1:
						DoSendQuantumText(SAY_ENRAGE2, me);
						break;
                    }
                }
				else EnrageTimer -= diff;
            }

            if (ArcingSmashTimer <= diff)
            {
                DoCastVictim(Phase1 ? SPELL_ARCING_SMASH_1 : SPELL_ARCING_SMASH_2);
                ArcingSmashTimer = 10000;
            }
			else ArcingSmashTimer -= diff;

            if (FelBreathTimer <= diff)
            {
                DoCastVictim(Phase1 ? SPELL_FELBREATH_1 : SPELL_FELBREATH_2);
                FelBreathTimer = 25000;
            }
			else FelBreathTimer -= diff;

            if (EjectTimer <= diff)
            {
                DoCastVictim(Phase1 ? SPELL_EJECT_1 : SPELL_EJECT_2);
                EjectTimer = 15000;
            }
			else EjectTimer -= diff;


            if (ChargeTimer <= diff)
            {
                if (me->GetDistance2d(me->GetVictim()) > 15)
                    DoCastVictim(SPELL_CHARGE);
                ChargeTimer = 10000;
            }
			else ChargeTimer -= diff;

            if (Phase1)
            {
                if (BewilderingStrikeTimer <= diff)
                {
                    DoCastVictim(SPELL_BEWILDERING_STRIKE);
                    BewilderingStrikeTimer = 20000;
                }
				else BewilderingStrikeTimer -= diff;

                //if (EjectTimer <= diff)
                //{
                //    DoCastVictim(SPELL_EJECT1);
                //    DoModifyThreatPercent(me->GetVictim(), -40);
                //    EjectTimer = 15000;
                //}else EjectTimer -= diff;

                //if (AcidicWoundTimer <= diff)
                //{
                //    DoCastVictim(SPELL_ACIDIC_WOUND);
                //    AcidicWoundTimer = 10000;
                //}else AcidicWoundTimer -= diff;

                if (BloodboilTimer <= diff)
                {
                    if (BloodboilCount < 5)
                    {
						DoCastAOE(SPELL_BLOODBOIL);
                        ++BloodboilCount;
                        BloodboilTimer = 10000;
                    }
                }
				else BloodboilTimer -= diff;
            }

            if (!Phase1)
            {
                //if (FelGeyserTimer <= diff)
                //{
                //    DoCastVictim(SPELL_FEL_GEYSER,true);
                //    FelGeyserTimer = 30000;
                //}
			      //else FelGeyserTimer -= diff;

                if (me->GetVictim() && me->GetVictim()->IsImmunedToDamage(SPELL_SCHOOL_MASK_ALL))
                    me->getThreatManager().modifyThreatPercent(me->GetVictim(),-100);
            }

            if (PhaseChangeTimer <= diff)
            {
                if (Phase1)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target && target->IsAlive())
                    {
                        Phase1 = false;

                        TargetThreat = DoGetThreat(target);
                        TargetGUID = target->GetGUID();
                        if (DoGetThreat(target))
                            DoModifyThreatPercent(target, -100);
                        me->AddThreat(target, 50000000.0f);
                        target->CastSpell(me, SPELL_TAUNT_GURTOGG, true);
                        me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                        DoCast(me, SPELL_INSIGNIFIGANCE, true);
                        DoCast(target, SPELL_FEL_RAGE_1, true);
                        DoCast(target, SPELL_FEL_RAGE_2, true);
                        DoCast(target, SPELL_FEL_RAGE_3, true);
                        DoCast(target, SPELL_FEL_RAGE_SCALE, true);
                        DoCast(target, SPELL_FEL_GEYSER, true);
                        DoCast(me, SPELL_FEL_RAGE_SELF);

                        switch(rand()%2)
                        {
                        case 0:
							DoSendQuantumText(SAY_SPECIAL1, me);
							break;
                        case 1:
							DoSendQuantumText(SAY_SPECIAL2, me);
							break;
                        }

                        FelGeyserTimer = 1;
                        PhaseChangeTimer = 30000;
                    }
                }
				else                                           // Encounter is a loop pretty much. Phase 1 -> Phase 2 -> Phase 1 -> Phase 2 till death or enrage
                {
                    if (TargetGUID)
                        RevertThreatOnTarget(TargetGUID);
                    TargetGUID = 0;
                    Phase1 = true;
                    BloodboilTimer = 10000;
                    BloodboilCount = 0;
                    /*AcidicWoundTimer += 2000;*/
                    ArcingSmashTimer += 2000;
                    FelBreathTimer += 2000;
                    EjectTimer += 2000;
                    PhaseChangeTimer = 65000;
                    me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                }
            }
			else PhaseChangeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gurtogg_bloodboilAI(creature);
    }
};

class npc_fel_geyser : public CreatureScript
{
public:
    npc_fel_geyser() : CreatureScript("npc_fel_geyser") { }

    struct npc_fel_geyserAI : public QuantumBasicAI
    {
        npc_fel_geyserAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint32 damageTimer;

        void Reset()
        {
            damageTimer = 1000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (damageTimer <= diff)
            {
                DoCastAOE(SPELL_FEL_GEYSER_DAMAGE);
                damageTimer = 60000;
            }
			else damageTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fel_geyserAI(creature);
    }
};

class spell_gurtogg_bloodboil_bloodboil : public SpellScriptLoader
{
    public:
        spell_gurtogg_bloodboil_bloodboil() : SpellScriptLoader("spell_gurtogg_bloodboil_bloodboil") { }

        class spell_gurtogg_bloodboil_bloodboil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gurtogg_bloodboil_bloodboil_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                if (unitList.size() <= 5)
                    return;

                unitList.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
                unitList.resize(5);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_gurtogg_bloodboil_bloodboil_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gurtogg_bloodboil_bloodboil_SpellScript();
        }
};

void AddSC_boss_gurtogg_bloodboil()
{
    new boss_gurtogg_bloodboil();
    new npc_fel_geyser();
	new spell_gurtogg_bloodboil_bloodboil();
}