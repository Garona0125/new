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
    SAY_AGGRO   = -1595026,
    SAY_SLAY_1  = -1595027,
    SAY_SLAY_2  = -1595028,
    SAY_SLAY_3  = -1595029,
    SAY_SPAWN   = -1595030,
    SAY_DEATH   = -1595031,
};

enum Spells
{
    SPELL_CONSTRICTING_CHAINS_5N       = 52696,
    SPELL_CONSTRICTING_CHAINS_5H       = 58823,
    SPELL_DISEASE_EXPULSION_5N         = 52666,
    SPELL_DISEASE_EXPULSION_5H         = 58824,
    SPELL_FRENZY                       = 58841,
};

class boss_meathook : public CreatureScript
{
public:
    boss_meathook() : CreatureScript("boss_meathook") { }

    struct boss_meathookAI : public QuantumBasicAI
    {
        boss_meathookAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			DoSendQuantumText(SAY_SPAWN, me);
        }

		InstanceScript* instance;

        uint32 ConstrictingChainTimer;
        uint32 DiseaseExpulsionTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            ConstrictingChainTimer = 2*IN_MILLISECONDS;
            DiseaseExpulsionTimer = 3*IN_MILLISECONDS;
            FrenzyTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_MEATHOOK_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_MEATHOOK_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_MEATHOOK_EVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DiseaseExpulsionTimer <= diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_DISEASE_EXPULSION_5N, SPELL_DISEASE_EXPULSION_5H));
                DiseaseExpulsionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
            }
			else DiseaseExpulsionTimer -= diff;

            if (FrenzyTimer <= diff)
            {
				DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
                DoCast(me, SPELL_FRENZY);
                FrenzyTimer = 10*IN_MILLISECONDS;
            }
			else FrenzyTimer -= diff;

            if (ConstrictingChainTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_CONSTRICTING_CHAINS_5N, SPELL_CONSTRICTING_CHAINS_5H));
					ConstrictingChainTimer = 15*IN_MILLISECONDS;
				}
            }
			else ConstrictingChainTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_meathookAI(creature);
    }
};

void AddSC_boss_meathook()
{
    new boss_meathook();
}