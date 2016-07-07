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
#include "../../../scripts/EasternKingdoms/SunwellPlateau/sunwell_plateau.h"

enum SunwellTrashSpells
{
	SPELL_ARCANE_EXPLOSION      = 46553,
	SPELL_BLINK                 = 46573,
	SPELL_FROST_NOVA            = 46555,
	SPELL_SUMMON_IMP            = 46544,
	SPELL_SHADOW_BOLT           = 47248,
	SPELL_IGNITE_MANA           = 46543,
	SPELL_HOLY_NOVA             = 46564,
	SPELL_HOLYFORM              = 46565,
	SPELL_RENEW                 = 46563,
	SPELL_SHADOW_WORD_PAIN      = 46560,
	SPELL_MIND_FLAY             = 46562,
	SPELL_FEAR                  = 46561,
	SPELL_MORTAL_STRIKE         = 39171,
	SPELL_CLEAVE                = 46559,
	SPELL_FIRE_BREATH           = 47251,
	SPELL_SLAYING_SHOT          = 46557,
	SPELL_SCATTER_SHOT          = 46681,
	SPELL_SHOOT                 = 47001,
	SPELL_FEL_LIGHTNING         = 46480,
	SPELL_ACTIVATE_PROTECTOR    = 46475,
	SPELL_SINISTER_STRIKE       = 46558,
	SPELL_ASSASSIN_MARK         = 46459,
	SPELL_GREATER_INVISIBILITY  = 16380,
	SPELL_SHADOWSTEP            = 46463,
	SPELL_DRAIN_LIFE            = 46466,
	SPELL_SW_ARCANE_EXPLOSION   = 46457,
	SPELL_DRAIN_MANA            = 46453,
	SPELL_CHILLING_TOUCH        = 46744,
	SPELL_SUMMON_FELGUARD       = 46241,
	SPELL_SUMMON_INFERNAL       = 46229,
	SPELL_CURSE_OF_EXHAUSTION   = 46434,
	SPELL_DOMINATION            = 46427,
	SPELL_FLASH_OF_DARKNESS     = 46442,
	SPELL_MELT_ARMOR            = 46469,
	SPELL_SW_CLEAVE             = 46468,
	SPELL_SHIELD_SLAM           = 46762,
	SPELL_BATTLE_SHOUT          = 46763,
	SPELL_VOLATILE_DISEASE      = 46483,
	SPELL_DISEASE_BUFFET        = 46481,
	SPELL_EARTHQUAKE            = 46932,
	SPELL_EARTHQUAKE_DMG        = 46244,
	SPELL_BEAR_DOWN             = 46239,
	SPELL_CORRUPTING_STRIKE     = 45029,
	SPELL_APG_CLEAVE            = 40504,
	SPELL_DEATH_COIL            = 46283,
	SPELL_INFERNAL_DEFENSE      = 46287,
	SPELL_CATACLYSM_BREATH      = 46292,
	SPELL_ENRAGE                = 47399,
	SPELL_CGZ_DRAIN_LIFE        = 46291,
	SPELL_PETRIFY               = 46288,
	SPELL_TENTACLE_SWEEP        = 46290,
	SPELL_DOOMFIRE_IMMOLATION   = 31722,
	SPELL_CREATE_DOOMFIRE_SHARD = 46306,
	SPELL_POLYMORPH             = 46280,
	SPELL_FLAME_BUFFET          = 46279,
	SPELL_BRING_PAIN            = 46277,
	SPELL_BURN_MANA             = 46267,
	SPELL_WHIRLWIND             = 46270,
	SPELL_BURNING_DESTRUCTION   = 47287,
	SPELL_FELFIRE_FISSION       = 45779,
	SPELL_DUAL_WIELD            = 674,
	SPELL_BLADE_FLURRY          = 46160,
	SPELL_FEL_FIREBALL          = 46101,
	SPELL_SPELL_FURY            = 46102,
	SPELL_SHADOW_PULSE          = 46087,
    SPELL_VOID_BLAST            = 46161,
	SPELL_SHADOW_BOLT_VOLLEY    = 46082,
	SPELL_FIRE_NOVA             = 46551,
	SPELL_VOID_SPAWN            = 46071,
};

class npc_sunblade_arch_mage : public CreatureScript
{
public:
    npc_sunblade_arch_mage() : CreatureScript("npc_sunblade_arch_mage") {}

    struct npc_sunblade_arch_mageAI : public QuantumBasicAI
    {
        npc_sunblade_arch_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaneExplosionTimer;
        uint32 BlinkTimer;
        uint32 FrostNovaTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ArcaneExplosionTimer = 1000;
            FrostNovaTimer = 3000;
			BlinkTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArcaneExplosionTimer <= diff)
            {
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(3000, 4000);
            }
			else ArcaneExplosionTimer -= diff;

			if (FrostNovaTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
					DoCastAOE(SPELL_FROST_NOVA);
					FrostNovaTimer = urand(6000, 7000);
                }
            }
			else FrostNovaTimer -= diff;

            if (BlinkTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_BLINK);
					BlinkTimer = urand(8000, 9000);
				}
            }
			else BlinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_arch_mageAI(creature);
    }
};

class npc_sunblade_cabalist : public CreatureScript
{
public:
    npc_sunblade_cabalist() : CreatureScript("npc_sunblade_cabalist") {}

    struct npc_sunblade_cabalistAI : public QuantumBasicAI
    {
        npc_sunblade_cabalistAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowBoltTimer;
        uint32 IgniteManaTimer;
        uint32 SummonImpTimer;

		SummonList Summons;

        void Reset()
        {
			ShadowBoltTimer = 500;
            IgniteManaTimer = 2000;
			SummonImpTimer = 8000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_IMP, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_FIRE_FIEND)
				Summons.Summon(summon);
		}

		void UpdateAI(uint32 const diff)
		{
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(2000, 3000);
				}
			}
			else ShadowBoltTimer -= diff;

            if (IgniteManaTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_IGNITE_MANA);
						IgniteManaTimer = urand(4000, 5000);
					}
				}
			}
			else IgniteManaTimer -= diff;

			if (SummonImpTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_IMP, true);
				SummonImpTimer = urand(9000, 10000);
			}
			else SummonImpTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_cabalistAI(creature);
    }
};

class npc_sunblade_dawn_priest : public CreatureScript
{
public:
    npc_sunblade_dawn_priest() : CreatureScript("npc_sunblade_dawn_priest") {}

	struct npc_sunblade_dawn_priestAI : public QuantumBasicAI
    {
		npc_sunblade_dawn_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 HolyNovaTimer;
        uint32 RenewTimer;

        void Reset()
        {
			HolyNovaTimer = 2000;
			RenewTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_HOLYFORM);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HolyNovaTimer < diff)
            {
				DoCastAOE(SPELL_HOLY_NOVA);
				HolyNovaTimer = 4000;
            }
			else HolyNovaTimer -= diff;

            if (RenewTimer < diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_RENEW);
					RenewTimer = 6000;
                }
            }
			else RenewTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_dawn_priestAI(creature);
	}
};

class npc_sunblade_dusk_priest : public CreatureScript
{
public:
    npc_sunblade_dusk_priest() : CreatureScript("npc_sunblade_dusk_priest") {}

	struct npc_sunblade_dusk_priestAI : public QuantumBasicAI
    {
        npc_sunblade_dusk_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FearTimer;
        uint32 MindFlayTimer;
        uint32 ShadowWordPainTimer;

        void Reset()
        {
			FearTimer = 500;
			ShadowWordPainTimer = 3000;
            MindFlayTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FearTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = urand(4000, 5000);
				}
            }
			else FearTimer -= diff;

			if (ShadowWordPainTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_WORD_PAIN);
					ShadowWordPainTimer = urand(6000, 7000);
				}
            }
			else ShadowWordPainTimer -= diff;

            if (MindFlayTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_FLAY);
					MindFlayTimer = urand(8000, 9000);
				}
            }
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_dusk_priestAI(creature);
	}
};

class npc_sunblade_vindicator : public CreatureScript
{
public:
    npc_sunblade_vindicator() : CreatureScript("npc_sunblade_vindicator") {}

    struct npc_sunblade_vindicatorAI : public QuantumBasicAI
    {
        npc_sunblade_vindicatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CleaveTimer;
        uint32 MortalStrikeTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            CleaveTimer = 2000;
            MortalStrikeTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CleaveTimer < diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 4000;
            }
			else CleaveTimer -= diff;

            if (MortalStrikeTimer < diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = 6000;
            }
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_vindicatorAI(creature);
	}
};

class npc_sunblade_dragonhawk : public CreatureScript
{
public:
    npc_sunblade_dragonhawk() : CreatureScript("npc_sunblade_dragonhawk") {}

	struct npc_sunblade_dragonhawkAI : public QuantumBasicAI
    {
		npc_sunblade_dragonhawkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BreathTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			BreathTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BreathTimer < diff)
            {
				DoCastVictim(SPELL_FIRE_BREATH);
				BreathTimer = 5000;
            }
			else BreathTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_dragonhawkAI(creature);
	}
};

class npc_sunblade_slayer : public CreatureScript
{
public:
    npc_sunblade_slayer() : CreatureScript("npc_sunblade_slayer") {}

    struct npc_sunblade_slayerAI : public QuantumBasicAI
    {
        npc_sunblade_slayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 SlayingShotTimer;
        uint32 ScatterShotTimer;
        uint32 ShootTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ShootTimer = 500;
			SlayingShotTimer = 3000;
            ScatterShotTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
			QuantumBasicAI::AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SlayingShotTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_SLAYING_SHOT);
					SlayingShotTimer = 5000;
                }
            }
			else SlayingShotTimer -= diff;

            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
					DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
            else
            {
                if (me->GetDistance2d(me->GetVictim()) > 10)
					DoStartNoMovement(me->GetVictim());
                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

                if (ScatterShotTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
						DoCast(target, SPELL_SCATTER_SHOT);
						ScatterShotTimer = 7000;
					}
                }
				else ScatterShotTimer -= diff;

                if (ShootTimer < diff)
                {
					DoCastVictim(SPELL_SHOOT);
                    ShootTimer = 2000;
                }
				else ShootTimer -= diff;
            }
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_slayerAI(creature);
	}
};

class npc_sunblade_protector : public CreatureScript
{
public:
    npc_sunblade_protector() : CreatureScript("npc_sunblade_protector") {}

	struct npc_sunblade_protectorAI : public QuantumBasicAI
    {
        npc_sunblade_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 LigtningTimer;

        bool Activated;

        void Reset()
        {
			Activated = me->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE;

			LigtningTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/)
        {
            Activated = true;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (Activated)
				QuantumBasicAI::MoveInLineOfSight(who);
        }

        void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (LigtningTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEL_LIGHTNING);
                    LigtningTimer = 4000;
                }
            }
			else LigtningTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_protectorAI(creature);
	}
};

class npc_sunblade_scout : public CreatureScript
{
public:
    npc_sunblade_scout() : CreatureScript("npc_sunblade_scout") {}

	struct npc_sunblade_scoutAI : public QuantumBasicAI
	{
		npc_sunblade_scoutAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 SinisterStrikeTimer;
        uint32 ActivateTimer;
        uint64 Starter;
        uint64 InactiveProtector;

        bool IsActivating;
        bool SpellHit;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD);
            me->InterruptNonMeleeSpells(false);
            me->RemoveAurasDueToSpell(SPELL_ACTIVATE_PROTECTOR);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            SinisterStrikeTimer = 2000;
            ActivateTimer = 4000;

            IsActivating = false;
            SpellHit = false;

            Starter = 0;
            InactiveProtector = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!IsActivating)
                QuantumBasicAI::MoveInLineOfSight(who);
        }

        void AttackStart(Unit* who)
        {
            if (!IsActivating)
            {
                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);

                    if (!me->IsInCombatActive())
                        EnterToBattle(who);

                    if (!IsActivating)
						DoStartMovement(who);
                }
            }
        }

        void EnterToBattle(Unit* who)
        {
            if (Creature* protector = me->FindCreatureWithDistance(NPC_SUNWELL_PROTECTOR, 200.0f))
            {
                InactiveProtector = protector->GetGUID();
                float x,y;
                Starter = who->GetGUID();
                protector->GetNearPoint2D(x, y, 20, me->GetAngle(protector) - M_PI);
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(1, x, y, protector->GetPositionZ());
                IsActivating = true;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (id != 1 || type != POINT_MOTION_TYPE)
                return;

            Creature* protector = Creature::GetCreature(*me, InactiveProtector);

            me->GetMotionMaster()->MoveIdle();

            if (protector)
				DoCast(protector, SPELL_ACTIVATE_PROTECTOR);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (!IsActivating)
                return;

            if (spell->Id == SPELL_ACTIVATE_PROTECTOR)
            {
                if (!SpellHit)
                {
                    SpellHit = true;
                    ActivateTimer = 5000;
                }
            }
        }

        void UpdateAI(uint32 const diff)
		{
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (IsActivating)
            {
                if (SpellHit)
				{
                    if (ActivateTimer < diff)
                    {
                        Unit* attacker = Unit::GetUnit(*me, Starter);
                        if (attacker)
                        {
                            if (Creature* protector = Creature::GetCreature(*me, InactiveProtector))
                                protector->AI()->AttackStart(attacker);

                            IsActivating = false;
                            SpellHit = false;

                            DoStartMovement(attacker);
                        }
						else DoStartMovement(me->GetVictim());

                        ActivateTimer = 9999999;
                    }
					else ActivateTimer -= diff;
					return;
				}
			}

            if (SinisterStrikeTimer < diff)
            {
                DoCastVictim(SPELL_SINISTER_STRIKE);
                SinisterStrikeTimer = 4000;
            }
			else SinisterStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_scoutAI(creature);
	}
};

class npc_shadowsword_assassin : public CreatureScript
{
public:
	npc_shadowsword_assassin() : CreatureScript("npc_shadowsword_assassin") {}

	struct npc_shadowsword_assassinAI : public QuantumBasicAI
    {
        npc_shadowsword_assassinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowStepTimer;
        uint64 MarkTarget;

        bool Marked;

        void Reset()
        {
            ShadowStepTimer = 2000;
            MarkTarget = 0;
            Marked = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
            DoCast(me, SPELL_GREATER_INVISIBILITY);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 25.0f))
					me->AI()->EnterToBattle(player);
			}
		}

        void EnterToBattle(Unit* who)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE,  SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                DoCast(who, SPELL_ASSASSIN_MARK);
                me->getThreatManager().addThreat(who, 10000.0f);
                MarkTarget = who->GetGUID();
                Marked = true;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (Marked)
			{
                if (Player* player = Unit::GetPlayer(*me, MarkTarget))
                {
                    if (!player->HasAura(SPELL_ASSASSIN_MARK))
                    {
                        MarkTarget = 0;
                        Marked = false;
                        DoResetThreat();

                        me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE,  SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                    }
					else me->getThreatManager().addThreat(player, 10000.0f);
                }
			}

            if (ShadowStepTimer < diff)
            {
                DoCast(me, SPELL_SHADOWSTEP);
                ShadowStepTimer = 6000;
            }
			else ShadowStepTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_assassinAI(creature);
	}
};

class npc_shadowsword_lifeshaper : public CreatureScript
{
public:
	npc_shadowsword_lifeshaper() : CreatureScript("npc_shadowsword_lifeshaper") {}

    struct npc_shadowsword_lifeshaperAI : public QuantumBasicAI
    {
        npc_shadowsword_lifeshaperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DrainLifeTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            DrainLifeTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DrainLifeTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DRAIN_LIFE);
                    DrainLifeTimer = 5000;
                }
            }
			else DrainLifeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_lifeshaperAI(creature);
	}
};

class npc_shadowsword_manafiend : public CreatureScript
{
public:
	npc_shadowsword_manafiend() : CreatureScript("npc_shadowsword_manafiend") {}

    struct npc_shadowsword_manafiendAI : public QuantumBasicAI
    {
        npc_shadowsword_manafiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DrainManaTimer;
        uint32 ArcaneExplosionTimer;

        void Reset()
        {
            DrainManaTimer = 1000;
            ArcaneExplosionTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
            DoCast(me, SPELL_CHILLING_TOUCH);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DrainManaTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_DRAIN_MANA);
						DrainManaTimer = 4000;
					}
				}
            }
			else DrainManaTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
            {
				DoCastAOE(SPELL_SW_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = 6000;
            }
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_manafiendAI(creature);
	}
};

class npc_shadowsword_soulbinder : public CreatureScript
{
public:
	npc_shadowsword_soulbinder() : CreatureScript("npc_shadowsword_soulbinder") {}

    struct npc_shadowsword_soulbinderAI : public QuantumBasicAI
    {
        npc_shadowsword_soulbinderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CurseOfExhaustionTimer;
        uint32 FlashOfDarknessTimer;
        uint32 DominationTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            CurseOfExhaustionTimer = 1000;
            FlashOfDarknessTimer = 3000;
            DominationTimer = 5000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (rand()%2 == 0)
				DoCast(me, SPELL_SUMMON_FELGUARD);

			else DoCast(me, SPELL_SUMMON_INFERNAL);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfExhaustionTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_CURSE_OF_EXHAUSTION);
					CurseOfExhaustionTimer = 3000;
                }
            }
			else CurseOfExhaustionTimer -= diff;

            if (FlashOfDarknessTimer < diff)
            {
				DoCastAOE(SPELL_FLASH_OF_DARKNESS);
				FlashOfDarknessTimer = 5000;
            }
			else FlashOfDarknessTimer -= diff;

            if (DominationTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_DOMINATION);
					DominationTimer = 10000;
				}
            }
			else DominationTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_soulbinderAI(creature);
	}
};

class npc_shadowsword_vanquisher : public CreatureScript
{
public:
	npc_shadowsword_vanquisher() : CreatureScript("npc_shadowsword_vanquisher") {}

    struct npc_shadowsword_vanquisherAI : public QuantumBasicAI
    {
        npc_shadowsword_vanquisherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 MeltArmorTimer;
        uint32 CleaveTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            MeltArmorTimer = 1000;
            CleaveTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MeltArmorTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_MELT_ARMOR);
					MeltArmorTimer = 2000;
                }
            }
			else MeltArmorTimer -= diff;

            if (CleaveTimer < diff)
            {
				DoCastVictim(SPELL_SW_CLEAVE);
				CleaveTimer = 4000;
            }
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_vanquisherAI(creature);
	}
};

class npc_shadowsword_commander : public CreatureScript
{
public:
	npc_shadowsword_commander() : CreatureScript("npc_shadowsword_commander") {}

    struct npc_shadowsword_commanderAI : public QuantumBasicAI
    {
        npc_shadowsword_commanderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShieldSlamTimer;
        uint32 BattleShoutTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            ShieldSlamTimer = 2000;
            BattleShoutTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShieldSlamTimer < diff)
            {
				DoCastVictim(SPELL_SHIELD_SLAM);
				ShieldSlamTimer = 4000;
            }
			else ShieldSlamTimer -= diff;

            if (BattleShoutTimer < diff)
            {
				DoCastAOE(SPELL_BATTLE_SHOUT);
				BattleShoutTimer = 6000;
            }
			else BattleShoutTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_commanderAI(creature);
	}
};

class npc_shadowsword_deathbringer : public CreatureScript
{
public:
	npc_shadowsword_deathbringer() : CreatureScript("npc_shadowsword_deathbringer") {}

    struct npc_shadowsword_deathbringerAI : public QuantumBasicAI
    {
        npc_shadowsword_deathbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 VolatileDiseaseTimer;
        uint32 DiseaseBuffetTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            VolatileDiseaseTimer = 1000;
            DiseaseBuffetTimer = 6000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (VolatileDiseaseTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_VOLATILE_DISEASE);
					VolatileDiseaseTimer = 4000;
                }
            }
			else VolatileDiseaseTimer -= diff;

            if (DiseaseBuffetTimer < diff)
            {
				DoCastVictim(SPELL_DISEASE_BUFFET);
				DiseaseBuffetTimer = 6000;
            }
			else DiseaseBuffetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_deathbringerAI(creature);
	}
};

class npc_shadowsword_guardian : public CreatureScript
{
public:
	npc_shadowsword_guardian() : CreatureScript("npc_shadowsword_guardian") {}

    struct npc_shadowsword_guardianAI : public QuantumBasicAI
    {
        npc_shadowsword_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BearDownTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            BearDownTimer = 1000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_EARTHQUAKE);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BearDownTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_BEAR_DOWN);
					BearDownTimer = 5000;
                }
            }
			else BearDownTimer -= diff;

            DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_guardianAI(creature);
	}
};

class npc_apocalypse_guard : public CreatureScript
{
public:
	npc_apocalypse_guard() : CreatureScript("npc_apocalypse_guard") {}

	struct npc_apocalypse_guardAI : public QuantumBasicAI
	{
		npc_apocalypse_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        bool Shielded;

        uint32 DeathCoilTimer;
        uint32 CorruptingStrikeTimer;
        uint32 CleaveTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            Shielded = false;

            DeathCoilTimer = 1000;
            CorruptingStrikeTimer = 3000;
            CleaveTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeathCoilTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_DEATH_COIL);
					DeathCoilTimer = 3000;
				}
            }
			else DeathCoilTimer -= diff;

			if (CorruptingStrikeTimer < diff)
            {
                DoCastVictim(SPELL_CORRUPTING_STRIKE);
                CorruptingStrikeTimer = 5000;
            }
			else CorruptingStrikeTimer -= diff;

            if (CleaveTimer < diff)
            {
                DoCastVictim(SPELL_APG_CLEAVE);
                CleaveTimer = 7000;
            }
			else CleaveTimer -= diff;

            if (!Shielded)
			{
                if ((me->GetHealth()*100 / me->GetMaxHealth()) <= 10)
                {
                    DoCast(me, SPELL_INFERNAL_DEFENSE, true);
                    Shielded = true;
                }
				DoMeleeAttackIfReady();
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_apocalypse_guardAI(creature);
	}
};

uint32 CataclysmAura[8] =
{
	46293, 46294, 46295, 46296,
    46297, 46298, 46299, 46300,
};

class npc_cataclysm_hound : public CreatureScript
{
public:
	npc_cataclysm_hound() : CreatureScript("npc_cataclysm_hound") {}

    struct npc_cataclysm_houndAI : public QuantumBasicAI
    {
        npc_cataclysm_houndAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CataclysmBreathTimer;
        uint32 EnrageTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            CataclysmBreathTimer = 2000;
            EnrageTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CATACLYSM_BREATH && target->GetTypeId() == TYPE_ID_PLAYER)
            {
                uint32 miss = 0;

                for (int i = 0; i<8; i++)
                {
                    if (rand()%2 == 0 || miss >= 3)
                        me->CastSpell(target, CataclysmAura[i], true);

                    else 
						miss++;
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CataclysmBreathTimer < diff)
            {
				DoCastVictim(SPELL_CATACLYSM_BREATH);
				CataclysmBreathTimer = 5000;
            }
			else CataclysmBreathTimer -= diff;

            if (EnrageTimer < diff)
            {
				DoCast(me, SPELL_ENRAGE);
				EnrageTimer = 10000;
            }
			else EnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_cataclysm_houndAI(creature);
	}
};

class npc_chaos_gazer : public CreatureScript
{
public:
	npc_chaos_gazer() : CreatureScript("npc_chaos_gazer") {}

    struct npc_chaos_gazerAI : public QuantumBasicAI
    {
        npc_chaos_gazerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DrainLifeTimer;
        uint32 PetrifyTimer;
        uint32 TentacleSweepTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            DrainLifeTimer = 1000;
            PetrifyTimer = 3000;
            TentacleSweepTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DrainLifeTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_CGZ_DRAIN_LIFE);
					DrainLifeTimer = 4000;
                }
            }
			else DrainLifeTimer -= diff;

			if (PetrifyTimer < diff)
            {
                DoCastVictim(SPELL_PETRIFY);
                PetrifyTimer = 6000;
            }
			else PetrifyTimer -= diff;

            if (TentacleSweepTimer < diff)
            {
                DoCastVictim(SPELL_TENTACLE_SWEEP);
                TentacleSweepTimer = 8000;
            }
			else TentacleSweepTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_chaos_gazerAI(creature);
	}
};

class npc_doomfire_destroyer : public CreatureScript
{
public:
	npc_doomfire_destroyer() : CreatureScript("npc_doomfire_destroyer") {}

    struct npc_doomfire_destroyerAI : public QuantumBasicAI
    {
        npc_doomfire_destroyerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CreateDoomfireShardTimer;

        void Reset()
        {
            CreateDoomfireShardTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
            DoCast(me, SPELL_DOOMFIRE_IMMOLATION, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CreateDoomfireShardTimer < diff)
            {
				DoCast(me, SPELL_CREATE_DOOMFIRE_SHARD);
				CreateDoomfireShardTimer = 8000;
            }
			else CreateDoomfireShardTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_doomfire_destroyerAI(creature);
	}
};

class npc_doomfire_shard : public CreatureScript
{
public:
	npc_doomfire_shard() : CreatureScript("npc_doomfire_shard") {}

	struct npc_doomfire_shardAI : public QuantumBasicAI
    {
        npc_doomfire_shardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
            DoCast(me, SPELL_DOOMFIRE_IMMOLATION, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_doomfire_shardAI(creature);
	}
};

class npc_oblivion_mage : public CreatureScript
{
public:
	npc_oblivion_mage() : CreatureScript("npc_oblivion_mage") {}

    struct npc_oblivion_mageAI : public QuantumBasicAI
    {
        npc_oblivion_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 PolymorphTimer;
        uint32 FlameBuffetTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            FlameBuffetTimer = 1000;
			PolymorphTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FlameBuffetTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_FLAME_BUFFET);
					FlameBuffetTimer = 3000;
                }
            }
			else FlameBuffetTimer -= diff;

            if (PolymorphTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_POLYMORPH);
					PolymorphTimer = 5000;
                }
            }
			else PolymorphTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_oblivion_mageAI(creature);
	}
};

class npc_painbringer : public CreatureScript
{
public:
	npc_painbringer() : CreatureScript("npc_painbringer") {}

    struct npc_painbringerAI : public QuantumBasicAI
    {
        npc_painbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_BRING_PAIN, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_painbringerAI(creature);
	}
};

class npc_priestess_of_torment : public CreatureScript
{
public:
	npc_priestess_of_torment() : CreatureScript("npc_priestess_of_torment") {}

	struct npc_priestess_of_tormentAI : public QuantumBasicAI
    {
        npc_priestess_of_tormentAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 WhirlwindTimer;

        void Reset()
        {
            WhirlwindTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
            DoCast(me, SPELL_BURN_MANA, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (WhirlwindTimer < diff)
            {
                DoCastAOE(SPELL_WHIRLWIND);
                WhirlwindTimer = 6000;
            }
			else WhirlwindTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_priestess_of_tormentAI(creature);
	}
};

class npc_volatile_fiend : public CreatureScript
{
public:
	npc_volatile_fiend() : CreatureScript("npc_volatile_fiend") {}

    struct npc_volatile_fiendAI : public QuantumBasicAI
    {
        npc_volatile_fiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (me->GetEntry() == NPC_VOLATILE_FIEND)
                DoCast(me, SPELL_BURNING_DESTRUCTION);
        }

        void DamageTaken(Unit* /*doneby*/, uint32 &damage)
        {
            if (me->GetHealth() <= damage)
                DoCastAOE(SPELL_FELFIRE_FISSION, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_volatile_fiendAI(creature);
	}
};

class npc_shadowsword_berserker : public CreatureScript
{
public:
	npc_shadowsword_berserker() : CreatureScript("npc_shadowsword_berserker") {}

	struct npc_shadowsword_berserkerAI : public QuantumBasicAI
    {
        npc_shadowsword_berserkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BladeFlurryTimer;

        void Reset()
        {
            BladeFlurryTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BladeFlurryTimer < diff)
            {
                DoCast(me, SPELL_BLADE_FLURRY);
                BladeFlurryTimer = 6500;
            }
			else BladeFlurryTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowsword_berserkerAI(creature);
	}
};

class npc_shadowsword_fury_mage : public CreatureScript
{
public:
    npc_shadowsword_fury_mage() : CreatureScript("npc_shadowsword_fury_mage") {}

    struct npc_shadowsword_fury_mageAI : public QuantumBasicAI
    {
        npc_shadowsword_fury_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FelFireballTimer;

        void Reset()
        {
			FelFireballTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_SPELL_FURY, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FelFireballTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_FEL_FIREBALL);
					FelFireballTimer = urand(3000, 4000);
				}
            }
			else FelFireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowsword_fury_mageAI(creature);
    }
};

class npc_void_sentinel : public CreatureScript
{
public:
    npc_void_sentinel() : CreatureScript("npc_void_sentinel") { }

    struct npc_void_sentinelAI : public QuantumBasicAI
    {
        npc_void_sentinelAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 PulseTimer;
        uint32 VoidBlastTimer;

        void Reset()
        {
            PulseTimer = 500;
            VoidBlastTimer = 2000; 

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			float x, y, z, o;
            me->GetHomePosition(x, y, z, o);
            DoTeleportTo(x, y, 71);
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
			DoCastAOE(SPELL_VOID_SPAWN, true);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (PulseTimer <= diff)
            {
                DoCastAOE(SPELL_SHADOW_PULSE, true);
                PulseTimer = 3000;
            }
			else PulseTimer -= diff;

            if (VoidBlastTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VOID_BLAST);
					VoidBlastTimer = urand(4000, 5000);
				}
            }
			else VoidBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_void_sentinelAI(creature);
    }
};

class npc_void_spawn : public CreatureScript
{
public:
    npc_void_spawn() : CreatureScript("npc_void_spawn") { }

    struct npc_void_spawnAI : public QuantumBasicAI
    {
        npc_void_spawnAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 ShadowBoltVolleyTimer;

        void Reset()
        {
            ShadowBoltVolleyTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShadowBoltVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
					ShadowBoltVolleyTimer = urand(3000, 4000);
				}
            }
			else ShadowBoltVolleyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_void_spawnAI(creature);
    }
};

class npc_fire_fiend : public CreatureScript
{
public:
    npc_fire_fiend() : CreatureScript("npc_fire_fiend") { }

    struct npc_fire_fiendAI : public QuantumBasicAI
    {
        npc_fire_fiendAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 FireNovaTimer;

        void Reset()
        {
            FireNovaTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FireNovaTimer <= diff)
            {
				DoCastAOE(SPELL_FIRE_NOVA);
				FireNovaTimer = 4000;
            }
			else FireNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fire_fiendAI(creature);
    }
};

void AddSC_sunwel_plateau_trash()
{
	new npc_sunblade_arch_mage();
	new npc_sunblade_cabalist();
	new npc_sunblade_dawn_priest();
	new npc_sunblade_dusk_priest();
	new npc_sunblade_vindicator();
	new npc_sunblade_dragonhawk();
	new npc_sunblade_slayer();
	new npc_sunblade_protector();
	new npc_sunblade_scout();
	new npc_shadowsword_assassin();
	new npc_shadowsword_lifeshaper();
	new npc_shadowsword_manafiend();
	new npc_shadowsword_soulbinder();
	new npc_shadowsword_vanquisher();
	new npc_shadowsword_commander();
	new npc_shadowsword_deathbringer();
	new npc_shadowsword_guardian();
	new npc_apocalypse_guard();
	new npc_cataclysm_hound();
	new npc_chaos_gazer();
	new npc_doomfire_destroyer();
	new npc_doomfire_shard();
	new npc_oblivion_mage();
	new npc_painbringer();
	new npc_priestess_of_torment();
	new npc_volatile_fiend();
	new npc_shadowsword_berserker();
	new npc_shadowsword_fury_mage();
	new npc_void_sentinel();
	new npc_void_spawn();
	new npc_fire_fiend();
}