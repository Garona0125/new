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

enum GuardSpells
{
	SPELL_ENRAGE         = 8599,
	SPELL_HAMSTRING      = 9080,
	SPELL_SHIELD_BASH    = 11972,
	SPELL_SHIELD_BLOCK   = 12169,
	SPELL_REVENGE        = 12170,
	SPELL_STRIKE         = 14516,
	SPELL_ALL_CLEAVE     = 15284,
	SPELL_CLEAVE         = 40505,
};

enum GuardTexts
{
	SAY_OG_AGGRO_1       = -1520770,
	SAY_OG_AGGRO_2       = -1520771,
	SAY_OG_AGGRO_3       = -1520772,
	SAY_OG_AGGRO_4       = -1520773,
	SAY_SG_AGGRO_1       = -1520774,
	SAY_SG_AGGRO_2       = -1520775,
	SAY_SG_AGGRO_3       = -1520776,
	SAY_SG_AGGRO_4       = -1520777,
	SAY_SG_AGGRO_5       = -1520778,
	SAY_SG_AGGRO_6       = -1520779,
	SAY_SG_AGGRO_7       = -1520780,
	SAY_SG_AGGRO_8       = -1520781,
	SAY_KO_AGGRO_1       = -1520782,
	SAY_KO_AGGRO_2       = -1520783,
	SAY_RD_AGGRO_1       = -1520784,
	SAY_RD_AGGRO_2       = -1520785,

	NPC_VOIDWALKER       = 5676,    
};

class npc_orgrimmar_grunt : public CreatureScript
{
public:
    npc_orgrimmar_grunt() : CreatureScript("npc_orgrimmar_grunt") {}

    struct npc_orgrimmar_gruntAI : public QuantumBasicAI
    {
        npc_orgrimmar_gruntAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_VOIDWALKER)
				return;
		}

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_OG_AGGRO_1, SAY_OG_AGGRO_2, SAY_OG_AGGRO_3, SAY_OG_AGGRO_4), me);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void ReceiveEmote(Player* /*player*/, uint32 emote)
		{
			if (emote == TEXT_EMOTE_SALUTE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);

			if (emote == TEXT_EMOTE_WAVE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);

			if (emote == TEXT_EMOTE_CHEER)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

			if (emote == TEXT_EMOTE_KISS)
				me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);

			if (emote == TEXT_EMOTE_RUDE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);

			if (emote == TEXT_EMOTE_CHICKEN)
				me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);

			if (emote == TEXT_EMOTE_SHY)
				me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

			if (emote == TEXT_EMOTE_POINT)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);

			if (emote == TEXT_EMOTE_DANCE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
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
        return new npc_orgrimmar_gruntAI(creature);
    }
};

class npc_silvermoon_city_guardian : public CreatureScript
{
public:
    npc_silvermoon_city_guardian() : CreatureScript("npc_silvermoon_city_guardian") {}

    struct npc_silvermoon_city_guardianAI : public QuantumBasicAI
    {
        npc_silvermoon_city_guardianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void ReceiveEmote(Player* /*player*/, uint32 emote)
		{
			if (emote == TEXT_EMOTE_SALUTE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);

			if (emote == TEXT_EMOTE_WAVE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);

			if (emote == TEXT_EMOTE_CHEER)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

			if (emote == TEXT_EMOTE_KISS)
				me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);

			if (emote == TEXT_EMOTE_RUDE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);

			if (emote == TEXT_EMOTE_CHICKEN)
				me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);

			if (emote == TEXT_EMOTE_SHY)
				me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

			if (emote == TEXT_EMOTE_POINT)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);

			if (emote == TEXT_EMOTE_DANCE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
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
        return new npc_silvermoon_city_guardianAI(creature);
    }
};

class npc_stormwind_city_guard : public CreatureScript
{
public:
    npc_stormwind_city_guard() : CreatureScript("npc_stormwind_city_guard") {}

    struct npc_stormwind_city_guardAI : public QuantumBasicAI
    {
        npc_stormwind_city_guardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 ShieldBlockTimer;
		uint32 RevengeTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
			ShieldBlockTimer = 4000;
			RevengeTimer = 6000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_SG_AGGRO_1, SAY_SG_AGGRO_2, SAY_SG_AGGRO_3, SAY_SG_AGGRO_4, SAY_SG_AGGRO_5, SAY_SG_AGGRO_6, SAY_SG_AGGRO_7, SAY_SG_AGGRO_8), me);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void ReceiveEmote(Player* /*player*/, uint32 emote)
		{
			if (emote == TEXT_EMOTE_SALUTE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);

			if (emote == TEXT_EMOTE_WAVE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);

			if (emote == TEXT_EMOTE_CHEER)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

			if (emote == TEXT_EMOTE_KISS)
				me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);

			if (emote == TEXT_EMOTE_RUDE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);

			if (emote == TEXT_EMOTE_CHICKEN)
				me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);

			if (emote == TEXT_EMOTE_SHY)
				me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

			if (emote == TEXT_EMOTE_POINT)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);

			if (emote == TEXT_EMOTE_DANCE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_SHIELD_BLOCK);
				ShieldBlockTimer = urand(5000, 6000);
			}
			else ShieldBlockTimer -= diff;

			if (RevengeTimer <= diff)
			{
				DoCastVictim(SPELL_REVENGE);
				RevengeTimer = urand(7000, 8000);
			}
			else RevengeTimer -= diff;
			
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
        return new npc_stormwind_city_guardAI(creature);
    }
};

class npc_korkron_overseer : public CreatureScript
{
public:
    npc_korkron_overseer() : CreatureScript("npc_korkron_overseer") {}

    struct npc_korkron_overseerAI : public QuantumBasicAI
    {
        npc_korkron_overseerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_KO_AGGRO_1, SAY_KO_AGGRO_2), me);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void ReceiveEmote(Player* /*player*/, uint32 emote)
		{
			if (emote == TEXT_EMOTE_SALUTE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);

			if (emote == TEXT_EMOTE_WAVE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);

			if (emote == TEXT_EMOTE_CHEER)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

			if (emote == TEXT_EMOTE_KISS)
				me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);

			if (emote == TEXT_EMOTE_RUDE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);

			if (emote == TEXT_EMOTE_CHICKEN)
				me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);

			if (emote == TEXT_EMOTE_SHY)
				me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

			if (emote == TEXT_EMOTE_POINT)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);

			if (emote == TEXT_EMOTE_DANCE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
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
        return new npc_korkron_overseerAI(creature);
    }
};

class npc_royal_dreadguard : public CreatureScript
{
public:
    npc_royal_dreadguard() : CreatureScript("npc_royal_dreadguard") {}

    struct npc_royal_dreadguardAI : public QuantumBasicAI
    {
        npc_royal_dreadguardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HamstringTimer;
		uint32 ShieldBashTimer;
		uint32 StrikeTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			HamstringTimer = 1000;
			ShieldBashTimer = 3000;
			StrikeTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_RD_AGGRO_1, SAY_RD_AGGRO_2), me);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void ReceiveEmote(Player* /*player*/, uint32 emote)
		{
			if (emote == TEXT_EMOTE_SALUTE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);

			if (emote == TEXT_EMOTE_WAVE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);

			if (emote == TEXT_EMOTE_CHEER)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

			if (emote == TEXT_EMOTE_KISS)
				me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);

			if (emote == TEXT_EMOTE_RUDE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);

			if (emote == TEXT_EMOTE_CHICKEN)
				me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);

			if (emote == TEXT_EMOTE_SHY)
				me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

			if (emote == TEXT_EMOTE_POINT)
				me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);

			if (emote == TEXT_EMOTE_DANCE)
				me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = urand(3000, 4000);
			}
			else HamstringTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(5000, 6000);
			}
			else ShieldBashTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(6000, 7000);
			}
			else StrikeTimer -= diff;

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
        return new npc_royal_dreadguardAI(creature);
    }
};

void AddSC_alliance_and_horde_guards()
{
	new npc_orgrimmar_grunt();
	new npc_silvermoon_city_guardian();
	new npc_stormwind_city_guard();
	new npc_korkron_overseer();
	new npc_royal_dreadguard();
}