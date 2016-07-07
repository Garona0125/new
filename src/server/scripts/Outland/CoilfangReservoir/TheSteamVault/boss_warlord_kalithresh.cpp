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
#include "steam_vault.h"

enum Texts
{
	SAY_INTRO   = -1545016,
	SAY_REGEN   = -1545017,
	SAY_AGGRO_1 = -1545018,
	SAY_AGGRO_2 = -1545019,
	SAY_AGGRO_3 = -1545020,
	SAY_SLAY_1  = -1545021,
	SAY_SLAY_2  = -1545022,
	SAY_DEATH   = -1545023,
};

enum Spells
{
	SPELL_SPELL_REFLECTION   = 31534,
	SPELL_IMPALE             = 39061,
	SPELL_WARLORDS_RAGE      = 37081,
	SPELL_WARLORDS_RAGE_NAGA = 31543,
	SPELL_WARLORDS_RAGE_PROC = 36453,
};

class npc_naga_distiller : public CreatureScript
{
public:
    npc_naga_distiller() : CreatureScript("npc_naga_distiller") { }

    struct npc_naga_distillerAI : public QuantumBasicAI
    {
        npc_naga_distillerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            if (instance)
            {
                if (instance->GetData(TYPE_DISTILLER) == IN_PROGRESS)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }
        }

        void EnterToBattle(Unit* /*who*/) {}

        void StartRageGen(Unit* /*caster*/)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            DoCast(me, SPELL_WARLORDS_RAGE_NAGA, true);

            if (instance)
                instance->SetData(TYPE_DISTILLER, IN_PROGRESS);
        }

        void DamageTaken(Unit* /*done_by*/, uint32 &damage)
        {
			if (me->GetHealth() <= damage)
				if (instance)
					instance->SetData(TYPE_DISTILLER, DONE);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_naga_distillerAI(creature);
    }
};

class boss_warlord_kalithresh : public CreatureScript
{
public:
    boss_warlord_kalithresh() : CreatureScript("boss_warlord_kalithresh") { }

    struct boss_warlord_kalithreshAI : public QuantumBasicAI
    {
        boss_warlord_kalithreshAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

		uint32 ImpaleTimer;
        uint32 ReflectionTimer;
        uint32 RageTimer;

        bool CanRage;

        void Reset()
        {
			ImpaleTimer = 500;
            ReflectionTimer = 3000;
            RageTimer = 45000;
            CanRage = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if (instance)
                instance->SetData(TYPE_WARLORD_KALITHRESH, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

            if (instance)
                instance->SetData(TYPE_WARLORD_KALITHRESH, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_WARLORDS_RAGE_PROC)
			{
                if (instance)
				{
                    if (instance->GetData(TYPE_DISTILLER) == DONE)
                        me->RemoveAurasDueToSpell(SPELL_WARLORDS_RAGE_PROC);
				}
			}
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(TYPE_WARLORD_KALITHRESH, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (ImpaleTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_IMPALE);
					ImpaleTimer = urand(3000, 4000);
				}
            }
			else ImpaleTimer -= diff;

			if (ReflectionTimer <= diff)
            {
                DoCast(me, SPELL_SPELL_REFLECTION);
                ReflectionTimer = urand(5000, 6000);
            }
			else ReflectionTimer -= diff;

            if (RageTimer <= diff)
            {
                if (Creature* distiller = me->FindCreatureWithDistance(17954, 100.0f))
                {
                    DoSendQuantumText(SAY_REGEN, me);
                    DoCast(me, SPELL_WARLORDS_RAGE);
                    CAST_AI(npc_naga_distiller::npc_naga_distillerAI, distiller->AI())->StartRageGen(me);
                }
                RageTimer = urand(7000, 8000);
            }
			else RageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_warlord_kalithreshAI(creature);
    }
};

void AddSC_boss_warlord_kalithresh()
{
	new npc_naga_distiller();
	new boss_warlord_kalithresh();
}