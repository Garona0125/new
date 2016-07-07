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
#include "arcatraz.h"

enum MillhouseSays
{
    SAY_INTRO_1                = -1552010,
    SAY_INTRO_2                = -1552011,
    SAY_WATER                  = -1552012,
    SAY_BUFFS                  = -1552013,
    SAY_DRINK                  = -1552014,
    SAY_READY                  = -1552015,
    SAY_KILL_1                 = -1552016,
    SAY_KILL_2                 = -1552017,
    SAY_PYROBLAST              = -1552018,
    SAY_ICE_BLOCK              = -1552019,
    SAY_LOW_HP                 = -1552020,
    SAY_DEATH                  = -1552021,
    SAY_COMPLETE               = -1552022,
};

enum Spells
{
    SPELL_CONJURE_WATER        = 36879,
    SPELL_ARCANE_INTELLECT     = 36880,
    SPELL_ICE_ARMOR            = 36881,
    SPELL_ARCANE_MISSILES      = 33833,
    SPELL_CONE_OF_COLD         = 12611,
    SPELL_FIRE_BLAST           = 13341,
    SPELL_FIREBALL             = 14034,
    SPELL_FROSTBOLT            = 15497,
    SPELL_PYROBLAST            = 33975,

	FACTION_MILLHOUSE          = 1665,
};

Position const MillhouseMoveToCenter[1] =
{
	{445.844f, -152.542f, 43.077f, 4.73476f},
};

class npc_millhouse_manastorm : public CreatureScript
{
    public:
        npc_millhouse_manastorm() : CreatureScript("npc_millhouse_manastorm") { }

        struct npc_millhouse_manastormAI : public QuantumBasicAI
        {
            npc_millhouse_manastormAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 EventProgressTimer;
            uint32 Phase;

            bool Init;
            bool LowHp;

            uint32 PyroblastTimer;
            uint32 FireballTimer;

            void Reset()
            {
                EventProgressTimer = 2000;

                LowHp = false;
                Init = false;
                Phase = 1;

                PyroblastTimer = 1000;
                FireballTimer = 2500;

                if (instance)
                {
                    if (instance->GetData(TYPE_WARDEN_2) == DONE)
                        Init = true;

                    if (instance->GetData(TYPE_HARBINGERSKYRISS) == DONE)
                        DoSendQuantumText(SAY_COMPLETE, me);
                }
            }

            void EnterToBattle(Unit* who)
			{
				me->AddThreat(who, 1.0f);
				me->SetInCombatWith(who);
				who->SetInCombatWith(me);
				me->GetMotionMaster()->MoveChase(who, 25.0f);
			}

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);

				FailQuest();
            }

			void FailQuest()
			{
				// Fail Quest after killing millhouse manastorm
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();

				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
						player->FailQuest(QUEST_TRIAL_OF_THE_NAARU_TENACITY);
				}
			}

            void UpdateAI(const uint32 diff)
            {
                if (!Init)
                {
                    if (EventProgressTimer <= diff)
                    {
                        if (Phase < 8)
                        {
                            switch (Phase)
                            {
                            case 1:
                                DoSendQuantumText(SAY_INTRO_1, me);
                                EventProgressTimer = 18000;
                                break;
                            case 2:
                                DoSendQuantumText(SAY_INTRO_2, me);
                                EventProgressTimer = 18000;
                                break;
                            case 3:
                                DoSendQuantumText(SAY_WATER, me);
                                DoCast(me, SPELL_CONJURE_WATER);
                                EventProgressTimer = 7000;
                                break;
                            case 4:
                                DoSendQuantumText(SAY_BUFFS, me);
                                DoCast(me, SPELL_ICE_ARMOR);
                                EventProgressTimer = 7000;
                                break;
                            case 5:
                                DoSendQuantumText(SAY_DRINK, me);
                                DoCast(me, SPELL_ARCANE_INTELLECT);
                                EventProgressTimer = 7000;
                                break;
                            case 6:
                                DoSendQuantumText(SAY_READY, me);
								me->SetCurrentFaction(FACTION_MILLHOUSE);
								me->AI()->DoZoneInCombat();
								me->GetMotionMaster()->MovePoint(0, MillhouseMoveToCenter[0]);
                                EventProgressTimer = 6000;
                                break;
                            case 7:
                                if (instance)
                                    instance->SetData(TYPE_WARDEN_2, DONE);
                                Init = true;
                                break;
                            }
                            ++Phase;
                        }
                    }
                    else EventProgressTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (!LowHp && HealthBelowPct(20))
                {
                    DoSendQuantumText(SAY_LOW_HP, me);
                    LowHp = true;
                }

				if (FireballTimer <= diff)
                {
                    DoCastVictim(SPELL_FIREBALL);
                    FireballTimer = 4000;
                }
                else FireballTimer -=diff;

                if (PyroblastTimer <= diff)
                {
                    if (me->IsNonMeleeSpellCasted(false))
                        return;

                    DoSendQuantumText(SAY_PYROBLAST, me);
                    DoCastVictim(SPELL_PYROBLAST);
                    PyroblastTimer = 7000;
                }
                else PyroblastTimer -=diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_millhouse_manastormAI(creature);
        }
};

enum WardenSays
{
    SAY_WARDEN_INTRO_1       = -1552023,
    SAY_WARDEN_INTRO_2       = -1552024,
    SAY_WARDEN_RELEASE_1     = -1552025,
    SAY_WARDEN_RELEASE_2A    = -1552026,
    SAY_WARDEN_RELEASE_2B    = -1552027,
    SAY_WARDEN_RELEASE_3     = -1552028,
    SAY_WARDEN_RELEASE_4     = -1552029,
    SAY_WARDEN_WELCOME       = -1552030,
};

enum WardenSpells
{
    SPELL_TARGET_ALPHA  = 36856,
    SPELL_TARGET_BETA   = 36854,
    SPELL_TARGET_DELTA  = 36857,
    SPELL_TARGET_GAMMA  = 36858,
    SPELL_TARGET_OMEGA  = 36852,
    SPELL_BUBBLE_VISUAL = 36849,
};

class npc_warden_mellichar : public CreatureScript
{
    public:
        npc_warden_mellichar() : CreatureScript("npc_warden_mellichar") { }

        struct npc_warden_mellicharAI : public QuantumBasicAI
        {
            npc_warden_mellicharAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            bool IsRunning;
            bool CanSpawn;

            uint32 EventProgressTimer;
            uint32 Phase;

            void Reset()
            {
                IsRunning = false;
                CanSpawn = false;

                EventProgressTimer = 22000;
                Phase = 1;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				//if (Creature* Omega = me->FindCreatureWithDistance(NPC_OMEGA_POD_TARGET, 100.0f, true))
				DoCast(SPELL_TARGET_OMEGA);

                if (instance)
                    instance->SetData(TYPE_HARBINGERSKYRISS, NOT_STARTED);
            }

            void AttackStart(Unit* /*who*/) {}

            void MoveInLineOfSight(Unit* who)
            {
                if (IsRunning)
                    return;

                if (!me->GetVictim() && me->CanCreatureAttack(who))
                {
                    if (!me->CanFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                        return;

                    if (who->GetTypeId() != TYPE_ID_PLAYER)
                        return;

                    float attackRadius = me->GetAttackDistance(who)/10;

					if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
                        EnterToBattle(who);
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
				me->InterruptSpell(CURRENT_CHANNELED_SPELL);

                DoSendQuantumText(SAY_WARDEN_INTRO_1, me);

                DoCast(me, SPELL_BUBBLE_VISUAL, true);

                if (instance)
                {
                    instance->SetData(TYPE_HARBINGERSKYRISS, IN_PROGRESS);
                    instance->HandleGameObject(instance->GetData64(DATA_SPHERE_SHIELD), false);
					DoCast(SPELL_TARGET_OMEGA);
                    IsRunning = true;
                }
            }

            bool CanProgress()
            {
                if (instance)
                {
                    if (Phase == 7 && instance->GetData(TYPE_WARDEN_4) == DONE)
                        return true;
                    if (Phase == 6 && instance->GetData(TYPE_WARDEN_3) == DONE)
                        return true;
                    if (Phase == 5 && instance->GetData(TYPE_WARDEN_2) == DONE)
                        return true;
                    if (Phase == 4)
                        return true;
                    if (Phase == 3 && instance->GetData(TYPE_WARDEN_1) == DONE)
                        return true;
                    if (Phase == 2 && instance->GetData(TYPE_HARBINGERSKYRISS) == IN_PROGRESS)
                        return true;
                    if (Phase == 1 && instance->GetData(TYPE_HARBINGERSKYRISS) == IN_PROGRESS)
                        return true;
                    return false;
                }
                return false;
            }

            void DoPrepareForPhase()
            {
                if (instance)
                {
                    me->InterruptNonMeleeSpells(true);
                    me->RemoveAurasByType(SPELL_AURA_DUMMY);

                    switch (Phase)
                    {
                    case 2:
						me->InterruptSpell(CURRENT_CHANNELED_SPELL);
						if (Creature* Alpha = me->FindCreatureWithDistance(NPC_ALPHA_POD_TARGET, 100.0f, true))
							DoCast(Alpha, SPELL_TARGET_ALPHA, true);

                        instance->SetData(TYPE_WARDEN_1, IN_PROGRESS);
                        instance->HandleGameObject(instance->GetData64(DATA_SPHERE_SHIELD), false);
                        break;
                    case 3:
						me->InterruptSpell(CURRENT_CHANNELED_SPELL);
						if (Creature* Beta = me->FindCreatureWithDistance(NPC_BETA_POD_TARGET, 100.0f, true))
							DoCast(Beta, SPELL_TARGET_BETA, true);

                        instance->SetData(TYPE_WARDEN_2, IN_PROGRESS);
                        break;
                    case 5:
						me->InterruptSpell(CURRENT_CHANNELED_SPELL);
						if (Creature* Delta = me->FindCreatureWithDistance(NPC_DELTA_POD_TARGET, 100.0f, true))
							DoCast(Delta, SPELL_TARGET_DELTA, true);

                        instance->SetData(TYPE_WARDEN_3, IN_PROGRESS);
                        break;
                    case 6:
						me->InterruptSpell(CURRENT_CHANNELED_SPELL);
						if (Creature* Gamma = me->FindCreatureWithDistance(NPC_GAMMA_POD_TARGET, 100.0f, true))
							DoCast(Gamma, SPELL_TARGET_GAMMA, true);

                        instance->SetData(TYPE_WARDEN_4, IN_PROGRESS);
                        break;
                    case 7:
                        instance->SetData(TYPE_WARDEN_5, IN_PROGRESS);
                        break;
                    }
                    CanSpawn = true;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!IsRunning)
                    return;

                if (EventProgressTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(TYPE_HARBINGERSKYRISS) == FAIL)
                        {
                            Reset();
                            return;
                        }
                    }

                    if (CanSpawn)
                    {
						if (Phase != 7)
						{
							//if (Creature* Omega = me->FindCreatureWithDistance(NPC_OMEGA_POD_TARGET, 100.0f, true))
							DoCast(SPELL_TARGET_OMEGA);
						}

                        switch (Phase)
                        {
                        case 2:
                            switch (urand(0, 1))
                            {
                            case 0:
                                me->SummonCreature(NPC_TRICKSTER, 478.326f, -148.505f, 42.56f, 3.19f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                                break;
                            case 1:
                                me->SummonCreature(NPC_PH_HUNTER, 478.326f, -148.505f, 42.56f, 3.19f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                                break;
                            }
                            break;
                        case 3:
                            me->SummonCreature(NPC_MILLHOUSE_MANASTORM, 413.292f, -148.378f, 42.56f, 6.27f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                            break;
                        case 4:
                            DoSendQuantumText(SAY_WARDEN_RELEASE_2B, me);
                            break;
                        case 5:
                            switch (urand(0, 1))
                            {
                            case 0:
                                me->SummonCreature(NPC_AKKIRIS, 420.179f, -174.396f, 42.58f, 0.02f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                                break;
                            case 1:
                                me->SummonCreature(NPC_SULFURON, 420.179f, -174.396f, 42.58f, 0.02f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                                break;
                            }
                            break;
                        case 6:
                            switch (urand(0, 1))
                            {
                            case 0:
                                me->SummonCreature(NPC_TW_DRAK, 471.795f, -174.58f, 42.58f, 3.06f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                                break;
                            case 1:
                                me->SummonCreature(NPC_BL_DRAK, 471.795f, -174.58f, 42.58f, 3.06f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                                break;
                            }
                            break;
                        case 7:
                            me->SummonCreature(NPC_SKYRISS, 445.763f, -191.639f, 44.64f, 1.60f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 800000);
                            DoSendQuantumText(SAY_WARDEN_WELCOME, me);
                            break;
                        }
                        CanSpawn = false;
                        ++Phase;
                    }
                    if (CanProgress())
                    {
                        switch (Phase)
                        {
                        case 1:
                            DoSendQuantumText(SAY_WARDEN_INTRO_2, me);
                            EventProgressTimer = 10000;
                            ++Phase;
                            break;
                        case 2:
                            DoSendQuantumText(SAY_WARDEN_RELEASE_1, me);
                            DoPrepareForPhase();
                            EventProgressTimer = 7000;
                            break;
                        case 3:
                            DoSendQuantumText(SAY_WARDEN_RELEASE_2A, me);
                            DoPrepareForPhase();
                            EventProgressTimer = 10000;
                            break;
                        case 4:
                            DoPrepareForPhase();
                            EventProgressTimer = 15000;
                            break;
                        case 5:
                            DoSendQuantumText(SAY_WARDEN_RELEASE_3, me);
                            DoPrepareForPhase();
                            EventProgressTimer = 15000;
                            break;
                        case 6:
                            DoSendQuantumText(SAY_WARDEN_RELEASE_4, me);
                            DoPrepareForPhase();
                            EventProgressTimer = 15000;
                            break;
                        case 7:
                            DoPrepareForPhase();
                            EventProgressTimer = 15000;
                            break;
                        }
                    }
                }
                else EventProgressTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_warden_mellicharAI(creature);
        }
};

void AddSC_arcatraz()
{
    new npc_millhouse_manastorm();
    new npc_warden_mellichar();
}