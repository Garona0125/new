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
#include "Spell.h"
#include "black_temple.h"

enum Miscs
{
	NUMBER_ENSLAVED_SOUL        = 8,
};

enum Says
{
	SUFFERING_SAY_FREED         = -1564047,
	SUFFERING_SAY_AGGRO         = -1564048,
	SUFFERING_SAY_SLAY1         = -1564049,
	SUFFERING_SAY_SLAY2         = -1564050,
	SUFFERING_SAY_SLAY3         = -1564051,
	SUFFERING_SAY_RECAP         = -1564052,
	SUFFERING_SAY_AFTER         = -1564053,
	SUFFERING_EMOTE_ENRAGE      = -1564054,
	// Desire
	DESIRE_SAY_FREED           = -1564055,
	DESIRE_SAY_SLAY1           = -1564056,
	DESIRE_SAY_SLAY2           = -1564057,
	DESIRE_SAY_SLAY3           = -1564058,
	DESIRE_SAY_SPEC            = -1564059,
	DESIRE_SAY_RECAP           = -1564060,
	DESIRE_SAY_AFTER           = -1564061,
	// Anger
	ANGER_SAY_FREED            = -1564062,
	ANGER_SAY_FREED2           = -1564063,
	ANGER_SAY_SLAY1            = -1564064,
	ANGER_SAY_SLAY2            = -1564065,
	ANGER_SAY_SPEC             = -1564066,
	ANGER_SAY_BEFORE           = -1564067,
	ANGER_SAY_DEATH            = -1564068,
};

enum Spells
{
	AURA_OF_SUFFERING              = 41292,
	AURA_OF_SUFFERING_ARMOR        = 42017,
	ESSENCE_OF_SUFFERING_PASSIVE   = 41296,
	ESSENCE_OF_SUFFERING_PASSIVE2  = 41623,
	SPELL_FIXATE_TARGET            = 41294,
	SPELL_FIXATE_TAUNT             = 41295,
	SPELL_ENRAGE                   = 41305,
	SPELL_SOUL_DRAIN               = 41303,
	AURA_OF_DESIRE                 = 41350,
	AURA_OF_DESIRE_DAMAGE          = 41352,
	SPELL_RUNE_SHIELD              = 41431,
	SPELL_DEADEN                   = 41410,
	SPELL_SOUL_SHOCK               = 41426,
	AURA_OF_ANGER                  = 41337,
	SPELL_SELF_SEETHE              = 41364,
	SPELL_ENEMY_SEETHE             = 41520,
	SPELL_SOUL_SCREAM              = 41545,
	SPELL_SPITE_TARGET             = 41376,
	SPELL_SPITE_DAMAGE             = 41377,
	SPELL_ENSLAVED_SOUL_PASSIVE    = 41535,
	SPELL_SOUL_RELEASE             = 41542,
	SPELL_SUBMERGE                 = 37550,
};

struct Position2d
{
    float x, y;
};

static Position2d Coords[] =
{
    {450.4f, 212.3f},
    {542.1f, 212.3f},
    {542.1f, 168.3f},
    {542.1f, 137.4f},
    {450.4f, 137.4f},
    {450.4f, 168.3f}
};

struct boss_soul_essenceAI : public QuantumBasicAI
{
    boss_soul_essenceAI(Creature* creature) : QuantumBasicAI(creature) { }

    uint32 pulseTimer;
    uint64 ReliquaryGUID;

    void Reset()
    {
        pulseTimer = 10000;

        if (ReliquaryGUID)
        {
            Creature* Reliquary = (Unit::GetCreature(*me, ReliquaryGUID));
            if (Reliquary)
                Reliquary->AI()->EnterEvadeMode();
        }
    }

    void DoAggroPuls(const uint32 diff)
    {
        if (pulseTimer <= diff)
        {
            DoAttackerAreaInCombat(me->GetVictim(),50);
            pulseTimer = 10000;
        }
		else pulseTimer -= diff;
    }

    void EnterToBattle(Unit* /*who*/) {}

    void UpdateAI(const uint32 /*diff*/) {}
};

class npc_enslaved_soul : public CreatureScript
{
public:
    npc_enslaved_soul() : CreatureScript("npc_enslaved_soul") { }

    struct npc_enslaved_soulAI : public QuantumBasicAI
    {
        npc_enslaved_soulAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint64 ReliquaryGUID;

        void Reset()
		{
			ReliquaryGUID = 0;
		}

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_ENSLAVED_SOUL_PASSIVE, true);
            DoZoneInCombat();
        }

        void JustDied(Unit* /*killer*/);
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enslaved_soulAI(creature);
    }
};

class boss_reliquary_of_souls : public CreatureScript
{
public:
    boss_reliquary_of_souls() : CreatureScript("boss_reliquary_of_souls") { }

    struct boss_reliquary_of_soulsAI : public QuantumBasicAI
    {
        boss_reliquary_of_soulsAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            EssenceGUID = 0;
        }

        InstanceScript* instance;

        uint64 EssenceGUID;

        uint32 Phase;
        uint32 Counter;
        uint32 Timer;

        uint32 SoulCount;
        uint32 SoulDeathCount;

        uint32 pulseTimer;

        void Reset()
        {
			instance->SetData(DATA_RELIQUARYOFSOULSEVENT, NOT_STARTED);

            if (EssenceGUID)
            {
                if (Creature* Essence = Unit::GetCreature(*me, EssenceGUID))
                    Essence->DespawnAfterAction();

                EssenceGUID = 0;
            }

            Phase = 0;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_NONE);
            me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (!who)
				return;

			if (me->IsInCombatActive())
				return;

			if (who->GetTypeId() != TYPE_ID_PLAYER)
				return;

			if (me->GetDistance(who) > 50.0f)
				return;

			AttackStartNoMove(who);
		}

        void DoAggroPuls(const uint32 diff)
        {
            if (pulseTimer <= diff)
            {
                DoAttackerAreaInCombat(me->GetVictim(),50);
                pulseTimer = 10000;
            }
			else pulseTimer -= diff;
        }

        void EnterToBattle(Unit* who)
        {
            me->AddThreat(who, 10000.0f);
            DoZoneInCombat();

			instance->SetData(DATA_RELIQUARYOFSOULSEVENT, IN_PROGRESS);

            Phase = 1;
            Counter = 0;
            Timer = 0;
            pulseTimer = 10000;
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            me->Attack(who, true);
        }

        bool SummonSoul()
        {
            uint32 random = rand()%6;
            float x = Coords[random].x;
            float y = Coords[random].y;
            Creature* Soul = me->SummonCreature(NPC_ENSLAVED_SOUL, x, y, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 0);
            if (!Soul) return false;
            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
            {
                CAST_AI(npc_enslaved_soul::npc_enslaved_soulAI,Soul->AI())->ReliquaryGUID = me->GetGUID();
                Soul->AI()->AttackStart(target);
            }
			else EnterEvadeMode();
            return true;
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_RELIQUARYOFSOULSEVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!Phase)
                return;

            if (me->getThreatManager().getThreatList().empty()) // Reset if event is begun and we don't have a threatlist
            {
                EnterEvadeMode();
                return;
            }

            Creature* Essence;
            if (EssenceGUID)
            {
                Essence = Unit::GetCreature(*me, EssenceGUID);
                if (!Essence)
                {
                    EnterEvadeMode();
                    return;
                }
            }

            DoAggroPuls(diff);

            if (Timer <= diff)
            {
                switch (Counter)
                {
                case 0:
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);  // I R ANNNGRRRY!
                    Timer = 3000;
                    break;
                case 1:
                    Timer = 2800;
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_SUBMERGE);  // Release the cube
                    DoCast(me,SPELL_SUBMERGE);
                    break;
                case 2:
                    Timer = 5000;
                    if (Creature* Summon = DoSpawnCreature(23417+Phase, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                    {
                        me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SUBMERGED);  // Ribs: open

                        if (Unit* target = SelectTarget(TARGET_NEAREST, 0))
                            Summon->AI()->AttackStart(target);
                        EssenceGUID = Summon->GetGUID();
                        ((boss_soul_essenceAI*)Summon->AI())->ReliquaryGUID = me->GetGUID();
                    }
					else EnterEvadeMode();
                    break;
                case 3:
                    Timer = 1000;
                    if (Phase == 3)
                    {
                        if (!Essence->IsAlive())
                            me->CastSpell(me, 7, true);
                        else return;
                    }
                    else
                    {
                        if (Essence->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                        {
                            //Essence->AI()->EnterEvadeMode();
                            Essence->GetMotionMaster()->MoveFollow(me, 0, 0);
                        }
						else return;
                    }
                    break;
                case 4:
                    Timer = 1500;
                    if (Essence->IsWithinDistInMap(me, 10))
                    {
                        me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                        Essence->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_SUBMERGE); //rotate and disappear
                    }
                    else
                    {
                        //Essence->AI()->EnterEvadeMode();
                        Essence->GetMotionMaster()->MoveFollow(me, 0, 0);
                        return;
                    }
                    break;
                case 5:
                    if (Phase == 1)
                    {
                        DoSendQuantumText(SUFFERING_SAY_AFTER, Essence);
                    }
                    else
                        DoSendQuantumText(DESIRE_SAY_AFTER, Essence);

                    Essence->DespawnAfterAction();
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
                    EssenceGUID = 0;
                    SoulCount = 0;
                    SoulDeathCount = 0;
                    Timer = 3000;
                    break;
                case 6:
                    if (SoulCount < NUMBER_ENSLAVED_SOUL)
                    {
                        if (SummonSoul())
                            SoulCount++;
                        Timer = 500;
                        return;
                    }
					break;
                case 7:
                    if (SoulDeathCount >= SoulCount)
                    {
                        Counter = 1;
                        Phase++;
                        Timer = 5000;
                    }
                    return;
                default:
                    break;
                }
                Counter++;
            }
			else Timer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_reliquary_of_soulsAI(creature);
    }
};

class boss_essence_of_suffering : public CreatureScript
{
public:
    boss_essence_of_suffering() : CreatureScript("boss_essence_of_suffering") { }

    struct  boss_essence_of_sufferingAI : public boss_soul_essenceAI
    {
        boss_essence_of_sufferingAI(Creature* creature) : boss_soul_essenceAI(creature) { }

        uint64 StatAuraGUID;

        uint32 AggroYellTimer;
        uint32 FixateTimer;
        uint32 EnrageTimer;
        uint32 SoulDrainTimer;
        uint32 AuraTimer;

        void Reset()
        {
            StatAuraGUID = 0;

            AggroYellTimer = 5000;
            FixateTimer = 8000;
            EnrageTimer = 30000;
            SoulDrainTimer = 45000;
            AuraTimer = 5000;

            boss_soul_essenceAI::Reset();
        }

        void DamageTaken(Unit* /*doneby*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
                damage = 0;

                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    DoSendQuantumText(SUFFERING_SAY_RECAP, me);

				me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SUFFERING_SAY_FREED, me);
            DoZoneInCombat();
            me->CastSpell(me, AURA_OF_SUFFERING, true); // linked aura need core support
            me->CastSpell(me, ESSENCE_OF_SUFFERING_PASSIVE, true);
            me->CastSpell(me, ESSENCE_OF_SUFFERING_PASSIVE2, true);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(SUFFERING_SAY_SLAY1, SUFFERING_SAY_SLAY2, SUFFERING_SAY_SLAY3), me);
        }

        void CastFixate()
        {
            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            if (m_threatlist.empty())
                return; // No point continuing if empty threatlist.
            std::list<Unit*> targets;
            std::list<HostileReference*>::iterator itr = m_threatlist.begin();
            for(;itr != m_threatlist.end(); ++itr)
            {
                Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                if (unit && unit->IsAlive() && (unit->GetTypeId() == TYPE_ID_PLAYER)) // Only alive players
                    targets.push_back(unit);
            }
            if (targets.empty())
                return; // No targets added for some reason. No point continuing.
            targets.sort(Trinity::ObjectDistanceOrderPred(me)); // Sort players by distance.
            targets.resize(1); // Only need closest target.
            Unit* target = targets.front(); // Get the first target.
            if (target)
                target->CastSpell(me, SPELL_FIXATE_TAUNT, true);
            DoResetThreat();
            me->AddThreat(target, 99999);
        }

        void UpdateAI(const uint32 diff)
        {
            if (me->IsInCombatActive())
            {
                //Supposed to be cast on nearest target
                if (FixateTimer <= diff)
                {
                    CastFixate();
                    FixateTimer = 5000;

                    if (!(rand()%16))
                        DoSendQuantumText(SUFFERING_SAY_AGGRO, me);
                }
				else FixateTimer -= diff;
            }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            DoAggroPuls(diff);

            if (EnrageTimer <= diff)
            {
                DoCast(me, SPELL_ENRAGE);
                EnrageTimer = 60000;
                DoSendQuantumText(SUFFERING_EMOTE_ENRAGE, me);
            }
			else EnrageTimer -= diff;

            if (SoulDrainTimer <= diff)
            {
                DoCast(SelectTarget(TARGET_RANDOM,0), SPELL_SOUL_DRAIN);
                SoulDrainTimer = 60000;
            }
			else SoulDrainTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_essence_of_sufferingAI(creature);
    }
};

class boss_essence_of_desire : public CreatureScript
{
public:
    boss_essence_of_desire() : CreatureScript("boss_essence_of_desire") { }

    struct  boss_essence_of_desireAI : public boss_soul_essenceAI
    {
        boss_essence_of_desireAI(Creature* creature) : boss_soul_essenceAI(creature) {}

        uint32 RuneShieldTimer;
        uint32 DeadenTimer;
        uint32 SoulShockTimer;

        void Reset()
        {
            RuneShieldTimer = 60000;
            DeadenTimer = 30000;
            SoulShockTimer = 2000;

            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);

            boss_soul_essenceAI::Reset();
        }

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
            if (doneby == me)
                return;

            if (damage >= me->GetHealth())
            {
                damage = 0;

                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    DoSendQuantumText(DESIRE_SAY_RECAP, me);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->SetReactState(REACT_PASSIVE);
            }
            else
            {
                int32 bp0 = damage / 2;
                me->CastCustomSpell(doneby, AURA_OF_DESIRE_DAMAGE, &bp0, NULL, NULL, true);
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
			{
                for (uint8 i = 0; i < 3; ++i)
				{
                    if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
					{
                        if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_SOUL_SHOCK
                            || me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_DEADEN) me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
					}
				}
			}
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(DESIRE_SAY_FREED, me);
            DoZoneInCombat();
            DoCast(me, AURA_OF_DESIRE, true);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(DESIRE_SAY_SLAY1, DESIRE_SAY_SLAY2, DESIRE_SAY_SLAY3), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoAggroPuls(diff);

            if (RuneShieldTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    me->CastSpell(me, SPELL_RUNE_SHIELD, true);
                    SoulShockTimer += 2000;
                    DeadenTimer += 2000;
                    RuneShieldTimer = 60000;
                }
            }
			else RuneShieldTimer -= diff;

            if (SoulShockTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCastVictim(SPELL_SOUL_SHOCK);
                    SoulShockTimer = 8000;
                }
            }
			else SoulShockTimer -= diff;

            if (DeadenTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCastVictim(SPELL_DEADEN);
                    DeadenTimer = urand(25000, 35000);
					DoSendQuantumText(DESIRE_SAY_SPEC, me);
                }
            }
			else DeadenTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_essence_of_desireAI(creature);
    }
};

class boss_essence_of_anger : public CreatureScript
{
public:
    boss_essence_of_anger() : CreatureScript("boss_essence_of_anger") { }

    struct  boss_essence_of_angerAI : public boss_soul_essenceAI
    {
        boss_essence_of_angerAI(Creature* creature) : boss_soul_essenceAI(creature) { }

        uint64 AggroTargetGUID;

        uint32 CheckTankTimer;
        uint32 SoulScreamTimer;
        uint32 SpiteTimer;

        std::list<uint64> SpiteTargetGUID;

        bool CheckedAggro;

        void Reset()
        {
            AggroTargetGUID = 0;

            CheckTankTimer = 5000;
            SoulScreamTimer = 10000;
            SpiteTimer = 30000;

            SpiteTargetGUID.clear();

            CheckedAggro = false;

            boss_soul_essenceAI::Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
			DoSendQuantumText(RAND(ANGER_SAY_FREED, ANGER_SAY_FREED2), me);

            DoZoneInCombat();
            DoCast(me, AURA_OF_ANGER, true);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(ANGER_SAY_DEATH, me);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(ANGER_SAY_SLAY1, ANGER_SAY_SLAY2), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoAggroPuls(diff);

            if (!CheckedAggro)
            {
                AggroTargetGUID = me->GetVictim()->GetGUID();
                CheckedAggro = true;
            }

            if (CheckTankTimer <= diff)
            {
                if (me->GetVictim()->GetGUID() != AggroTargetGUID)
                {
                    DoSendQuantumText(ANGER_SAY_BEFORE, me);
                    DoCast(me, SPELL_SELF_SEETHE, true);
                    AggroTargetGUID = me->GetVictim()->GetGUID();
                }
                CheckTankTimer = 2000;
            }
			else CheckTankTimer -= diff;

            if (SoulScreamTimer <= diff)
            {
                DoCastVictim(SPELL_SOUL_SCREAM);
                SoulScreamTimer = urand(9000, 11000);

                if (!(rand()%3))
                    DoSendQuantumText(ANGER_SAY_SPEC, me);
            }
			else SoulScreamTimer -= diff;

            if (SpiteTimer <= diff)
            {
                DoCast(me, SPELL_SPITE_TARGET);
                SpiteTimer = 30000;
                DoSendQuantumText(ANGER_SAY_SPEC, me);
            }
			else SpiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_essence_of_angerAI(creature);
    }
};

void npc_enslaved_soul::npc_enslaved_soulAI::JustDied(Unit* killer)
{
    if (ReliquaryGUID)
    {
        Creature* Reliquary = (Unit::GetCreature(*me, ReliquaryGUID));
        if (Reliquary)
            CAST_AI(boss_reliquary_of_souls::boss_reliquary_of_soulsAI,Reliquary->AI())->SoulDeathCount++;
    }
    DoCast(me, SPELL_SOUL_RELEASE, true);
}

void AddSC_boss_reliquary_of_souls()
{
    new boss_reliquary_of_souls();
    new boss_essence_of_suffering();
    new boss_essence_of_desire();
    new boss_essence_of_anger();
    new npc_enslaved_soul();
}