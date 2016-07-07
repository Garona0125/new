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

// Coords for going for changing form
#define CENTER_X 120.148811f
#define CENTER_Y 703.713684f
#define CENTER_Z 45.111477f

// Speech
const char* YELL_TRANSFORM_TO_LYNX = "Let me introduce to you my new bruddahs: fang and claw!";
#define SOUND_TRANSFORM_TO_LYNX 12094

const char* YELL_TRANSFORM_TO_BEAR = "Got me some new tricks...like me bruddah bear!";
#define SOUND_TRANSFORM_TO_BEAR 12092

const char* YELL_TRANSFORM_TO_DRAGONHAWK = "Ya don have to look to da sky to see da dragonhawk!";
#define SOUND_TRANSFORM_TO_DRAGONHAWK 12095

const char* YELL_TRANSFORM_TO_EAGLE = "Dere be no hidin' from da eagle!";
#define SOUND_TRANSFORM_TO_EAGLE 12093

enum Texts
{
	SAY_AGGRO     = -1534098,
	SAY_DEATH     = -1534099,
	SAY_SLAY_1    = -1534100,
	SAY_SLAY_2    = -1534101,
	SAY_BERSERK   = -1534102,
	SAY_BREATH    = -1534103,
	SAY_INTRO     = -1534104,
};

enum Spells
{
	// Troll Form
	SPELL_WHIRLWIND             = 17207,
	SPELL_GRIEVOUS_THROW        = 43093,
	// Bear Form
	SPELL_CREEPING_PARALYSIS    = 43095,
	SPELL_OVERPOWER             = 43456,
	// Eagle Form
	SPELL_ENERGY_STORM          = 43983,
	SPELL_ZAP_INFORM            = 42577,
	SPELL_ZAP_DAMAGE            = 43137,
	SPELL_SUMMON_CYCLONE        = 43112,
	CREATURE_FEATHER_VORTEX     = 24136,
	SPELL_CYCLONE_VISUAL        = 43119,
	SPELL_CYCLONE_PASSIVE       = 43120,
	// Lynx Form
	SPELL_CLAW_RAGE_HASTE       = 42583,
	SPELL_CLAW_RAGE_TRIGGER     = 43149,
	SPELL_CLAW_RAGE_DAMAGE      = 43150,
	SPELL_LYNX_RUSH_HASTE       = 43152,
	SPELL_LYNX_RUSH_DAMAGE      = 43153,
	// Dragonhawk Form
	SPELL_FLAME_WHIRL           = 43213,
	SPELL_FLAME_BREATH          = 43215,
	SPELL_SUMMON_PILLAR         = 43216,
	CREATURE_COLUMN_OF_FIRE     = 24187,
	SPELL_PILLAR_TRIGGER        = 43218,
	// Cosmetic
	SPELL_SPIRIT_AURA           = 42466,
	SPELL_SIPHON_SOUL           = 43501,
	// Transforms:
	SPELL_SHAPE_OF_THE_BEAR     = 42594,
	SPELL_SHAPE_OF_THE_EAGLE    = 42606,
	SPELL_SHAPE_OF_THE_LYNX     = 42607,
	SPELL_SHAPE_OF_THE_HAWK     = 42608,
	SPELL_BERSERK               = 45078,
};

enum Phases
{
	PHASE_BEAR        = 0,
	PHASE_EAGLE       = 1,
	PHASE_LYNX        = 2,
	PHASE_DRAGONHAWK  = 3,
	PHASE_TROLL       = 4,
};

struct SpiritInfoStruct
{
    uint32 entry;
    float x, y, z, orient;
};

static SpiritInfoStruct SpiritInfo[4] =
{
    {23878, 147.87f, 706.51f, 45.11f, 3.04f},
    {23880, 88.95f, 705.49f, 45.11f, 6.11f},
    {23877, 137.23f, 725.98f, 45.11f, 3.71f},
    {23879, 104.29f, 726.43f, 45.11f, 5.43f}
};

struct TransformStruct
{
    uint32 sound;
    std::string text;
    uint32 spell, unaura;
};

static TransformStruct Transform[4] =
{
    {SOUND_TRANSFORM_TO_BEAR, YELL_TRANSFORM_TO_BEAR, SPELL_SHAPE_OF_THE_BEAR, SPELL_WHIRLWIND},
    {SOUND_TRANSFORM_TO_EAGLE, YELL_TRANSFORM_TO_EAGLE, SPELL_SHAPE_OF_THE_EAGLE, SPELL_SHAPE_OF_THE_BEAR},
    {SOUND_TRANSFORM_TO_LYNX, YELL_TRANSFORM_TO_LYNX, SPELL_SHAPE_OF_THE_LYNX, SPELL_SHAPE_OF_THE_EAGLE},
    {SOUND_TRANSFORM_TO_DRAGONHAWK, YELL_TRANSFORM_TO_DRAGONHAWK, SPELL_SHAPE_OF_THE_HAWK, SPELL_SHAPE_OF_THE_LYNX}
};

class boss_zuljin : public CreatureScript
{
    public:
        boss_zuljin() : CreatureScript("boss_zuljin") {}

        struct boss_zuljinAI : public QuantumBasicAI
        {
            boss_zuljinAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint64 SpiritGUID[4];
            uint64 ClawTargetGUID;
            uint64 TankGUID;

            uint32 Phase;
            uint32 health_20;

            uint32 IntroTimer;
            uint32 BerserkTimer;

            uint32 WhirlwindTimer;
            uint32 GrievousThrowTimer;

            uint32 Creeping_ParalysisTimer;
            uint32 OverpowerTimer;

            uint32 ClawRageTimer;
            uint32 LynxRushTimer;
            uint32 ClawCounter;
            uint32 ClawLoopTimer;

            uint32 FlameWhirlTimer;
            uint32 FlameBreathTimer;
            uint32 PillarOfFireTimer;

            SummonList Summons;

            void Reset()
            {
				instance->SetData(DATA_ZULJIN_EVENT, NOT_STARTED);

                Phase = 0;

                health_20 = me->CountPctFromMaxHealth(HEALTH_PERCENT_20);

                IntroTimer = 37000;
                BerserkTimer = 600000;

                WhirlwindTimer = 7000;
                GrievousThrowTimer = 8000;

                Creeping_ParalysisTimer = 7000;
                OverpowerTimer = 0;

                ClawRageTimer = 5000;
                LynxRushTimer = 14000;
                ClawLoopTimer = 0;
                ClawCounter = 0;

                FlameWhirlTimer = 5000;
                FlameBreathTimer = 6000;
                PillarOfFireTimer = 7000;

                ClawTargetGUID = 0;
                TankGUID = 0;

                Summons.DespawnAll();

                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 33975);
                //me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 218172674);
                //me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				instance->SetData(DATA_ZULJIN_EVENT, IN_PROGRESS);

                DoZoneInCombat();

				DoSendQuantumText(SAY_AGGRO, me);
                SpawnAdds();
                EnterPhase(0);
            }

            void KilledUnit(Unit* /*victim*/)
            {
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
				instance->SetData(DATA_ZULJIN_EVENT, DONE);

				DoSendQuantumText(SAY_DEATH, me);
                Summons.DespawnEntry(CREATURE_COLUMN_OF_FIRE);

                if (Unit* Temp = Unit::GetUnit(*me, SpiritGUID[3]))
                    Temp->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
            }

            void AttackStart(Unit* who)
            {
                if (Phase == 2)
                    AttackStartNoMove(who);
                else
                    QuantumBasicAI::AttackStart(who);
            }

            void DoMeleeAttackIfReady()
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (me->IsAttackReady() && me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        if (Phase == 1 && !OverpowerTimer)
                        {
                            uint32 health = me->GetVictim()->GetHealth();
                            me->AttackerStateUpdate(me->GetVictim());
                            if (me->GetVictim() && health == me->GetVictim()->GetHealth())
                            {
                                DoCastVictim(SPELL_OVERPOWER, false);
                                OverpowerTimer = 5000;
                            }
                        }
						else me->AttackerStateUpdate(me->GetVictim());
                        me->ResetAttackTimer();
                    }
                }
            }

            void SpawnAdds()
            {
                Creature* creature = NULL;
                for (uint8 i = 0; i < 4; ++i)
                {
                    creature = me->SummonCreature(SpiritInfo[i].entry, SpiritInfo[i].x, SpiritInfo[i].y, SpiritInfo[i].z, SpiritInfo[i].orient, TEMPSUMMON_DEAD_DESPAWN, 0);
                    if (creature)
                    {
                        creature->CastSpell(creature, SPELL_SPIRIT_AURA, true);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        SpiritGUID[i] = creature->GetGUID();
                    }
                }
            }

            void DespawnAdds()
            {
                for (uint8 i = 0; i < 4; ++i)
                {
                    Unit* Temp = NULL;
                    if (SpiritGUID[i])
                    {
                        Temp = Unit::GetUnit(*me, SpiritGUID[i]);
                        if (Temp)
                        {
                            Temp->SetVisible(false);
                            Temp->setDeathState(DEAD);
                        }
                    }
                    SpiritGUID[i] = 0;
                }
            }

            void JustSummoned(Creature* summon)
            {
                Summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                Summons.Despawn(summon);
            }

            void EnterPhase(uint32 NextPhase)
            {
                switch (NextPhase)
                {
                case 0:
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                    DoTeleportTo(CENTER_X, CENTER_Y, CENTER_Z, 100);
                    DoResetThreat();
                    me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                    me->RemoveAurasDueToSpell(Transform[Phase].unaura);
                    DoCast(me, Transform[Phase].spell);
                    me->MonsterYell(Transform[Phase].text.c_str(), LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, Transform[Phase].sound);
                    if (Phase > 0)
                    {
                        if (Unit* Temp = Unit::GetUnit(*me, SpiritGUID[Phase - 1]))
                            Temp->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
                    }
                    if (Unit* Temp = Unit::GetUnit(*me, SpiritGUID[NextPhase - 1]))
                        Temp->CastSpell(me, SPELL_SIPHON_SOUL, false); // should m cast on temp
                    if (NextPhase == 2)
                    {
                        me->GetMotionMaster()->Clear();
                        DoCast(me, SPELL_ENERGY_STORM, true); // enemy aura
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            Creature* Vortex = DoSpawnCreature(CREATURE_FEATHER_VORTEX, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                            if (Vortex)
                            {
                                Vortex->CastSpell(Vortex, SPELL_CYCLONE_PASSIVE, true);
                                Vortex->CastSpell(Vortex, SPELL_CYCLONE_VISUAL, true);
                                Vortex->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                Vortex->SetSpeed(MOVE_RUN, 1.0f);
                                Vortex->AI()->AttackStart(SelectTarget(TARGET_RANDOM, 0));
                                DoZoneInCombat(Vortex);
                            }
                        }
                    }
                    else
                        me->AI()->AttackStart(me->GetVictim());
                    if (NextPhase == 3)
                    {
                        me->RemoveAurasDueToSpell(SPELL_ENERGY_STORM);
                        Summons.DespawnEntry(CREATURE_FEATHER_VORTEX);
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                    break;
                default:
                    break;
                }
                Phase = NextPhase;
            }

            void UpdateAI(const uint32 diff)
            {
                if (!TankGUID)
                {
                    if (!UpdateVictim())
                        return;

                    if (me->GetHealth() < health_20 * (4 - Phase))
                        EnterPhase(Phase + 1);
                }

                if (BerserkTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK, true);
					DoSendQuantumText(SAY_BERSERK, me);
                    BerserkTimer = 60000;
                }
				else BerserkTimer -= diff;

                switch (Phase)
                {
                case 0:
                    if (IntroTimer)
                    {
                        if (IntroTimer <= diff)
                        {
							DoSendQuantumText(SAY_INTRO, me);
                            IntroTimer = 0;
                        }
						else IntroTimer -= diff;
                    }

                    if (WhirlwindTimer <= diff)
                    {
                        DoCast(me, SPELL_WHIRLWIND);
                        WhirlwindTimer = urand(15000, 20000);
                    }
					else WhirlwindTimer -= diff;

                    if (GrievousThrowTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, SPELL_GRIEVOUS_THROW, false);
							GrievousThrowTimer = 10000;
						}
                    }
					else GrievousThrowTimer -= diff;
                    break;
                case 1:
                    if (Creeping_ParalysisTimer <= diff)
                    {
                        DoCast(me, SPELL_CREEPING_PARALYSIS);
                        Creeping_ParalysisTimer = 20000;
                    }
					else Creeping_ParalysisTimer -= diff;

                    if (OverpowerTimer <= diff)
                    {
                        // implemented in DoMeleeAttackIfReady()
                        OverpowerTimer = 0;
                    }
					else OverpowerTimer -= diff;
                    break;

                case 2:
                    return;

                case 3:
                    if (ClawRageTimer <= diff)
                    {
                        if (!TankGUID)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                TankGUID = me->GetVictim()->GetGUID();
                                me->SetSpeed(MOVE_RUN, 5.0f);
                                AttackStart(target); // change victim
                                ClawRageTimer = 0;
                                ClawLoopTimer = 500;
                                ClawCounter = 0;
                            }
                        }
                        else if (!ClawRageTimer) // do not do this when Lynx_Rush
                        {
                            if (ClawLoopTimer <= diff)
                            {
                                Unit* target = me->GetVictim();

                                if (!target || !target->IsTargetableForAttack())
									target = Unit::GetUnit(*me, TankGUID);

                                if (!target || !target->IsTargetableForAttack())
									target = SelectTarget(TARGET_RANDOM, 0);

                                if (target)
                                {
                                    AttackStart(target);
                                    if (me->IsWithinMeleeRange(target))
                                    {
                                        DoCast(target, SPELL_CLAW_RAGE_DAMAGE, true);
                                        ++ClawCounter;
                                        if (ClawCounter == 12)
                                        {
                                            ClawRageTimer = urand(15000, 20000);
                                            me->SetSpeed(MOVE_RUN, 1.2f);
                                            AttackStart(Unit::GetUnit(*me, TankGUID));
                                            TankGUID = 0;
                                            return;
                                        }
                                        else ClawLoopTimer = 500;
                                    }
                                }
                                else
                                {
                                    EnterEvadeMode(); // if (target)
                                    return;
                                }
                            }
							else ClawLoopTimer -= diff;
                        } //if (TankGUID)
                    }
					else ClawRageTimer -= diff;

                    if (LynxRushTimer <= diff)
                    {
                        if (!TankGUID)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                TankGUID = me->GetVictim()->GetGUID();
                                me->SetSpeed(MOVE_RUN, 5.0f);
                                AttackStart(target); // change victim
                                LynxRushTimer = 0;
                                ClawCounter = 0;
                            }
                        }
                        else if (!LynxRushTimer)
                        {
                            Unit* target = me->GetVictim();
                            if (!target || !target->IsTargetableForAttack())
                            {
                                target = SelectTarget(TARGET_RANDOM, 0);
                                AttackStart(target);
                            }
                            if (target)
                            {
                                if (me->IsWithinMeleeRange(target))
                                {
                                    DoCast(target, SPELL_LYNX_RUSH_DAMAGE, true);
                                    ++ClawCounter;
                                    if (ClawCounter == 9)
                                    {
                                        LynxRushTimer = urand(15000, 20000);
                                        me->SetSpeed(MOVE_RUN, 1.2f);
                                        AttackStart(Unit::GetUnit(*me, TankGUID));
                                        TankGUID = 0;
                                    }
                                    else AttackStart(SelectTarget(TARGET_RANDOM, 0));
                                }
                            }
                            else
                            {
                                EnterEvadeMode(); // if (target)
                                return;
                            }
                        } //if (TankGUID)
                    }
					else LynxRushTimer -= diff;
                    break;
                case 4:
                    if (FlameWhirlTimer <= diff)
                    {
                        DoCast(me, SPELL_FLAME_WHIRL);
                        FlameWhirlTimer = 12000;
                    }
					FlameWhirlTimer -= diff;

                    if (PillarOfFireTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_SUMMON_PILLAR);
							PillarOfFireTimer = 10000;
						}
                    }
					else PillarOfFireTimer -= diff;

                    if (FlameBreathTimer <= diff)
                    {
						DoSendQuantumText(SAY_BREATH, me);

                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            me->SetInFront(target);
							DoCast(me, SPELL_FLAME_BREATH);
							FlameBreathTimer = 10000;
						}
                    }
					else FlameBreathTimer -= diff;
                    break;

                default:
                    break;
                }

                if (!TankGUID)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_zuljinAI(creature);
        }
};

class mob_zuljin_vortex : public CreatureScript
{
    public:
        mob_zuljin_vortex() : CreatureScript("mob_zuljin_vortex") {}

        struct mob_zuljin_vortexAI : public QuantumBasicAI
        {
            mob_zuljin_vortexAI(Creature* creature) : QuantumBasicAI(creature) {}

            void Reset()
			{
				me->SetCurrentFaction(16);
			}

            void EnterToBattle(Unit* /*who*/) {}

            void SpellHit(Unit* caster, SpellInfo* const spell)
            {
                if (spell->Id == SPELL_ZAP_INFORM)
                    DoCast(caster, SPELL_ZAP_DAMAGE, true);
            }

            void UpdateAI(const uint32 /*diff*/)
            {
                //if the vortex reach the target, it change his target to another player
                if (me->IsWithinMeleeRange(me->GetVictim()))
                    AttackStart(SelectTarget(TARGET_RANDOM, 0));
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_zuljin_vortexAI(creature);
        }
};

void AddSC_boss_zuljin()
{
    new boss_zuljin();
    new mob_zuljin_vortex();
}