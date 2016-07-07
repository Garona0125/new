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
#include "Group.h"
#include "temple_of_ahnqiraj.h"

enum Texts
{
	SAY_AGGRO_1                = -1531000,
	SAY_AGGRO_2                = -1531001,
	SAY_AGGRO_3                = -1531002,
	SAY_SLAY_1                 = -1531003,
	SAY_SLAY_2                 = -1531004,
	SAY_SLAY_3                 = -1531005,
	SAY_SPLIT                  = -1531006,
	SAY_DEATH                  = -1531007,
};

enum Spells
{
	SPELL_ARCANE_EXPLOSION     = 26192,
	SPELL_EARTH_SHOCK          = 26194,
	SPELL_TRUE_FULFILLMENT     = 785,
	SPELL_BLINK                = 28391,
};

class MyCoordinates
{
public:
	float x, y, z, r;
	MyCoordinates(float cx, float cy, float cz, float cr)
	{
		x = cx; y = cy; z = cz; r = cr;
	}
};

class boss_prophet_skeram : public CreatureScript
{
public:
    boss_prophet_skeram() : CreatureScript("boss_prophet_skeram") { }

    struct boss_prophet_skeramAI : public QuantumBasicAI
    {
        boss_prophet_skeramAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
            IsImage = false;
        }

		InstanceScript* instance;

        uint32 ArcaneExplosionTimer;
        uint32 EarthShockTimer;
        uint32 FullFillmentTimer;
        uint32 BlinkTimer;
        uint32 InvisibleTimer;

        bool Images75;
        bool Images50;
        bool Images25;
        bool IsImage;
        bool Invisible;

        void Reset()
        {
			InvisibleTimer = 500;
            ArcaneExplosionTimer = 4000;
            EarthShockTimer = 2000;
			BlinkTimer = urand(8000, 20000);
            FullFillmentTimer = 15000;

            Images75 = false;
            Images50 = false;
            Images25 = false;
            Invisible = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisible(true);

			instance->SetData(DATA_PROPHET_SKERAM, NOT_STARTED);

            if (IsImage)
                me->setDeathState(JUST_DIED);
        }

		void EnterToBattle(Unit* who)
        {
			instance->SetData(DATA_PROPHET_SKERAM, IN_PROGRESS);

            if (IsImage || Images75)
                return;

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
		}

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void JustDied(Unit* /*Killer*/)
        {
			instance->SetData(DATA_PROPHET_SKERAM, DONE);

            if (!IsImage)
                DoSendQuantumText(SAY_DEATH, me);
        }

		void DoSplit(int atPercent /*75 50 25*/)
        {
            DoSendQuantumText(SAY_SPLIT, me);

            MyCoordinates* place1 = new MyCoordinates(-8340.782227f, 2083.814453f, 125.648788f, 0);
            MyCoordinates* place2 = new MyCoordinates(-8341.546875f, 2118.504639f, 133.058151f, 0);
            MyCoordinates* place3 = new MyCoordinates(-8318.822266f, 2058.231201f, 133.058151f, 0);
            MyCoordinates* bossc = place1, *i1 = place2, *i2 = place3;

            switch (urand(0, 2))
            {
                case 0:
                    bossc = place1;
                    i1 = place2;
                    i2 = place3;
                    break;
                case 1:
                    bossc = place2;
                    i1 = place1;
                    i2 = place3;
                    break;
                case 2:
                    bossc = place3;
                    i1 = place1;
                    i2 = place2;
                    break;
            }

            for (uint16 i = 0; i < 41; ++i)
            {
                if (Player* target = CAST_PLR(SelectTarget(TARGET_RANDOM, 0, 100, true)))
                {
                    if (Group* group = target->GetGroup())
					{
                        for (uint8 ico = 0; ico < TARGET_ICON_COUNT; ++ico)
                        {
                            //if (grp->m_targetIcons[ico] == me->GetGUID()) -- private member :(
                            group->SetTargetIcon(ico, 0, 0);
                        }
					}
                    break;
                }
            }

            me->RemoveAllAuras();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisible(false);
            me->SetPosition(bossc->x, bossc->y, bossc->z, bossc->r);
            Invisible = true;
            DoResetThreat();
            DoStopAttack();

            switch (atPercent)
            {
                case 75:
					Images75 = true;
					break;
                case 50:
					Images50 = true;
					break;
                case 25:
					Images25 = true;
					break;
            }

            Unit* target = SelectTarget(TARGET_RANDOM, 0);

            Creature* Image1 = me->SummonCreature(NPC_PROPHET_SKERAM, i1->x, i1->y, i1->z, i1->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
            if (Image1)
            {
                Image1->SetMaxHealth(me->GetMaxHealth() / 5);
                Image1->SetHealth(me->GetHealth() / 5);
                if (target)
                    Image1->AI()->AttackStart(target);
                CAST_AI(boss_prophet_skeram::boss_prophet_skeramAI, Image1->AI())->IsImage = true;
            }

            Creature* Image2 = me->SummonCreature(NPC_PROPHET_SKERAM, i2->x, i2->y, i2->z, i2->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
            if (Image2)
            {
                Image2->SetMaxHealth(me->GetMaxHealth() / 5);
                Image2->SetHealth(me->GetHealth() / 5);
                if (target)
                    Image2->AI()->AttackStart(target);
                CAST_AI(boss_prophet_skeram::boss_prophet_skeramAI, Image2->AI())->IsImage = true;
            }
            Invisible = true;

            delete place1;
            delete place2;
            delete place3;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArcaneExplosionTimer <= diff)
            {
                DoCastAOE(SPELL_ARCANE_EXPLOSION);
                ArcaneExplosionTimer = urand(8000, 18000);
            }
			else ArcaneExplosionTimer -= diff;

			if (FullFillmentTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 45, true))
				{
					DoCast(target, SPELL_TRUE_FULFILLMENT);
					FullFillmentTimer = urand(20000, 22000);
				}
            }
			else FullFillmentTimer -= diff;

            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->IsAttackReady() && !me->IsNonMeleeSpellCasted(false))
                {
                    me->AttackerStateUpdate(me->GetVictim());
                    me->ResetAttackTimer();
                }
            }
			else
            {
                if (EarthShockTimer <= diff)
                {
                    DoCastVictim(SPELL_EARTH_SHOCK);
                    EarthShockTimer = 1000;
                }
				else EarthShockTimer -= diff;
            }

            if (BlinkTimer <= diff)
            {
                //DoCast(me, SPELL_BLINK);
                switch (urand(0, 2))
                {
                    case 0:
                        me->SetPosition(-8340.782227f, 2083.814453f, 125.648788f, 0.0f);
                        DoResetThreat();
                        break;
                    case 1:
                        me->SetPosition(-8341.546875f, 2118.504639f, 133.058151f, 0.0f);
                        DoResetThreat();
                        break;
                    case 2:
                        me->SetPosition(-8318.822266f, 2058.231201f, 133.058151f, 0.0f);
                        DoResetThreat();
                        break;
                }
                DoStopAttack();

                BlinkTimer= urand(20000, 40000);
            }
			else BlinkTimer -= diff;

            int procent = (int) (me->GetHealthPct() + 0.5f);

            if ((!Images75 && !IsImage) && (procent <= 75 && procent > 70))
                DoSplit(75);

            if ((!Images50 && !IsImage) && (procent <= 50 && procent > 45))
                DoSplit(50);

            if ((!Images25 && !IsImage) && (procent <= 25 && procent > 20))
                DoSplit(25);

            if (Invisible)
            {
                if (InvisibleTimer <= diff)
                {
                    me->SetVisible(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    InvisibleTimer = 2500;
                    Invisible = false;
                }
				else InvisibleTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_prophet_skeramAI(creature);
    }
};

void AddSC_boss_prophet_skeram()
{
    new boss_prophet_skeram();
}