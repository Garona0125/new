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
#include "temple_of_ahnqiraj.h"

enum Spells
{
	SPELL_CLEAVE       = 26350,
	SPELL_TOXIC_VOLLEY = 25812,
	SPELL_POISON_CLOUD = 38718,
	SPELL_ENRAGE       = 34624,
	SPELL_CHARGE       = 26561,
	SPELL_KNOCKBACK    = 26027,
	SPELL_HEAL         = 25807,
	SPELL_FEAR         = 19408,
};

class boss_lord_kri : public CreatureScript
{
public:
    boss_lord_kri() : CreatureScript("boss_lord_kri") { }

    struct boss_lord_kriAI : public QuantumBasicAI
    {
        boss_lord_kriAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CleaveTimer;
        uint32 ToxicVolleyTimer;
        uint32 CheckTimer;

		bool VemDead;

        void Reset()
        {
            CleaveTimer = 3000;
            ToxicVolleyTimer = 4500;
            CheckTimer = 2000;

			VemDead = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_POISON_CLOUD);
		}

        void JustDied(Unit* /*killer*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(4000, 5000);
            }
			else CleaveTimer -= diff;

            if (ToxicVolleyTimer <= diff)
            {
                DoCastAOE(SPELL_TOXIC_VOLLEY);
                ToxicVolleyTimer = urand(7000, 8000);
            }
			else ToxicVolleyTimer -= diff;

			if (CheckTimer <= diff && !VemDead)
			{
				if (Creature* Vem = Unit::GetCreature(*me, instance->GetData64(DATA_VEM)))
				{
					if (!Vem->IsAlive())
					{
						DoCast(me, SPELL_ENRAGE);
						DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
						CheckTimer = 2000;
						VemDead = true;
					}
				}
			}
			else CheckTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lord_kriAI(creature);
    }
};

class boss_vem : public CreatureScript
{
public:
    boss_vem() : CreatureScript("boss_vem") { }

    struct boss_vemAI : public QuantumBasicAI
    {
        boss_vemAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 ChargeTimer;
        uint32 KnockBackTimer;
        uint32 EnrageTimer;

        bool Enraged;

        void Reset()
        {
            ChargeTimer = 500;
            KnockBackTimer = 4000;
            EnrageTimer = 120000;

            Enraged = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void JustDied(Unit* /*killer*/){}

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, SPELL_CHARGE);
                    AttackStart(target);
					ChargeTimer = urand(6000, 7000);
				}
            }
			else ChargeTimer -= diff;

            if (KnockBackTimer <= diff)
            {
                DoCastAOE(SPELL_KNOCKBACK);

                if (DoGetThreat(me->GetVictim()))
                    DoModifyThreatPercent(me->GetVictim(), -80);

                KnockBackTimer = urand(9000, 10000);
            }
			else KnockBackTimer -= diff;

			if (EnrageTimer <= diff && !Enraged)
            {
                DoCast(me, SPELL_ENRAGE);
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
                Enraged = true;
            }
			else ChargeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vemAI(creature);
    }
};

class boss_princess_yauj : public CreatureScript
{
public:
    boss_princess_yauj() : CreatureScript("boss_princess_yauj") { }

    struct boss_princess_yaujAI : public QuantumBasicAI
    {
        boss_princess_yaujAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

		uint32 FearTimer;
		uint32 CheckTimer;
        uint32 HealTimer;

        bool VemDead;

        void Reset()
        {
			CheckTimer = 2000;
			FearTimer = 4000;
            HealTimer = 30000;

            VemDead = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void JustDied(Unit* /*Killer*/)
        {
			for (uint8 i = 0; i < 10; ++i)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					Creature* Summoned = me->SummonCreature(NPC_YAUJ_BROOD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 90000);

					if (Summoned && target)
						Summoned->AI()->AttackStart(target);
				}
            }
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FearTimer <= diff)
            {
                DoCastAOE(SPELL_FEAR);
                DoResetThreat();
                FearTimer = 10000;
            }
			else FearTimer -= diff;

            if (HealTimer <= diff)
            {
				Unit* kri = Unit::GetUnit(*me, instance->GetData64(DATA_LORD_KRI));
				Unit* vem = Unit::GetUnit(*me, instance->GetData64(DATA_VEM));

				switch (urand(0, 2))
				{
                    case 0:
						if (kri)
							DoCast(kri, SPELL_HEAL);
						break;
					case 1:
						if (vem)
							DoCast(vem, SPELL_HEAL);
						break;
					case 2:
						DoCast(me, SPELL_HEAL);
						break;
				}
                HealTimer = 15000+rand()%15000;
            }
			else HealTimer -= diff;

			if (CheckTimer <= diff && !VemDead)
			{
				if (Creature* Vem = Unit::GetCreature(*me, instance->GetData64(DATA_VEM)))
				{
					if (!Vem->IsAlive())
					{
						DoCast(me, SPELL_ENRAGE);
						DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
						CheckTimer = 2000;
						VemDead = true;
					}
				}
			}
			else CheckTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_princess_yaujAI(creature);
    }
};

void AddSC_bug_trio()
{
    new boss_lord_kri();
    new boss_vem();
    new boss_princess_yauj();
}