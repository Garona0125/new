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
#include "azjol_nerub.h"

enum Yells
{
    SAY_AGGRO                = -1601011,
    SAY_SLAY_1               = -1601012,
    SAY_SLAY_2               = -1601013,
    SAY_DEATH                = -1601014,
	SAY_SWARM_1              = -1601015,
    SAY_SWARM_2              = -1601016,
    SAY_PREFIGHT_1           = -1601017,
    SAY_PREFIGHT_2           = -1601018,
    SAY_PREFIGHT_3           = -1601019,
    SAY_SEND_GROUP_1         = -1601020,
    SAY_SEND_GROUP_2         = -1601021,
    SAY_SEND_GROUP_3         = -1601022,
};

enum Spells
{
    SPELL_MIND_FLAY_5N                = 52586,
    SPELL_MIND_FLAY_5H                = 59367,
    SPELL_CURSE_OF_FATIGUE_5N         = 52592,
    SPELL_CURSE_OF_FATIGUE_5H         = 59368,
    SPELL_FRENZY                      = 28747,
    SPELL_ENRAGE                      = 52470,
    SPELL_INFECTED_BITE_5N            = 52469,
    SPELL_INFECTED_BITE_5H            = 59364,
    SPELL_WEB_WRAP                    = 52086,
	SPELL_WEB_WRAP_STUN               = 52087,
    SPELL_BLINDING_WEBS_5N            = 52524,
    SPELL_BLINDING_WEBS_5H            = 59365,
    SPELL_POSION_SPRAY_5N             = 52493,
    SPELL_POSION_SPRAY_5H             = 59366,
	SPELL_SWARM                       = 52440,
};

const Position SpawnPoint[] =
{
    { 566.164f, 682.087f, 769.079f, 2.21657f},
    { 529.042f, 706.941f, 777.298f, 1.0821f},
    { 489.975f, 671.239f, 772.131f, 0.261799f},
    { 488.556f, 692.95f,  771.764f, 4.88692f},
    { 553.34f,  640.387f, 777.419f, 1.20428f},
    { 517.486f, 706.398f, 777.335f, 5.35816f},
    { 504.01f,  637.693f, 777.479f, 0.506145f},
    { 552.625f, 706.408f, 777.177f, 3.4383f},
};

class boss_krik_thir_the_gatewatcher : public CreatureScript
{
public:
    boss_krik_thir_the_gatewatcher() : CreatureScript("boss_krik_thir_the_gatewatcher") { }

    struct boss_krik_thir_the_gatewatcherAI : public QuantumBasicAI
    {
        boss_krik_thir_the_gatewatcherAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 MindFlayTimer;
        uint32 CurseFatigueTimer;
        uint32 SummonTimer;
		uint32 PrefightDialogTimer;

        void Reset()
        {
			PrefightDialogTimer = 500;
            MindFlayTimer = 15*IN_MILLISECONDS;
            CurseFatigueTimer = 12*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);
            Summon();
            SummonTimer = 15*IN_MILLISECONDS;

			instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, IN_PROGRESS);
        }

        void Summon()
        {
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[0], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[0], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[1], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[1], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[2], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[2], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[3], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[3], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_INFECTIOR, SpawnPoint[4], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[4], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_INFECTIOR, SpawnPoint[5], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[5], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_INFECTIOR, SpawnPoint[6], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[6], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[7], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
			me->SummonCreature(NPC_SKITTERING_SWARMER, SpawnPoint[7], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
        }

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (PrefightDialogTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(RAND(SAY_PREFIGHT_1, SAY_PREFIGHT_2, SAY_PREFIGHT_3), me);
				PrefightDialogTimer = 60000; // 1 Minute
			}
			else PrefightDialogTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SummonTimer <= diff)
            {
				DoSendQuantumText(RAND(SAY_SWARM_1, SAY_SWARM_2), me);
				DoCast(me, SPELL_SWARM);

                Summon();
                SummonTimer = 15*IN_MILLISECONDS;
            }
			else SummonTimer -= diff;

            if (MindFlayTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_FLAY_5N, SPELL_MIND_FLAY_5H));
					MindFlayTimer = 15*IN_MILLISECONDS;
				}
			}
			else MindFlayTimer -= diff;

            if (CurseFatigueTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                    DoCast(target, DUNGEON_MODE(SPELL_CURSE_OF_FATIGUE_5N, SPELL_CURSE_OF_FATIGUE_5H));

                if (Unit* tankTarget = SelectTarget(TARGET_RANDOM, 1, 100, true))
                    DoCast(tankTarget, DUNGEON_MODE(SPELL_CURSE_OF_FATIGUE_5N, SPELL_CURSE_OF_FATIGUE_5H));

                CurseFatigueTimer = 10*IN_MILLISECONDS;
            }
			else CurseFatigueTimer -= diff;

            if (!me->HasAura(SPELL_FRENZY) && HealthBelowPct(10))
                DoCast(me, SPELL_FRENZY);

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, DONE);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_krik_thir_the_gatewatcherAI(creature);
    }
};

class npc_web_wrap : public CreatureScript
{
    public:
        npc_web_wrap() : CreatureScript("npc_web_wrap") { }

        struct npc_web_wrapAI : public QuantumBasicAI
        {
            npc_web_wrapAI(Creature* creature) : QuantumBasicAI(creature) {}

			uint64 WrapTargetGUID;

            void Reset()
            {
                WrapTargetGUID = 0;
            }

            void EnterToBattle(Unit* /*who*/){}

            void AttackStart(Unit* /*who*/){}

            void MoveInLineOfSight(Unit* /*who*/){}

            void JustDied(Unit* /*killer*/)
            {
                if (WrapTargetGUID)
                {
                    Unit* target = Unit::GetUnit(*me, WrapTargetGUID);
                    if (target)
                        target->RemoveAurasDueToSpell(SPELL_WEB_WRAP_STUN);
                }
                me->RemoveCorpse();
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!me->ToTempSummon())
                    return;

                // should not be here?
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    WrapTargetGUID = summoner->GetGUID();

                Unit* temp = Unit::GetUnit(*me, WrapTargetGUID);
                if ((temp && temp->IsAlive() && !temp->HasAura(SPELL_WEB_WRAP_STUN)) || !temp)
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_web_wrapAI(creature);
        }
};

class npc_krikthir_guard : public CreatureScript
{
    public:
        npc_krikthir_guard() : CreatureScript("npc_krikthir_guard") { }

        struct npc_krikthir_guardAI : public QuantumBasicAI
        {
            npc_krikthir_guardAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 WebWrapTimer;
            uint32 InfectedBiteTimer;
            uint32 SpecialSpell;
            uint32 SpecialSpellTimer;
            uint32 EnwrapTimer;
            uint64 WrapTarget;
            bool Enwrapping;

            void Reset()
            {
                switch (me->GetEntry())
                {
                    case NPC_WATCHER_NARJIL:
                        SpecialSpell = DUNGEON_MODE<uint32>(SPELL_BLINDING_WEBS_5N, SPELL_BLINDING_WEBS_5H);
                        break;
                    case NPC_WATCHER_GASHRA:
                        me->SetCurrentFaction(16);
                        SpecialSpell = SPELL_ENRAGE;
                        break;
                    case NPC_WATCHER_SILTHIK:
                        SpecialSpell = DUNGEON_MODE<uint32>(SPELL_POSION_SPRAY_5N, SPELL_POSION_SPRAY_5H);
                        break;
                    default:
                        SpecialSpell = 0;
                        break;
                }

                WebWrapTimer = 10*IN_MILLISECONDS;
                InfectedBiteTimer = 6*IN_MILLISECONDS;
                SpecialSpellTimer = 15*IN_MILLISECONDS;
                EnwrapTimer = 3*IN_MILLISECONDS;
                WrapTarget = 0;
                Enwrapping = false;

				instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, NOT_STARTED);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				Creature* Krikthir = Unit::GetCreature(*me, instance->GetData64(DATA_KRIKTHIR_THE_GATEWATCHER));

                if (Krikthir && Krikthir->IsAlive())
                    DoSendQuantumText(RAND(SAY_SEND_GROUP_1, SAY_SEND_GROUP_2, SAY_SEND_GROUP_3), Krikthir);

                instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, SPECIAL);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (Enwrapping)
                {
                    if (EnwrapTimer <= diff)
                    {
                        if (Unit* target = Unit::GetUnit(*me, WrapTarget))
                        {
                            target->CastSpell(target, SPELL_WEB_WRAP_STUN, true);
                            WrapTarget = NULL;
                        }
                        Enwrapping = false;
                        EnwrapTimer = 3*IN_MILLISECONDS;
                    }
                    else EnwrapTimer -= diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (WebWrapTimer <= diff)
                {
                    Unit* target = NULL;
                    std::list<Unit *> playerList;
                    SelectTargetList(playerList, 5, TARGET_RANDOM, 40, true);
                    for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                    {
                        target = (*itr);
                        if (!target->HasAura(SPELL_WEB_WRAP) && !target->HasAura(SPELL_WEB_WRAP_STUN))
                            break;  // found someone

                        target = NULL;
                    }
                    // if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true, -SPELL_WEB_WRAP_STUN)
                    if (target)
                    {
                        Enwrapping = true;
                        WrapTarget = target->GetGUID();
                        DoCast(target, SPELL_WEB_WRAP, false);
                    }
                    WebWrapTimer = 15*IN_MILLISECONDS;
                }
                else WebWrapTimer -= diff;

                if (InfectedBiteTimer <= diff)
                {
					DoCastVictim(DUNGEON_MODE(SPELL_INFECTED_BITE_5N, SPELL_INFECTED_BITE_5H));
                    InfectedBiteTimer = 15*IN_MILLISECONDS;
                }
                else InfectedBiteTimer -= diff;

                if (SpecialSpell)
				{
                    if (SpecialSpellTimer <= diff)
                    {
                        DoCast(SpecialSpell);
                        SpecialSpellTimer = 20*IN_MILLISECONDS;
                    }
                    else SpecialSpellTimer -= diff;
				}

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_krikthir_guardAI(creature);
        }
};

class achievement_watch_him_die : public AchievementCriteriaScript
{
    public:
        achievement_watch_him_die() : AchievementCriteriaScript("achievement_watch_him_die") { }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            InstanceScript* instance = target->GetInstanceScript();
            Creature* Watcher[3];

            if (!instance)
                return false;

            for (uint8 n = 0; n < 3; ++n)
            {
                Watcher[n] = ObjectAccessor::GetCreature(*target, instance->GetData64(DATA_WATCHER_GASHRA + n));
                if (Watcher[n] && !Watcher[n]->IsAlive())
                    return false;
            }

            return true;
        }
};

void AddSC_boss_krik_thir_the_gatewatcher()
{
    new boss_krik_thir_the_gatewatcher();
	new npc_web_wrap();
	new npc_krikthir_guard();
    new achievement_watch_him_die();
}