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
#include "sethekk_halls.h"

enum Texts
{
	SAY_SUMMON               = -1556000,
	SAY_AGGRO_1              = -1556001,
	SAY_AGGRO_2              = -1556002,
	SAY_AGGRO_3              = -1556003,
	SAY_SLAY_1               = -1556004,
	SAY_SLAY_2               = -1556005,
	SAY_DEATH                = -1556006,
};

enum Spells
{
	SPELL_FLAME_SHOCK_5N     = 15039,
	SPELL_FLAME_SHOCK_5H     = 15616,
	SPELL_FROST_SHOCK_5N     = 12548,
	SPELL_FROST_SHOCK_5H     = 21401,
	SPELL_SHADOW_SHOCK_5N    = 33620,
	SPELL_SHADOW_SHOCK_5H    = 38136,
	SPELL_ARCANE_SHOCK_5N    = 33534,
	SPELL_ARCANE_SHOCK_5H    = 38135,
	SPELL_CHAIN_LIGHTNING_5N = 15659,
	SPELL_CHAIN_LIGHTNING_5H = 15305,
	SPELL_FLAME_BUFFET_5N    = 33526,
	SPELL_FLAME_BUFFET_5H    = 38141,
	SPELL_ARCANE_BUFFET_5N   = 33527,
	SPELL_ARCANE_BUFFET_5H   = 38138,
	SPELL_FROST_BUFFET_5N    = 33528,
	SPELL_FROST_BUFFET_5H    = 38142,
	SPELL_SHADOW_BUFFET_5N   = 33529,
	SPELL_SHADOW_BUFFET_5H   = 38143,
	SPELL_SUMMON_SYTH_FIRE   = 33537,
	SPELL_SUMMON_SYTH_ARCANE = 33538,
	SPELL_SUMMON_SYTH_FROST  = 33539,
	SPELL_SUMMON_SYTH_SHADOW = 33540,
};

class boss_darkweaver_syth : public CreatureScript
{
public:
    boss_darkweaver_syth() : CreatureScript("boss_darkweaver_syth") { }

    struct boss_darkweaver_sythAI : public QuantumBasicAI
    {
        boss_darkweaver_sythAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 FlameShockTimer;
        uint32 ArcaneShockTimer;
        uint32 FrostShockTimer;
        uint32 ShadowShockTimer;
        uint32 ChainLightningTimer;

        bool Summon90;
        bool Summon55;
        bool Summon15;

        void Reset()
        {
            FlameShockTimer = 1000;
            ArcaneShockTimer = 3000;
            FrostShockTimer = 5000;
            ShadowShockTimer = 7000;
            ChainLightningTimer = 9000;

            Summon90 = false;
            Summon55 = false;
            Summon15 = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustSummoned(Creature *summoned)
        {
            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                summoned->AI()->AttackStart(target);
        }

        void SythSummoning()
        {
            DoSendQuantumText(SAY_SUMMON, me);

            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            DoCast(me, SPELL_SUMMON_SYTH_ARCANE, true);
            DoCast(me, SPELL_SUMMON_SYTH_FIRE, true);
            DoCast(me, SPELL_SUMMON_SYTH_FROST, true);
            DoCast(me, SPELL_SUMMON_SYTH_SHADOW, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HealthBelowPct(90) && !Summon90)
            {
                SythSummoning();
                Summon90 = true;
            }

            if (HealthBelowPct(55) && !Summon55)
            {
                SythSummoning();
                Summon55 = true;
            }

            if (HealthBelowPct(15) && !Summon15)
            {
                SythSummoning();
                Summon15 = true;
            }

            if (FlameShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_FLAME_SHOCK_5N, SPELL_FLAME_SHOCK_5H));
					FlameShockTimer = urand(3000, 4000);
				}
            }
			else FlameShockTimer -= diff;

            if (ArcaneShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_ARCANE_SHOCK_5N, SPELL_ARCANE_SHOCK_5H));
					ArcaneShockTimer = urand(5000, 6000);
				}
            }
			else ArcaneShockTimer -= diff;

            if (FrostShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_FROST_SHOCK_5N, SPELL_FROST_SHOCK_5H));
					FrostShockTimer = urand(7000, 8000);
				}
            }
			else FrostShockTimer -= diff;

            if (ShadowShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SHADOW_SHOCK_5N, SPELL_SHADOW_SHOCK_5H));
					ShadowShockTimer = urand(9000, 10000);
				}
            }
			else ShadowShockTimer -= diff;

            if (ChainLightningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_CHAIN_LIGHTNING_5N, SPELL_CHAIN_LIGHTNING_5H));
					ChainLightningTimer = urand(11000, 12000);
				}
            }
			else ChainLightningTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_darkweaver_sythAI(creature);
    }
};

class npc_syth_fire_elemental : public CreatureScript
{
public:
    npc_syth_fire_elemental() : CreatureScript("npc_syth_fire_elemental") { }

    struct npc_syth_fire_elementalAI : public QuantumBasicAI
    {
        npc_syth_fire_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 FlameBuffetTimer;

        void Reset()
        {
            FlameBuffetTimer = 1000;

			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FlameBuffetTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_FLAME_BUFFET_5N, SPELL_FLAME_BUFFET_5H));
					FlameBuffetTimer = 5000;
				}
            }
			else FlameBuffetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syth_fire_elementalAI(creature);
    }
};

class npc_syth_frost_elemental : public CreatureScript
{
public:
    npc_syth_frost_elemental() : CreatureScript("npc_syth_frost_elemental") { }

    struct npc_syth_frost_elementalAI : public QuantumBasicAI
    {
        npc_syth_frost_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 FrostBuffetTimer;

        void Reset()
        {
            FrostBuffetTimer = 1000;

			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FrostBuffetTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_FROST_BUFFET_5N, SPELL_FROST_BUFFET_5H));
					FrostBuffetTimer = 5000;
				}
            }
			else FrostBuffetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syth_frost_elementalAI(creature);
    }
};

class npc_syth_arcane_elemental : public CreatureScript
{
public:
    npc_syth_arcane_elemental() : CreatureScript("npc_syth_arcane_elemental") { }

    struct npc_syth_arcane_elementalAI : public QuantumBasicAI
    {
        npc_syth_arcane_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 ArcaneBuffetTimer;

        void Reset()
        {
            ArcaneBuffetTimer = 1000;

			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ArcaneBuffetTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BUFFET_5N, SPELL_ARCANE_BUFFET_5H));
					ArcaneBuffetTimer = 5000;
				}
            }
			else ArcaneBuffetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syth_arcane_elementalAI(creature);
    }
};

class npc_syth_shadow_elemental : public CreatureScript
{
public:
    npc_syth_shadow_elemental() : CreatureScript("npc_syth_shadow_elemental") { }

    struct npc_syth_shadow_elementalAI : public QuantumBasicAI
    {
        npc_syth_shadow_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 ShadowShockTimer;
        uint32 ShadowBuffetTimer;

        void Reset()
        {
            ShadowShockTimer = 1000;
            ShadowBuffetTimer = 3000;

			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SHADOW_SHOCK_5N,SPELL_SHADOW_SHOCK_5H));
					ShadowShockTimer = 3000;
				}
            }
			else ShadowShockTimer -= diff;

            if (ShadowBuffetTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BUFFET_5N, SPELL_SHADOW_BUFFET_5H));
					ShadowBuffetTimer = 5000;
				}
            }
			else ShadowBuffetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syth_shadow_elementalAI(creature);
    }
};

void AddSC_boss_darkweaver_syth()
{
    new boss_darkweaver_syth();
    new npc_syth_fire_elemental();
	new npc_syth_frost_elemental();
    new npc_syth_arcane_elemental();
    new npc_syth_shadow_elemental();
}