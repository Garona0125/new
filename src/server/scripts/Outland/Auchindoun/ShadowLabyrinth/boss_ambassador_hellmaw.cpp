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
#include "shadow_labyrinth.h"

enum Texts
{
    SAY_INTRO   = -1555000,
    SAY_AGGRO_1 = -1555001,
    SAY_AGGRO_2 = -1555002,
    SAY_AGGRO_3 = -1555003,
    SAY_HELP    = -1555004,
    SAY_SLAY_1  = -1555005,
    SAY_SLAY_2  = -1555006,
    SAY_DEATH   = -1555007,
};

enum Spells
{
    SPELL_BANISH         = 30231,
    SPELL_CORROSIVE_ACID = 33551,
    SPELL_FEAR           = 33547,
    SPELL_ENRAGE         = 34970,
};

class boss_ambassador_hellmaw : public CreatureScript
{
public:
    boss_ambassador_hellmaw() : CreatureScript("boss_ambassador_hellmaw") { }

    struct boss_ambassador_hellmawAI : public QuantumBasicAI
    {
        boss_ambassador_hellmawAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 EventCheckTimer;
        uint32 CorrosiveAcidTimer;
        uint32 FearTimer;
        uint32 EnrageTimer;

        bool Intro;
        bool IsBanished;
        bool Enraged;

        void Reset()
        {
            CorrosiveAcidTimer = 2000;
            FearTimer = 4000;
            EnrageTimer = 180000;

            Intro = false;
            IsBanished = true;
            Enraged = false;

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, false);

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_BANISH, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_HELLMAW, NOT_STARTED);
        }

        void JustReachedHome()
        {
			instance->SetData(DATA_HELLMAW, FAIL);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 35.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 35.0f))
				DoIntro();
		}

        void DoIntro()
        {
            if (me->HasAura(SPELL_BANISH))
                me->RemoveAurasDueToSpell(SPELL_BANISH);

            IsBanished = false;

            Intro = true;

			if (instance->GetData(DATA_HELLMAW) != FAIL)
				DoSendQuantumText(SAY_INTRO, me);

			instance->SetData(DATA_HELLMAW, IN_PROGRESS);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);

			instance->SetData(DATA_HELLMAW, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*victim*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_HELLMAW, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

            if (CorrosiveAcidTimer <= diff)
            {
                DoCast(SPELL_CORROSIVE_ACID);
                CorrosiveAcidTimer = urand(6000, 7000);
            }
			else CorrosiveAcidTimer -= diff;

            if (FearTimer <= diff)
            {
                DoCastAOE(SPELL_FEAR);
                FearTimer = urand(9000, 10000);
            }
			else FearTimer -= diff;

			if (EnrageTimer <= diff && !Enraged && IsHeroic())
			{
				DoCast(me, SPELL_ENRAGE);
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				Enraged = true;
			}
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ambassador_hellmawAI(creature);
    }
};

void AddSC_boss_ambassador_hellmaw()
{
    new boss_ambassador_hellmaw();
}