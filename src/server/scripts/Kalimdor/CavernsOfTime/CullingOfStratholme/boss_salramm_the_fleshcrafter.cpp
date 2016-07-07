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
#include "culling_of_stratholme.h"

enum Texts
{
    SAY_AGGRO            = -1595032,
    SAY_SPAWN            = -1595033,
    SAY_SLAY_1           = -1595034,
    SAY_SLAY_2           = -1595035,
    SAY_SLAY_3           = -1595036,
    SAY_DEATH            = -1595037,
    SAY_EXPLODE_GHOUL_1  = -1595038,
    SAY_EXPLODE_GHOUL_2  = -1595039,
    SAY_STEAL_FLESH_1    = -1595040,
    SAY_STEAL_FLESH_2    = -1595041,
    SAY_STEAL_FLESH_3    = -1595042,
    SAY_SUMMON_GHOULS_1  = -1595043,
    SAY_SUMMON_GHOULS_2  = -1595044,
};

enum Spells
{
    SPELL_CURSE_OF_TWISTED_FLESH      = 58845,
    SPELL_EXPLODE_GHOUL_5N            = 52480,
    SPELL_EXPLODE_GHOUL_5H            = 58825,
    SPELL_SHADOW_BOLT_5N              = 57725,
    SPELL_SHADOW_BOLT_5H              = 58827,
    SPELL_STEAL_FLESH                 = 52708,
    SPELL_STEAL_FLESH_VICTIM          = 52711,
    SPELL_STEAL_FLESH_SELF            = 52712,
    SPELL_SUMMON_GHOULS               = 52451,
};

class boss_salramm_the_fleshcrafter : public CreatureScript
{
public:
    boss_salramm_the_fleshcrafter() : CreatureScript("boss_salramm_the_fleshcrafter") { }

    struct boss_salramm_the_fleshcrafterAI : public QuantumBasicAI
    {
        boss_salramm_the_fleshcrafterAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			DoSendQuantumText(SAY_SPAWN, me);
        }

        uint32 CurseFleshTimer;
        uint32 ExplodeGhoulTimer;
        uint32 ShadowBoltTimer;
        uint32 StealFleshTimer;
        uint32 SummonGhoulsTimer;

        InstanceScript* instance;

        void Reset()
        {
			CurseFleshTimer = 10000;
			ExplodeGhoulTimer = 50000;
			ShadowBoltTimer = urand(8000, 12000);
			StealFleshTimer = 15000;
			SummonGhoulsTimer = urand(15000, 20000);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_SALRAMM_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_SALRAMM_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_SALRAMM_EVENT, DONE);
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetCorpseDelay(0);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) 
        {
            if (spell->Id == SPELL_STEAL_FLESH)
            {
                DoCast(target, SPELL_STEAL_FLESH_VICTIM, true);
                DoCast(me, SPELL_STEAL_FLESH_SELF, true);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

			if (CurseFleshTimer <= diff && IsHeroic())
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 50, true))
				{
					DoCast(target, SPELL_CURSE_OF_TWISTED_FLESH);
					CurseFleshTimer = 25000;
				}
			}
			else CurseFleshTimer -= diff;

            if (ShadowBoltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(8000, 10000);
				}
            }
			else ShadowBoltTimer -= diff;

            if (StealFleshTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 50, true))
				{
					DoSendQuantumText(RAND(SAY_STEAL_FLESH_1, SAY_STEAL_FLESH_2, SAY_STEAL_FLESH_3), me);
                    DoCast(target, SPELL_STEAL_FLESH);
					StealFleshTimer = 15000;
				}
            }
			else StealFleshTimer -= diff;

            if (SummonGhoulsTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 20, false))
				{
					DoSendQuantumText(RAND(SAY_SUMMON_GHOULS_1, SAY_SUMMON_GHOULS_2), me);
                    DoCast(target, SPELL_SUMMON_GHOULS);
					SummonGhoulsTimer = urand(18000, 22000);
					ExplodeGhoulTimer = urand(8000, 10000);
				}
            }
			else SummonGhoulsTimer -= diff;

            if (ExplodeGhoulTimer <= diff)
            {
                DoSendQuantumText(RAND(SAY_EXPLODE_GHOUL_1, SAY_EXPLODE_GHOUL_2), me);
                DoCast(DUNGEON_MODE(SPELL_EXPLODE_GHOUL_5N, SPELL_EXPLODE_GHOUL_5H));
                ExplodeGhoulTimer = urand(6000, 8000);
            }
			else ExplodeGhoulTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_salramm_the_fleshcrafterAI(creature);
    }
};

void AddSC_boss_salramm_the_fleshcrafter()
{
    new boss_salramm_the_fleshcrafter();
}