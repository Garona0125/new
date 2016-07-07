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
#include "Config.h"

enum GuardGlobalSpells
{
	SPELL_DUAL_WIELD = 674,
};

enum Creatures
{
	NPC_AZURE_DRAGON = 27608,
	NPC_AZURE_DRAKE  = 27682,
};

enum DehtaDruids
{
	SPELL_ANIMAL_BLOOD = 46221,
	SPELL_DRUID_NET    = 41580,
	// Texts
	SAY_DRUID_AGGRO_1  = -1230600,
	SAY_DRUID_AGGRO_2  = -1230601,
	SAY_DRUID_AGGRO_3  = -1230602,
};

class npc_dehta_druid : public CreatureScript
{
public:
	npc_dehta_druid() : CreatureScript("npc_dehta_druid") {}

    struct npc_dehta_druidAI : public QuantumBasicAI
    {
        npc_dehta_druidAI(Creature* creature) : QuantumBasicAI(creature){}

		bool Attacked;

        void Reset()
		{
			Attacked = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Attacked || who->GetTypeId() != TYPE_ID_PLAYER || !who->HasAura(SPELL_ANIMAL_BLOOD))
				return;

			if (Attacked == false && who->GetTypeId() == TYPE_ID_PLAYER && who->HasAura(SPELL_ANIMAL_BLOOD))
			{
				Attacked = true;
				me->AI()->AttackStart(who);
				DoSendQuantumText(RAND(SAY_DRUID_AGGRO_1, SAY_DRUID_AGGRO_2, SAY_DRUID_AGGRO_3), me);

				if (GetConfigSettings::GetBoolState("Dehta.Animal.Killer.Enable", true))
				{
					std::string sText = ("D.E.H.T.A [BOREAN TUNDRA]:|CFFFF33FF Player: " + std::string(who->GetName()) + "|r Has Been Caught For The Killing Animals!");
					sWorld->SendServerMessage(SERVER_MSG_STRING, sText.c_str());
				}
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_DRUID_NET);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
			{
				EnterEvadeMode();
				return;
			}

			if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
			{
				EnterEvadeMode();
				return;
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dehta_druidAI(creature);
    }
};

enum WyrmrestProtector
{
	SPELL_CLEAVE        = 15496,
	SPELL_MORTAL_STRIKE = 17547,
	SPELL_SUNDER_ARMOR  = 16145,
};

class npc_wyrmrest_protector : public CreatureScript
{
public:
    npc_wyrmrest_protector() : CreatureScript("npc_wyrmrest_protector") {}

    struct npc_wyrmrest_protectorAI : public QuantumBasicAI
    {
        npc_wyrmrest_protectorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 CleaveTimer;
		uint32 SunderArmorTimer;

        void Reset()
        {
			MortalStrikeTimer = 1*IN_MILLISECONDS;
			CleaveTimer = 3*IN_MILLISECONDS;
			SunderArmorTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else SunderArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wyrmrest_protectorAI(creature);
    }
};

enum WyrmrestTempleDrake
{
	SPELL_FLAME_BREATH = 49113,
};

class npc_wyrmrest_temple_drake : public CreatureScript
{
public:
    npc_wyrmrest_temple_drake() : CreatureScript("npc_wyrmrest_temple_drake") {}

    struct npc_wyrmrest_temple_drakeAI : public QuantumBasicAI
    {
        npc_wyrmrest_temple_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlameBreathTimer;

        void Reset()
        {
			FlameBreathTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_AZURE_DRAGON || NPC_AZURE_DRAKE)
				return;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameBreathTimer <= diff)
			{
				DoCastVictim(SPELL_FLAME_BREATH);
				FlameBreathTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FlameBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wyrmrest_temple_drakeAI(creature);
    }
};

enum WyrmrestGuardian
{
	SPELL_WG_FLAME_BREATH = 49110,
};

class npc_wyrmrest_guardian : public CreatureScript
{
public:
    npc_wyrmrest_guardian() : CreatureScript("npc_wyrmrest_guardian") {}

    struct npc_wyrmrest_guardianAI : public QuantumBasicAI
    {
        npc_wyrmrest_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FlameBreathTimer;

        void Reset()
        {
			FlameBreathTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_AZURE_DRAGON || NPC_AZURE_DRAKE)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameBreathTimer <= diff)
			{
				DoCastVictim(SPELL_WG_FLAME_BREATH);
				FlameBreathTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FlameBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wyrmrest_guardianAI(creature);
    }
};

enum ArgentPeacekeeper
{
	SPELL_AP_HAMMER_OF_JUSTICE  = 39077,
	SPELL_AP_JUDGEMENT_OF_WRATH = 46033,
	SPELL_AP_HOLY_LIGHT         = 43451,
	CRUSADER_MOUNT_ID           = 28918,
};

class npc_argent_peacekeeper : public CreatureScript
{
public:
    npc_argent_peacekeeper() : CreatureScript("npc_argent_peacekeeper") {}

    struct npc_argent_peacekeeperAI : public QuantumBasicAI
    {
        npc_argent_peacekeeperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HammerOfJusticeTimer;
		uint32 JudgementOfWrathTimer;
		uint32 HolyLightTimer;

        void Reset()
        {
			HammerOfJusticeTimer = 1*IN_MILLISECONDS;
			JudgementOfWrathTimer = 2*IN_MILLISECONDS;
			HolyLightTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->Mount(CRUSADER_MOUNT_ID);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfJusticeTimer <= diff)
			{
				DoCastVictim(SPELL_AP_HAMMER_OF_JUSTICE);
				HammerOfJusticeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HammerOfJusticeTimer -= diff;

			if (JudgementOfWrathTimer <= diff)
			{
				DoCastVictim(SPELL_AP_JUDGEMENT_OF_WRATH);
				JudgementOfWrathTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else JudgementOfWrathTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				DoCast(me, SPELL_AP_HOLY_LIGHT);
				HolyLightTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else HolyLightTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_peacekeeperAI(creature);
    }
};

enum MoongladeWarden
{
	SPELL_INCAPACIATING_SHOUT = 18328,
	SPELL_DISMOUNTING_BLAST   = 18396,
	SPELL_MW_SHOOT            = 23337,
	SPELL_MW_MORTAL_STRIKE    = 19643,
	SPELL_SNAP_KICK           = 15618,
};

class npc_moonglade_warden : public CreatureScript
{
public:
    npc_moonglade_warden() : CreatureScript("npc_moonglade_warden") {}

    struct npc_moonglade_wardenAI : public QuantumBasicAI
    {
        npc_moonglade_wardenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DismountingBlastTimer;
		uint32 IncapaciatingShoutTimer;
		uint32 MortalStrikeTimer;
		uint32 SnapKickTimer;

        void Reset()
		{
			DismountingBlastTimer = 1*IN_MILLISECONDS;
			IncapaciatingShoutTimer = 3*IN_MILLISECONDS;
			MortalStrikeTimer = 5*IN_MILLISECONDS;
			SnapKickTimer = 7*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_MW_SHOOT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DismountingBlastTimer <= diff)
			{
				DoCastVictim(SPELL_DISMOUNTING_BLAST);
				DismountingBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else DismountingBlastTimer -= diff;

			if (IncapaciatingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_INCAPACIATING_SHOUT);
				IncapaciatingShoutTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else IncapaciatingShoutTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MW_MORTAL_STRIKE);
				MortalStrikeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (SnapKickTimer <= diff)
			{
				DoCastVictim(SPELL_SNAP_KICK);
				SnapKickTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else SnapKickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_moonglade_wardenAI(creature);
    }
};

enum WarsongBattleguard
{
	SPELL_WB_ENRAGE = 8599,
	WB_MOUNT_ID     = 1166,
};

class npc_warsong_battleguard : public CreatureScript
{
public:
    npc_warsong_battleguard() : CreatureScript("npc_warsong_battleguard") {}

    struct npc_warsong_battleguardAI : public QuantumBasicAI
    {
        npc_warsong_battleguardAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->Mount(WB_MOUNT_ID);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_WB_ENRAGE, 0))
				{
					DoCast(me, SPELL_WB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warsong_battleguardAI(creature);
    }
};

enum WarsongHonorGuard
{
	SPELL_WHG_ENRAGE = 8599,
};

class npc_warsong_honor_guard : public CreatureScript
{
public:
    npc_warsong_honor_guard() : CreatureScript("npc_warsong_honor_guard") {}

    struct npc_warsong_honor_guardAI : public QuantumBasicAI
    {
        npc_warsong_honor_guardAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_WHG_ENRAGE, 0))
				{
					DoCast(me, SPELL_WHG_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warsong_honor_guardAI(creature);
    }
};

enum LightsBreachDefender
{
	SPELL_LB_HAMMER_OF_JUSTICE  = 39077,
	SPELL_LB_JUDGEMENT_OF_WRATH = 46033,
	SPELL_LB_HOLY_LIGHT         = 43451,
};

class npc_lights_breach_defender : public CreatureScript
{
public:
    npc_lights_breach_defender() : CreatureScript("npc_lights_breach_defender") {}

    struct npc_lights_breach_defenderAI : public QuantumBasicAI
    {
        npc_lights_breach_defenderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HammerOfJusticeTimer;
		uint32 JudgementOfWrathTimer;
		uint32 HolyLightTimer;

        void Reset()
        {
			HammerOfJusticeTimer = 1*IN_MILLISECONDS;
			JudgementOfWrathTimer = 3*IN_MILLISECONDS;
			HolyLightTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfJusticeTimer <= diff)
			{
				DoCastVictim(SPELL_LB_HAMMER_OF_JUSTICE);
				HammerOfJusticeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HammerOfJusticeTimer -= diff;

			if (JudgementOfWrathTimer <= diff)
			{
				DoCastVictim(SPELL_LB_JUDGEMENT_OF_WRATH);
				JudgementOfWrathTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else JudgementOfWrathTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				DoCast(me, SPELL_LB_HOLY_LIGHT);
				HolyLightTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else HolyLightTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lights_breach_defenderAI(creature);
    }
};

enum ZimTorgaGuardian
{
	SPELL_FROSTBOLT = 23102,
};

class npc_zim_torga_guardian : public CreatureScript
{
public:
    npc_zim_torga_guardian() : CreatureScript("npc_zim_torga_guardian") {}

    struct npc_zim_torga_guardianAI : public QuantumBasicAI
    {
        npc_zim_torga_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;

        void Reset()
        {
			FrostboltTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = 2*IN_MILLISECONDS;
				}
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zim_torga_guardianAI(creature);
    }
};

enum SunreaverScouts
{
	SPELL_SHOOT           = 6660,
	SPELL_MULTI_SHOT      = 14443,
	SPELL_CONCUSSIVE_SHOT = 17174,

	MOUNT_SUNREAVERS_ID   = 19483,
};

class npc_sunreaver_scout : public CreatureScript
{
public:
    npc_sunreaver_scout() : CreatureScript("npc_sunreaver_scout") {}

    struct npc_sunreaver_scoutAI : public QuantumBasicAI
    {
		npc_sunreaver_scoutAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 MultiShot;
		uint32 ConcussiveShotTimer;

        void Reset()
        {
			ShootTimer = 1*IN_MILLISECONDS;
			MultiShot = 2*IN_MILLISECONDS;
			ConcussiveShotTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, 258);
        }

		void EnterToBattle(Unit* /*who*/){}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

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

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}

			if (MultiShot <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_MULTI_SHOT);
						MultiShot = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else MultiShot -= diff;

			if (ConcussiveShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_CONCUSSIVE_SHOT);
						ConcussiveShotTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
					}
				}
			}
			else ConcussiveShotTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunreaver_scoutAI(creature);
    }
};

class npc_sunreaver_hawkrider : public CreatureScript
{
public:
    npc_sunreaver_hawkrider() : CreatureScript("npc_sunreaver_hawkrider") {}

    struct npc_sunreaver_hawkriderAI : public QuantumBasicAI
    {
		npc_sunreaver_hawkriderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 MultiShot;
		uint32 ConcussiveShotTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			MultiShot = 1*IN_MILLISECONDS;
			ConcussiveShotTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->Mount(MOUNT_SUNREAVERS_ID);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();
		}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

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

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}

			if (MultiShot <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_MULTI_SHOT);
						MultiShot = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
			}
			else MultiShot -= diff;

			if (ConcussiveShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_CONCUSSIVE_SHOT);
						ConcussiveShotTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
					}
				}
			}
			else ConcussiveShotTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunreaver_hawkriderAI(creature);
    }
};

enum ThrallmarGuards
{
	SPELL_TG_SHOOT           = 6660,
	SPELL_TG_SHAP_KICK       = 15618,
	SPELL_TG_MORTAL_STRIKE   = 19643,
	SPELL_TG_HAMSTRING       = 26211,

	WOLF_RIDER_MOUNT_ID      = 14334,
};

class npc_thrallmar_grunt : public CreatureScript
{
public:
    npc_thrallmar_grunt() : CreatureScript("npc_thrallmar_grunt") {}

    struct npc_thrallmar_gruntAI : public QuantumBasicAI
    {
        npc_thrallmar_gruntAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SnapKickTimer;
		uint32 MortalStrikeTimer;
		uint32 HamstringTimer;

        void Reset()
        {
			SnapKickTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 3*IN_MILLISECONDS;
			HamstringTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_TG_SHOOT);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SnapKickTimer <= diff)
			{
				DoCastVictim(SPELL_TG_SHAP_KICK);
				SnapKickTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else SnapKickTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TG_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_TG_HAMSTRING);
				HamstringTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thrallmar_gruntAI(creature);
    }
};

class npc_thrallmar_wolf_rider : public CreatureScript
{
public:
    npc_thrallmar_wolf_rider() : CreatureScript("npc_thrallmar_wolf_rider") {}

    struct npc_thrallmar_wolf_riderAI : public QuantumBasicAI
    {
        npc_thrallmar_wolf_riderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SnapKickTimer;
		uint32 MortalStrikeTimer;
		uint32 HamstringTimer;

        void Reset()
        {
			SnapKickTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 3*IN_MILLISECONDS;
			HamstringTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->Mount(WOLF_RIDER_MOUNT_ID);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SnapKickTimer <= diff)
			{
				DoCastVictim(SPELL_TG_SHAP_KICK);
				SnapKickTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else SnapKickTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TG_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_TG_HAMSTRING);
				HamstringTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thrallmar_wolf_riderAI(creature);
	}
};

enum CrusaderOfVirtue
{
	SPELL_HAMMER_OF_JUSTICE  = 13005,
	SPELL_JUDGEMENT_OF_WRATH = 46033,
	SPELL_HOLY_LIGHT         = 58053,
};

class npc_crusader_of_virtue : public CreatureScript
{
public:
    npc_crusader_of_virtue() : CreatureScript("npc_crusader_of_virtue") {}

    struct npc_crusader_of_virtueAI : public QuantumBasicAI
    {
        npc_crusader_of_virtueAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HammerOfJusticeTimer;
		uint32 JudgementOfWrathTimer;
		uint32 HolyLightTimer;

        void Reset()
        {
			HammerOfJusticeTimer = 1*IN_MILLISECONDS;
			JudgementOfWrathTimer = 3*IN_MILLISECONDS;
			HolyLightTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfJusticeTimer <= diff)
			{
				DoCastVictim(SPELL_HAMMER_OF_JUSTICE);
				HammerOfJusticeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HammerOfJusticeTimer -= diff;

			if (JudgementOfWrathTimer <= diff)
			{
				DoCastVictim(SPELL_JUDGEMENT_OF_WRATH);
				JudgementOfWrathTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else JudgementOfWrathTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				DoCast(me, SPELL_HOLY_LIGHT);
				HolyLightTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else HolyLightTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crusader_of_virtueAI(creature);
    }
};

enum BootyBayGuards
{
	SPELL_BB_HAMSTRING     = 9080,
	SPELL_BB_NET           = 12024,
	SPELL_BB_MORTAL_STRIKE = 16856,
	SPELL_BB_SHOOT         = 23337,
};

class npc_booty_bay_bruiser : public CreatureScript
{
public:
    npc_booty_bay_bruiser() : CreatureScript("npc_booty_bay_bruiser") {}

    struct npc_booty_bay_bruiserAI : public QuantumBasicAI
    {
        npc_booty_bay_bruiserAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NetTimer;

        void Reset()
        {
			NetTimer = 1.5*IN_MILLISECONDS;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_BB_SHOOT);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NetTimer <= diff)
			{
				DoCastVictim(SPELL_BB_NET);
				NetTimer = 5*IN_MILLISECONDS;
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_booty_bay_bruiserAI(creature);
    }
};

class npc_booty_bay_elite : public CreatureScript
{
public:
    npc_booty_bay_elite() : CreatureScript("npc_booty_bay_elite") {}

    struct npc_booty_bay_eliteAI : public QuantumBasicAI
    {
        npc_booty_bay_eliteAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HamstringTimer;
		uint32 MortalStrikeTimer;

        void Reset()
        {
			HamstringTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_BB_SHOOT);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_BB_HAMSTRING);
				HamstringTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BB_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_booty_bay_eliteAI(creature);
    }
};

enum ExpeditionWarden
{
	SPELL_EW_CLEAVE        = 15496,
	SPELL_EW_MORTAL_STRIKE = 17547,
	SPELL_EW_SUNDER_ARMOR  = 16145,
};

class npc_expedition_warden : public CreatureScript
{
public:
    npc_expedition_warden() : CreatureScript("npc_expedition_warden") {}

    struct npc_expedition_wardenAI : public QuantumBasicAI
    {
        npc_expedition_wardenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;
		uint32 CleaveTimer;
		uint32 SunderArmorTimer;

        void Reset()
        {
			MortalStrikeTimer = 2*IN_MILLISECONDS;
			CleaveTimer = 4*IN_MILLISECONDS;
			SunderArmorTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_EW_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_EW_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_EW_SUNDER_ARMOR);
				SunderArmorTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else SunderArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_expedition_wardenAI(creature);
    }
};

enum StormspireGuards
{
	SPELL_SN_HAMSTRING     = 9080,
	SPELL_SN_CLEAVE        = 15284,
	SPELL_SN_REND          = 16509,
	SPELL_SN_MORTAL_STRIKE = 16856,
	SPELL_DRAKE_VISUAL     = 30987,
	SPELL_NETHER_BLAST     = 38265,
};

class npc_stormspire_nexus_guard : public CreatureScript
{
public:
    npc_stormspire_nexus_guard() : CreatureScript("npc_stormspire_nexus_guard") {}

    struct npc_stormspire_nexus_guardAI : public QuantumBasicAI
    {
        npc_stormspire_nexus_guardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HamstringTimer;
		uint32 CleaveTimer;
		uint32 RendTimer;
		uint32 MortalStrikeTimer;

        void Reset()
        {
			HamstringTimer = 2*IN_MILLISECONDS;
			CleaveTimer = 4*IN_MILLISECONDS;
			RendTimer = 5*IN_MILLISECONDS;
			MortalStrikeTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_SN_HAMSTRING);
				HamstringTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else HamstringTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_SN_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_SN_REND);
				RendTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SN_MORTAL_STRIKE);
				MortalStrikeTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormspire_nexus_guardAI(creature);
    }
};

class npc_stormspire_drake : public CreatureScript
{
public:
    npc_stormspire_drake() : CreatureScript("npc_stormspire_drake") {}

    struct npc_stormspire_drakeAI : public QuantumBasicAI
    {
        npc_stormspire_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NetherBlastTimer;

        void Reset()
        {
			NetherBlastTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetCanFly(true);
			me->RemoveAurasDueToSpell(SPELL_DRAKE_VISUAL);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DRAKE_VISUAL, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
			me->SetCanFly(false);
			me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_DOWN);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NetherBlastTimer <= diff)
			{
				DoCast(SPELL_NETHER_BLAST);
				NetherBlastTimer = 5*IN_MILLISECONDS;
			}
			else NetherBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormspire_drakeAI(creature);
    }
};

enum ArgentDefender
{
	SPELL_AD_SHOOT = 6660,
	SPELL_AD_NET   = 12024,
};

class npc_argent_defender : public CreatureScript
{
public:
    npc_argent_defender() : CreatureScript("npc_argent_defender") {}

    struct npc_argent_defenderAI : public QuantumBasicAI
    {
        npc_argent_defenderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 NetTimer;

        void Reset()
        {
			ShootTimer = 0.2*IN_MILLISECONDS;
			NetTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AD_SHOOT);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AD_NET);
					NetTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_defenderAI(creature);
    }
};

enum SplintertreeGuard
{
	SPELL_ENRAGE         = 8599,
	SPELL_SHIELD_BASH    = 11972,
	SPELL_SHIELD_BLOCK   = 12169,
	SPELL_ALL_CLEAVE     = 15284,
};

class npc_splintertree_guard : public CreatureScript
{
public:
    npc_splintertree_guard() : CreatureScript("npc_splintertree_guard") {}

    struct npc_splintertree_guardAI : public QuantumBasicAI
    {
        npc_splintertree_guardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShieldBlockTimer;
		uint32 ShieldBashTimer;
		uint32 CleaveTimer;

        void Reset()
        {
			ShieldBlockTimer = 0.5*IN_MILLISECONDS;
			ShieldBashTimer = 1.5*IN_MILLISECONDS;
			CleaveTimer = 2.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_SHIELD_BLOCK);
				ShieldBlockTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShieldBlockTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ShieldBashTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_ALL_CLEAVE);
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_splintertree_guardAI(creature);
    }
};

enum AshenvaleSentinel
{
	SPELL_AS_ENRAGE        = 8599,
	SPELL_AS_MORTAL_STRIKE = 24573,
	SPELL_AS_CLEAVE        = 15284,
};

class npc_ashenvale_sentinel : public CreatureScript
{
public:
    npc_ashenvale_sentinel() : CreatureScript("npc_ashenvale_sentinel") {}

    struct npc_ashenvale_sentinelAI : public QuantumBasicAI
    {
        npc_ashenvale_sentinelAI(Creature* creature) : QuantumBasicAI(creature){}
        
		uint32 CleaveTimer;
		uint32 MortalStrikeTimer;

        void Reset()
        {
			CleaveTimer = 1*IN_MILLISECONDS;
			MortalStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AS_MORTAL_STRIKE);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_AS_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_AS_MORTAL_STRIKE);
				MortalStrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_AS_ENRAGE, 0))
				{
					DoCast(me, SPELL_AS_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashenvale_sentinelAI(creature);
    }
};

enum EbonBladeCommander
{
	SPELL_DEATH_GRIP    = 49560,
	SPELL_ICY_TOUCH     = 49723,
	SPELL_PLAGUE_STRIKE = 52379,
};

class npc_ebon_blade_commander : public CreatureScript
{
public:
    npc_ebon_blade_commander() : CreatureScript("npc_ebon_blade_commander") {}

    struct npc_ebon_blade_commanderAI : public QuantumBasicAI
    {
        npc_ebon_blade_commanderAI(Creature* creature) : QuantumBasicAI(creature){}
        
		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;

        void Reset()
        {
			IcyTouchTimer = 0.5*IN_MILLISECONDS;
			PlagueStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(who, SPELL_DEATH_GRIP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH);
				IcyTouchTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ebon_blade_commanderAI(creature);
    }
};

enum ArgentCommander
{
	SPELL_AC_HAMMER_OF_JUSTICE  = 39077,
	SPELL_AC_JUDGEMENT_OF_WRATH = 46033,
	SPELL_AC_HOLY_LIGHT         = 43451,
};

class npc_argent_commander : public CreatureScript
{
public:
    npc_argent_commander() : CreatureScript("npc_argent_commander") {}

    struct npc_argent_commanderAI : public QuantumBasicAI
    {
        npc_argent_commanderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HammerOfJusticeTimer;
		uint32 JudgementOfWrathTimer;
		uint32 HolyLightTimer;

        void Reset()
        {
			HammerOfJusticeTimer = 1*IN_MILLISECONDS;
			JudgementOfWrathTimer = 2*IN_MILLISECONDS;
			HolyLightTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfJusticeTimer <= diff)
			{
				DoCastVictim(SPELL_AC_HAMMER_OF_JUSTICE);
				HammerOfJusticeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HammerOfJusticeTimer -= diff;

			if (JudgementOfWrathTimer <= diff)
			{
				DoCastVictim(SPELL_AC_JUDGEMENT_OF_WRATH);
				JudgementOfWrathTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else JudgementOfWrathTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				DoCast(me, SPELL_AC_HOLY_LIGHT);
				HolyLightTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else HolyLightTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_commanderAI(creature);
    }
};

void AddSC_world_guard_scripts()
{
	new npc_dehta_druid();
	new npc_wyrmrest_protector();
	new npc_wyrmrest_temple_drake();
	new npc_wyrmrest_guardian();
	new npc_argent_peacekeeper();
	new npc_moonglade_warden();
	new npc_warsong_battleguard();
	new npc_warsong_honor_guard();
	new npc_lights_breach_defender();
	new npc_zim_torga_guardian();
	new npc_sunreaver_scout();
	new npc_sunreaver_hawkrider();
	new npc_thrallmar_grunt();
	new npc_thrallmar_wolf_rider();
	new npc_crusader_of_virtue();
	new npc_booty_bay_bruiser();
	new npc_booty_bay_elite();
	new npc_expedition_warden();
	new npc_stormspire_nexus_guard();
	new npc_stormspire_drake();
	new npc_argent_defender();
	new npc_splintertree_guard();
	new npc_ashenvale_sentinel();
	new npc_ebon_blade_commander();
	new npc_argent_commander();
}