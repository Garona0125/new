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
#include "trial_of_the_champion.h"

enum Texts
{
    SAY_EADRIC_AGGRO            = -1650045,
    SAY_EADRIC_HAMMER           = -1650046,
	SAY_EADRIC_SLAY_1           = -1650047,
	SAY_EADRIC_SLAY_2           = -1650048,
	SAY_EADRIC_DEATH            = -1650049,
    SAY_EADRIC_EMOTE_RADIANCE   = -1650050,
    SAY_EADRIC_HAMMER_WARNING   = -1650051,
	SAY_PALETRESS_AGGRO         = -1650052,
	SAY_PALETRESS_SUMMON_MEMORY = -1650053,
	SAY_PALETRESS_SLAY_1        = -1650054,
	SAY_PALETRESS_SLAY_2        = -1650055,
	SAY_PALETRESS_MEMORY_DIES   = -1650056,
	SAY_PALETRESS_DEATH         = -1650057,
	SAY_PALETRESS_NIGHTMARE     = -1650058,
};

enum Spells
{
    // Argent Soldiers
    SPELL_KNEEL                 = 68442,
    SPELL_DIVINE_SHIELD         = 67251,
    SPELL_FINAL_MEDITATION      = 67255,
    SPELL_FLURRY_OF_BLOWS       = 67233,
    SPELL_PUMMEL                = 67235,
    SPELL_BLAZING_LIGHT         = 67247,
    SPELL_CLEAVE                = 15284,
    SPELL_UNBALANCING_STRIKE    = 67237,
    SPELL_HOLY_SMITE_5N         = 36176,
    SPELL_HOLY_SMITE_5H         = 67289,
    SPELL_SHADOW_WORD_PAIN_5N   = 34941,
    SPELL_SHADOW_WORD_PAIN_5H   = 34942,
    SPELL_MIND_CONTROL          = 67229,
    SPELL_FOUNTAIN_OF_LIGHT     = 67194,
    SPELL_EADRIC_ACHIEVEMENT    = 68197,
    SPELL_HAMMER_JUSTICE        = 66863,
    SPELL_HAMMER_RIGHTEOUS      = 66867,
    SPELL_RADIANCE              = 66935,
    SPELL_VENGEANCE             = 66865,
    SPELL_VENGANCE_AURA         = 66889,
    SPELL_HAMMER_RIGHTEOUS_P    = 66904,
    SPELL_HAMMER_RIGHTEOUS_RET  = 66905,
    SPELL_HAMMER_JUSTICE_STUN   = 66940,
    SPELL_SMITE_5N              = 66536,
    SPELL_SMITE_5H              = 67674,
    SPELL_HOLY_FIRE_5N          = 66538,
    SPELL_HOLY_FIRE_5H          = 67676,
    SPELL_RENEW_5N              = 66537,
    SPELL_RENEW_5H              = 67675,
    SPELL_HOLY_NOVA             = 66546,
    SPELL_SHIELD                = 66515,
    SPELL_CONFESS               = 66680,
    SPELL_SUMMON_MEMORY         = 66545,
    SPELL_SHADOW_FORM           = 34429, // THIS IS NOT CORRECT!
    SPELL_OLD_WOUNDS_5N         = 66620,
    SPELL_OLD_WOUNDS_5H         = 67679,
    SPELL_SHADOWS_PAST_5N       = 66619,
    SPELL_SHADOWS_PAST_5H       = 67678,
    SPELL_WAKING_NIGHTMARE_5N   = 66552,
    SPELL_WAKING_NIGHTMARE_5H   = 67677,
	SPELL_LIGHT_RAIN            = 67196,
	SPELL_ARGENT_CHAMPION       = 63501,
	SPELL_DUAL_WIELD            = 674,
};

class boss_eadric_the_pure : public CreatureScript
{
public:
    boss_eadric_the_pure() : CreatureScript("boss_eadric_the_pure") { }

    struct boss_eadric_the_pureAI : public QuantumBasicAI
    {
        boss_eadric_the_pureAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 VenganceTimer;
        uint32 RadianceTimer;
        uint32 HammerJusticeTimer;

        bool Defeated;

        void Reset()
        {
            VenganceTimer = 2000;
            RadianceTimer = 7000;
            HammerJusticeTimer = 10000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ARGENT_CHAMPION);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            Defeated = false;
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo *spell)
        {
            if (IsHeroic())
			{
                if (spell->Id == SPELL_HAMMER_RIGHTEOUS_RET)
				{
                    if (me->GetHealth() <= (uint32)spell->Effects[0].BasePoints)
					{
                        if (InstanceScript* instance = me->GetInstanceScript())
                            instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_EADRIC_ACHIEVEMENT);
					}
				}
			}
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_EADRIC_AGGRO, me);

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_ARGENT_CHALLENGE_E) != IN_PROGRESS)
                    instance->SetData(BOSS_ARGENT_CHALLENGE_E, IN_PROGRESS);
			}
        }

        void JustReachedHome()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_ARGENT_CHALLENGE_E) == IN_PROGRESS)
                    instance->SetData(BOSS_ARGENT_CHALLENGE_E, FAIL);
			}
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_EADRIC_SLAY_1, SAY_EADRIC_SLAY_2), me);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (Defeated)
            {
                damage = 0;
                return;
            }

            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoSendQuantumText(SAY_EADRIC_DEATH, me);
                BindPlayersToInstance(me);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
                me->DespawnAfterAction(0);
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Defeated || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (VenganceTimer <= diff)
            {
                DoCast(me, SPELL_VENGEANCE);
                VenganceTimer = 7000;
            }
			else VenganceTimer -= diff;

            if (RadianceTimer <= diff)
            {
                DoSendQuantumText(SAY_EADRIC_EMOTE_RADIANCE, me);
                DoCastAOE(SPELL_RADIANCE);
                RadianceTimer = 10000;
            }
			else RadianceTimer -= diff;

			if (HammerJusticeTimer <= diff)
            {
                me->InterruptNonMeleeSpells(true);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 250, true))
                {
					DoSendQuantumText(SAY_EADRIC_HAMMER, me);
					DoCast(target, SPELL_HAMMER_JUSTICE);
					DoSendQuantumText(SAY_EADRIC_HAMMER_WARNING, me, target);
					DoCast(target, SPELL_HAMMER_RIGHTEOUS);
					HammerJusticeTimer = 25000;
				}
            }
			else HammerJusticeTimer -= diff;

            if (me->IsAttackReady() && me->IsWithinMeleeRange(me->GetVictim()))
            {
                me->AttackerStateUpdate(me->GetVictim());
                me->ResetAttackTimer();

                if (Aura* vengance = me->GetAura(SPELL_VENGANCE_AURA))
				{
                    if (vengance->GetDuration() < 4900)
                        me->RemoveAurasDueToSpell(SPELL_VENGANCE_AURA);
				}
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_eadric_the_pureAI(creature);
    }
};

class boss_argent_confessor_paletress : public CreatureScript
{
public:
    boss_argent_confessor_paletress() : CreatureScript("boss_argent_confessor_paletress") { }

    struct boss_argent_confessor_paletressAI : public QuantumBasicAI
    {
        boss_argent_confessor_paletressAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 memoryGUID;

        bool Shielded;
        bool Defeated;

        uint32 HolyFireTimer;
        uint32 HolySmiteTimer;
        uint32 RenewTimer;

        void Reset()
        {
            me->RemoveAllAuras();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            HolyFireTimer = urand(9000, 12000);
            HolySmiteTimer = urand(5000, 7000);
            RenewTimer = urand(2000, 5000);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ARGENT_CHAMPION);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if (Creature* memory = Unit::GetCreature(*me, memoryGUID))
                memory->DespawnAfterAction(1000);

            memoryGUID = 0;

            Shielded = false;
            Defeated = false;
        }

        void SetData(uint32 id, uint32 /*value*/)
        {
            if (id == 1)
            {
                if (Creature* memory = Unit::GetCreature(*me, memoryGUID))
                    memory->DespawnAfterAction();

                DoSendQuantumText(SAY_PALETRESS_MEMORY_DIES, me);
                me->RemoveAura(SPELL_SHIELD);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_PALETRESS_AGGRO, me);

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_ARGENT_CHALLENGE_P) != IN_PROGRESS)
                    instance->SetData(BOSS_ARGENT_CHALLENGE_P, IN_PROGRESS);
			}
        }

        void JustReachedHome()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_ARGENT_CHALLENGE_P) == IN_PROGRESS)
                    instance->SetData(BOSS_ARGENT_CHALLENGE_P, FAIL);
			}
        }

        void KilledUnit(Unit* victim)
		{
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_PALETRESS_SLAY_1, SAY_PALETRESS_SLAY_2), me);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (Defeated)
            {
                damage = 0;
                return;
            }

            if (damage >= me->GetHealth())
            {
                damage = 0;
                Defeated = true;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, me->GetHomePosition());
                me->SetTarget(0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoSendQuantumText(SAY_PALETRESS_DEATH, me);
                BindPlayersToInstance(me);

                if (Creature* memory = Unit::GetCreature(*me, memoryGUID))
                    memory->DespawnAfterAction(1000);
            }

            if (!Shielded && HealthBelowPct(25))
            {
                me->InterruptNonMeleeSpells(true);
                DoCastAOE(SPELL_HOLY_NOVA, false);
                DoCast(me, SPELL_SHIELD);
                DoCastAOE(SPELL_SUMMON_MEMORY, false);
                DoCastAOE(SPELL_CONFESS, false);
                DoSendQuantumText(SAY_PALETRESS_SUMMON_MEMORY, me);

                Shielded = true;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
                me->DespawnAfterAction(0);
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_CHAMPION_DEFEATED)
                return Defeated ? 1 : 0;

            return 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Defeated || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (HolyFireTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 250, true))
					DoCast(target, DUNGEON_MODE(SPELL_HOLY_FIRE_5N, SPELL_HOLY_FIRE_5H));

				if (me->HasAura(SPELL_SHIELD))
					HolyFireTimer = 13000;

				else HolyFireTimer = urand(9000, 12000);
            }
			else HolyFireTimer -= diff;

            if (HolySmiteTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 250, true))
					DoCast(target, DUNGEON_MODE(SPELL_SMITE_5N, SPELL_SMITE_5H));

                if (me->HasAura(SPELL_SHIELD))
                    HolySmiteTimer = 9000;
                else
                    HolySmiteTimer = urand(5000, 7000);
            }
			else HolySmiteTimer -= diff;

            if (me->HasAura(SPELL_SHIELD))
            {
                if (RenewTimer <= diff)
                {
                    me->InterruptNonMeleeSpells(true);
                    uint8 target = urand(0, 1);
                    switch(target)
                    {
                        case 0:
                            DoCast(me, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
                            break;
                        case 1:
                            if (Creature* memory = Unit::GetCreature(*me, memoryGUID))
							{
                                if (memory->IsAlive())
                                    DoCast(memory, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
							}
                            break;
                    }
                    RenewTimer = urand(15000, 17000);
                }
				else RenewTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }

        void JustSummoned(Creature* summon)
        {
            memoryGUID = summon->GetGUID();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_argent_confessor_paletressAI(creature);
    }
};

class npc_memory : public CreatureScript
{
public:
    npc_memory() : CreatureScript("npc_memory") { }

    struct npc_memoryAI : public QuantumBasicAI
    {
        npc_memoryAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 OldWoundsTimer;
        uint32 ShadowPastTimer;
        uint32 WakingNightmareTimer;

        void Reset()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
                instance->SetData(DATA_MEMORY_ENTRY, me->GetEntry());

            OldWoundsTimer = 12000;
            ShadowPastTimer = 5000;
            WakingNightmareTimer = 7000;

            DoCast(SPELL_SHADOW_FORM);

			if (me->GetEntry() == NPC_MEMORY_ALGALON || me->GetEntry() == NPC_MEMORY_ILLIDAN || me->GetEntry() == NPC_MEMORY_MALCHEZAAR || me->GetEntry() == NPC_MEMORY_VAN_CLEEF)
				DoCast(me, SPELL_DUAL_WIELD, true);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* /*killer*/)
        {
            if (me->IsSummon())
			{
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
				{
                    if (summoner->IsAlive())
                        summoner->GetAI()->SetData(1, 0);
				}
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (OldWoundsTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM))
                {
					DoCast(target, DUNGEON_MODE(SPELL_OLD_WOUNDS_5N, SPELL_OLD_WOUNDS_5H));
					OldWoundsTimer = 12000;
				}
            }
			else OldWoundsTimer -= diff;

            if (WakingNightmareTimer <= diff)
            {
                DoSendQuantumText(SAY_PALETRESS_NIGHTMARE, me);
                DoCast(me, DUNGEON_MODE(SPELL_WAKING_NIGHTMARE_5N, SPELL_WAKING_NIGHTMARE_5H));
                WakingNightmareTimer = 15000;
            }
			else WakingNightmareTimer -= diff;

            if (ShadowPastTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM))
                {
					DoCast(target, DUNGEON_MODE(SPELL_SHADOWS_PAST_5N, SPELL_SHADOWS_PAST_5H));
					ShadowPastTimer = 5000;
				}
            }
			else ShadowPastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_memoryAI(creature);
    }
};

class npc_argent_monk : public CreatureScript
{
public:
    npc_argent_monk() : CreatureScript("npc_argent_monk") { }

    struct npc_argent_monkAI : public QuantumBasicAI
    {
        npc_argent_monkAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 TimerFlurryBlows;
        uint32 TimerPummel;

        bool ShieldCasted;
        bool Defeated;

        void Reset()
        {
            TimerFlurryBlows = 2000;
            TimerPummel = 1000;

            ShieldCasted = false;
            Defeated = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ARGENT_CHAMPION);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_UNARMED);
        }

        void JustReachedHome()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(MINION_ENCOUNTER) == IN_PROGRESS)
                    instance->SetData(MINION_ENCOUNTER, FAIL);
			}
        }

        void EnterToBattle(Unit* /*attacker*/)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(MINION_ENCOUNTER) != IN_PROGRESS)
                    instance->SetData(MINION_ENCOUNTER, IN_PROGRESS);
			}
        }

        void DamageTaken(Unit* /*attacker*/, uint32 & damage)
        {
            if (damage >= me->GetHealth() && !ShieldCasted)
            {
                damage = 0;
                DoCast(me, SPELL_DIVINE_SHIELD, true);
                DoCastVictim(SPELL_FINAL_MEDITATION);
                ShieldCasted = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Defeated || me->HasAura(SPELL_DIVINE_SHIELD))
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (TimerFlurryBlows <= diff)
            {
                DoCast(me, SPELL_FLURRY_OF_BLOWS);
                TimerFlurryBlows = urand(7000, 10000);
            }
			else TimerFlurryBlows -= diff;

            if (TimerPummel <= diff)
            {
                DoCastVictim(SPELL_PUMMEL);
                TimerPummel = urand(3000, 6000);
            }
			else TimerPummel -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_monkAI(creature);
    }
};

class npc_argent_lightwielder : public CreatureScript
{
public:
    npc_argent_lightwielder() : CreatureScript("npc_argent_lightwielder") { }

    struct npc_argent_lightwielderAI : public QuantumBasicAI
    {
        npc_argent_lightwielderAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 TimerBlazingLight;
        uint32 TimerCleave;
        uint32 TimerUnbalancingStrike;

        bool Defeated;

        void Reset()
        {
            TimerBlazingLight = 5000;
            TimerCleave = 2000;
            TimerUnbalancingStrike = 3000;

            Defeated = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ARGENT_CHAMPION);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

        void JustReachedHome()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(MINION_ENCOUNTER) == IN_PROGRESS)
                    instance->SetData(MINION_ENCOUNTER, FAIL);
			}
        }

        void EnterToBattle(Unit* /*attacker*/)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(MINION_ENCOUNTER) != IN_PROGRESS)
                    instance->SetData(MINION_ENCOUNTER, IN_PROGRESS);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (TimerBlazingLight <= diff)
            {
                Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW);

                if (!target || target->GetHealth() > me->GetHealth())
                    target = me;

                DoCast(target, SPELL_BLAZING_LIGHT);
                TimerBlazingLight = urand(8000, 10000);
            }
			else TimerBlazingLight -= diff;

            if (TimerCleave <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                TimerCleave = urand(7000, 8500);
            }
			else TimerCleave -= diff;

            if (TimerUnbalancingStrike <= diff)
            {
                DoCastVictim(SPELL_UNBALANCING_STRIKE);
                TimerUnbalancingStrike = urand(3000, 6000);
            }
			else TimerUnbalancingStrike -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_lightwielderAI(creature);
    }
};

class npc_argent_priestess : public CreatureScript
{
public:
    npc_argent_priestess() : CreatureScript("npc_argent_priestess") { }

    struct npc_argent_priestessAI : public QuantumBasicAI
    {
        npc_argent_priestessAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MindControlTimer;
        uint32 ShadowWordPainTimer;
        uint32 FountainOfLightTimer;
        uint32 SmiteTimer;

        bool Defeated;

        void Reset()
        {
            MindControlTimer = 7000;
            ShadowWordPainTimer = 2000;
            FountainOfLightTimer = 9000;
            SmiteTimer = 500;

            Defeated = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ARGENT_CHAMPION);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

        void JustReachedHome()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(MINION_ENCOUNTER) == IN_PROGRESS)
                    instance->SetData(MINION_ENCOUNTER, FAIL);
			}
        }

        void EnterToBattle(Unit* /*attacker*/)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(MINION_ENCOUNTER) != IN_PROGRESS)
                    instance->SetData(MINION_ENCOUNTER, IN_PROGRESS);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Defeated)
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (FountainOfLightTimer <= diff)
            {
                DoCast(SPELL_FOUNTAIN_OF_LIGHT);
                FountainOfLightTimer = urand(40000, 45000);
            }
			else FountainOfLightTimer -= diff;

            if (MindControlTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_MIND_CONTROL);
					MindControlTimer = urand(12000, 16000);
				}
                return;
            }
			else MindControlTimer -= diff;

            if (ShadowWordPainTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    if (!target->HasAura(DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H)))
					{
                        DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H));
						ShadowWordPainTimer = urand(3000, 5000);
					}
				}
            }
			else ShadowWordPainTimer -= diff;

            if (SmiteTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_HOLY_SMITE_5N, SPELL_HOLY_SMITE_5H));
					SmiteTimer = urand(1000, 2000);
				}
            }
			else SmiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_priestessAI(creature);
    }
};

class npc_fountain_of_light : public CreatureScript
{
public:
    npc_fountain_of_light() : CreatureScript("npc_fountain_of_light") { }

    struct npc_fountain_of_lightAI : public QuantumBasicAI
    {
        npc_fountain_of_lightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
        {
			me->SetReactState(REACT_PASSIVE);
			DoCastAOE(SPELL_LIGHT_RAIN);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fountain_of_lightAI(creature);
    }
};

class OrientationCheck : public std::unary_function<Unit*, bool>
{
public:
	explicit OrientationCheck(Unit* caster) : caster(caster) {}

	bool operator() (Unit* unit)
	{
		return !unit->isInFront(caster, 2.5f) || !unit->IsWithinDist(caster, 40.0f);
	}
private:
	Unit* caster;
};

class spell_eadric_radiance : public SpellScriptLoader
{
    public:
        spell_eadric_radiance() : SpellScriptLoader("spell_eadric_radiance") { }

        class spell_eadric_radiance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_eadric_radiance_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (OrientationCheck(GetCaster()));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_eadric_radiance_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_eadric_radiance_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript *GetSpellScript() const
        {
            return new spell_eadric_radiance_SpellScript();
        }
};

class spell_eadric_hammer_of_righteous : public SpellScriptLoader
{
    public:
        spell_eadric_hammer_of_righteous() : SpellScriptLoader("spell_eadric_hammer_of_righteous") {}

        class spell_eadric_hammer_of_righteousSpellScript : public SpellScript
        {
            PrepareSpellScript(spell_eadric_hammer_of_righteousSpellScript)

            void HandleBeforeHit()
            {
                Unit* caster = GetCaster();
                Unit* target = GetExplTargetUnit();

                if (!caster || !target)
                    return;

                Aura* HammerJustice = target->GetAura(SPELL_HAMMER_JUSTICE_STUN);

                if (!HammerJustice)
                {
                    caster->CastSpell(target, SPELL_HAMMER_RIGHTEOUS_P);
                    PreventHitDamage();
                }
            }

            void Register()
            {
                BeforeHit += SpellHitFn(spell_eadric_hammer_of_righteousSpellScript::HandleBeforeHit);
            }
        };

        SpellScript *GetSpellScript() const
        {
            return new spell_eadric_hammer_of_righteousSpellScript();
        }
};

class spell_paletress_shield : public SpellScriptLoader
{
    public:
        spell_paletress_shield() : SpellScriptLoader("spell_paletress_shield") { }

        class spell_paletress_shieldAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_paletress_shieldAuraScript);

            void HandleOnEffectAbsorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* caster = GetCaster();
                Unit* attacker = dmgInfo.GetAttacker();

                if (!caster || !attacker)
                    return;

                absorbAmount = dmgInfo.GetDamage();
                caster->DealDamage(attacker, (uint32)(absorbAmount*0.25f), NULL, dmgInfo.GetDamageType(), dmgInfo.GetSchoolMask());
                caster->SendSpellNonMeleeDamageLog(attacker, GetSpellInfo()->Id, (uint32) (absorbAmount*0.25f), dmgInfo.GetSchoolMask(), 0, 0, true, 0);
            }

            void Register()
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_paletress_shieldAuraScript::HandleOnEffectAbsorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_paletress_shieldAuraScript();
        }
};

void AddSC_boss_argent_challenge()
{
    new boss_eadric_the_pure();
    new boss_argent_confessor_paletress();
    new npc_memory();
    new npc_argent_monk();
    new npc_argent_lightwielder();
    new npc_argent_priestess();
	new npc_fountain_of_light();
	new spell_eadric_radiance();
    new spell_eadric_hammer_of_righteous();
	new spell_paletress_shield();
}