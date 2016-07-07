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

enum FelpawWolf
{
	SPELL_INFECTED_WOUND = 3427,
};

class npc_felpaw_wolf : public CreatureScript
{
public:
    npc_felpaw_wolf() : CreatureScript("npc_felpaw_wolf") {}

    struct npc_felpaw_wolfAI : public QuantumBasicAI
    {
        npc_felpaw_wolfAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedWoundTimer;

        void Reset()
        {
			InfectedWoundTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedWoundTimer <= diff)
			{
				DoCastVictim(SPELL_INFECTED_WOUND);
				InfectedWoundTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else InfectedWoundTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felpaw_wolfAI(creature);
    }
};

enum FelpawScavenger
{
	SPELL_FS_INFECTED_WOUND = 3427,
	SPELL_TENDON_RIP        = 3604,
};

class npc_felpaw_scavenger : public CreatureScript
{
public:
    npc_felpaw_scavenger() : CreatureScript("npc_felpaw_scavenger") {}

    struct npc_felpaw_scavengerAI : public QuantumBasicAI
    {
        npc_felpaw_scavengerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 InfectedWoundTimer;
		uint32 TendonRipTimer;

        void Reset()
        {
			InfectedWoundTimer = 2*IN_MILLISECONDS;
			TendonRipTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (InfectedWoundTimer <= diff)
			{
				DoCastVictim(SPELL_INFECTED_WOUND);
				InfectedWoundTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else InfectedWoundTimer -= diff;

			if (TendonRipTimer <= diff)
			{
				DoCastVictim(SPELL_TENDON_RIP);
				TendonRipTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TendonRipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felpaw_scavengerAI(creature);
    }
};

enum IronbeakOwl
{
	SPELL_SWOOP = 5708,
};

class npc_ironbeak_owl : public CreatureScript
{
public:
    npc_ironbeak_owl() : CreatureScript("npc_ironbeak_owl") {}

    struct npc_ironbeak_owlAI : public QuantumBasicAI
    {
        npc_ironbeak_owlAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SwoopTimer;

        void Reset()
        {
			SwoopTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SwoopTimer <= diff)
			{
				DoCastVictim(SPELL_SWOOP);
				SwoopTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SwoopTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ironbeak_owlAI(creature);
    }
};

enum IronbeakHunter
{
	SPELL_REND = 13443,
};

class npc_ironbeak_hunter : public CreatureScript
{
public:
    npc_ironbeak_hunter() : CreatureScript("npc_ironbeak_hunter") {}

    struct npc_ironbeak_hunterAI : public QuantumBasicAI
    {
        npc_ironbeak_hunterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;

        void Reset()
        {
			RendTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_REND);
				RendTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ironbeak_hunterAI(creature);
    }
};

enum AngerclawBear
{
	SPELL_ENRAGE          = 8599,
	SPELL_SUNDERING_SWIPE = 35147,
};

class npc_angerclaw_bear : public CreatureScript
{
public:
    npc_angerclaw_bear() : CreatureScript("npc_angerclaw_bear") {}

    struct npc_angerclaw_bearAI : public QuantumBasicAI
    {
        npc_angerclaw_bearAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SunderingSwipeTimer;

        void Reset()
        {
			SunderingSwipeTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SunderingSwipeTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDERING_SWIPE);
				SunderingSwipeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else SunderingSwipeTimer -= diff;

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
        return new npc_angerclaw_bearAI(creature);
    }
};

enum AngerclawMauler
{
	SPELL_MAUL = 15793,
};

class npc_angerclaw_mauler : public CreatureScript
{
public:
    npc_angerclaw_mauler() : CreatureScript("npc_angerclaw_mauler") {}

    struct npc_angerclaw_maulerAI : public QuantumBasicAI
    {
        npc_angerclaw_maulerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MaulTimer;

        void Reset()
        {
			MaulTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MaulTimer <= diff)
			{
				DoCastVictim(SPELL_MAUL);
				MaulTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else MaulTimer -= diff;

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
        return new npc_angerclaw_maulerAI(creature);
    }
};

enum AngerclawGrizzly
{
	SPELL_LOW_SWIPE = 8716,
};

class npc_angerclaw_grizzly : public CreatureScript
{
public:
    npc_angerclaw_grizzly() : CreatureScript("npc_angerclaw_grizzly") {}

    struct npc_angerclaw_grizzlyAI : public QuantumBasicAI
    {
        npc_angerclaw_grizzlyAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LowSwipeTimer;

        void Reset()
        {
			LowSwipeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LowSwipeTimer <= diff)
			{
				DoCastVictim(SPELL_LOW_SWIPE);
				LowSwipeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else LowSwipeTimer -= diff;

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
        return new npc_angerclaw_grizzlyAI(creature);
    }
};

enum ShadowhornStags
{
	SPELL_SHADOWHORN_CHARGE = 6921,
	SPELL_SHADOWHORN_CURSE  = 6922,
};

class npc_shadowhorn_stag : public CreatureScript
{
public:
    npc_shadowhorn_stag() : CreatureScript("npc_shadowhorn_stag") {}

    struct npc_shadowhorn_stagAI : public QuantumBasicAI
    {
        npc_shadowhorn_stagAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowhornCurseTimer;

        void Reset()
        {
			ShadowhornCurseTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHADOWHORN_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowhornCurseTimer <= diff)
			{
				DoCastVictim(SPELL_SHADOWHORN_CURSE);
				ShadowhornCurseTimer = 4*IN_MILLISECONDS;
			}
			else ShadowhornCurseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowhorn_stagAI(creature);
    }
};

class npc_elder_shadowhorn_stag : public CreatureScript
{
public:
    npc_elder_shadowhorn_stag() : CreatureScript("npc_elder_shadowhorn_stag") {}

    struct npc_elder_shadowhorn_stagAI : public QuantumBasicAI
    {
        npc_elder_shadowhorn_stagAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowhornCurseTimer;

        void Reset()
        {
			ShadowhornCurseTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHADOWHORN_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowhornCurseTimer <= diff)
			{
				DoCastVictim(SPELL_SHADOWHORN_CURSE);
				ShadowhornCurseTimer = 4*IN_MILLISECONDS;
			}
			else ShadowhornCurseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elder_shadowhorn_stagAI(creature);
    }
};

enum DeadwoodBears
{
	SPELL_CURSE_OF_THE_DEADWOOD = 13583,
	SPELL_STRIKE                = 13584,
	SPELL_SHOOT                 = 6660,
	SPELL_PIERCING_SHOT         = 6685,
	SPELL_REJUVENATION          = 12160,
	SPELL_HEALING_WAVE          = 11986,
};

class npc_deadwood_warrior : public CreatureScript
{
public:
    npc_deadwood_warrior() : CreatureScript("npc_deadwood_warrior") {}

    struct npc_deadwood_warriorAI : public QuantumBasicAI
    {
        npc_deadwood_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfTheDeadwoodTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			CurseOfTheDeadwoodTimer = 1*IN_MILLISECONDS;
			StrikeTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfTheDeadwoodTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_THE_DEADWOOD);
				CurseOfTheDeadwoodTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CurseOfTheDeadwoodTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deadwood_warriorAI(creature);
    }
};

class npc_deadwood_pathfinder : public CreatureScript
{
public:
    npc_deadwood_pathfinder() : CreatureScript("npc_deadwood_pathfinder") {}

    struct npc_deadwood_pathfinderAI : public QuantumBasicAI
    {
        npc_deadwood_pathfinderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfTheDeadwoodTimer;
		uint32 PiercingShotTimer;

        void Reset()
        {
			CurseOfTheDeadwoodTimer = 1*IN_MILLISECONDS;
			PiercingShotTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHOOT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfTheDeadwoodTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_THE_DEADWOOD);
				CurseOfTheDeadwoodTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CurseOfTheDeadwoodTimer -= diff;

			if (PiercingShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_PIERCING_SHOT);
						PiercingShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
			}
			else PiercingShotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deadwood_pathfinderAI(creature);
    }
};

class npc_deadwood_gardener : public CreatureScript
{
public:
    npc_deadwood_gardener() : CreatureScript("npc_deadwood_gardener") {}

    struct npc_deadwood_gardenerAI : public QuantumBasicAI
    {
        npc_deadwood_gardenerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfTheDeadwoodTimer;
		uint32 RejuvenationTimer;

        void Reset()
        {
			CurseOfTheDeadwoodTimer = 1*IN_MILLISECONDS;
			RejuvenationTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfTheDeadwoodTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_THE_DEADWOOD);
				CurseOfTheDeadwoodTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CurseOfTheDeadwoodTimer -= diff;

			if (RejuvenationTimer <= diff)
			{
				DoCast(me, SPELL_REJUVENATION);
				RejuvenationTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RejuvenationTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, SPELL_HEALING_WAVE);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deadwood_gardenerAI(creature);
    }
};

enum JadefireDemons
{
	SPELL_JADEFIRE          = 13578,
	SPELL_GOUGE             = 13579,
	SPELL_BACKSTAB          = 7159,
	SPELL_CURSE_OF_TONGUES  = 13338,
	SPELL_CURSE_OF_WEAKNESS = 11980,
	SPELL_CLEAVE            = 40504,
	SPELL_JB_REND           = 13738,
	SPELL_SHADOW_BOLT       = 9613,
	SPELL_CRIPPLE           = 11443,
	SPELL_STEALTH           = 5916,
	SPELL_SLOWING_POISON    = 14897,
	SPELL_FIREBALL          = 20823,
	SPELL_RAIN_OF_FIRE      = 11990,
};

class npc_jadefire_satyr : public CreatureScript
{
public:
    npc_jadefire_satyr() : CreatureScript("npc_jadefire_satyr") {}

    struct npc_jadefire_satyrAI : public QuantumBasicAI
    {
        npc_jadefire_satyrAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 7*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_satyrAI(creature);
    }
};

class npc_jadefire_rogue : public CreatureScript
{
public:
    npc_jadefire_rogue() : CreatureScript("npc_jadefire_rogue") {}

    struct npc_jadefire_rogueAI : public QuantumBasicAI
    {
        npc_jadefire_rogueAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 GougeTimer;
		uint32 BackstabTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;
			GougeTimer = 4*IN_MILLISECONDS;
			BackstabTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 5*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			if (GougeTimer <= diff)
			{
				DoCastVictim(SPELL_GOUGE);
				GougeTimer = 7*IN_MILLISECONDS;
			}
			else GougeTimer -= diff;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_BACKSTAB);
				BackstabTimer = 9*IN_MILLISECONDS;
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_rogueAI(creature);
    }
};

class npc_jadefire_trickster : public CreatureScript
{
public:
    npc_jadefire_trickster() : CreatureScript("npc_jadefire_trickster") {}

    struct npc_jadefire_tricksterAI : public QuantumBasicAI
    {
        npc_jadefire_tricksterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 CurseOfTonguesTimer;
		uint32 CurseOfWeaknessTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;
			CurseOfTonguesTimer = 4*IN_MILLISECONDS;
			CurseOfWeaknessTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 5*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			if (CurseOfTonguesTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_TONGUES);
				CurseOfTonguesTimer = 7*IN_MILLISECONDS;
			}
			else CurseOfTonguesTimer -= diff;

			if (CurseOfWeaknessTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_WEAKNESS);
				CurseOfWeaknessTimer = 9*IN_MILLISECONDS;
			}
			else CurseOfWeaknessTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_tricksterAI(creature);
    }
};

class npc_jadefire_betrayer : public CreatureScript
{
public:
    npc_jadefire_betrayer() : CreatureScript("npc_jadefire_betrayer") {}

    struct npc_jadefire_betrayerAI : public QuantumBasicAI
    {
        npc_jadefire_betrayerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 RendTimer;
		uint32 CleaveTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;
			RendTimer = 4*IN_MILLISECONDS;
			CleaveTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 5*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_JB_REND);
				RendTimer = 7*IN_MILLISECONDS;
			}
			else RendTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 9*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_betrayerAI(creature);
    }
};

class npc_jadefire_felsworn : public CreatureScript
{
public:
    npc_jadefire_felsworn() : CreatureScript("npc_jadefire_felsworn") {}

    struct npc_jadefire_felswornAI : public QuantumBasicAI
    {
        npc_jadefire_felswornAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 ShadowBoltTimer;
		uint32 CrippleTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;
			ShadowBoltTimer = 4*IN_MILLISECONDS;
			CrippleTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 5*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				DoCastVictim(SPELL_SHADOW_BOLT);
				ShadowBoltTimer = 6*IN_MILLISECONDS;
			}
			else ShadowBoltTimer -= diff;

			if (CrippleTimer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLE);
				CrippleTimer = 8*IN_MILLISECONDS;
			}
			else CrippleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_felswornAI(creature);
    }
};

class npc_jadefire_shadowstalker : public CreatureScript
{
public:
    npc_jadefire_shadowstalker() : CreatureScript("npc_jadefire_shadowstalker") {}

    struct npc_jadefire_shadowstalkerAI : public QuantumBasicAI
    {
        npc_jadefire_shadowstalkerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 SlowingPoisonTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;
			SlowingPoisonTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_STEALTH);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 5*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			if (SlowingPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_SLOWING_POISON);
				SlowingPoisonTimer = 7*IN_MILLISECONDS;
			}
			else SlowingPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_shadowstalkerAI(creature);
    }
};

class npc_jadefire_hellcaller : public CreatureScript
{
public:
    npc_jadefire_hellcaller() : CreatureScript("npc_jadefire_hellcaller") {}

    struct npc_jadefire_hellcallerAI : public QuantumBasicAI
    {
        npc_jadefire_hellcallerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 FireballTimer;
		uint32 RainOfFireTimer;

        void Reset()
        {
			JadefireTimer = 2*IN_MILLISECONDS;
			FireballTimer = 4*IN_MILLISECONDS;
			RainOfFireTimer = 6*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_JADEFIRE);
				JadefireTimer = 3*IN_MILLISECONDS;
			}
			else JadefireTimer -= diff;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = 5*IN_MILLISECONDS;
				}
			}
			else FireballTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAIN_OF_FIRE);
					RainOfFireTimer = 7*IN_MILLISECONDS;
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jadefire_hellcallerAI(creature);
    }
};

enum AlshirrBanebreath
{
	SPELL_AB_JADEFIRE    = 13578,
	SPELL_AB_SHADOW_BOLT = 9613,
	SPELL_IMMOLATE       = 11962,
};

class npc_alshirr_banerbreath : public CreatureScript
{
public:
    npc_alshirr_banerbreath() : CreatureScript("npc_alshirr_banerbreath") {}

    struct npc_alshirr_banerbreathAI : public QuantumBasicAI
    {
        npc_alshirr_banerbreathAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 JadefireTimer;
		uint32 ShadowBoltTimer;
		uint32 ImmolateTimer;

        void Reset()
        {
			JadefireTimer = 1*IN_MILLISECONDS;
			ShadowBoltTimer = 3*IN_MILLISECONDS;
			ImmolateTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JadefireTimer <= diff)
			{
				DoCast(me, SPELL_AB_JADEFIRE);
				JadefireTimer = urand (5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else JadefireTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				DoCastVictim(SPELL_AB_SHADOW_BOLT);
				ShadowBoltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShadowBoltTimer -= diff;

			if (ImmolateTimer <= diff)
			{
				DoCastVictim(SPELL_IMMOLATE);
				ImmolateTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ImmolateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alshirr_banerbreathAI(creature);
    }
};

enum Plainstrider
{
	SPELL_SPRINT = 2983,
};

class npc_plainstrider : public CreatureScript
{
public:
    npc_plainstrider() : CreatureScript("npc_plainstrider") {}

    struct npc_plainstriderAI : public QuantumBasicAI
    {
        npc_plainstriderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SprintTimer;

        void Reset()
        {
			SprintTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SprintTimer <= diff)
			{
				DoCast(me, SPELL_SPRINT);
				SprintTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else SprintTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plainstriderAI(creature);
    }
};

enum AdultPlainstrider
{
	SPELL_AP_SPRINT = 8696,
};

class npc_adult_plainstrider : public CreatureScript
{
public:
    npc_adult_plainstrider() : CreatureScript("npc_adult_plainstrider") {}

    struct npc_adult_plainstriderAI : public QuantumBasicAI
    {
        npc_adult_plainstriderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SprintTimer;

        void Reset()
        {
			SprintTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SprintTimer <= diff)
			{
				DoCast(me, SPELL_AP_SPRINT);
				SprintTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else SprintTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_adult_plainstriderAI(creature);
    }
};

enum Battleboar
{
	SPELL_BOAR_CHARGE = 3385,
};

class npc_battleboar : public CreatureScript
{
public:
    npc_battleboar() : CreatureScript("npc_battleboar") {}

    struct npc_battleboarAI : public QuantumBasicAI
    {
        npc_battleboarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoarChargeTimer;

        void Reset()
        {
			BoarChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoarChargeTimer <= diff)
			{
				DoCast(me, SPELL_BOAR_CHARGE);
				BoarChargeTimer = 5*IN_MILLISECONDS;
			}
			else BoarChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_battleboarAI(creature);
    }
};

enum MountainCougar
{
	SPELL_RAKE = 24331,
};

class npc_mountain_cougar : public CreatureScript
{
public:
    npc_mountain_cougar() : CreatureScript("npc_mountain_cougar") {}

    struct npc_mountain_cougarAI : public QuantumBasicAI
    {
        npc_mountain_cougarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RakeTimer;

        void Reset()
        {
			RakeTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RakeTimer <= diff)
			{
				DoCastVictim(SPELL_RAKE);
				RakeTimer = 3*IN_MILLISECONDS;
			}
			else RakeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mountain_cougarAI(creature);
    }
};

enum BristlebackShaman
{
	SPELL_LIGHTNING_BOLT = 9532,
};

class npc_bristleback_shaman : public CreatureScript
{
public:
    npc_bristleback_shaman() : CreatureScript("npc_bristleback_shaman") {}

    struct npc_bristleback_shamanAI : public QuantumBasicAI
    {
        npc_bristleback_shamanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bristleback_shamanAI(creature);
    }
};

class npc_bristleback_battleboar : public CreatureScript
{
public:
    npc_bristleback_battleboar() : CreatureScript("npc_bristleback_battleboar") {}

    struct npc_bristleback_battleboarAI : public QuantumBasicAI
    {
        npc_bristleback_battleboarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoarChargeTimer;

        void Reset()
        {
			BoarChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoarChargeTimer <= diff)
			{
				DoCast(me, SPELL_BOAR_CHARGE);
				BoarChargeTimer = 5*IN_MILLISECONDS;
			}
			else BoarChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bristleback_battleboarAI(creature);
    }
};

enum GrimtotemSpiritShifter
{
	SPELL_GSS_SHOCK           = 11824,
	SPELL_SUMMON_SPIRIT_WOLF  = 43110,

	NPC_GRIMTOTEM_SPIRIT_WOLF = 26111,
};

class npc_grimtotem_spirit_shifter : public CreatureScript
{
public:
    npc_grimtotem_spirit_shifter() : CreatureScript("npc_grimtotem_spirit_shifter") {}

    struct npc_grimtotem_spirit_shifterAI : public QuantumBasicAI
    {
        npc_grimtotem_spirit_shifterAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ShockTimer;
		uint32 SummonSpiritWolfTimer;

		SummonList Summons;

        void Reset()
        {
			ShockTimer = 0.5*IN_MILLISECONDS;
			SummonSpiritWolfTimer = 1*IN_MILLISECONDS;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_GRIMTOTEM_SPIRIT_WOLF)
			{
				Summons.Summon(summon);

				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_SPIRIT_WOLF, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GSS_SHOCK);
					ShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShockTimer -= diff;

			if (SummonSpiritWolfTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_SPIRIT_WOLF);
				SummonSpiritWolfTimer = 12*IN_MILLISECONDS;
			}
			else SummonSpiritWolfTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grimtotem_spirit_shifterAI(creature);
    }
};

enum SonOfCenarius
{
	SPELL_SUMMON_TREANT_ALLY = 7993,
	NPC_TREANT_ALLY          = 5806,
};

class npc_son_of_cenarius : public CreatureScript
{
public:
	npc_son_of_cenarius () : CreatureScript("npc_son_of_cenarius") {}

	struct npc_son_of_cenariusAI: public QuantumBasicAI
	{
		npc_son_of_cenariusAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		SummonList Summons;

		void Reset()
		{
			DoCast(me, SPELL_SUMMON_TREANT_ALLY, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_TREANT_ALLY)
			{
				Summons.Summon(summon);

				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);
			}
		}

		void UpdateAI(uint32 const /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_son_of_cenariusAI(creature);
	}
};

class npc_elder_mottled_boar : public CreatureScript
{
public:
    npc_elder_mottled_boar() : CreatureScript("npc_elder_mottled_boar") {}

    struct npc_elder_mottled_boarAI : public QuantumBasicAI
    {
        npc_elder_mottled_boarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoarChargeTimer;

        void Reset()
        {
			BoarChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoarChargeTimer <= diff)
			{
				DoCast(me, SPELL_BOAR_CHARGE);
				BoarChargeTimer = 5*IN_MILLISECONDS;
			}
			else BoarChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elder_mottled_boarAI(creature);
    }
};

enum MarinerStillglider
{
	SPELL_MS_NET   = 12024,
	SPELL_MS_SHOOT = 23337,
};

class npc_mariner_stillglider : public CreatureScript
{
public:
    npc_mariner_stillglider() : CreatureScript("npc_mariner_stillglider") {}

    struct npc_mariner_stillgliderAI : public QuantumBasicAI
    {
        npc_mariner_stillgliderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 NetTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			NetTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
						DoCast(target, SPELL_MS_SHOOT);
						ShootTimer = 2*IN_MILLISECONDS;
					}
				}
			}
			else ShootTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 20)
					{
						DoCast(target, SPELL_MS_NET);
						NetTimer = 6*IN_MILLISECONDS;
					}
				}
			}
			else NetTimer -= diff;

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
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
						{
							DoCast(target, SPELL_MS_SHOOT);
							ShootTimer = 7*IN_MILLISECONDS;
						}
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mariner_stillgliderAI(creature);
    }
};

enum GelkisScout
{
	SPELL_GS_SHOOT = 6660,
};

class npc_gelkis_scout : public CreatureScript
{
public:
    npc_gelkis_scout() : CreatureScript("npc_gelkis_scout") {}

    struct npc_gelkis_scoutAI : public QuantumBasicAI
    {
		npc_gelkis_scoutAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
						DoCast(target, SPELL_GS_SHOOT);
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
						DoCast(target, SPELL_GS_SHOOT);
						ShootTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gelkis_scoutAI(creature);
    }
};

enum Polly
{
	SPELL_P_STEALTH     = 8822,
	SPELL_POLLY_CRACKER = 9976,
	SPELL_SUMMON_POLLY  = 9998,
};

class npc_polly : public CreatureScript
{
public:
    npc_polly() : CreatureScript("npc_polly") {}

    struct npc_pollyAI : public QuantumBasicAI
    {
        npc_pollyAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StealthTimer;

        void Reset()
		{
			StealthTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_POLLY_CRACKER)
				DoCast(me, SPELL_SUMMON_POLLY);
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (StealthTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_P_STEALTH);
				StealthTimer = 30*IN_MILLISECONDS;
			}
			else StealthTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pollyAI(creature);
    }
};

enum SilithidInvader
{
	SPELL_SI_PIERCE_ARMOR = 6016,
	SPELL_SILITHID_POX    = 8137,
};

class npc_silithid_invader : public CreatureScript
{
public:
    npc_silithid_invader() : CreatureScript("npc_silithid_invader") {}

    struct npc_silithid_invaderAI : public QuantumBasicAI
    {
        npc_silithid_invaderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SilithidPoxTimer;
		uint32 PierceArmorTimer;

        void Reset()
        {
			SilithidPoxTimer = 0.5*IN_MILLISECONDS;
			PierceArmorTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SilithidPoxTimer <= diff)
			{
				DoCastVictim(SPELL_SILITHID_POX);
				SilithidPoxTimer = 4*IN_MILLISECONDS;
			}
			else SilithidPoxTimer -= diff;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SI_PIERCE_ARMOR);
				PierceArmorTimer = 6*IN_MILLISECONDS;
			}
			else PierceArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silithid_invaderAI(creature);
    }
};

enum SilithidRavager
{
	SPELL_STRONG_CLEAVE = 8255,
};

class npc_silithid_ravager : public CreatureScript
{
public:
    npc_silithid_ravager() : CreatureScript("npc_silithid_ravager") {}

    struct npc_silithid_ravagerAI : public QuantumBasicAI
    {
        npc_silithid_ravagerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrongCleaveTimer;

        void Reset()
        {
			StrongCleaveTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrongCleaveTimer <= diff)
			{
				DoCastVictim(SPELL_STRONG_CLEAVE);
				StrongCleaveTimer = 4*IN_MILLISECONDS;
			}
			else StrongCleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silithid_ravagerAI(creature);
    }
};

enum DaughterOfCenarius
{
	SPELL_THROW        = 10277,
	SPELL_DISPEL_MAGIC = 527,
};

class npc_daughter_of_cenarius : public CreatureScript
{
public:
    npc_daughter_of_cenarius() : CreatureScript("npc_daughter_of_cenarius") {}

    struct npc_daughter_of_cenariusAI : public QuantumBasicAI
    {
        npc_daughter_of_cenariusAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThrowTimer;
		uint32 DispelMagicTimer;

        void Reset()
        {
			ThrowTimer = 0.5*IN_MILLISECONDS;
			DispelMagicTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_THROW);
					ThrowTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ThrowTimer -= diff;

			if (DispelMagicTimer <= diff)
			{
				DoCast(SPELL_DISPEL_MAGIC);
				DispelMagicTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DispelMagicTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daughter_of_cenariusAI(creature);
    }
};

enum Lunaclaw
{
	SPELL_THRASH                 = 3391,
	SPELL_SUMMON_LUNACLAW_SPIRIT = 18986,
};

class npc_lunaclaw : public CreatureScript
{
public:
    npc_lunaclaw() : CreatureScript("npc_lunaclaw") {}

    struct npc_lunaclawAI : public QuantumBasicAI
    {
        npc_lunaclawAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(me, SPELL_SUMMON_LUNACLAW_SPIRIT, true);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lunaclawAI(creature);
    }
};

enum Stinglasher
{
	SPELL_VENOM_STING = 5416,
	SPELL_LASH        = 6607,
};

class npc_stinglasher : public CreatureScript
{
public:
    npc_stinglasher() : CreatureScript("npc_stinglasher") {}

    struct npc_stinglasherAI : public QuantumBasicAI
    {
        npc_stinglasherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VenomStingTimer;
		uint32 LashTimer;

        void Reset()
        {
			VenomStingTimer = 1*IN_MILLISECONDS;
			LashTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomStingTimer <= diff)
			{
				DoCastVictim(SPELL_VENOM_STING);
				VenomStingTimer = 4*IN_MILLISECONDS;
			}
			else VenomStingTimer -= diff;

			if (LashTimer <= diff)
			{
				DoCastVictim(SPELL_LASH);
				LashTimer = 6*IN_MILLISECONDS;
			}
			else LashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stinglasherAI(creature);
    }
};

enum HailstoneLieutenant
{
	SPELL_KNOCKDOWN = 11428,
	SPELL_FREEZE    = 5276,
};

class npc_hailstone_lieutenant : public CreatureScript
{
public:
    npc_hailstone_lieutenant() : CreatureScript("npc_hailstone_lieutenant") {}

    struct npc_hailstone_lieutenantAI : public QuantumBasicAI
    {
        npc_hailstone_lieutenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockdownTimer;
		uint32 FreezeTimer;

        void Reset()
        {
			KnockdownTimer = 2*IN_MILLISECONDS;
			FreezeTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = 6*IN_MILLISECONDS;
			}
			else KnockdownTimer -= diff;

			if (FreezeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FREEZE);
					FreezeTimer = 8*IN_MILLISECONDS;
				}
			}
			else FreezeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hailstone_lieutenantAI(creature);
    }
};

enum FrostwaveLieutenant
{
	SPELL_FROST_NOVA  = 11831,
	SPELL_FROST_SHOCK = 15089,
};

class npc_frostwave_lieutenant : public CreatureScript
{
public:
    npc_frostwave_lieutenant() : CreatureScript("npc_frostwave_lieutenant") {}

    struct npc_frostwave_lieutenantAI : public QuantumBasicAI
    {
        npc_frostwave_lieutenantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostShockTimer;
		uint32 FrostNovaTimer;

        void Reset()
        {
			FrostShockTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_SHOCK);
					FrostShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostShockTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostwave_lieutenantAI(creature);
    }
};

enum HereticBodyguard
{
	SPELL_HB_STRIKE            = 12057,
	SPELL_HB_CLEAVE            = 15284,
	SPELL_HB_FRIGHTENING_SHOUT = 19134,
};

class npc_heretic_bodyguard : public CreatureScript
{
public:
    npc_heretic_bodyguard() : CreatureScript("npc_heretic_bodyguard") {}

    struct npc_heretic_bodyguardAI : public QuantumBasicAI
    {
        npc_heretic_bodyguardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 CleaveTimer;
		uint32 FrighteningShoutTimer;

        void Reset()
		{
			StrikeTimer = 1*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;
			FrighteningShoutTimer = 3*IN_MILLISECONDS;

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

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HB_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_HB_CLEAVE);
				CleaveTimer = 6*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastAOE(SPELL_HB_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = 8*IN_MILLISECONDS;
			}
			else FrighteningShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_heretic_bodyguardAI(creature);
    }
};

enum HereticEmissary
{
	SPELL_HE_FROST_ARMOR = 12544,
	SPELL_HE_FROSTBOLT   = 20792,
};

class npc_heretic_emissary : public CreatureScript
{
public:
    npc_heretic_emissary() : CreatureScript("npc_heretic_emissary") {}

    struct npc_heretic_emissaryAI : public QuantumBasicAI
    {
        npc_heretic_emissaryAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;

        void Reset()
		{
			FrostboltTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat
			if (!me->HasAura(SPELL_HE_FROST_ARMOR) && !me->IsInCombatActive())
				DoCast(me, SPELL_HE_FROST_ARMOR);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Frost Armor Buff Check in Combat
			if (!me->HasAura(SPELL_HE_FROST_ARMOR))
				DoCast(me, SPELL_HE_FROST_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HE_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_heretic_emissaryAI(creature);
    }
};

enum HereticCrystalGuard
{
	SPELL_HCG_STRIKE = 12057,
	SPELL_HCG_CLEAVE = 15284,
};

class npc_heretic_crystal_guard : public CreatureScript
{
public:
    npc_heretic_crystal_guard() : CreatureScript("npc_heretic_crystal_guard") {}

    struct npc_heretic_crystal_guardAI : public QuantumBasicAI
    {
        npc_heretic_crystal_guardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			StrikeTimer = 1*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;

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

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HCG_STRIKE);
				StrikeTimer = 3*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_HCG_CLEAVE);
				CleaveTimer = 5*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_heretic_crystal_guardAI(creature);
    }
};

enum BloodtalonTaillasher
{
	SPELL_RUSHING_CHARGE = 6268,
};

class npc_bloodtalon_taillasher : public CreatureScript
{
public:
    npc_bloodtalon_taillasher() : CreatureScript("npc_bloodtalon_taillasher") {}

    struct npc_bloodtalon_taillasherAI : public QuantumBasicAI
    {
        npc_bloodtalon_taillasherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RushingChargeTimer;

        void Reset()
		{
			RushingChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RushingChargeTimer <= diff)
			{
				DoCast(me, SPELL_RUSHING_CHARGE);
				RushingChargeTimer = 4*IN_MILLISECONDS;
			}
			else RushingChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodtalon_taillasherAI(creature);
    }
};

enum CorruptedScorpid
{
	SPELL_NOXIOUS_CATALYST = 5413,
	SPELL_CS_POISON_PROC   = 11919,
};

class npc_corrupted_scorpid : public CreatureScript
{
public:
    npc_corrupted_scorpid() : CreatureScript("npc_corrupted_scorpid") {}

    struct npc_corrupted_scorpidAI : public QuantumBasicAI
    {
        npc_corrupted_scorpidAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 NoxiousCatalystTimer;
		uint32 PoisonTimer;

        void Reset()
		{
			NoxiousCatalystTimer = 1*IN_MILLISECONDS;
			PoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NoxiousCatalystTimer <= diff)
			{
				DoCastVictim(SPELL_NOXIOUS_CATALYST);
				NoxiousCatalystTimer = 4*IN_MILLISECONDS;
			}
			else NoxiousCatalystTimer -= diff;

			if (PoisonTimer <= diff)
			{
				DoCast(SPELL_CS_POISON_PROC);
				PoisonTimer = 6*IN_MILLISECONDS;
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_corrupted_scorpidAI(creature);
    }
};

enum CorruptedMottledBoar
{
	SPELL_CM_BOAR_CHARGE      = 3385,
	SPELL_CORRUPTED_INTELLECT = 6823,
};

class npc_corrupted_mottled_boar : public CreatureScript
{
public:
    npc_corrupted_mottled_boar() : CreatureScript("npc_corrupted_mottled_boar") {}

    struct npc_corrupted_mottled_boarAI : public QuantumBasicAI
    {
        npc_corrupted_mottled_boarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoarChargeTimer;

        void Reset()
        {
			BoarChargeTimer = 0.2*IN_MILLISECONDS;

			DoCast(me, SPELL_CORRUPTED_INTELLECT);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoarChargeTimer <= diff)
			{
				DoCast(me, SPELL_CM_BOAR_CHARGE);
				BoarChargeTimer = 5*IN_MILLISECONDS;
			}
			else BoarChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_corrupted_mottled_boarAI(creature);
    }
};

enum VenomtailScorpid
{
	SPELL_VS_VENOM_STING = 5416,
};

class npc_venomtail_scorpid : public CreatureScript
{
public:
    npc_venomtail_scorpid() : CreatureScript("npc_venomtail_scorpid") {}

    struct npc_venomtail_scorpidAI : public QuantumBasicAI
    {
        npc_venomtail_scorpidAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VenomStingTimer;

        void Reset()
        {
			VenomStingTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomStingTimer <= diff)
			{
				DoCastVictim(SPELL_VS_VENOM_STING);
				VenomStingTimer = 4*IN_MILLISECONDS;
			}
			else VenomStingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venomtail_scorpidAI(creature);
    }
};

enum ArmoredScorpid
{
	SPELL_SCORPID_POISON = 5416,
};

class npc_armored_scorpid : public CreatureScript
{
public:
    npc_armored_scorpid() : CreatureScript("npc_armored_scorpid") {}

    struct npc_armored_scorpidAI : public QuantumBasicAI
    {
        npc_armored_scorpidAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ScorpidPoisonTimer;

        void Reset()
        {
			ScorpidPoisonTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ScorpidPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_SCORPID_POISON);
				ScorpidPoisonTimer = 4*IN_MILLISECONDS;
			}
			else ScorpidPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_armored_scorpidAI(creature);
    }
};

enum TaurStonehoof
{
	SPELL_COSMETIC_STEAM_BLAST = 51920,
};

class npc_taur_stonehoof : public CreatureScript
{
public:
    npc_taur_stonehoof() : CreatureScript("npc_taur_stonehoof") {}

    struct npc_taur_stonehoofAI : public QuantumBasicAI
    {
        npc_taur_stonehoofAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SteamTimer;

        void Reset()
        {
			SteamTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SteamTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_COSMETIC_STEAM_BLAST, true);
				SteamTimer = 7*IN_MILLISECONDS;
			}
			else SteamTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_taur_stonehoofAI(creature);
    }
};

enum OrmStonehoof
{
	SAY_ORM_STONEHOOF = -1000409,

	SPELL_FIRE_PILLAR = 43541,
};

class npc_orm_stonehoof : public CreatureScript
{
public:
    npc_orm_stonehoof() : CreatureScript("npc_orm_stonehoof") {}

    struct npc_orm_stonehoofAI : public QuantumBasicAI
    {
        npc_orm_stonehoofAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PillarTimer;
		uint32 SayTimer;

        void Reset()
        {
			PillarTimer = 0.5*IN_MILLISECONDS;
			SayTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PillarTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_FIRE_PILLAR);
				PillarTimer = 7*IN_MILLISECONDS;
			}
			else PillarTimer -= diff;

			if (SayTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(SAY_ORM_STONEHOOF, me);
				SayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orm_stonehoofAI(creature);
    }
};

class npc_thrag_stonehoof : public CreatureScript
{
public:
    npc_thrag_stonehoof() : CreatureScript("npc_thrag_stonehoof") {}

    struct npc_thrag_stonehoofAI : public QuantumBasicAI
    {
        npc_thrag_stonehoofAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EmoteTimer;

        void Reset()
        {
			EmoteTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (EmoteTimer <= diff && !me->IsInCombatActive())
			{
				me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
				EmoteTimer = urand(30*IN_MILLISECONDS, 45*IN_MILLISECONDS);
			}
			else EmoteTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thrag_stonehoofAI(creature);
    }
};

enum BefouledWaterElemental
{
	SPELL_FOUL_SHILL   = 6873,
	SPELL_BW_FROSTBOLT = 9672,
};

class npc_befouled_water_elemental : public CreatureScript
{
public:
    npc_befouled_water_elemental() : CreatureScript("npc_befouled_water_elemental") {}

    struct npc_befouled_water_elementalAI : public QuantumBasicAI
    {
        npc_befouled_water_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostboltTimer;
		uint32 FoulChillTimer;

        void Reset()
		{
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FoulChillTimer = 2.5*IN_MILLISECONDS;

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
					DoCast(target, SPELL_BW_FROSTBOLT);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FoulChillTimer <= diff)
			{
				DoCastVictim(SPELL_FOUL_SHILL);
				FoulChillTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FoulChillTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_befouled_water_elementalAI(creature);
    }
};

enum AshenvaleBear
{
	SPELL_AB_ENRAGE          = 8599,
	SPELL_AB_SWIPE           = 8716,
	SPELL_AB_SUNDERING_SWIPE = 35147,
};

class npc_ashenvale_bear : public CreatureScript
{
public:
    npc_ashenvale_bear() : CreatureScript("npc_ashenvale_bear") {}

    struct npc_ashenvale_bearAI : public QuantumBasicAI
    {
        npc_ashenvale_bearAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SwipeTimer;
		uint32 SunderingSwipeTimer;

        void Reset()
		{
			SwipeTimer = 1*IN_MILLISECONDS;
			SunderingSwipeTimer = 2*IN_MILLISECONDS;

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

			if (SwipeTimer <= diff)
			{
				DoCastVictim(SPELL_AB_SWIPE);
				SwipeTimer = 6*IN_MILLISECONDS;
			}
			else SwipeTimer -= diff;

			if (SunderingSwipeTimer <= diff)
			{
				DoCastVictim(SPELL_AB_SUNDERING_SWIPE);
				SunderingSwipeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else SunderingSwipeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_AB_ENRAGE, 0))
				{
					DoCast(me, SPELL_AB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashenvale_bearAI(creature);
    }
};

enum WanderingProtector
{
	SPELL_WP_WAR_STOMP        = 45,
	SPELL_WP_ENTANGLING_ROOTS = 11922,
};

class npc_wandering_protector : public CreatureScript
{
public:
    npc_wandering_protector() : CreatureScript("npc_wandering_protector") {}

    struct npc_wandering_protectorAI : public QuantumBasicAI
    {
        npc_wandering_protectorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WarStompTimer;
		uint32 EntanglingRootsTimer;

        void Reset()
		{
			WarStompTimer = 2*IN_MILLISECONDS;
			EntanglingRootsTimer = 3*IN_MILLISECONDS;

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

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WP_WAR_STOMP);
				WarStompTimer = 6*IN_MILLISECONDS;
			}
			else WarStompTimer -= diff;

			if (EntanglingRootsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WP_ENTANGLING_ROOTS);
					EntanglingRootsTimer = 8*IN_MILLISECONDS;
				}
			}
			else EntanglingRootsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wandering_protectorAI(creature);
    }
};

enum Ursangous
{
	SPELL_UR_KNOCK_AWAY = 10101,
	SPELL_UR_REND       = 13443,
};

class npc_ursangous : public CreatureScript
{
public:
    npc_ursangous() : CreatureScript("npc_ursangous") {}

    struct npc_ursangousAI : public QuantumBasicAI
    {
        npc_ursangousAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KnockAwayTimer;
		uint32 RendTimer;

        void Reset()
		{
			KnockAwayTimer = 2*IN_MILLISECONDS;
			RendTimer = 3*IN_MILLISECONDS;

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

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_UR_KNOCK_AWAY);
				KnockAwayTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else KnockAwayTimer -= diff;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_UR_REND);
				RendTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ursangousAI(creature);
    }
};

enum ShadethicketWoodShaper
{
	SPELL_SW_ENTANGLING_ROOTS = 12747,
};

class npc_shadethicket_wood_shaper : public CreatureScript
{
public:
    npc_shadethicket_wood_shaper() : CreatureScript("npc_shadethicket_wood_shaper") {}

    struct npc_shadethicket_wood_shaperAI : public QuantumBasicAI
    {
        npc_shadethicket_wood_shaperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EntanglingRootsTimer;

        void Reset()
		{
			EntanglingRootsTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EntanglingRootsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SW_ENTANGLING_ROOTS);
					EntanglingRootsTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else EntanglingRootsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadethicket_wood_shaperAI(creature);
    }
};

enum ShadethicketMossEater
{
	SPELL_MOSS_COVERED_HANDS_PROC = 6867,
	SPELL_MOSS_COVERED_FEET_PROC  = 6871,
};

class npc_shadethicket_moss_eater : public CreatureScript
{
public:
    npc_shadethicket_moss_eater() : CreatureScript("npc_shadethicket_moss_eater") {}

    struct npc_shadethicket_moss_eaterAI : public QuantumBasicAI
    {
        npc_shadethicket_moss_eaterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MossCoveredHandsTimer;
		uint32 MossCoveredFeetTimer;

        void Reset()
		{
			MossCoveredHandsTimer = 1*IN_MILLISECONDS;
			MossCoveredFeetTimer = 2*IN_MILLISECONDS;

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

			if (MossCoveredHandsTimer <= diff)
			{
				DoCast(SPELL_MOSS_COVERED_HANDS_PROC);
				MossCoveredHandsTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else MossCoveredHandsTimer -= diff;

			if (MossCoveredFeetTimer <= diff)
			{
				DoCast(SPELL_MOSS_COVERED_FEET_PROC);
				MossCoveredFeetTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MossCoveredFeetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadethicket_moss_eaterAI(creature);
    }
};

enum ShadethicketStoneMover
{
	SPELL_STRENGTH_OF_STONE = 6864,
};

class npc_shadethicket_stone_mover : public CreatureScript
{
public:
    npc_shadethicket_stone_mover() : CreatureScript("npc_shadethicket_stone_mover") {}

    struct npc_shadethicket_stone_moverAI : public QuantumBasicAI
    {
        npc_shadethicket_stone_moverAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrengthOfStoneTimer;

        void Reset()
		{
			StrengthOfStoneTimer = 0.2*IN_MILLISECONDS;

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

			if (StrengthOfStoneTimer <= diff)
			{
				DoCast(me, SPELL_STRENGTH_OF_STONE);
				StrengthOfStoneTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else StrengthOfStoneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadethicket_stone_moverAI(creature);
    }
};

enum ShadethicketRaincaller
{
	SPELL_SR_LIGHTNING_CLOUD = 6535,
	SPELL_SR_LIGHTNING_BOLT  = 9532,
};

class npc_shadethicket_raincaller : public CreatureScript
{
public:
    npc_shadethicket_raincaller() : CreatureScript("npc_shadethicket_raincaller") {}

    struct npc_shadethicket_raincallerAI : public QuantumBasicAI
    {
        npc_shadethicket_raincallerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningCloudTimer;
		uint32 LightningBoltTimer;

        void Reset()
		{
			LightningCloudTimer = 0.5*IN_MILLISECONDS;
			LightningBoltTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SR_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (LightningCloudTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SR_LIGHTNING_CLOUD, true);
					LightningCloudTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else LightningCloudTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadethicket_raincallerAI(creature);
    }
};

enum ShadethicketBarkRipper
{
	SPELL_SB_TENDON_RIP = 3604,
};

class npc_shadethicket_bark_ripper : public CreatureScript
{
public:
    npc_shadethicket_bark_ripper() : CreatureScript("npc_shadethicket_bark_ripper") {}

    struct npc_shadethicket_bark_ripperAI : public QuantumBasicAI
    {
        npc_shadethicket_bark_ripperAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TendonRipTimer;

        void Reset()
		{
			TendonRipTimer = 1*IN_MILLISECONDS;

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

			if (TendonRipTimer <= diff)
			{
				DoCastVictim(SPELL_SB_TENDON_RIP);
				TendonRipTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TendonRipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadethicket_bark_ripperAI(creature);
    }
};

enum ShadethicketOracle
{
	SPELL_SO_CHAIN_LIGHTNING  = 12058,
	SPELL_SO_LIGHTNING_SHIELD = 12550,
};

class npc_shadethicket_oracle : public CreatureScript
{
public:
    npc_shadethicket_oracle() : CreatureScript("npc_shadethicket_oracle") {}

    struct npc_shadethicket_oracleAI : public QuantumBasicAI
    {
        npc_shadethicket_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainLightningTimer;

        void Reset()
        {
			ChainLightningTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat
			if (!me->HasAura(SPELL_SO_LIGHTNING_SHIELD) && !me->IsInCombatActive())
				DoCast(me, SPELL_SO_LIGHTNING_SHIELD);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_SO_LIGHTNING_SHIELD))
				DoCast(me, SPELL_SO_LIGHTNING_SHIELD, true);

			if (ChainLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SO_CHAIN_LIGHTNING);
					ChainLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ChainLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadethicket_oracleAI(creature);
    }
};

enum EarthSpirit
{
	SPELL_ELEMENTAL_INVISIBILITY = 8203,
};

class npc_redrock_earth_spirit : public CreatureScript
{
public:
    npc_redrock_earth_spirit() : CreatureScript("npc_redrock_earth_spirit") {}

    struct npc_redrock_earth_spiritAI : public QuantumBasicAI
    {
        npc_redrock_earth_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_ELEMENTAL_INVISIBILITY);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_redrock_earth_spiritAI(creature);
    }
};

enum DustwindPillager
{
	SPELL_REND_FLESH = 3147,
};

class npc_dustwind_pillager : public CreatureScript
{
public:
    npc_dustwind_pillager() : CreatureScript("npc_dustwind_pillager") {}

    struct npc_dustwind_pillagerAI : public QuantumBasicAI
    {
        npc_dustwind_pillagerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendFleshTimer;

        void Reset()
        {
			RendFleshTimer = 2*IN_MILLISECONDS;

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

			if (RendFleshTimer <= diff)
			{
				DoCastVictim(SPELL_REND_FLESH);
				RendFleshTimer = 4*IN_MILLISECONDS;
			}
			else RendFleshTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dustwind_pillagerAI(creature);
    }
};

enum ThistleBoar
{
	SPELL_TB_BOAR_CHARGE = 3385,
};

class npc_thistle_boar : public CreatureScript
{
public:
    npc_thistle_boar() : CreatureScript("npc_thistle_boar") {}

    struct npc_thistle_boarAI : public QuantumBasicAI
    {
        npc_thistle_boarAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BoarChargeTimer;

        void Reset()
        {
			BoarChargeTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoarChargeTimer <= diff)
			{
				DoCast(me, SPELL_TB_BOAR_CHARGE);
				BoarChargeTimer = 5*IN_MILLISECONDS;
			}
			else BoarChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thistle_boarAI(creature);
    }
};

void AddSC_kalimdor_npc_scripts()
{
	new npc_felpaw_wolf();
	new npc_felpaw_scavenger();
	new npc_ironbeak_owl();
	new npc_ironbeak_hunter();
	new npc_angerclaw_bear();
	new npc_angerclaw_mauler();
	new npc_angerclaw_grizzly();
	new npc_shadowhorn_stag();
	new npc_elder_shadowhorn_stag();
	new npc_deadwood_warrior();
	new npc_deadwood_pathfinder();
	new npc_deadwood_gardener();
	new npc_jadefire_satyr();
	new npc_jadefire_rogue();
	new npc_jadefire_trickster();
	new npc_jadefire_betrayer();
	new npc_jadefire_felsworn();
	new npc_jadefire_shadowstalker();
	new npc_jadefire_hellcaller();
	new npc_alshirr_banerbreath();
	new npc_plainstrider();
	new npc_adult_plainstrider();
	new npc_battleboar();
	new npc_mountain_cougar();
	new npc_bristleback_shaman();
	new npc_bristleback_battleboar();
	new npc_grimtotem_spirit_shifter();
	new npc_son_of_cenarius();
	new npc_elder_mottled_boar();
	new npc_mariner_stillglider();
	new npc_gelkis_scout();
	new npc_polly();
	new npc_silithid_invader();
	new npc_silithid_ravager();
	new npc_daughter_of_cenarius();
	new npc_lunaclaw();
	new npc_stinglasher();
	new npc_hailstone_lieutenant();
	new npc_frostwave_lieutenant();
	new npc_heretic_bodyguard();
	new npc_heretic_emissary();
	new npc_heretic_crystal_guard();
	new npc_bloodtalon_taillasher();
	new npc_corrupted_scorpid();
	new npc_corrupted_mottled_boar();
	new npc_venomtail_scorpid();
	new npc_armored_scorpid();
	new npc_taur_stonehoof();
	new npc_orm_stonehoof();
	new npc_thrag_stonehoof();
	new npc_befouled_water_elemental();
	new npc_ashenvale_bear();
	new npc_wandering_protector();
	new npc_ursangous();
	new npc_shadethicket_wood_shaper();
	new npc_shadethicket_moss_eater();
	new npc_shadethicket_stone_mover();
	new npc_shadethicket_raincaller();
	new npc_shadethicket_bark_ripper();
	new npc_shadethicket_oracle();
	new npc_redrock_earth_spirit();
	new npc_dustwind_pillager();
	new npc_thistle_boar();
}