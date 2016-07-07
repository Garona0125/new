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
#include "../../../scripts/Northrend/UtgardeKeep/UtgardeKeep/utgarde_keep.h"

enum UtgardeKeepTrash
{
	SPELL_RINGING_SLAP_5N        = 42780,
	SPELL_RINGING_SLAP_5H        = 59606,
	SPELL_HEROIC_STRIKE_5N       = 57846,
	SPELL_HEROIC_STRIKE_5H       = 59607,
	SPELL_BURNING_BRAND_5N       = 43757,
	SPELL_BURNING_BRAND_5H       = 59601,
	SPELL_CAUTERIZE              = 60211,
	SPELL_ENRAGE                 = 8599,
	SPELL_SUNDER_ARMOR_5N        = 15572,
	SPELL_SUNDER_ARMOR_5H        = 59608,
	SPELL_DISARM                 = 6713,
	SPELL_CONCUSSION_BLOW        = 22427,
	SPELL_CLEAVE                 = 42724,
	SPELL_THROW_5N               = 54983,
	SPELL_THROW_5H               = 59696,
	SPELL_DEBILITATING_STRIKE_5N = 38621,
	SPELL_DEBILITATING_STRIKE_5H = 59695,
	SPELL_UNHOLY_RAGE_5N         = 43664,
	SPELL_UNHOLY_RAGE_5H         = 59694,
	SPELL_RUNE_OF_FLAME_5N       = 54965,
	SPELL_RUNE_OF_FLAME_5H       = 59617,
	SPELL_RUNE_OF_PROTECTION_5N  = 42740,
	SPELL_RUNE_OF_PROTECTION_5H  = 59616,
	SPELL_HURL_DAGGER_5N         = 42772,
	SPELL_HURL_DAGGER_5H         = 59685,
	SPELL_BLIND                  = 42972,
	SPELL_EXPLOSION_VISUAL       = 52052,
	SPELL_SUMMON_TICKING_BOMB_5N = 54962,
	SPELL_SUMMON_TICKING_BOMB_5H = 60227,
	SPELL_TICKING_BOMB_AURA_5N   = 54955,
	SPELL_TICKING_BOMB_AURA_5H   = 59686,
	SPELL_EXPLOSION_PROC_5N      = 54954,
	SPELL_EXPLOSION_PROC_5H      = 59687,
	SPELL_CHARGE_5N              = 35570,
	SPELL_CHARGE_5H              = 59611,
	SPELL_BATTLE_SHOUT_5N        = 38232,
	SPELL_BATTLE_SHOUT_5H        = 59614,
	SPELL_DEMORALIZING_SHOUT_5N  = 16244,
	SPELL_DEMORALIZING_SHOUT_5H  = 59613,
	SPELL_HEAD_CRACK_5N          = 9791,
	SPELL_HEAD_CRACK_5H          = 59599,
	SPELL_KNOCKDOWN_SPIN         = 43935,
	SPELL_DH_THROW_5N            = 43665,
	SPELL_DH_THROW_5H            = 59603,
	SPELL_WING_CLIP_5N           = 32908,
	SPELL_WING_CLIP_5H           = 59604,
	SPELL_PIERCING_JAB_5N        = 31551,
	SPELL_PIERCING_JAB_5H        = 59605,
	SPELL_HEALING_WAVE           = 51586,
	SPELL_LIGHTNING_BOLT         = 51587,
	SPELL_FLAME_SHOCK            = 51588,
	SPELL_FLIGHT                 = 52211,
	SPELL_KNOCK_AWAY             = 49722,
	SPELL_REND_5N                = 43931,
	SPELL_REND_5H                = 59691,
	SPELL_FLAME_BREATH_5N        = 50653,
	SPELL_FLAME_BREATH_5H        = 59692,
	SPELL_STRIKE                 = 13446,
	SPELL_DECREPIFY_5N           = 42702,
	SPELL_DECREPIFY_5H           = 59397,
	SPELL_FIXATE                 = 40414,
	SPELL_SW_ENRAGE              = 42745,
	SPELL_POUNCE                 = 55077,
};

enum Texts
{
	SAY_DRAGONFLAYER_AGGRO_1 = -1300040,
	SAY_DRAGONFLAYER_AGGRO_2 = -1300041,
	SAY_DRAGONFLAYER_AGGRO_3 = -1300042,
	SAY_DRAGONFLAYER_AGGRO_4 = -1300043,
	SAY_DRAGONFLAYER_AGGRO_5 = -1300044,
	SAY_DRAGONFLAYER_AGGRO_6 = -1300045,
	SAY_DRAGONFLAYER_AGGRO_7 = -1300046,
	SAY_DRAGONFLAYER_AGGRO_8 = -1300047,
	SAY_DRAGONFLAYER_AGGRO_9 = -1300048,
	SAY_DRAGONFLAYER_AGGRO_0 = -1300049,
	SAY_SPIRITUALIST_AGGRO_1 = -1300050,
	SAY_SPIRITUALIST_AGGRO_2 = -1300051,
	SAY_SPIRITUALIST_AGGRO_3 = -1300052,
	SAY_SPIRITUALIST_AGGRO_4 = -1300053,
	SAY_SPIRITUALIST_AGGRO_5 = -1300054,
	SAY_SPIRITUALIST_AGGRO_6 = -1300055,
	SAY_SPIRITUALIST_AGGRO_7 = -1300056,
	SAY_SPIRITUALIST_AGGRO_8 = -1300057,
	SAY_SPIRITUALIST_AGGRO_9 = -1300058,
};

int32 DragonflayerSayAggro[] =
{
    SAY_DRAGONFLAYER_AGGRO_1,
	SAY_DRAGONFLAYER_AGGRO_2,
	SAY_DRAGONFLAYER_AGGRO_3,
	SAY_DRAGONFLAYER_AGGRO_4,
	SAY_DRAGONFLAYER_AGGRO_5,
	SAY_DRAGONFLAYER_AGGRO_6,
	SAY_DRAGONFLAYER_AGGRO_7,
	SAY_DRAGONFLAYER_AGGRO_8,
	SAY_DRAGONFLAYER_AGGRO_9,
	SAY_DRAGONFLAYER_AGGRO_0,
};

int32 SpiritualistSayAggro[] =
{
    SAY_SPIRITUALIST_AGGRO_1,
	SAY_SPIRITUALIST_AGGRO_2,
	SAY_SPIRITUALIST_AGGRO_3,
	SAY_SPIRITUALIST_AGGRO_4,
	SAY_SPIRITUALIST_AGGRO_5,
	SAY_SPIRITUALIST_AGGRO_6,
	SAY_SPIRITUALIST_AGGRO_7,
	SAY_SPIRITUALIST_AGGRO_8,
	SAY_SPIRITUALIST_AGGRO_9,
};

enum Misc
{
	PROTO_DRAKE_MOUNT_ID = 22657,
};

class npc_dragonflayer_ironhelm : public CreatureScript
{
public:
    npc_dragonflayer_ironhelm() : CreatureScript("npc_dragonflayer_ironhelm") {}

    struct npc_dragonflayer_ironhelmAI : public QuantumBasicAI
    {
        npc_dragonflayer_ironhelmAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RingingSlapTimer;
		uint32 HeroicStrikeTimer;

        void Reset()
        {
			RingingSlapTimer = 2000;
			HeroicStrikeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RingingSlapTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_RINGING_SLAP_5N, SPELL_RINGING_SLAP_5H));
				RingingSlapTimer = 4000;
			}
			else RingingSlapTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_HEROIC_STRIKE_5N, SPELL_HEROIC_STRIKE_5H));
				HeroicStrikeTimer = 6000;
			}
			else HeroicStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_ironhelmAI(creature);
    }
};

uint32 entry_search[3] =
{
    GO_GLOWING_ANVIL_1,
    GO_GLOWING_ANVIL_2,
    GO_GLOWING_ANVIL_3,
};

class npc_dragonflayer_forge_master : public CreatureScript
{
public:
    npc_dragonflayer_forge_master() : CreatureScript("npc_dragonflayer_forge_master") { }

    struct npc_dragonflayer_forge_masterAI : public QuantumBasicAI
    {
        npc_dragonflayer_forge_masterAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            FmType = 0;

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        }

        InstanceScript* instance;

        uint8 FmType;

		uint32 BurningBrandTimer;
		uint32 CauterizeTimer;

        void Reset()
        {
            if (FmType == 0)
				FmType = GetForgeMasterType();

			BurningBrandTimer = 2000;
			CauterizeTimer = 4000;

			CheckForge();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CUSTOM_SPELL_01);
        }

		void EnterToBattle(Unit* who)
        {
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_NONE);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
        }

        void JustDied(Unit* /*killer*/)
        {
            if (FmType == 0)
				FmType = GetForgeMasterType();

            if (instance)
            {
                switch (FmType)
                {
                    case 1:
						instance->SetData(EVENT_FORGE_1, DONE);
						break;
					case 2:
						instance->SetData(EVENT_FORGE_2, DONE);
						break;
					case 3:
						instance->SetData(EVENT_FORGE_3, DONE);
						break;
				}
			}
		}

        uint8 GetForgeMasterType()
        {
            float diff = 30.0f;
            uint8 Nearf = 0;

            for (uint8 i = 0; i < 3; ++i)
            {
                GameObject* temp;
                temp = me->FindGameobjectWithDistance(entry_search[i], 30.0f);
                if (temp)
                {
                    if (me->IsWithinDist(temp, diff, false))
                    {
                        Nearf = i + 1;
                        diff = me->GetDistance2d(temp);
                    }
                }
            }

            switch (Nearf)
            {
                case 1:
					return 1;
				case 2:
					return 2;
				case 3:
					return 3;
				default:
					return 0;
			}
        }

		void CheckForge()
        {
			if (instance)
			{
				switch (FmType)
                {
                case 1:
                    instance->SetData(EVENT_FORGE_1, me->IsAlive() ? NOT_STARTED : DONE);
                    break;
                case 2:
                    instance->SetData(EVENT_FORGE_2, me->IsAlive() ? NOT_STARTED : DONE);
                    break;
                case 3:
                    instance->SetData(EVENT_FORGE_3, me->IsAlive() ? NOT_STARTED : DONE);
                    break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (FmType == 0)
                FmType = GetForgeMasterType();

            if (!UpdateVictim())
                return;

			if (BurningBrandTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BURNING_BRAND_5N, SPELL_BURNING_BRAND_5H));
				BurningBrandTimer = 4000;
			}
			else BurningBrandTimer -= diff;

			if (CauterizeTimer <= diff)
			{
				DoCast(SPELL_CAUTERIZE);
				CauterizeTimer = 6000;
			}
			else CauterizeTimer -= diff;

            DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_forge_masterAI(creature);
	}
};

class npc_dragonflayer_metalworker : public CreatureScript
{
public:
    npc_dragonflayer_metalworker() : CreatureScript("npc_dragonflayer_metalworker") {}

    struct npc_dragonflayer_metalworkerAI : public QuantumBasicAI
    {
        npc_dragonflayer_metalworkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SunderArmorTimer;

        void Reset()
        {
			SunderArmorTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_SUNDER_ARMOR_5N, SPELL_SUNDER_ARMOR_5H));
				SunderArmorTimer = 2000;
			}
			else SunderArmorTimer -= diff;

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
        return new npc_dragonflayer_metalworkerAI(creature);
    }
};

class npc_dragonflayer_weaponsmith : public CreatureScript
{
public:
    npc_dragonflayer_weaponsmith() : CreatureScript("npc_dragonflayer_weaponsmith") {}

    struct npc_dragonflayer_weaponsmithAI : public QuantumBasicAI
    {
        npc_dragonflayer_weaponsmithAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DisarmTimer;
		uint32 CleaveTimer;
		uint32 ConcussionBlowTimer;

        void Reset()
        {
			DisarmTimer = 2000;
			CleaveTimer = 3000;
			ConcussionBlowTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DisarmTimer <= diff)
			{
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = urand(3000, 4000);
			}
			else DisarmTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(5000, 6000);
			}
			else CleaveTimer -= diff;

			if (ConcussionBlowTimer <= diff)
			{
				DoCastVictim(SPELL_CONCUSSION_BLOW);
				ConcussionBlowTimer = urand(7000, 8000);
			}
			else ConcussionBlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_weaponsmithAI(creature);
    }
};

class npc_dragonflayer_runecaster : public CreatureScript
{
public:
    npc_dragonflayer_runecaster() : CreatureScript("npc_dragonflayer_runecaster") {}

    struct npc_dragonflayer_runecasterAI : public QuantumBasicAI
    {
        npc_dragonflayer_runecasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RuneOfFlameTimer;
		uint32 RuneOfProtectionTimer;

        void Reset()
        {
			RuneOfFlameTimer = 500;
			RuneOfProtectionTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RuneOfFlameTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_RUNE_OF_FLAME_5N, SPELL_RUNE_OF_FLAME_5H), true);
				RuneOfFlameTimer = 4000;
			}
			else RuneOfFlameTimer -= diff;

			if (RuneOfProtectionTimer <= diff)
			{
				std::list<Creature*> FriendlyList = DoFindFriendlyMissingBuff(35.0f, DUNGEON_MODE(SPELL_RUNE_OF_PROTECTION_5N, SPELL_RUNE_OF_PROTECTION_5H));
				if (!FriendlyList.empty())
				{
					if (Unit* target = *(FriendlyList.begin()))
					{
						DoCast(target, DUNGEON_MODE(SPELL_RUNE_OF_PROTECTION_5N, SPELL_RUNE_OF_PROTECTION_5H), true);
						RuneOfProtectionTimer = 6000;
					}
                }
			}
			else RuneOfProtectionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_runecasterAI(creature);
    }
};

class npc_dragonflayer_strategist : public CreatureScript
{
public:
    npc_dragonflayer_strategist() : CreatureScript("npc_dragonflayer_strategist") {}

    struct npc_dragonflayer_strategistAI : public QuantumBasicAI
    {
        npc_dragonflayer_strategistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HurlDaggerTimer;
		uint32 TickingTimeBombTimer;
		uint32 BlindTimer;

        void Reset()
        {
			HurlDaggerTimer = 500;
			TickingTimeBombTimer = 2000;
			BlindTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HurlDaggerTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HURL_DAGGER_5N, SPELL_HURL_DAGGER_5H), true);
					HurlDaggerTimer = urand(3000, 4000);
				}
			}
			else HurlDaggerTimer -= diff;

			if (TickingTimeBombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SUMMON_TICKING_BOMB_5N, SPELL_SUMMON_TICKING_BOMB_5H), true);
					TickingTimeBombTimer = urand(5000, 6000);
				}
			}
			else TickingTimeBombTimer -= diff;

			if (BlindTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLIND);
					BlindTimer = urand(7000, 8000);
				}
			}
			else BlindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_strategistAI(creature);
    }
};

class npc_dragonflayer_overseer : public CreatureScript
{
public:
    npc_dragonflayer_overseer() : CreatureScript("npc_dragonflayer_overseer") {}

    struct npc_dragonflayer_overseerAI : public QuantumBasicAI
    {
        npc_dragonflayer_overseerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BattleShoutTimer;
		uint32 ChargeTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
        {
			BattleShoutTimer = 500;
			ChargeTimer = 2000;
			DemoralizingShoutTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCastVictim(DUNGEON_MODE(SPELL_CHARGE_5N, SPELL_CHARGE_5H));

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BATTLE_SHOUT_5N, SPELL_BATTLE_SHOUT_5H));
				BattleShoutTimer = 4000;
			}
			else BattleShoutTimer -= diff;

			if (ChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHARGE_5N, SPELL_CHARGE_5H), true);
					ChargeTimer = 6000;
				}
			}
			else ChargeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_DEMORALIZING_SHOUT_5N, SPELL_DEMORALIZING_SHOUT_5H));
				DemoralizingShoutTimer = 8000;
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_overseerAI(creature);
    }
};

class npc_dragonflayer_bonecrusher : public CreatureScript
{
public:
    npc_dragonflayer_bonecrusher() : CreatureScript("npc_dragonflayer_bonecrusher") {}

    struct npc_dragonflayer_bonecrusherAI : public QuantumBasicAI
    {
        npc_dragonflayer_bonecrusherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HeadCrackTimer;
		uint32 KnockdownSpinTimer;

        void Reset()
        {
			HeadCrackTimer = 2000;
			KnockdownSpinTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeadCrackTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_HEAD_CRACK_5N, SPELL_HEAD_CRACK_5H));
				HeadCrackTimer = 4000;
			}
			else HeadCrackTimer -= diff;

			if (KnockdownSpinTimer <= diff)
			{
				DoCast(me, SPELL_KNOCKDOWN_SPIN);
				KnockdownSpinTimer = 6000;
			}
			else KnockdownSpinTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_bonecrusherAI(creature);
    }
};

class npc_dragonflayer_heartsplitter : public CreatureScript
{
public:
    npc_dragonflayer_heartsplitter() : CreatureScript("npc_dragonflayer_heartsplitter") {}

    struct npc_dragonflayer_heartsplitterAI : public QuantumBasicAI
    {
        npc_dragonflayer_heartsplitterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThrowTimer;
		uint32 PiercingJabTimer;
		uint32 WingClipTimer;

        void Reset()
        {
			ThrowTimer = 500;
			PiercingJabTimer = 3000;
			WingClipTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
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
					DoCast(target, DUNGEON_MODE(SPELL_DH_THROW_5N, SPELL_DH_THROW_5H), true);
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (PiercingJabTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_PIERCING_JAB_5N, SPELL_PIERCING_JAB_5H));
				PiercingJabTimer = urand(5000, 6000);
			}
			else PiercingJabTimer -= diff;

			if (WingClipTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_WING_CLIP_5N, SPELL_WING_CLIP_5H));
				WingClipTimer = urand(7000, 8000);
			}
			else WingClipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_heartsplitterAI(creature);
    }
};

class npc_dragonflayer_spiritualist : public CreatureScript
{
public:
    npc_dragonflayer_spiritualist() : CreatureScript("npc_dragonflayer_spiritualist") {}

    struct npc_dragonflayer_spiritualistAI : public QuantumBasicAI
    {
        npc_dragonflayer_spiritualistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FlameShockTimer;
		uint32 LightningBoltTimer;
		uint32 HealingWaveTimer;

        void Reset()
        {
			FlameShockTimer = 500;
			LightningBoltTimer = 1000;
			HealingWaveTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(SpiritualistSayAggro)/sizeof(int32));
					DoSendQuantumText(SpiritualistSayAggro[TextID], me);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_SHOCK);
					FlameShockTimer = urand(3000, 4000);
				}
			}
			else FlameShockTimer -= diff;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LIGHTNING_BOLT);
					LightningBoltTimer = urand(5000, 6000);
				}
			}
			else LightningBoltTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_HEALING_WAVE);
					HealingWaveTimer = urand(7000, 8000);
				}
			}
			else HealingWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_spiritualistAI(creature);
    }
};

class npc_ticking_bomb : public CreatureScript
{
public:
    npc_ticking_bomb() : CreatureScript("npc_ticking_bomb") {}

    struct npc_ticking_bombAI : public QuantumBasicAI
    {
        npc_ticking_bombAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 ExplosionTimer;

        void Reset()
        {
			ExplosionTimer = 4000;

			DoCast(me, SPELL_TICKING_BOMB_AURA_5N, true);
			
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_EXPLOSION_PROC_5N)
			{
				DoCast(me, SPELL_EXPLOSION_VISUAL); 
				me->DespawnAfterAction();
			}
        }

        void UpdateAI(uint32 const diff)
        {
			if (ExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_EXPLOSION_PROC_5N, true);
				ExplosionTimer = 999999;
			}
			else ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ticking_bombAI(creature);
    }
};

class npc_ticking_time_bomb : public CreatureScript
{
public:
    npc_ticking_time_bomb() : CreatureScript("npc_ticking_time_bomb") {}

    struct npc_ticking_time_bombAI : public QuantumBasicAI
    {
        npc_ticking_time_bombAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		uint32 ExplosionTimer;

        void Reset()
        {
			ExplosionTimer = 4000;

			DoCast(me, SPELL_TICKING_BOMB_AURA_5H, true);
			
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_EXPLOSION_PROC_5H)
			{
				DoCast(me, SPELL_EXPLOSION_VISUAL); 
				me->DespawnAfterAction();
			}
        }

        void UpdateAI(uint32 const diff)
        {
			if (ExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_EXPLOSION_PROC_5H, true);
				ExplosionTimer = 999999;
			}
			else ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ticking_time_bombAI(creature);
    }
};

class npc_proto_drake_handler : public CreatureScript
{
public:
    npc_proto_drake_handler() : CreatureScript("npc_proto_drake_handler") {}

    struct npc_proto_drake_handlerAI : public QuantumBasicAI
    {
        npc_proto_drake_handlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThrowTimer;
		uint32 DebilitatingStrikeTimer;
		uint32 UnholyRageTimer;

        void Reset()
        {
			ThrowTimer = 500;
			DebilitatingStrikeTimer = 3000;
			UnholyRageTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
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
					DoCast(target, DUNGEON_MODE(SPELL_THROW_5N, SPELL_THROW_5H), true);
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (DebilitatingStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_DEBILITATING_STRIKE_5N, SPELL_DEBILITATING_STRIKE_5H));
				DebilitatingStrikeTimer = urand(5000, 6000);
			}
			else DebilitatingStrikeTimer -= diff;

			if (UnholyRageTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_UNHOLY_RAGE_5N, SPELL_UNHOLY_RAGE_5H));
				UnholyRageTimer = urand(7000, 8000);
			}
			else UnholyRageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_proto_drake_handlerAI(creature);
    }
};

class npc_proto_drake_rider : public CreatureScript
{
public:
    npc_proto_drake_rider() : CreatureScript("npc_proto_drake_rider") {}

    struct npc_proto_drake_riderAI : public QuantumBasicAI
    {
        npc_proto_drake_riderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThrowTimer;
		uint32 PiercingJabTimer;
		uint32 WingClipTimer;

		void Initialize()
		{
			me->SetCanFly(true);
			me->SetDisableGravity(true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_FLIGHT, true);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

			me->Mount(PROTO_DRAKE_MOUNT_ID);
		}

        void Reset()
        {
			Initialize();

			ThrowTimer = 500;
			PiercingJabTimer = 3000;
			WingClipTimer = 5000;
        }

		void EnterToBattle(Unit* who)
		{
			me->RemoveAurasDueToSpell(SPELL_FLIGHT);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
			me->RemoveMount();

			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if ((rand()%50))
				{
					int32 TextID = rand()% (sizeof(DragonflayerSayAggro)/sizeof(int32));
					DoSendQuantumText(DragonflayerSayAggro[TextID], me);
				}
			}
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
					DoCast(target, DUNGEON_MODE(SPELL_DH_THROW_5N, SPELL_DH_THROW_5H), true);
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (PiercingJabTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_PIERCING_JAB_5N, SPELL_PIERCING_JAB_5H));
				PiercingJabTimer = urand(5000, 6000);
			}
			else PiercingJabTimer -= diff;

			if (WingClipTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_WING_CLIP_5N, SPELL_WING_CLIP_5H));
				WingClipTimer = urand(7000, 8000);
			}
			else WingClipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_proto_drake_riderAI(creature);
    }
};

class npc_enslaved_proto_drake : public CreatureScript
{
public:
    npc_enslaved_proto_drake() : CreatureScript("npc_enslaved_proto_drake") {}

    struct npc_enslaved_proto_drakeAI : public QuantumBasicAI
    {
        npc_enslaved_proto_drakeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RendTimer;
		uint32 FlameBreathTimer;
		uint32 KnockAwayTimer;

        void Reset()
        {
			RendTimer = 1000;
			FlameBreathTimer = 2000;
			KnockAwayTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_REND_5N, SPELL_REND_5H));
				RendTimer = urand(3000, 4000);
			}
			else RendTimer -= diff;

			if (FlameBreathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 5.0f, true))
				{
					me->SetFacingToObject(target);
					DoCast(target, DUNGEON_MODE(SPELL_FLAME_BREATH_5N, SPELL_FLAME_BREATH_5H));
					FlameBreathTimer = urand(5000, 6000);
				}
			}
			else FlameBreathTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCK_AWAY);
				KnockAwayTimer = urand(7000, 8000);
			}
			else KnockAwayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enslaved_proto_drakeAI(creature);
    }
};

class npc_tunneling_ghoul : public CreatureScript
{
public:
    npc_tunneling_ghoul() : CreatureScript("npc_tunneling_ghoul") {}

    struct npc_tunneling_ghoulAI : public QuantumBasicAI
    {
        npc_tunneling_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StrikeTimer;
		uint32 DecrepifyTimer;

        void Reset()
        {
			StrikeTimer = 1000;
			DecrepifyTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_ATTACK_1H);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(3000, 4000);
			}
			else StrikeTimer -= diff;

			if (DecrepifyTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_DECREPIFY_5N, SPELL_DECREPIFY_5H));

				if (DoGetThreat(me->GetVictim()))
					DoModifyThreatPercent(me->GetVictim(), -100);

				DecrepifyTimer = urand(5000, 6000);
			}
			else DecrepifyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tunneling_ghoulAI(creature);
    }
};

class npc_frenzied_geist : public CreatureScript
{
public:
    npc_frenzied_geist() : CreatureScript("npc_frenzied_geist") {}

    struct npc_frenzied_geistAI : public QuantumBasicAI
    {
        npc_frenzied_geistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FixateTimer;

        void Reset()
        {
			FixateTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FixateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100.0f, true))
				{
					DoCast(target, SPELL_FIXATE);
					FixateTimer = 5000;
				}
			}
			else FixateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzied_geistAI(creature);
    }
};

class npc_savage_worg : public CreatureScript
{
public:
    npc_savage_worg() : CreatureScript("npc_savage_worg") {}

    struct npc_savage_worgAI : public QuantumBasicAI
    {
        npc_savage_worgAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EnrageTimer;
		uint32 PounceTimer;

        void Reset()
        {
			EnrageTimer = 500;
			PounceTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PounceTimer <= diff)
			{
				DoCast(SPELL_POUNCE);
				PounceTimer = 5000;
			}
			else PounceTimer -= diff;

			if (EnrageTimer <= diff)
			{
				DoCast(me, SPELL_SW_ENRAGE);
				EnrageTimer = 12500;
			}
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_savage_worgAI(creature);
    }
};

void AddSC_utgarde_keep_trash()
{
	new npc_dragonflayer_ironhelm();
	new npc_dragonflayer_forge_master();
	new npc_dragonflayer_metalworker();
	new npc_dragonflayer_weaponsmith();
	new npc_dragonflayer_runecaster();
	new npc_dragonflayer_strategist();
	new npc_dragonflayer_overseer();
	new npc_dragonflayer_bonecrusher();
	new npc_dragonflayer_heartsplitter();
	new npc_dragonflayer_spiritualist();
	new npc_ticking_bomb();
	new npc_ticking_time_bomb();
	new npc_proto_drake_handler();
	new npc_proto_drake_rider();
	new npc_enslaved_proto_drake();
	new npc_tunneling_ghoul();
	new npc_frenzied_geist();
	new npc_savage_worg();
}