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
#include "zulaman.h"

enum Texts
{
	SAY_AGGRO       = -1534070,
	SAY_DEATH       = -1534071,
	SAY_SLAY_1      = -1534072,
	SAY_SLAY_2      = -1534073,
	SAY_SABER_1     = -1534074,
	SAY_SABER_2     = -1534075,
	SAY_SPLIT       = -1534076,
	SAY_MERGE       = -1534077,
	SAY_BERSERK     = -1534078,
};

enum Spells
{
	SPELL_DUAL_WIELD              = 29651,
	SPELL_SABER_LASH              = 43267,
	SPELL_FRENZY                  = 43139,
	SPELL_FLAME_SHOCK             = 43303,
	SPELL_EARTH_SHOCK             = 43305,
	SPELL_TRANSFORM_SPLIT         = 43142,
	SPELL_TRANSFORM_SPLIT2        = 43573,
	SPELL_TRANSFORM_MERGE         = 43271,
	SPELL_SUMMON_LYNX             = 43143,
	SPELL_SUMMON_TOTEM            = 43302,
	SPELL_BERSERK                 = 45078,
	SPELL_LYNX_FRENZY             = 43290,
	SPELL_SHRED_ARMOR             = 43243,
};

enum PhaseHalazzi
{
    PHASE_NONE   = 0,
    PHASE_LYNX   = 1,
    PHASE_SPLIT  = 2,
    PHASE_HUMAN  = 3,
    PHASE_MERGE  = 4,
    PHASE_ENRAGE = 5,
};

class boss_halazzi : public CreatureScript
{
    public:
        boss_halazzi() : CreatureScript("boss_halazzi") {}

        struct boss_halazziAI : public QuantumBasicAI
        {
            boss_halazziAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 FrenzyTimer;
            uint32 SaberlashTimer;
            uint32 ShockTimer;
            uint32 TotemTimer;
            uint32 CheckTimer;
            uint32 BerserkTimer;

            uint32 TransformCount;

            PhaseHalazzi Phase;

            uint64 LynxGUID;

            void Reset()
            {
				instance->SetData(DATA_HALAZZI_EVENT, NOT_STARTED);

                TransformCount = 0;
                BerserkTimer = 600000;
                CheckTimer = 1000;

                DoCast(me, SPELL_DUAL_WIELD, true);

				me->SetPowerType(POWER_ENERGY);
				me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

                Phase = PHASE_NONE;
                EnterPhase(PHASE_LYNX);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				instance->SetData(DATA_HALAZZI_EVENT, IN_PROGRESS);

				DoSendQuantumText(SAY_AGGRO, me);

                EnterPhase(PHASE_LYNX);
            }

            void JustSummoned(Creature* summon)
            {
                summon->AI()->AttackStart(me->GetVictim());
                if (summon->GetEntry() == NPC_SPIRIT_LYNX)
                    LynxGUID = summon->GetGUID();
            }

            void DamageTaken(Unit* /*done_by*/, uint32 &damage)
            {
                if (damage >= me->GetHealth() && Phase != PHASE_ENRAGE)
                    damage = 0;
            }

            void SpellHit(Unit*, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_TRANSFORM_SPLIT2)
                    EnterPhase(PHASE_HUMAN);
            }

            void AttackStart(Unit* who)
            {
                if (Phase != PHASE_MERGE)
					QuantumBasicAI::AttackStart(who);
            }

            void EnterPhase(PhaseHalazzi NextPhase)
            {
                switch (NextPhase)
                {
                case PHASE_LYNX:
                case PHASE_ENRAGE:
                    if (Phase == PHASE_MERGE)
                    {
                        DoCast(me, SPELL_TRANSFORM_MERGE, true);
                        me->Attack(me->GetVictim(), true);
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                    if (Creature* Lynx = Unit::GetCreature(*me, LynxGUID))
                        Lynx->DisappearAndDie();
                    me->SetMaxHealth(600000);
                    me->SetHealth(600000 - 150000 * TransformCount);
                    FrenzyTimer = 16000;
                    SaberlashTimer = 20000;
                    ShockTimer = 10000;
                    TotemTimer = 12000;
                    break;
                case PHASE_SPLIT:
					DoSendQuantumText(SAY_SPLIT, me);
                    DoCast(me, SPELL_TRANSFORM_SPLIT, true);
                    break;
                case PHASE_HUMAN:
                    //DoCast(me, SPELL_SUMMON_LYNX, true);
                    DoSpawnCreature(NPC_SPIRIT_LYNX, 5, 5, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    me->SetMaxHealth(400000);
                    me->SetHealth(400000);
                    ShockTimer = 10000;
                    TotemTimer = 12000;
                    break;
                case PHASE_MERGE:
                    if (Unit* pLynx = Unit::GetUnit(*me, LynxGUID))
                    {
						DoSendQuantumText(SAY_MERGE, me);
                        pLynx->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pLynx->GetMotionMaster()->Clear();
                        pLynx->GetMotionMaster()->MoveFollow(me, 0, 0);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveFollow(pLynx, 0, 0);
                        ++TransformCount;
                    }
                    break;
                default:
                    break;
                }
                Phase = NextPhase;
            }

             void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (BerserkTimer <= diff)
                {
					DoSendQuantumText(SAY_BERSERK, me);
                    DoCast(me, SPELL_BERSERK, true);
                    BerserkTimer = 60000;
                }
				else BerserkTimer -= diff;

                if (Phase == PHASE_LYNX || Phase == PHASE_ENRAGE)
                {
                    if (SaberlashTimer <= diff)
                    {
                        // A tank with more than 490 defense skills should receive no critical hit
                        //DoCast(me, 41296, true);
                        DoCastVictim(SPELL_SABER_LASH, true);
						DoSendQuantumText(RAND(SAY_SABER_1, SAY_SABER_2), me);
                        //me->RemoveAurasDueToSpell(41296);
                        SaberlashTimer = 30000;
                    }
					else SaberlashTimer -= diff;

                    if (FrenzyTimer <= diff)
                    {
                        DoCast(me, SPELL_FRENZY);
                        FrenzyTimer = urand(10000, 15000);
                    }
					else FrenzyTimer -= diff;

                    if (Phase == PHASE_LYNX)
                    {
                        if (CheckTimer <= diff)
                        {
                            if (HealthBelowPct(25 * (3 - TransformCount)))
                                EnterPhase(PHASE_SPLIT);
                            CheckTimer = 1000;
                        }
						else CheckTimer -= diff;
                    }
                }

                if (Phase == PHASE_HUMAN || Phase == PHASE_ENRAGE)
                {
                    if (TotemTimer <= diff)
                    {
                        DoCast(me, SPELL_SUMMON_TOTEM);
                        TotemTimer = 20000;
                    }
					else TotemTimer -= diff;

                    if (ShockTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        {
                            if (target->IsNonMeleeSpellCasted(false))
                                DoCast(target, SPELL_EARTH_SHOCK);
                            else
                                DoCast(target, SPELL_FLAME_SHOCK);
                            ShockTimer = urand(10000, 15000);
                        }
                    }
					else ShockTimer -= diff;

                    if (Phase == PHASE_HUMAN)
                    {
                        if (CheckTimer <= diff)
                        {
                            if (!HealthAbovePct(20) /*HealthBelowPct(10)*/)
                                EnterPhase(PHASE_MERGE);
                            else
                            {
                                Unit* Lynx = Unit::GetUnit(*me, LynxGUID);
                                if (Lynx && !Lynx->HealthAbovePct(20) /*Lynx->HealthBelowPct(10)*/)
                                    EnterPhase(PHASE_MERGE);
                            }
                            CheckTimer = 1000;
                        }
						else CheckTimer -= diff;
                    }
                }

                if (Phase == PHASE_MERGE)
                {
                    if (CheckTimer <= diff)
                    {
                        Unit* Lynx = Unit::GetUnit(*me, LynxGUID);
                        if (Lynx)
                        {
                            Lynx->GetMotionMaster()->MoveFollow(me, 0, 0);
                            me->GetMotionMaster()->MoveFollow(Lynx, 0, 0);
                            if (me->IsWithinDistInMap(Lynx, 6.0f))
                            {
                                if (TransformCount < 3)
                                    EnterPhase(PHASE_LYNX);
                                else
                                    EnterPhase(PHASE_ENRAGE);
                            }
                        }
                        CheckTimer = 1000;
                    }
					else CheckTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }

            void KilledUnit(Unit* /*victim*/)
            {
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
				instance->SetData(DATA_HALAZZI_EVENT, DONE);

				DoSendQuantumText(SAY_DEATH, me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_halazziAI(creature);
        }
};

class mob_halazzi_lynx : public CreatureScript
{
    public:
        mob_halazzi_lynx() : CreatureScript("mob_halazzi_lynx"){}

        struct mob_halazzi_lynxAI : public QuantumBasicAI
        {
            mob_halazzi_lynxAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 FrenzyTimer;
            uint32 ShredderTimer;

            void Reset()
            {
                FrenzyTimer = urand(30000, 50000);  //frenzy every 30-50 seconds
                ShredderTimer = 4000;
            }

            void DamageTaken(Unit* /*done_by*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                    damage = 0;
            }

            void AttackStart(Unit* who)
            {
                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    QuantumBasicAI::AttackStart(who);
            }

            void EnterToBattle(Unit* /*who*/)
			{
				//DoZoneInCombat();
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (FrenzyTimer <= diff)
                {
                    DoCast(me, SPELL_LYNX_FRENZY);
                    FrenzyTimer = 30000;
                }
				else FrenzyTimer -= diff;

                if (ShredderTimer <= diff)
                {
                    DoCastVictim(SPELL_SHRED_ARMOR);
                    ShredderTimer = 4000;
                }
				else ShredderTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_halazzi_lynxAI(creature);
        }
};

void AddSC_boss_halazzi()
{
    new boss_halazzi();
    new mob_halazzi_lynx();
}