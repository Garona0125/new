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
#include "sunwell_plateau.h"

enum Quotes
{
	SAY_INTRO                 = -1580017,
	SAY_INTRO_BREAK_ICE       = -1580018,
	SAY_INTRO_CHARGE          = -1580019,
	SAY_INTRO_KILL_MADRIGOSA  = -1580020,
	SAY_INTRO_TAUNT           = -1580021,
	SAY_AGGRO                 = -1580022,
	SAY_KILL_1                = -1580023,
	SAY_KILL_2                = -1580024,
	SAY_KILL_3                = -1580025,
	SAY_STOMP_1               = -1580026,
	SAY_STOMP_2               = -1580027,
	SAY_STOMP_3               = -1580028,
	SAY_BERSERK               = -1580029,
	SAY_DEATH                 = -1580030,
	SAY_MADR_ICE_BARRIER      = -1580031,
	SAY_MADR_INTRO            = -1580032,
	SAY_MADR_ICE_BLOCK        = -1580033,
	SAY_MADR_TRAP             = -1580034,
	SAY_MADR_DEATH            = -1580035,
};

enum Spells
{
    SPELL_METEOR_SLASH                 = 45150,
    SPELL_BURN                         = 45141,
    SPELL_STOMP                        = 45185,
    SPELL_BERSERK                      = 26662,
    SPELL_DUAL_WIELD                   = 42459,
    SPELL_INTRO_FROST_BLAST            = 45203,
    SPELL_INTRO_FROSTBOLT              = 44843,
    SPELL_INTRO_ENCAPSULATE            = 45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING = 45661,
};

class boss_brutallus : public CreatureScript
{
public:
    boss_brutallus() : CreatureScript("boss_brutallus") {}

    struct boss_brutallusAI : public QuantumBasicAI
    {
        boss_brutallusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        Unit* Madrigosa;

        uint32 SlashTimer;
        uint32 BurnTimer;
        uint32 StompTimer;
        uint32 BerserkTimer;

        uint32 IntroPhase;
        uint32 IntroPhaseTimer;
        uint32 IntroFrostBoltTimer;

        bool Intro;
        bool IsIntro;
        bool Enraged;

        void Reset()
        {
            SlashTimer = 11000;
            StompTimer = 30000;
            BurnTimer = 60000;
            BerserkTimer = 360000;

            IntroPhase = 0;
            IntroPhaseTimer = 0;
            IntroFrostBoltTimer = 0;

            IsIntro = false;
            Enraged = false;
            Intro = true;

            me->CastSpell(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_UNARMED);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			instance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2, SAY_KILL_3), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_BRUTALLUS_EVENT, DONE);
        }

        void StartIntro()
        {
            if (!Intro)
                return;

            Madrigosa = Unit::GetUnit(*me, instance->GetData64(DATA_MADRIGOSA));
            if (Madrigosa)
            {
                 if (!Madrigosa->IsAlive())
                 {
                     EndIntro();
                     return;
				 }
				 Madrigosa->setDeathState(ALIVE);
				 Madrigosa->SetActive(true);
				 IsIntro = true;
			}
			else
                EndIntro();
        }

        void EndIntro()
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            Intro = false;
            IsIntro = false;
        }

        void DoIntro()
        {
            if (!Madrigosa)
                return;

            switch(IntroPhase)
            {
                case 0:
                    DoSendQuantumText(SAY_MADR_ICE_BARRIER, Madrigosa);
                    IntroPhaseTimer = 12000;
                    ++IntroPhase;
                    break;
                case 1:
                    me->SetInFront(Madrigosa);
                    Madrigosa->SetInFront(me);
                    DoSendQuantumText(SAY_MADR_INTRO, Madrigosa);
                    IntroPhaseTimer = 9000;
                    ++IntroPhase;
                    break;
                case 2:
                    DoSendQuantumText(SAY_INTRO, Madrigosa);
                    IntroPhaseTimer = 13000;
                    ++IntroPhase;
                    break;
                case 3:
                    DoCast(me, SPELL_INTRO_FROST_BLAST);
					Madrigosa->SetCanFly(true);
					Madrigosa->SetDisableGravity(true);
                    IntroFrostBoltTimer = 3000;
                    IntroPhaseTimer = 10000;
                    ++IntroPhase;
                    break;
                case 4:
                    DoSendQuantumText(SAY_INTRO_BREAK_ICE, me);
                    IntroPhaseTimer = 6000;
                    ++IntroPhase;
                    break;
                case 5:
                    Madrigosa->CastSpell(me, SPELL_INTRO_ENCAPSULATE_CHANELLING, false);
                    DoSendQuantumText(SAY_MADR_TRAP, Madrigosa);
                    DoCast(me, SPELL_INTRO_ENCAPSULATE);
                    IntroPhaseTimer = 11000;
                    ++IntroPhase;
                    break;
                case 6:
                    me->SetSpeed(MOVE_RUN, 4.0f, true);
                    DoSendQuantumText(SAY_INTRO_CHARGE, me);
                    IntroPhaseTimer = 5000;
                    ++IntroPhase;
                    break;
                case 7:
                    me->DealDamage(Madrigosa, Madrigosa->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true);
                    DoSendQuantumText(SAY_MADR_DEATH, Madrigosa);
                    me->SetFullHealth();
                    me->AttackStop();
                    me->SetSpeed(MOVE_RUN, 1.2f, true);
                    IntroPhaseTimer = 5000;
                    ++IntroPhase;
                    break;
                case 8:
                    DoSendQuantumText(SAY_INTRO_KILL_MADRIGOSA, me);
                    IntroPhaseTimer = 9000;
                    ++IntroPhase;
                    break;
                case 9:
                    DoSendQuantumText(SAY_INTRO_TAUNT, me);
                    IntroPhaseTimer = 5000;
					if (Creature* madrigosa = me->FindCreatureWithDistance(NPC_MADRIGOSA, 150.0f, true))
						madrigosa->DespawnAfterAction();
                    ++IntroPhase;
                    break;
                case 10:
                    EndIntro();
                    break;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
			instance->SetData(DATA_BRUTALLUS_EVENT, SPECIAL);

            if (Intro)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER && !((Player*)who)->IsGameMaster())
                {
                    if (me->IsWithinDistInMap(who, 100))
                        StartIntro();
                }
            }
			else
                QuantumBasicAI::MoveInLineOfSight(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (IsIntro)
            {
                if (IntroPhaseTimer <= diff)
                {
                    DoIntro();
                }
				else IntroPhaseTimer -= diff;

                if (IntroPhase == 3 + 1)
				{
                    if (IntroFrostBoltTimer <= diff)
                    {
                        if (Madrigosa)
						{
                            Madrigosa->CastSpell(me, SPELL_INTRO_FROSTBOLT, false);
                            IntroFrostBoltTimer = 2000;
                        }
                    }
					else IntroFrostBoltTimer -= diff;
                }
            }

            if (!UpdateVictim() || IsIntro)
                return;

            if (SlashTimer <= diff)
            {
                DoCastVictim(SPELL_METEOR_SLASH);
                SlashTimer = 11000;
            }
			else SlashTimer -= diff;

            if (StompTimer < diff)
            {
				DoSendQuantumText(RAND(SAY_STOMP_1, SAY_STOMP_2, SAY_STOMP_3), me);
                DoCastAOE(SPELL_STOMP);
                StompTimer = 30000;
            }
			else StompTimer -= diff;

            if (BurnTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 200, true))
				{
                    DoCast(target, SPELL_BURN, true);
					BurnTimer = 20000;
				}
            }
			else BurnTimer -= diff;

            if (BerserkTimer <= diff && !Enraged)
            {
                DoSendQuantumText(SAY_BERSERK, me);
                DoCast(me, SPELL_BERSERK);
                Enraged = true;
            }
			else BerserkTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_brutallusAI(creature);
    }
};

void AddSC_boss_brutallus()
{
    new boss_brutallus();
}