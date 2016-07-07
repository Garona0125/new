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
#include "SpellAuraEffects.h"
#include "zulaman.h"

#define ORIENT 1.5696f
#define POS_Y  921.2795f
#define POS_Z  33.8883f

enum Texts
{
	SAY_AGGRO    = -1534079,
	SAY_DEATH    = -1534080,
	SAY_SLAY_1   = -1534081,
	SAY_SLAY_2   = -1534082,
	SAY_BOLTS    = -1534083,
	SAY_DRAIN    = -1534084,
	SAY_SOUL     = -1534085,
};

enum Spells
{
	SPELL_SPIRIT_BOLTS      = 43383,
	SPELL_DRAIN_POWER       = 44131,
	SPELL_SIPHON_SOUL       = 43501,
	// Druid
	SPELL_DR_LIFEBLOOM      = 43421,
	SPELL_DR_THORNS         = 43420,
	SPELL_DR_MOONFIRE       = 43545,
	// Hunter
	SPELL_HU_EXPLOSIVE_TRAP = 43444,
	SPELL_HU_FREEZING_TRAP  = 43447,
	SPELL_HU_SNAKE_TRAP     = 43449,
	// Mage
	SPELL_MG_FIREBALL       = 41383,
	SPELL_MG_FROSTBOLT      = 43428,
	SPELL_MG_FROST_NOVA     = 43426,
	SPELL_MG_ICE_LANCE      = 43427,
	// Paladin
	SPELL_PA_CONSECRATION   = 43429,
	SPELL_PA_HOLY_LIGHT     = 43451,
	SPELL_PA_AVENGING_WRATH = 43430,
	// Priest
	SPELL_PR_HEAL           = 41372,
	SPELL_PR_MIND_CONTROL   = 43550,
	SPELL_PR_MIND_BLAST     = 41374,
	SPELL_PR_SW_DEATH       = 41375,
	SPELL_PR_PSYCHIC_SCREAM = 43432,
	SPELL_PR_PAIN_SUPP      = 44416,
	// Rogue
	SPELL_RO_BLIND          = 43433,
	SPELL_RO_SLICE_DICE     = 43457,
	SPELL_RO_WOUND_POISON   = 39665,
	// Shaman
	SPELL_SH_FIRE_NOVA      = 43436,
	SPELL_SH_HEALING_WAVE   = 43548,
	SPELL_SH_CHAIN_LIGHT    = 43435,
	// Warlock
	SPELL_WL_CURSE_OF_DOOM  = 43439,
	SPELL_WL_RAIN_OF_FIRE   = 43440,
	SPELL_WL_UNSTABLE_AFFL  = 43522,
	SPELL_WL_UNSTABLE_AFFL_DISPEL = 43523,
	// Warrior
	SPELL_WR_SPELL_REFLECT  = 43443,
	SPELL_WR_WHIRLWIND      = 43442,
	SPELL_WR_MORTAL_STRIKE  = 43441,
};

static float Pos_X[4] = {112.8827f, 107.8827f, 122.8827f, 127.8827f};

static uint32 AddEntryList[8]=
{
    24240, //Alyson Antille
    24241, //Thurg
    24242, //Slither
    24243, //Lord Raadan
    24244, //Gazakroth
    24245, //Fenstalker
    24246, //Darkheart
    24247  //Koragg
};

enum AbilityTarget
{
    ABILITY_TARGET_SELF = 0,
    ABILITY_TARGET_VICTIM = 1,
    ABILITY_TARGET_ENEMY = 2,
    ABILITY_TARGET_HEAL = 3,
    ABILITY_TARGET_BUFF = 4,
    ABILITY_TARGET_SPECIAL = 5,
};

struct PlayerAbilityStruct
{
    uint32 spell;
    AbilityTarget target;
    uint32 cooldown; // FIXME - it's never used
};

static PlayerAbilityStruct PlayerAbility[][3] =
{
	// 1 warrior
    {{SPELL_WR_SPELL_REFLECT, ABILITY_TARGET_SELF, 10000},
    {SPELL_WR_WHIRLWIND, ABILITY_TARGET_SELF, 10000},
    {SPELL_WR_MORTAL_STRIKE, ABILITY_TARGET_VICTIM, 6000}},
    // 2 paladin
    {{SPELL_PA_CONSECRATION, ABILITY_TARGET_SELF, 10000},
    {SPELL_PA_HOLY_LIGHT, ABILITY_TARGET_HEAL, 10000},
    {SPELL_PA_AVENGING_WRATH, ABILITY_TARGET_SELF, 10000}},
    // 3 hunter
    {{SPELL_HU_EXPLOSIVE_TRAP, ABILITY_TARGET_SELF, 10000},
    {SPELL_HU_FREEZING_TRAP, ABILITY_TARGET_SELF, 10000},
    {SPELL_HU_SNAKE_TRAP, ABILITY_TARGET_SELF, 10000}},
    // 4 rogue
    {{SPELL_RO_WOUND_POISON, ABILITY_TARGET_VICTIM, 3000},
    {SPELL_RO_SLICE_DICE, ABILITY_TARGET_SELF, 10000},
    {SPELL_RO_BLIND, ABILITY_TARGET_ENEMY, 10000}},
    // 5 priest
    {{SPELL_PR_PAIN_SUPP, ABILITY_TARGET_HEAL, 10000},
    {SPELL_PR_HEAL, ABILITY_TARGET_HEAL, 10000},
    {SPELL_PR_PSYCHIC_SCREAM, ABILITY_TARGET_SELF, 10000}},
    // 5* shadow priest
    {{SPELL_PR_MIND_CONTROL, ABILITY_TARGET_ENEMY, 15000},
    {SPELL_PR_MIND_BLAST, ABILITY_TARGET_ENEMY, 5000},
    {SPELL_PR_SW_DEATH, ABILITY_TARGET_ENEMY, 10000}},
    // 7 shaman
    {{SPELL_SH_FIRE_NOVA, ABILITY_TARGET_SELF, 10000},
    {SPELL_SH_HEALING_WAVE, ABILITY_TARGET_HEAL, 10000},
    {SPELL_SH_CHAIN_LIGHT, ABILITY_TARGET_ENEMY, 8000}},
    // 8 mage
    {{SPELL_MG_FIREBALL, ABILITY_TARGET_ENEMY, 5000},
    {SPELL_MG_FROSTBOLT, ABILITY_TARGET_ENEMY, 5000},
    {SPELL_MG_ICE_LANCE, ABILITY_TARGET_SPECIAL, 2000}},
    // 9 warlock
    {{SPELL_WL_CURSE_OF_DOOM, ABILITY_TARGET_ENEMY, 10000},
    {SPELL_WL_RAIN_OF_FIRE, ABILITY_TARGET_ENEMY, 10000},
    {SPELL_WL_UNSTABLE_AFFL, ABILITY_TARGET_ENEMY, 10000}},
    // 11 druid
    {{SPELL_DR_LIFEBLOOM, ABILITY_TARGET_HEAL, 10000},
    {SPELL_DR_THORNS, ABILITY_TARGET_SELF, 10000},
    {SPELL_DR_MOONFIRE, ABILITY_TARGET_ENEMY, 8000}}
};

struct boss_hexlord_addAI : public QuantumBasicAI
{
    InstanceScript* instance;

    boss_hexlord_addAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    void Reset() {}

    void EnterToBattle(Unit* /*who*/)
	{
		DoZoneInCombat();
	}

    void UpdateAI(const uint32 /*diff*/)
    {
        if (instance && instance->GetData(DATA_HEXLORD_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        DoMeleeAttackIfReady();
    }
};

class boss_hexlord_malacrass : public CreatureScript
{
    public:
        boss_hexlord_malacrass() : CreatureScript("boss_hexlord_malacrass") {}

        struct boss_hex_lord_malacrassAI : public QuantumBasicAI
        {
            boss_hex_lord_malacrassAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                SelectAddEntry();
                for (uint8 i = 0; i < 4; ++i)
                    AddGUID[i] = 0;
            }

            InstanceScript* instance;

            uint64 AddGUID[4];
            uint32 AddEntry[4];

            uint64 PlayerGUID;

            uint32 SpiritBoltsTimer;
            uint32 DrainPowerTimer;
            uint32 SiphonSoulTimer;
            uint32 PlayerAbilityTimer;
            uint32 CheckAddStateTimer;
            uint32 ResetTimer;

            uint32 PlayerClass;

            void Reset()
            {
                if (instance)
                    instance->SetData(DATA_HEXLORD_EVENT, NOT_STARTED);

                SpiritBoltsTimer = 20000;
                DrainPowerTimer = 60000;
                SiphonSoulTimer = 100000;
                PlayerAbilityTimer = 99999;
                CheckAddStateTimer = 5000;
                ResetTimer = 5000;

                SpawnAdds();

                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 46916);
                me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (instance)
                    instance->SetData(DATA_HEXLORD_EVENT, IN_PROGRESS);

                DoZoneInCombat();
				DoSendQuantumText(SAY_AGGRO, me);

                for (uint8 i = 0; i < 4; ++i)
                {
                    Unit* Temp = Unit::GetUnit(*me, AddGUID[i]);
                    if (Temp && Temp->IsAlive())
                        CAST_CRE(Temp)->AI()->AttackStart(me->GetVictim());
                    else
                    {
                        EnterEvadeMode();
                        break;
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/)
            {
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*victim*/)
            {
                if (instance)
                    instance->SetData(DATA_HEXLORD_EVENT, DONE);

				DoSendQuantumText(SAY_DEATH, me);

                for (uint8 i = 0; i < 4; ++i)
                {
                    Unit* Temp = Unit::GetUnit(*me, AddGUID[i]);
                    if (Temp && Temp->IsAlive())
                        Temp->DealDamage(Temp, Temp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }

            void SelectAddEntry()
            {
                std::vector<uint32> AddList;

                for (uint8 i = 0; i < 8; ++i)
                    AddList.push_back(AddEntryList[i]);

                while (AddList.size() > 4)
                    AddList.erase(AddList.begin()+rand()%AddList.size());

                uint8 i = 0;
                for (std::vector<uint32>::const_iterator itr = AddList.begin(); itr != AddList.end(); ++itr, ++i)
                    AddEntry[i] = *itr;
            }

            void SpawnAdds()
            {
                for (uint8 i = 0; i < 4; ++i)
                {
                    Creature* creature = (Unit::GetCreature(*me, AddGUID[i]));
                    if (!creature || !creature->IsAlive())
                    {
                if (creature) creature->DisappearAndDie();
                        creature = me->SummonCreature(AddEntry[i], Pos_X[i], POS_Y, POS_Z, ORIENT, TEMPSUMMON_DEAD_DESPAWN, 0);
                        if (creature) AddGUID[i] = creature->GetGUID();
                    }
                    else
                    {
                        creature->AI()->EnterEvadeMode();
                        creature->SetPosition(Pos_X[i], POS_Y, POS_Z, ORIENT);
                        creature->StopMoving();
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (ResetTimer <= diff)
                {
                    if (me->IsWithinDist3d(119.223f, 1035.45f, 29.4481f, 10))
                    {
                        EnterEvadeMode();
                        return;
                    }
                    ResetTimer = 5000;
                }
				else ResetTimer -= diff;

                if (CheckAddStateTimer <= diff)
                {
                    for (uint8 i = 0; i < 4; ++i)
                        if (Creature* temp = Unit::GetCreature(*me, AddGUID[i]))
                            if (temp->IsAlive() && !temp->GetVictim())
                                temp->AI()->AttackStart(me->GetVictim());

                    CheckAddStateTimer = 5000;
                }
				else CheckAddStateTimer -= diff;

                if (DrainPowerTimer <= diff)
                {
                    DoCast(me, SPELL_DRAIN_POWER, true);
					DoSendQuantumText(SAY_DRAIN, me);
                    DrainPowerTimer = urand(40000, 55000);    // must cast in 60 sec, or buff/debuff will disappear
                }
				else DrainPowerTimer -= diff;

                if (SpiritBoltsTimer <= diff)
                {
                    if (DrainPowerTimer < 12000)    // channel 10 sec
                        SpiritBoltsTimer = 13000;   // cast drain power first
                    else
                    {
                        DoCast(me, SPELL_SPIRIT_BOLTS, false);
						DoSendQuantumText(SAY_BOLTS, me);
                        SpiritBoltsTimer = 40000;
                        SiphonSoulTimer = 10000;    // ready to drain
                        PlayerAbilityTimer = 99999;
                    }
                }
				else SpiritBoltsTimer -= diff;

                if (SiphonSoulTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 70, true);
                    Unit* trigger = DoSpawnCreature(NPC_FIRE_BOMB, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
                    if (!target || !trigger)
                    {
                        EnterEvadeMode();
                        return;
                    }
                    else
                    {
                        trigger->SetDisplayId(MODEL_ID_INVISIBLE);
                        trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        trigger->CastSpell(target, SPELL_SIPHON_SOUL, true);
						DoSendQuantumText(SAY_SOUL, me);
                        trigger->GetMotionMaster()->MoveChase(me);

                        //DoCast(target, SPELL_SIPHON_SOUL, true);
                        //me->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, target->GetGUID());
                        //me->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SIPHON_SOUL);

                        PlayerGUID = target->GetGUID();
                        PlayerAbilityTimer = urand(8000, 10000);
                        PlayerClass = target->GetCurrentClass() - 1;

                        if (PlayerClass == CLASS_DRUID-1)
                            PlayerClass = CLASS_DRUID;
                        else if (PlayerClass == CLASS_PRIEST-1 && target->HasSpell(15473))
                            PlayerClass = CLASS_PRIEST; // shadow priest

                        SiphonSoulTimer = 99999;   // buff lasts 30 sec
                    }
                }
				else SiphonSoulTimer -= diff;

                if (PlayerAbilityTimer <= diff)
                {
                    //Unit* target = Unit::GetUnit(*me, PlayerGUID);
                    //if (target && target->IsAlive())
                    //{
                        UseAbility();
                        PlayerAbilityTimer = urand(8000, 10000);
                    //}
                }
				else PlayerAbilityTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void UseAbility()
            {
                uint8 random = urand(0, 2);
                Unit* target = NULL;
                switch (PlayerAbility[PlayerClass][random].target)
                {
                    case ABILITY_TARGET_SELF:
                        target = me;
                        break;
                    case ABILITY_TARGET_VICTIM:
                        target = me->GetVictim();
                        break;
                    case ABILITY_TARGET_ENEMY:
                    default:
                        target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                        break;
                    case ABILITY_TARGET_HEAL:
                        target = SelectTargetHpFriendly(50, 0);
                        break;
                    case ABILITY_TARGET_BUFF:
                        {
                            std::list<Creature*> templist = DoFindFriendlyMissingBuff(50, PlayerAbility[PlayerClass][random].spell);
                            if (!templist.empty())
                                target = *(templist.begin());
                        }
                        break;
                }
                if (target)
                    DoCast(target, PlayerAbility[PlayerClass][random].spell, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_hex_lord_malacrassAI(creature);
        }
};

#define SPELL_BLOODLUST       43578
#define SPELL_CLEAVE          15496

class boss_thurg : public CreatureScript
{
    public:
        boss_thurg() : CreatureScript("boss_thurg"){}

        struct boss_thurgAI : public boss_hexlord_addAI
        {

            boss_thurgAI(Creature* creature) : boss_hexlord_addAI(creature) {}

            uint32 BloodlustTimer;
            uint32 CleaveTimer;

            void Reset()
            {
                BloodlustTimer = 15000;
                CleaveTimer = 10000;

                boss_hexlord_addAI::Reset();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (BloodlustTimer <= diff)
                {
                    std::list<Creature*> templist = DoFindFriendlyMissingBuff(50, SPELL_BLOODLUST);
                    if (!templist.empty())
                    {
                        if (Unit* target = *(templist.begin()))
                            DoCast(target, SPELL_BLOODLUST, false);
                    }
                    BloodlustTimer = 12000;
                }
				else BloodlustTimer -= diff;

                if (CleaveTimer <= diff)
                {
                    DoCastVictim(SPELL_CLEAVE, false);
                    CleaveTimer = 12000; //3 sec cast
                }
				else CleaveTimer -= diff;

                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_thurgAI(creature);
        }
};

#define SPELL_FLASH_HEAL     43575
#define SPELL_DISPEL_MAGIC   43577

class boss_alyson_antille : public CreatureScript
{
    public:
        boss_alyson_antille() : CreatureScript("boss_alyson_antille"){}

        struct boss_alyson_antilleAI : public boss_hexlord_addAI
        {
            //Holy Priest
            boss_alyson_antilleAI(Creature* creature) : boss_hexlord_addAI(creature) {}

            uint32 FlashHealTimer;
            uint32 DispelMagicTimer;

            void Reset()
            {
                FlashHealTimer = 2500;
                DispelMagicTimer = 10000;

                //AcquireGUID();

                boss_hexlord_addAI::Reset();
            }

            void AttackStart(Unit* who)
            {
                if (!who)
                    return;

                if (who->IsTargetableForAttack())
                {
                    if (me->Attack(who, false))
                    {
                        me->GetMotionMaster()->MoveChase(who, 20);
                        me->AddThreat(who, 0.0f);
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (FlashHealTimer <= diff)
                {
                    Unit* target = SelectTargetHpFriendly(99, 30000);
                    if (target)
                    {
                        if (target->IsWithinDistInMap(me, 50))
                            DoCast(target, SPELL_FLASH_HEAL, false);
                        else
                        {
                            // bugged
                            //me->GetMotionMaster()->Clear();
                            //me->GetMotionMaster()->MoveChase(target, 20);
                        }
                    }
                    else
                    {
                        if (urand(0, 1))
                            target = SelectTargetHpFriendly(50, 0);
                        else
                            target = SelectTarget(TARGET_RANDOM, 0);
                        if (target)
                            DoCast(target, SPELL_DISPEL_MAGIC, false);
                    }
                    FlashHealTimer = 2500;
                }
				else FlashHealTimer -= diff;

                /*if (DispelMagicTimer <= diff)
                {
                if (urand(0, 1))
                {
                    Unit* target = SelectTarget();

                    DoCast(target, SPELL_DISPEL_MAGIC, false);
                }
                else
                    me->CastSpell(SelectTarget(TARGET_RANDOM, 0), SPELL_DISPEL_MAGIC, false);

                DispelMagicTimer = 12000;
                } else DispelMagicTimer -= diff;*/

                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_alyson_antilleAI(creature);
        }
};

#define SPELL_FIREBOLT        43584

class boss_gazakroth : public CreatureScript
{
public:
	boss_gazakroth() : CreatureScript("boss_gazakroth") { }

	struct boss_gazakrothAI : public boss_hexlord_addAI
	{
		boss_gazakrothAI(Creature* creature) : boss_hexlord_addAI(creature)
		{
			Initialize();
		}

		uint32 FireboltTimer;

		void Initialize()
		{
			FireboltTimer = 2 * IN_MILLISECONDS;
		}

		void Reset()
		{
			Initialize();
			boss_hexlord_addAI::Reset();
		}

		void AttackStart(Unit* who)
		{
			AttackStartCaster(who, 20.0f);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (FireboltTimer <= diff)
			{
				DoCastVictim(SPELL_FIREBOLT, false);
				FireboltTimer = 1*IN_MILLISECONDS;
			}
			else FireboltTimer -= diff;

			boss_hexlord_addAI::UpdateAI(diff);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_gazakrothAI(creature);
	}
};

#define SPELL_FLAME_BREATH    43582
#define SPELL_THUNDERCLAP     43583

class boss_lord_raadan : public CreatureScript
{
    public:
        boss_lord_raadan() : CreatureScript("boss_lord_raadan"){}

        struct boss_lord_raadanAI : public boss_hexlord_addAI
        {
            boss_lord_raadanAI(Creature* creature) : boss_hexlord_addAI(creature)  {}

            uint32 FlameBreathTimer;
            uint32 ThunderClapTimer;

            void Reset()
            {
                FlameBreathTimer = 8000;
                ThunderClapTimer = 13000;

                boss_hexlord_addAI::Reset();

            }
            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (ThunderClapTimer <= diff)
                {
                    DoCastVictim(SPELL_THUNDERCLAP, false);
                    ThunderClapTimer = 12000;
                }
				else ThunderClapTimer -= diff;

                if (FlameBreathTimer <= diff)
                {
                    DoCastVictim(SPELL_FLAME_BREATH, false);
                    FlameBreathTimer = 12000;
                }
				else FlameBreathTimer -= diff;

                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_lord_raadanAI(creature);
        }
};

#define SPELL_PSYCHIC_WAIL   43590

class boss_darkheart : public CreatureScript
{
    public:
        boss_darkheart() : CreatureScript("boss_darkheart"){}

        struct boss_darkheartAI : public boss_hexlord_addAI
        {
            boss_darkheartAI(Creature* creature) : boss_hexlord_addAI(creature)  {}

            uint32 PsychicWailTimer;

            void Reset()
            {
                PsychicWailTimer = 8000;
                boss_hexlord_addAI::Reset();
            }
            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (PsychicWailTimer <= diff)
                {
                    DoCastVictim(SPELL_PSYCHIC_WAIL, false);
                    PsychicWailTimer = 12000;
                }
				else PsychicWailTimer -= diff;

                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_darkheartAI(creature);
        }
};

#define SPELL_VENOM_SPIT    43579

class boss_slither : public CreatureScript
{
    public:
        boss_slither() : CreatureScript("boss_slither"){}

        struct boss_slitherAI : public boss_hexlord_addAI
        {
            boss_slitherAI(Creature* creature) : boss_hexlord_addAI(creature) {}

            uint32 VenomspiteTimer;

            void Reset()
            {
                VenomspiteTimer = 5000;
                boss_hexlord_addAI::Reset();
            }

            void AttackStart(Unit* who)
            {
                if (!who)
                    return;

                if (who->IsTargetableForAttack())
                {
                    if (me->Attack(who, false))
                    {
                        me->GetMotionMaster()->MoveChase(who, 20);
                        me->AddThreat(who, 0.0f);
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (VenomspiteTimer <= diff)
                {
                    if (Unit* victim = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        DoCast(victim, SPELL_VENOM_SPIT, false);
                    VenomspiteTimer = 2500;
                }
				else VenomspiteTimer -= diff;

                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_slitherAI(creature);
        }
};

//Fenstalker
#define SPELL_VOLATILE_INFECTION 43586

class boss_fenstalker : public CreatureScript
{
    public:
        boss_fenstalker() : CreatureScript("boss_fenstalker"){}

        struct boss_fenstalkerAI : public boss_hexlord_addAI
        {
            boss_fenstalkerAI(Creature* creature) : boss_hexlord_addAI(creature) {}

            uint32 VolatileinfTimer;

            void Reset()
            {
                VolatileinfTimer = 15000;
                boss_hexlord_addAI::Reset();

            }
            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (VolatileinfTimer <= diff)
                {
                    // core bug
                    me->GetVictim()->CastSpell(me->GetVictim(), SPELL_VOLATILE_INFECTION, false);
                    VolatileinfTimer = 12000;
                } else VolatileinfTimer -= diff;

                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_fenstalkerAI(creature);
        }
};

#define SPELL_COLD_STARE      43593
#define SPELL_MIGHTY_BLOW     43592

class boss_koragg : public CreatureScript
{
    public:
        boss_koragg() : CreatureScript("boss_koragg"){}

        struct boss_koraggAI : public boss_hexlord_addAI
        {
            boss_koraggAI(Creature* creature) : boss_hexlord_addAI(creature) {}

            uint32 ColdStareTimer;
            uint32 MightyBlowTimer;

            void Reset()
            {
                ColdStareTimer = 15000;
                MightyBlowTimer = 10000;
                boss_hexlord_addAI::Reset();

            }
            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (MightyBlowTimer <= diff)
                {
                    DoCastVictim(SPELL_MIGHTY_BLOW, false);
                    MightyBlowTimer = 12000;
                }
                if (ColdStareTimer <= diff)
                {
                    if (Unit* victim = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        DoCast(victim, SPELL_COLD_STARE, false);
                    ColdStareTimer = 12000;
                }
                boss_hexlord_addAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_koraggAI(creature);
        }
};

class spell_hexlord_unstable_affliction : public SpellScriptLoader
{
    public:
        spell_hexlord_unstable_affliction() : SpellScriptLoader("spell_hexlord_unstable_affliction") { }

        class spell_hexlord_unstable_affliction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hexlord_unstable_affliction_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WL_UNSTABLE_AFFL_DISPEL))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(dispelInfo->GetDispeller(), SPELL_WL_UNSTABLE_AFFL_DISPEL, true, NULL, GetEffect(EFFECT_0));
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_hexlord_unstable_affliction_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hexlord_unstable_affliction_AuraScript();
        }
};

void AddSC_boss_hex_lord_malacrass()
{
    new boss_hexlord_malacrass();
    new boss_thurg();
	new boss_gazakroth();
    new boss_lord_raadan();
    new boss_darkheart();
    new boss_slither();
    new boss_fenstalker();
    new boss_koragg();
    new boss_alyson_antille();
	new spell_hexlord_unstable_affliction();
}