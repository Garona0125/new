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

enum TyraelSpells
{
	SPELL_HOLY_REFLECTION = 34381,
	SPELL_HOLYFORM        = 46565,
	SPELL_HOLY_AURA       = 70788,
	SPELL_LIGHTS_BLESSING = 71797,
	SPELL_HOLY_CHAMPION   = 71954,
	SPELL_LIGHTS_FAVOR    = 69382,

	SPELL_SUNBEAM         = 62872,
	SPELL_HOLY_SMITE      = 62443,
	SPELL_HOLY_BOLT       = 38838,
};

class npc_tyrael_archangel_of_justice : public CreatureScript
{
public:
    npc_tyrael_archangel_of_justice() : CreatureScript("npc_tyrael_archangel_of_justice") {}

    struct npc_tyrael_archangel_of_justiceAI : public QuantumBasicAI
    {
        npc_tyrael_archangel_of_justiceAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SunbeamTimer;
		uint32 HolySmiteTimer;
		uint32 HolyBoltTimer;

        void Reset()
		{
			SunbeamTimer = 1000;
			HolySmiteTimer = 2000;
			HolyBoltTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_HOLY_AURA, true);
			DoCast(me, SPELL_LIGHTS_FAVOR, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			if (!me->HasAura(SPELL_HOLY_CHAMPION))
				DoCast(me, SPELL_HOLY_CHAMPION, true);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SunbeamTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SUNBEAM, true);
					SunbeamTimer = urand(3000, 4000);
				}
			}
			else SunbeamTimer -= diff;

			if (HolySmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HOLY_SMITE, true);
					HolySmiteTimer = urand(5000, 6000);
				}
			}
			else HolySmiteTimer -= diff;

			if (HolyBoltTimer <= diff)
			{
				DoCastAOE(SPELL_HOLY_BOLT, true);
				HolyBoltTimer = urand(7000, 8000);
			}
			else HolyBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tyrael_archangel_of_justiceAI(creature);
    }
};

void AddSC_battle_tyrael()
{
	new npc_tyrael_archangel_of_justice();
}