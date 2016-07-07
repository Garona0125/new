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
#include "GameEventMgr.h"
#include "nexus.h"

enum Texts
{
    SAY_AGGRO    = -1576000,
    SAY_SLAY     = -1576001,
    SAY_DEATH    = -1576002,
    SAY_MERGE    = -1576003,
    SAY_SPLIT_1  = -1576004,
    SAY_SPLIT_2  = -1576005,
};

enum Spells
{
    SPELL_ICE_NOVA_5N         = 47772,
    SPELL_ICE_NOVA_5H         = 56935,
    SPELL_FIREBOMB_5N         = 47773,
    SPELL_FIREBOMB_5H         = 56934,
    SPELL_GRAVITY_WELL        = 47756,
    SPELL_TELESTRA_BACK       = 47714,
    SPELL_FIRE_MAGUS_VISUAL   = 47705,
    SPELL_FROST_MAGUS_VISUAL  = 47706,
    SPELL_ARCANE_MAGUS_VISUAL = 47704,
    SPELL_CRITTER             = 47731,
    SPELL_TIMESTOP            = 47736,
	SPELL_WEAR_CHRISTMAS_HAT  = 61400,
};

enum AchievementTimers
{
    ACHIEVEMENT_TIMER = 5*IN_MILLISECONDS
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

const Position CenterOfRoom = {504.80f, 89.07f, -16.12f, 6.27f};

class boss_magus_telestra : public CreatureScript
{
public:
    boss_magus_telestra() : CreatureScript("boss_magus_telestra") { }

    struct boss_magus_telestraAI : public QuantumBasicAI
    {
        boss_magus_telestraAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint64 FireMagusGUID;
        uint64 FrostMagusGUID;
        uint64 ArcaneMagusGUID;

        bool FireMagusDead;
        bool FrostMagusDead;
        bool ArcaneMagusDead;
        bool IsWaitingToAppear;
        bool IsAchievementTimerRunning;

        uint32 IsWaitingToAppearTimer;
        uint32 IceNovaTimer;
        uint32 FireBombTimer;
        uint32 GravityWellTimer;
        uint32 Cooldown;
        uint32 AchievementTimer;

        uint8 Phase;
        uint8 AchievementProgress;

        void Reset()
        {
            Phase = 0;
            //These times are probably wrong
            IceNovaTimer =  7*IN_MILLISECONDS;
            FireBombTimer =  0;
            GravityWellTimer = 15*IN_MILLISECONDS;
            Cooldown = 0;

            FireMagusGUID = 0;
            FrostMagusGUID = 0;
            ArcaneMagusGUID = 0;

            AchievementProgress = 0;
            AchievementTimer = 0;

            IsAchievementTimerRunning = false;
            IsWaitingToAppear = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_TELESTRA_BACK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisible(true);

            if (instance)
                instance->SetData(DATA_MAGUS_TELESTRA_EVENT, NOT_STARTED);

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL) && !me->HasAura(SPELL_WEAR_CHRISTMAS_HAT))
				me->AddAura(SPELL_WEAR_CHRISTMAS_HAT, me);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(DATA_MAGUS_TELESTRA_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
            {
                if (IsHeroic() && AchievementProgress == 2)
					instance->DoCompleteAchievement(ACHIEVEMENT_SPLIT_PERSONALITY);

                instance->SetData(DATA_MAGUS_TELESTRA_EVENT, DONE);
            }
        }

        uint64 SplitPersonality(uint32 entry)
        {
            if (Creature* Summoned = me->SummonCreature(entry, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*IN_MILLISECONDS))
            {
                switch (entry)
                {
                    case NPC_FIRE_MAGUS:
                    {
                        Summoned->CastSpell(Summoned, SPELL_FIRE_MAGUS_VISUAL, false);
                        break;
                    }
                    case NPC_FROST_MAGUS:
                    {
                        Summoned->CastSpell(Summoned, SPELL_FROST_MAGUS_VISUAL, false);
                        break;
                    }
                    case NPC_ARCANE_MAGUS:
                    {
                        Summoned->CastSpell(Summoned, SPELL_ARCANE_MAGUS_VISUAL, false);
                        break;
                    }
                }

                if (Unit*target = SelectTarget(TARGET_RANDOM, 0))
                    Summoned->AI()->AttackStart(target);

                return Summoned->GetGUID();
            }
            return 0;
        }

        void SummonedCreatureDespawn(Creature *summon)
        {
            if (summon->IsAlive())
                return;

            if (summon->GetGUID() == FireMagusGUID)
            {
                FireMagusDead = true;
                IsAchievementTimerRunning = true;
            }
            else if (summon->GetGUID() == FrostMagusGUID)
            {
                FrostMagusDead = true;
                IsAchievementTimerRunning = true;
            }
            else if (summon->GetGUID() == ArcaneMagusGUID)
            {
                ArcaneMagusDead = true;
                IsAchievementTimerRunning = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (IsWaitingToAppear)
            {
                me->StopMoving();
                me->AttackStop();
                if (IsWaitingToAppearTimer <= diff)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    IsWaitingToAppear = false;
                }
				else IsWaitingToAppearTimer -= diff;
                return;
            }

            if ((Phase == 1) ||(Phase == 3))
            {
                if (IsAchievementTimerRunning)
                    AchievementTimer += diff;

                if (FireMagusDead && FrostMagusDead && ArcaneMagusDead)
                {
                    if (AchievementTimer <= ACHIEVEMENT_TIMER)
                        AchievementProgress += 1;
                    me->GetMotionMaster()->Clear();
                    me->SetPosition(CenterOfRoom.GetPositionX(), CenterOfRoom.GetPositionY(), CenterOfRoom.GetPositionZ(), CenterOfRoom.GetOrientation());
                    DoCast(me, SPELL_TELESTRA_BACK);
                    me->SetVisible(true);
                    Phase++;
                    FireMagusGUID = 0;
                    FrostMagusGUID = 0;
                    ArcaneMagusGUID = 0;
                    IsWaitingToAppear = true;
                    IsWaitingToAppearTimer = 4*IN_MILLISECONDS;
                    DoSendQuantumText(SAY_MERGE, me);
                    IsAchievementTimerRunning = false;
                    AchievementTimer = 0;
                }
                else return;
            }

            if ((Phase == 0) && HealthBelowPct(50))
            {
                Phase = 1;
                me->CastStop();
                me->RemoveAllAuras();
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                FireMagusGUID = SplitPersonality(NPC_FIRE_MAGUS);
                FrostMagusGUID = SplitPersonality(NPC_FROST_MAGUS);
                ArcaneMagusGUID = SplitPersonality(NPC_ARCANE_MAGUS);
                FireMagusDead = false;
                FrostMagusDead = false;
                ArcaneMagusDead = false;
                DoSendQuantumText(RAND(SAY_SPLIT_1, SAY_SPLIT_2), me);
                return;
			}

			if (IsHeroic() && (Phase == 2) && HealthBelowPct(15))
            {
                Phase = 3;
                me->CastStop();
                me->RemoveAllAuras();
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                FireMagusGUID = SplitPersonality(NPC_FIRE_MAGUS);
                FrostMagusGUID = SplitPersonality(NPC_FROST_MAGUS);
                ArcaneMagusGUID = SplitPersonality(NPC_ARCANE_MAGUS);
                FireMagusDead = false;
                FrostMagusDead = false;
                ArcaneMagusDead = false;
                DoSendQuantumText(RAND(SAY_SPLIT_1, SAY_SPLIT_2), me);
                return;
            }

            if (Cooldown)
            {
                if (Cooldown <= diff)
                    Cooldown = 0;
                else
                {
                    Cooldown -= diff;
                    return;
                }
            }

            if (IceNovaTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, DUNGEON_MODE(SPELL_ICE_NOVA_5N, SPELL_ICE_NOVA_5H), false);
                    Cooldown = 1500;
                }
                IceNovaTimer = 15*IN_MILLISECONDS;
            }
			else IceNovaTimer -= diff;

            if (GravityWellTimer <= diff)
            {
                if (Unit* target = me->GetVictim())
                {
                    DoCast(target, SPELL_GRAVITY_WELL);
                    Cooldown = 6*IN_MILLISECONDS;
                }
                GravityWellTimer = 15*IN_MILLISECONDS;
            }
			else GravityWellTimer -= diff;

            if (FireBombTimer <= diff)
            {
                if (Unit*target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, DUNGEON_MODE(SPELL_FIREBOMB_5N, SPELL_FIREBOMB_5H), false);
                    Cooldown = 2*IN_MILLISECONDS;
                }
                FireBombTimer = 2*IN_MILLISECONDS;
            }
			else FireBombTimer -=diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_magus_telestraAI(creature);
    }
};

class boss_magus_telestra_arcane : public CreatureScript
{
public:
    boss_magus_telestra_arcane() : CreatureScript("boss_magus_telestra_arcane") { }

    struct boss_magus_telestra_arcaneAI : public QuantumBasicAI
    {
        boss_magus_telestra_arcaneAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CritterTimer;
        uint32 TimeStopTimer;

        void Reset()
        {
			CritterTimer = urand(3000, 6000);
            TimeStopTimer = urand(7000, 10000);

			DoCast(me, SPELL_TELESTRA_BACK);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CritterTimer<=diff)
            {
                DoCast(SPELL_CRITTER);
                CritterTimer = urand(5000, 8000);
            }
			else CritterTimer-=diff;

            if (TimeStopTimer<=diff)
            {
                DoCastAOE(SPELL_TIMESTOP);
                TimeStopTimer = urand(15000, 18000);
            }
			else TimeStopTimer-=diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_magus_telestra_arcaneAI(creature);
    }
};

class spell_nexus_critter_targeting : public SpellScriptLoader
{
    public:
        spell_nexus_critter_targeting() : SpellScriptLoader("spell_nexus_critter_targeting") { }

        class spell_nexus_critter_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nexus_critter_targeting_SpellScript);

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
                sharedUnitList = unitList;
            }

            void FilterTargetsSubsequent(std::list<Unit*>& unitList)
            {
                unitList = sharedUnitList;
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_nexus_critter_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_nexus_critter_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_nexus_critter_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            std::list<Unit*> sharedUnitList;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_nexus_critter_targeting_SpellScript();
        }
};

void AddSC_boss_magus_telestra()
{
    new boss_magus_telestra();
    new boss_magus_telestra_arcane();
    new spell_nexus_critter_targeting();
}