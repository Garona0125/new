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
#include "shattered_halls.h"

enum Say
{
    YELL_DIE_L                  = -1540039,
    YELL_DIE_R                  = -1540040,
    EMOTE_ENRAGE                = -1540041,
};

enum Spells
{
    SPELL_BLAST_WAVE            = 30600,
    SPELL_FEAR                  = 30584,
    SPELL_THUNDERCLAP           = 30633,
    SPELL_BURNING_MAUL_5N       = 30598,
    SPELL_BURNING_MAUL_5H       = 36056,
};

struct Yell
{
    int32 id;
    uint32 creature;
};

static Yell GoCombat[] =
{
    {-1540018, NPC_LEFT_HEAD},
    {-1540019, NPC_LEFT_HEAD},
    {-1540020, NPC_LEFT_HEAD},
};

static Yell GoCombatDelay[] =
{
    {-1540021, NPC_RIGHT_HEAD},
    {-1540022, NPC_RIGHT_HEAD},
    {-1540023, NPC_RIGHT_HEAD},
};

static Yell Threat[] =
{
    {-1540024, NPC_LEFT_HEAD},
    {-1540025, NPC_RIGHT_HEAD},
    {-1540026, NPC_LEFT_HEAD},
    {-1540027, NPC_LEFT_HEAD},
};

static Yell ThreatDelay1[] =
{
    {-1540028, NPC_RIGHT_HEAD},
    {-1540029, NPC_LEFT_HEAD},
    {-1540030, NPC_RIGHT_HEAD},
    {-1540031, NPC_RIGHT_HEAD},
};

static Yell ThreatDelay2[] =
{
    {-1540032, NPC_LEFT_HEAD},
    {-1540033, NPC_RIGHT_HEAD},
    {-1540034, NPC_LEFT_HEAD},
    {-1540035, NPC_LEFT_HEAD},
};

static Yell Killing[] =
{
    {-1540036, NPC_LEFT_HEAD},
    {-1540037, NPC_RIGHT_HEAD},
};

static Yell KillingDelay[] =
{
    {-1540038, NPC_RIGHT_HEAD},
    {-1000000, NPC_LEFT_HEAD},
};

class mob_omrogg_heads : public CreatureScript
{
public:
	mob_omrogg_heads() : CreatureScript("mob_omrogg_heads") { }

	struct mob_omrogg_headsAI : public QuantumBasicAI
	{
		mob_omrogg_headsAI(Creature* creature) : QuantumBasicAI(creature) { }

		bool DeathYell;
		uint32 DeathTimer;

		void Reset()
		{
			DeathTimer = 4000;
			DeathYell = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
		}

		void EnterToBattle(Unit* /*who*/) {}

		void DoDeathYell()
		{
			DeathYell = true;
		}

		void UpdateAI(const uint32 diff)
		{
			if (!DeathYell)
				return;

			if (DeathTimer <= diff)
			{
				DoSendQuantumText(YELL_DIE_R, me);
				DeathTimer = false;
				me->setDeathState(JUST_DIED);
			}
			else DeathTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new mob_omrogg_headsAI(creature);
	}
};

class boss_warbringer_omrogg : public CreatureScript
{
    public:
        boss_warbringer_omrogg() : CreatureScript("boss_warbringer_omrogg") { }

        struct boss_warbringer_omroggAI : public QuantumBasicAI
        {
            boss_warbringer_omroggAI(Creature* creature) : QuantumBasicAI(creature)
            {
                LeftHeadGUID  = 0;
                RightHeadGUID = 0;
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint64 LeftHeadGUID;
            uint64 RightHeadGUID;
            int iaggro;
            int ithreat;
            int ikilling;

            bool AggroYell;
            bool ThreatYell;
            bool ThreatYell2;
            bool KillingYell;

            uint32 DelayTimer;
            uint32 BlastWaveTimer;
            uint32 BlastCount;
            uint32 FearTimer;
            uint32 BurningMaulTimer;
            uint32 ThunderClapTimer;
            uint32 ResetThreatTimer;

            void Reset()
            {
                if (Unit* pLeftHead  = Unit::GetUnit(*me, LeftHeadGUID))
                {
                    pLeftHead->setDeathState(JUST_DIED);
                    LeftHeadGUID = 0;
                }

                if (Unit* pRightHead  = Unit::GetUnit(*me, RightHeadGUID))
                {
                    pRightHead->setDeathState(JUST_DIED);
                    RightHeadGUID = 0;
                }

                AggroYell = false;
                ThreatYell = false;
                ThreatYell2 = false;
                KillingYell = false;

                DelayTimer = 4000;
                BlastWaveTimer = 0;
                BlastCount = 0;
                FearTimer = 8000;
                BurningMaulTimer = 25000;
                ThunderClapTimer = 15000;
                ResetThreatTimer = 30000;

                if (instance)
                    instance->SetData(TYPE_OMROGG, NOT_STARTED);   //End boss can use this later. O'mrogg must be defeated(DONE) or he will come to aid.
            }

            void DoYellForThreat()
            {
                Unit* pLeftHead  = Unit::GetUnit(*me, LeftHeadGUID);
                Unit* pRightHead = Unit::GetUnit(*me, RightHeadGUID);

                if (!pLeftHead || !pRightHead)
                    return;

                ithreat = rand()%4;

                Unit* source = (pLeftHead->GetEntry() == Threat[ithreat].creature ? pLeftHead : pRightHead);

                DoSendQuantumText(Threat[ithreat].id, source);

                DelayTimer = 3500;
                ThreatYell = true;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SummonCreature(NPC_LEFT_HEAD, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                me->SummonCreature(NPC_RIGHT_HEAD, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);

                if (Unit* pLeftHead = Unit::GetUnit(*me, LeftHeadGUID))
                {
                    iaggro = rand()%3;

                    DoSendQuantumText(GoCombat[iaggro].id, pLeftHead);

                    DelayTimer = 3500;
                    AggroYell = true;
                }

                if (instance)
                    instance->SetData(TYPE_OMROGG, IN_PROGRESS);
            }

            void JustSummoned(Creature* summoned)
            {
                if (summoned->GetEntry() == NPC_LEFT_HEAD)
                    LeftHeadGUID = summoned->GetGUID();

                if (summoned->GetEntry() == NPC_RIGHT_HEAD)
                    RightHeadGUID = summoned->GetGUID();

                summoned->SetVisible(false);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                Unit* pLeftHead  = Unit::GetUnit(*me, LeftHeadGUID);
                Unit* pRightHead = Unit::GetUnit(*me, RightHeadGUID);

                if (!pLeftHead || !pRightHead)
                    return;

                ikilling = rand()%2;

                Unit* source = (pLeftHead->GetEntry() == Killing[ikilling].creature ? pLeftHead : pRightHead);

                switch (ikilling)
                {
                    case 0:
                        DoSendQuantumText(Killing[ikilling].id, source);
                        DelayTimer = 3500;
                        KillingYell = true;
                        break;
                    case 1:
                        DoSendQuantumText(Killing[ikilling].id, source);
                        KillingYell = false;
                        break;
                }
            }

            void JustDied(Unit* /*Killer*/)
            {
                Unit* pLeftHead  = Unit::GetUnit(*me, LeftHeadGUID);
                Unit* pRightHead = Unit::GetUnit(*me, RightHeadGUID);

                if (!pLeftHead || !pRightHead)
                    return;

                DoSendQuantumText(YELL_DIE_L, pLeftHead);

                CAST_AI(mob_omrogg_heads::mob_omrogg_headsAI, CAST_CRE(pRightHead)->AI())->DoDeathYell();

                if (instance)
                    instance->SetData(TYPE_OMROGG, DONE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (DelayTimer <= diff)
                {
                    DelayTimer = 3500;

                    Unit* pLeftHead  = Unit::GetUnit(*me, LeftHeadGUID);
                    Unit* pRightHead = Unit::GetUnit(*me, RightHeadGUID);

                    if (!pLeftHead || !pRightHead)
                        return;

                    if (AggroYell)
                    {
                        DoSendQuantumText(GoCombatDelay[iaggro].id, pRightHead);
                        AggroYell = false;
                    }

                    if (ThreatYell2)
                    {
                        Unit* source = (pLeftHead->GetEntry() == ThreatDelay2[ithreat].creature ? pLeftHead : pRightHead);

                        DoSendQuantumText(ThreatDelay2[ithreat].id, source);
                        ThreatYell2 = false;
                    }

                    if (ThreatYell)
                    {
                        Unit* source = (pLeftHead->GetEntry() == ThreatDelay1[ithreat].creature ? pLeftHead : pRightHead);

                        DoSendQuantumText(ThreatDelay1[ithreat].id, source);
                        ThreatYell = false;
                        ThreatYell2 = true;
                    }

                    if (KillingYell)
                    {
                        Unit* source = (pLeftHead->GetEntry() == KillingDelay[ikilling].creature ? pLeftHead : pRightHead);

                        DoSendQuantumText(KillingDelay[ikilling].id, source);
                        KillingYell = false;
                    }
                }
				else DelayTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (BlastCount && BlastWaveTimer <= diff)
                {
                    DoCast(me, SPELL_BLAST_WAVE);
                    BlastWaveTimer = 5000;
                    ++BlastCount;

                    if (BlastCount == 3)
                        BlastCount = 0;
                }
                else
                    BlastWaveTimer -= diff;

                if (BurningMaulTimer <= diff)
                {
                    DoSendQuantumText(EMOTE_ENRAGE, me);
                    DoCast(me, DUNGEON_MODE(SPELL_BURNING_MAUL_5N, SPELL_BURNING_MAUL_5H));
                    BurningMaulTimer = 40000;
                    BlastWaveTimer = 16000;
                    BlastCount = 1;
                }
                else BurningMaulTimer -= diff;

                if (ResetThreatTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoYellForThreat();
                        DoResetThreat();
                        me->AddThreat(target, 0.0f);
                    }
                    ResetThreatTimer = 25000+rand()%15000;
                }
                else ResetThreatTimer -= diff;

                if (FearTimer <= diff)
                {
                    DoCast(me, SPELL_FEAR);
                    FearTimer = 15000+rand()%20000;
                }
                else FearTimer -= diff;

                if (ThunderClapTimer <= diff)
                {
                    DoCast(me, SPELL_THUNDERCLAP);
                    ThunderClapTimer = 15000+rand()%15000;
                }
                else ThunderClapTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_warbringer_omroggAI(creature);
        }
};

void AddSC_boss_warbringer_omrogg()
{
    new boss_warbringer_omrogg();
    new mob_omrogg_heads();
}