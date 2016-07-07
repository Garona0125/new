/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "gruuls_lair.h"

enum Says
{
	SAY_AGGRO          = -1565000,
	SAY_ENRAGE         = -1565001,
	SAY_OGRE_DEATH_1   = -1565002,
	SAY_OGRE_DEATH_2   = -1565003,
	SAY_OGRE_DEATH_3   = -1565004,
	SAY_OGRE_DEATH_4   = -1565005,
	SAY_SLAY_1         = -1565006,
	SAY_SLAY_2         = -1565007,
	SAY_SLAY_3         = -1565008,
	SAY_DEATH          = -1565009,
};

enum Spells
{
	// High King Maulgar
	SPELL_ARCING_SMASH       = 39144,
	SPELL_MIGHTY_BLOW        = 33230,
	SPELL_WHIRLWIND          = 33238,
	SPELL_BERSERKER_C        = 26561,
	SPELL_ROAR               = 16508,
	SPELL_FLURRY             = 33232,
	SPELL_DUAL_WIELD         = 29651,
	// Olm the Summoner
	SPELL_DARK_DECAY         = 33129,
	SPELL_DEATH_COIL         = 33130,
	SPELL_SUMMON_WFH         = 33131,
	//Kiggler the Craed
	SPELL_GREATER_POLYMORPH  = 33173,
	SPELL_LIGHTNING_BOLT     = 36152,
	SPELL_ARCANE_SHOCK       = 33175,
	SPELL_ARCANE_EXPLOSION   = 33237,
	//Blindeye the Seer
	SPELL_GREATER_PW_SHIELD  = 33147,
	SPELL_HEAL               = 33144,
	SPELL_PRAYER_OH          = 33152,
	//Krosh Firehand
	SPELL_GREATER_FIREBALL   = 41484, // 33051 bugged
	SPELL_SPELL_SHIELD       = 33054,
	SPELL_BLAST_WAVE         = 33061,
};

class boss_high_king_maulgar : public CreatureScript
{
public:
    boss_high_king_maulgar() : CreatureScript("boss_high_king_maulgar") { }

    struct boss_high_king_maulgarAI : public QuantumBasicAI
    {
        boss_high_king_maulgarAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 ArcingSmashTimer;
        uint32 MightyBlowTimer;
        uint32 WhirlwindTimer;
        uint32 ChargingTimer;
        uint32 RoarTimer;

        bool Phase2;

        void Reset()
        {
            ArcingSmashTimer = 10000;
            MightyBlowTimer = 40000;
            WhirlwindTimer = 30000;
            ChargingTimer = 0;
            RoarTimer = 0;

            DoCast(me, SPELL_DUAL_WIELD, false);

            Phase2 = false;

            if (instance)
                instance->SetData(DATA_MAULGAR_EVENT, NOT_STARTED);
        }

        void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			DoSendQuantumText(SAY_AGGRO, me);
            instance->SetData(DATA_MAULGAR_EVENT, IN_PROGRESS);
            DoZoneInCombat();
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
			instance->SetData(DATA_MAULGAR_EVENT, DONE);
		}

		void EnterEvadeMode()
		{
			if (Creature* seer = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_BLIND_EYE_THE_SEER) : 0))
			{
				seer->Respawn();
				seer->GetMotionMaster()->MoveTargetedHome();
			}

			if (Creature* kiggler = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_KIGGLER_THE_CRAZED) : 0))
			{
				kiggler->Respawn();
				kiggler->GetMotionMaster()->MoveTargetedHome();
			}

			if (Creature* krosh = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_KROSH_FIRE_HAND) : 0))
			{
				krosh->Respawn();
				krosh->GetMotionMaster()->MoveTargetedHome();
			}

			if (Creature* olm = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_OLM_THE_SUMMONER) : 0))
			{
				olm->Respawn();
				olm->GetMotionMaster()->MoveTargetedHome();
			}

			QuantumBasicAI::EnterEvadeMode();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArcingSmashTimer <= diff)
            {
                DoCastVictim(SPELL_ARCING_SMASH);
                ArcingSmashTimer = 10000;
            }
			else ArcingSmashTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastVictim(SPELL_WHIRLWIND);
				WhirlwindTimer = 55000;
			}
			else WhirlwindTimer -= diff;

            if (MightyBlowTimer <= diff)
            {
                DoCastVictim(SPELL_MIGHTY_BLOW);
                MightyBlowTimer = 30000+rand()%10000;
            }
			else MightyBlowTimer -= diff;

            if (!Phase2 && HealthBelowPct(50))
            {
                Phase2 = true;
                DoSendQuantumText(SAY_ENRAGE, me);

                DoCast(me, SPELL_DUAL_WIELD, true);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);
            }

            if (Phase2)
            {
                if (ChargingTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        AttackStart(target);
                        DoCast(target, SPELL_BERSERKER_C);
						ChargingTimer = 20000;
					}
                }
				else ChargingTimer -= diff;

                if (RoarTimer <= diff)
                {
                    DoCast(me, SPELL_ROAR);
                    RoarTimer = 40000+(rand()%10000);
                }
				else RoarTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_king_maulgarAI(creature);
    }
};

class boss_olm_the_summoner : public CreatureScript
{
public:
    boss_olm_the_summoner() : CreatureScript("boss_olm_the_summoner") { }

    struct boss_olm_the_summonerAI : public QuantumBasicAI
    {
        boss_olm_the_summonerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 DarkDecayTimer;
        uint32 SummonTimer;
		uint32 DeathCoilTimer;

        InstanceScript* instance;
		SummonList Summons;

        void Reset()
        {
            DarkDecayTimer = 10000;
            SummonTimer = 15000;
            DeathCoilTimer = 20000;

			Summons.DespawnAll();
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
		{
			if (Creature* maulgar = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MAULGAR) : 0))
			{
				if (maulgar->IsAlive())
					DoSendQuantumText(SAY_OGRE_DEATH_1, maulgar);
			}

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_WILD_FEL_STALKER)
			{
				Summons.Summon(summon);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DarkDecayTimer <= diff)
            {
                DoCastVictim(SPELL_DARK_DECAY);
                DarkDecayTimer = 20000;
            }
			else DarkDecayTimer -= diff;

            if (SummonTimer <= diff)
            {
                DoCast(me, SPELL_SUMMON_WFH, true);
                SummonTimer = 30000;
            }
			else SummonTimer -= diff;

            if (DeathCoilTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_DEATH_COIL);
					DeathCoilTimer = 20000;
				}
            }
			else DeathCoilTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_olm_the_summonerAI(creature);
    }
};

class boss_kiggler_the_crazed : public CreatureScript
{
public:
    boss_kiggler_the_crazed() : CreatureScript("boss_kiggler_the_crazed") { }

    struct boss_kiggler_the_crazedAI : public QuantumBasicAI
    {
        boss_kiggler_the_crazedAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 GreaterPolymorphTimer;
        uint32 LightningBoltTimer;
        uint32 ArcaneShockTimer;
        uint32 ArcaneExplosionTimer;

        InstanceScript* instance;

        void Reset()
        {
            GreaterPolymorphTimer = 5000;
            LightningBoltTimer = 10000;
            ArcaneShockTimer = 20000;
            ArcaneExplosionTimer = 30000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
		{
			if (Creature* maulgar = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MAULGAR) : 0))
			{
				if (maulgar->IsAlive())
					DoSendQuantumText(SAY_OGRE_DEATH_2, maulgar);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (GreaterPolymorphTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_GREATER_POLYMORPH);
					GreaterPolymorphTimer = urand(15000, 20000);
				}
            }
			else GreaterPolymorphTimer -= diff;

            if (LightningBoltTimer <= diff)
            {
                DoCastVictim(SPELL_LIGHTNING_BOLT);
                LightningBoltTimer = 15000;
            }
			else LightningBoltTimer -= diff;

            if (ArcaneShockTimer <= diff)
            {
                DoCastVictim(SPELL_ARCANE_SHOCK);
                ArcaneShockTimer = 20000;
			}
			else ArcaneShockTimer -= diff;

            if (ArcaneExplosionTimer <= diff)
            {
                DoCastVictim(SPELL_ARCANE_EXPLOSION);
                ArcaneExplosionTimer = 30000;
            }
			else ArcaneExplosionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kiggler_the_crazedAI(creature);
    }
};

class boss_blindeye_the_seer : public CreatureScript
{
public:
    boss_blindeye_the_seer() : CreatureScript("boss_blindeye_the_seer") { }

    struct boss_blindeye_the_seerAI : public QuantumBasicAI
    {
        boss_blindeye_the_seerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 GreaterPowerWordShieldTimer;
        uint32 HealTimer;
        uint32 PrayerofHealingTimer;

        InstanceScript* instance;

        void Reset()
        {
            GreaterPowerWordShieldTimer = 5000;
            HealTimer = urand(25000, 40000);
            PrayerofHealingTimer = urand(45000, 55000);
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
		{
			if (Creature* maulgar = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MAULGAR) : 0))
			{
				if (maulgar->IsAlive())
					DoSendQuantumText(SAY_OGRE_DEATH_3, maulgar);
			}
		}

         void UpdateAI(const uint32 diff)
		 {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (GreaterPowerWordShieldTimer <= diff)
            {
                DoCast(me, SPELL_GREATER_PW_SHIELD);
                GreaterPowerWordShieldTimer = 40000;
            }
			else GreaterPowerWordShieldTimer -= diff;

            if (HealTimer <= diff)
            {
                DoCast(me, SPELL_HEAL);
                HealTimer = urand(15000, 40000);
            }
			else HealTimer -= diff;

            if (PrayerofHealingTimer <= diff)
            {
                DoCast(me, SPELL_PRAYER_OH);
                PrayerofHealingTimer = urand(35000, 50000);
            }
			else PrayerofHealingTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_blindeye_the_seerAI(creature);
    }
};

class boss_krosh_firehand : public CreatureScript
{
public:
    boss_krosh_firehand() : CreatureScript("boss_krosh_firehand") { }

    struct boss_krosh_firehandAI : public QuantumBasicAI
    {
        boss_krosh_firehandAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 GreaterFireballTimer;
        uint32 SpellShieldTimer;
        uint32 BlastWaveTimer;

        InstanceScript* instance;

        void Reset()
		{
			GreaterFireballTimer = 2000;
			SpellShieldTimer = 5000;
            BlastWaveTimer = 8000;
		}

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
		{
			if (Creature* maulgar = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MAULGAR) : 0))
			{
				if (maulgar->IsAlive())
					DoSendQuantumText(SAY_OGRE_DEATH_4, maulgar);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (GreaterFireballTimer <= diff)
            {
				DoCastVictim(SPELL_GREATER_FIREBALL);
				GreaterFireballTimer = urand(4000, 6000);
            }
			else GreaterFireballTimer -= diff;

            if (SpellShieldTimer <= diff)
            {
                DoCast(me, SPELL_SPELL_SHIELD);
                SpellShieldTimer = urand(8000, 10000);
            }
			else SpellShieldTimer -= diff;

            if (BlastWaveTimer <= diff)
            {
				DoCastAOE(SPELL_BLAST_WAVE);
				BlastWaveTimer = urand(12000, 14000);
            }
			else BlastWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_krosh_firehandAI(creature);
    }
};

void AddSC_boss_high_king_maulgar()
{
    new boss_high_king_maulgar();
    new boss_kiggler_the_crazed();
    new boss_blindeye_the_seer();
    new boss_olm_the_summoner();
    new boss_krosh_firehand();
}