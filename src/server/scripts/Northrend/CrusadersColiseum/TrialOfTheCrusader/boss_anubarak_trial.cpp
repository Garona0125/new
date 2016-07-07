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
#include "trial_of_the_crusader.h"

enum Texts
{
    SAY_INTRO               = 0,
    SAY_AGGRO               = 1,
    EMOTE_SUBMERGE          = 2,
    EMOTE_BURROWER          = 3,
    SAY_EMERGE              = 4,
    SAY_LEECHING_SWARM      = 5,
    EMOTE_LEECHING_SWARM    = 6,
    SAY_KILL_PLAYER         = 7,
    SAY_DEATH               = 8,
	SAY_BERSERK             = 9,
    EMOTE_SPIKE             = 0,
};

/*enum Texts
{
    SAY_INTRO               = -1649055,
    SAY_AGGRO               = -1649056,
    SAY_KILL1               = -1649057,
    SAY_KILL2               = -1649058,
    SAY_DEATH               = -1649059,
    EMOTE_SPIKE             = -1649060,
    SAY_BURROWER            = -1649061,
    EMOTE_LEECHING_SWARM    = -1649062,
    SAY_LEECHING_SWARM      = -1649063,
	SAY_BERSERK             = -1649064,
};*/

enum Spells
{
	SPELL_PENETRATING_COLD_10N = 66013,
	SPELL_PENETRATING_COLD_25N = 67700,
	SPELL_PENETRATING_COLD_10H = 68509,
	SPELL_PENETRATING_COLD_25H = 68510,
	SPELL_LEECHING_SWARM_10N   = 66118,
	SPELL_LEECHING_SWARM_25N   = 67630,
	SPELL_LEECHING_SWARM_10H   = 68646,
	SPELL_LEECHING_SWARM_25H   = 68647,
    SPELL_FREEZE_SLASH         = 66012,
    SPELL_LEECHING_HEAL        = 66125,
    SPELL_LEECHING_DAMAGE      = 66240,
    SPELL_MARK                 = 67574,
    SPELL_SPIKE_CALL           = 66169,
    SPELL_SUBMERGE_ANUBARAK    = 65981,
    SPELL_CLEAR_ALL_DEBUFFS    = 34098,
    SPELL_EMERGE_ANUBARAK      = 65982,
    SPELL_SUMMON_BEATLES       = 66339,
    SPELL_SUMMON_BURROWER      = 66332,
    // Burrow
    SPELL_CHURNING_GROUND   = 66969,
    // Scarab
    SPELL_DETERMINATION     = 66092,
    SPELL_ACID_MANDIBLE     = 65774, //Passive - Triggered
    // Burrower
    SPELL_SPIDER_FRENZY     = 66128,
    SPELL_EXPOSE_WEAKNESS   = 67720, //Passive - Triggered
    SPELL_SHADOW_STRIKE     = 66134,
    SPELL_SUBMERGE_EFFECT   = 53421,
    SPELL_EMERGE_EFFECT     = 66947,

    SUMMON_SCARAB           = NPC_SCARAB,
    SUMMON_FROSTSPHERE      = NPC_FROST_SPHERE,
    SPELL_BERSERK           = 26662,
    //Frost Sphere
    SPELL_FROST_SPHERE      = 67539,
    SPELL_PERMAFROST        = 66193,
    SPELL_PERMAFROST_VISUAL = 65882,
    //Spike
    SPELL_SUMMON_SPIKE      = 66169,
    SPELL_SPIKE_SPEED1      = 65920,
    SPELL_SPIKE_TRAIL       = 65921,
    SPELL_SPIKE_SPEED2      = 65922,
    SPELL_SPIKE_SPEED3      = 65923,
    SPELL_SPIKE_FAIL        = 66181,
    SPELL_SPIKE_TELE        = 66170,
};

#define SPELL_PERMAFROST_HELPER RAID_MODE<uint32>(66193, 67855, 67856, 67857)

enum SummonActions
{
    ACTION_SCARAB_SUBMERGE
};

enum MovementPoints
{
    POINT_FALL_GROUND   = 1,
};

enum PursuingSpikesPhases
{
    PHASE_NO_MOVEMENT   = 0,
    PHASE_IMPALE_NORMAL = 1,
    PHASE_IMPALE_MIDDLE = 2,
    PHASE_IMPALE_FAST   = 3,
};

class boss_anubarak_trial_of_the_crusader : public CreatureScript
{
public:
    boss_anubarak_trial_of_the_crusader() : CreatureScript("boss_anubarak_trial_of_the_crusader") { }

    struct boss_anubarak_trial_of_the_crusaderAI : public QuantumBasicAI
    {
        boss_anubarak_trial_of_the_crusaderAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        SummonList Summons;
        std::list<uint64> m_vBurrowGUID;

        uint32 FreezeSlashTimer;
        uint32 PenetratingColdTimer;
        uint32 SummonNerubianTimer;
        uint32 SubmergeTimer;
        uint32 PursuingSpikeTimer;
        uint32 SummonScarabTimer;
        uint32 SummonFrostSphereTimer;
        uint32 BerserkTimer;

        uint8 stage;
        bool Intro;
        bool ReachedPhase3;
        uint8 ScarabSummoned;

        void Reset()
        {
            FreezeSlashTimer = 15*IN_MILLISECONDS;
            PenetratingColdTimer = 20*IN_MILLISECONDS;
            SummonNerubianTimer = 10*IN_MILLISECONDS;
            SubmergeTimer = 80*IN_MILLISECONDS;
            PursuingSpikeTimer = 2*IN_MILLISECONDS;
            SummonScarabTimer = 2*IN_MILLISECONDS;
            SummonFrostSphereTimer = 20*IN_MILLISECONDS;
            BerserkTimer = 10*MINUTE*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            stage = 0;
            ScarabSummoned = 0;
            Intro = true;
            ReachedPhase3 = false;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            // clean up spawned Frost Spheres
            std::list<Creature*> FrostSphereList;
            me->GetCreatureListWithEntryInGrid(FrostSphereList, NPC_FROST_SPHERE, 150.0f);
            if (!FrostSphereList.empty())
                for (std::list<Creature*>::iterator itr = FrostSphereList.begin(); itr != FrostSphereList.end(); itr++)
                    (*itr)->DespawnAfterAction();

            Summons.DespawnAll();
            m_vBurrowGUID.clear();
        }

        void KilledUnit(Unit* who)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER)
                Talk(SAY_KILL_PLAYER);
        }

        void MoveInLineOfSight(Unit* /*who*/)
        {
            if (Intro)
            {
                Talk(SAY_INTRO);
                Intro = false;
            }
        }

        void JustReachedHome()
        {
			instance->SetData(TYPE_ANUBARAK, FAIL);

            //Summon Scarab Swarms neutral at random places
            for (int i = 0; i < 10; i++)
			{
                if (Creature* temp = me->SummonCreature(NPC_SCARAB, AnubarakLoc[1].GetPositionX()+urand(0, 50)-25, AnubarakLoc[1].GetPositionY()+urand(0, 50)-25, AnubarakLoc[1].GetPositionZ()))
                    temp->SetCurrentFaction(31);
			}
        }

        void JustDied(Unit* /*killer*/)
        {
            Summons.DespawnAll();
            Talk(SAY_DEATH);

			instance->SetData(TYPE_ANUBARAK, DONE);

            // despawn frostspheres and Burrowers on death
            std::list<Creature*> AddList;
            me->GetCreatureListWithEntryInGrid(AddList, NPC_FROST_SPHERE, 150.0f);
            me->GetCreatureListWithEntryInGrid(AddList, NPC_BURROWER, 150.0f);
            if (!AddList.empty())
				for (std::list<Creature*>::iterator itr = AddList.begin(); itr != AddList.end(); itr++)
					(*itr)->DespawnAfterAction();
        }

        void JustSummoned(Creature* summoned)
        {
            Unit* target = SelectTarget(TARGET_RANDOM, 0, 0, true);
            switch (summoned->GetEntry())
            {
                case NPC_BURROW:
                    m_vBurrowGUID.push_back(summoned->GetGUID());
                    summoned->SetReactState(REACT_PASSIVE);
                    summoned->CastSpell(summoned, SPELL_CHURNING_GROUND, false);
                    summoned->SetDisplayId(summoned->GetCreatureTemplate()->Modelid2);
                    break;
                case NPC_SPIKE:
                    summoned->CombatStart(target);
                    summoned->SetDisplayId(summoned->GetCreatureTemplate()->Modelid1);
                    Talk(EMOTE_SPIKE, target->GetGUID());
                    break;
            }
            Summons.Summon(summoned);
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            switch (summoned->GetEntry())
            {
                case NPC_SPIKE:
                    PursuingSpikeTimer = 2*IN_MILLISECONDS;
                    break;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetInCombatWithZone();
			instance->SetData(TYPE_ANUBARAK, IN_PROGRESS);
            //Despawn Scarab Swarms neutral
            EntryCheckPredicate pred(NPC_SCARAB);
            Summons.DoAction(ACTION_SCARAB_SUBMERGE, pred);
            //Spawn Burrow
            for (int i=0; i < 4; i++)
                me->SummonCreature(NPC_BURROW, AnubarakLoc[i+2]);
            //Spawn Frost Spheres
            for (int i=0; i < 6; i++)
                SummonFrostSphere();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (stage)
            {
                case 0:
                    if (FreezeSlashTimer <= diff)
                    {
                        DoCastVictim(SPELL_FREEZE_SLASH);
                        FreezeSlashTimer = 15*IN_MILLISECONDS;
                    }
					else FreezeSlashTimer -= diff;

                    if (PenetratingColdTimer <= diff)
                    {
						me->CastCustomSpell(RAID_MODE(SPELL_PENETRATING_COLD_10N, SPELL_PENETRATING_COLD_25N, SPELL_PENETRATING_COLD_10H, SPELL_PENETRATING_COLD_25H), SPELLVALUE_MAX_TARGETS, RAID_MODE(2, 5, 2, 5));
                        PenetratingColdTimer = 20*IN_MILLISECONDS;
                    }
					else PenetratingColdTimer -= diff;

                    if (SummonNerubianTimer <= diff && (IsHeroic() || !ReachedPhase3))
                    {
                        me->CastCustomSpell(SPELL_SUMMON_BURROWER, SPELLVALUE_MAX_TARGETS, RAID_MODE(1, 2, 2, 4));
                        SummonNerubianTimer = 45*IN_MILLISECONDS;
                    }
					else SummonNerubianTimer -= diff;

                    if (SubmergeTimer <= diff && !ReachedPhase3 && !me->HasAura(SPELL_BERSERK))
                    {
                        stage = 1;
                        SubmergeTimer = 60*IN_MILLISECONDS;
                    }
					else SubmergeTimer -= diff;
                    break;
                case 1:
                    DoCast(me, SPELL_SUBMERGE_ANUBARAK);
                    DoCast(me, SPELL_CLEAR_ALL_DEBUFFS);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    Talk(EMOTE_BURROWER);
                    ScarabSummoned = 0;
                    SummonScarabTimer = 4*IN_MILLISECONDS;
                    stage = 2;
                    break;
                case 2:
                    if (PursuingSpikeTimer <= diff)
                    {
                        DoCast(SPELL_SPIKE_CALL);
                        // Just to make sure it won't happen again in this phase
                        PursuingSpikeTimer = 90*IN_MILLISECONDS;
                    }
					else PursuingSpikeTimer -= diff;

                    if (SummonScarabTimer <= diff)
                    {
                        std::list<uint64>::iterator i = m_vBurrowGUID.begin();
                        uint32 at = urand(0, m_vBurrowGUID.size()-1);
                        for (uint32 k = 0; k < at; k++)
                            ++i;
                        if (Creature* pBurrow = Unit::GetCreature(*me, *i))
                            pBurrow->CastSpell(pBurrow, 66340, false);
                        ScarabSummoned++;
                        SummonScarabTimer = 4*IN_MILLISECONDS;
                        if (ScarabSummoned == 4) SummonScarabTimer = RAID_MODE(4, 20)*IN_MILLISECONDS;

                        /*It seems that this spell have something more that needs to be taken into account
                        //Need more sniff info
                        DoCast(SPELL_SUMMON_BEATLES);
                        // Just to make sure it won't happen again in this phase
                        SummonScarabTimer = 90*IN_MILLISECONDS;*/
                    }
					else SummonScarabTimer -= diff;

                    if (SubmergeTimer <= diff)
                    {
                        stage = 3;
                        SubmergeTimer = 80*IN_MILLISECONDS;
                    }
					else SubmergeTimer -= diff;
                    break;
                case 3:
                    stage = 0;
                    DoCast(SPELL_SPIKE_TELE);
                    Summons.DespawnEntry(NPC_SPIKE);
                    me->RemoveAurasDueToSpell(SPELL_SUBMERGE_ANUBARAK);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    DoCast(me, SPELL_EMERGE_ANUBARAK);
                    SummonNerubianTimer = 10*IN_MILLISECONDS;
                    SummonScarabTimer = 2*IN_MILLISECONDS;
                    break;
            }

            if (!IsHeroic())
            {
                if (SummonFrostSphereTimer <= diff)
                {
                    SummonFrostSphere();
                    SummonFrostSphereTimer = urand(20, 30)*IN_MILLISECONDS;
                }
				else SummonFrostSphereTimer -= diff;
            }

            if (HealthBelowPct(30) && stage == 0 && !ReachedPhase3)
            {
                ReachedPhase3 = true;
				DoCastAOE(RAID_MODE(SPELL_LEECHING_SWARM_10N, SPELL_LEECHING_SWARM_25N, SPELL_LEECHING_SWARM_10H, SPELL_LEECHING_SWARM_25H));
                Talk(EMOTE_LEECHING_SWARM);
                Talk(SAY_LEECHING_SWARM);
            }

            if (BerserkTimer <= diff && !me->HasAura(SPELL_BERSERK))
            {
                DoCast(me, SPELL_BERSERK);
				Talk(SAY_BERSERK);
            }
			else BerserkTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void SummonFrostSphere()
        {
            float x = frand(700.f, 780.f);
            float y = frand(105.f, 165.f);
            float z = 155.6f;
            me->SummonCreature(NPC_FROST_SPHERE, x, y, z);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_anubarak_trial_of_the_crusaderAI(creature);
    };
};

class npc_swarm_scarab : public CreatureScript
{
public:
    npc_swarm_scarab() : CreatureScript("npc_swarm_scarab") { }

    struct npc_swarm_scarabAI : public QuantumBasicAI
    {
        npc_swarm_scarabAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 DeterminationTimer;

        void Reset()
        {
            me->SetCorpseDelay(0);
            DeterminationTimer = urand(5*IN_MILLISECONDS, 60*IN_MILLISECONDS);
            DoCast(me, SPELL_ACID_MANDIBLE);
            me->SetInCombatWithZone();

            if (Unit* target = SelectTarget(TARGET_RANDOM))
                me->AddThreat(target, 20000.0f);

            if (!me->IsInCombatActive())
                me->DisappearAndDie();
        }

        void DoAction(const int32 action)
        {
            switch (action)
            {
                case ACTION_SCARAB_SUBMERGE:
                    DoCast(SPELL_SUBMERGE_EFFECT);
                    me->DespawnAfterAction(1000);
                    break;
            }
        }

        void JustDied(Unit* killer)
        {
            DoCast(killer, RAID_MODE(SPELL_TRAITOR_KING_10, SPELL_TRAITOR_KING_25));
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            /* Bosskillers don't recognize */
            if (DeterminationTimer <= diff)
            {
                DoCast(me, SPELL_DETERMINATION);
                DeterminationTimer = urand(10*IN_MILLISECONDS, 60*IN_MILLISECONDS);
            }
			else DeterminationTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swarm_scarabAI(creature);
    };
};

class npc_nerubian_burrower : public CreatureScript
{
    enum Phases
    {
        PHASE_GROUND    = 0,
        PHASE_SUBMERGED
    };

    enum Events
    {
        EVENT_SUBMERGE          = 1,
        EVENT_SHADOW_STRIKE
    };

public:
    npc_nerubian_burrower() : CreatureScript("npc_nerubian_burrower") { }

    struct npc_nerubian_burrowerAI : public QuantumBasicAI
    {
        npc_nerubian_burrowerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
            me->SetCorpseDelay(0);
            events.ScheduleEvent(EVENT_SUBMERGE, 30*IN_MILLISECONDS, 0, PHASE_GROUND);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_SHADOW_STRIKE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS), 0, PHASE_GROUND);
            DoCast(me, SPELL_EXPOSE_WEAKNESS);
            DoCast(me, SPELL_SPIDER_FRENZY);
            me->SetInCombatWithZone();
            phase = PHASE_GROUND;

            if (!me->IsInCombatActive())
                me->DisappearAndDie();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() && !me->HasAura(SPELL_SUBMERGE_EFFECT))
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 event = events.ExecuteEvent())
            {
                switch (event)
                {
                    case EVENT_SUBMERGE:
                        if (me->HasAura(SPELL_SUBMERGE_EFFECT))
                        {
                            me->RemoveAurasDueToSpell(SPELL_SUBMERGE_EFFECT);
                            DoCast(me, SPELL_EMERGE_EFFECT);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
                            me->CombatStart(me->SelectNearestTarget());
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SHADOW_STRIKE, 20*IN_MILLISECONDS, 0, PHASE_GROUND);
                            phase = PHASE_GROUND;
                        }
                        else
                        {
                            if (!me->HasAura(SPELL_PERMAFROST_HELPER))
                            {
                                DoCast(me, SPELL_SUBMERGE_EFFECT);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
                                me->CombatStop();
                                phase = PHASE_SUBMERGED;
                            }
                        }
                        events.ScheduleEvent(EVENT_SUBMERGE, 20*IN_MILLISECONDS);
                        return;
                    case EVENT_SHADOW_STRIKE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            DoCast(target, SPELL_SHADOW_STRIKE);
                        events.ScheduleEvent(EVENT_SHADOW_STRIKE, 20*IN_MILLISECONDS, 0, PHASE_GROUND);
                        return;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
        private:
            Phases phase;
            EventMap events;
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nerubian_burrowerAI(creature);
    };
};

class npc_frost_sphere : public CreatureScript
{
    public:
        npc_frost_sphere() : CreatureScript("npc_frost_sphere") { }

        struct npc_frost_sphereAI : public QuantumBasicAI
        {
            npc_frost_sphereAI(Creature* creature) : QuantumBasicAI(creature){}

            void Reset()
            {
                _isFalling = false;
                me->SetReactState(REACT_PASSIVE);
                //! Confirmed sniff 3.3.5.a
                me->SetDisableGravity(true);
                me->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                //! end
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                me->SetSpeed(MOVE_FLIGHT, 0.5f, false);
                me->CombatStop(true);
                me->GetMotionMaster()->MoveRandom(20.0f);
                DoCast(SPELL_FROST_SPHERE);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage)
            {
                if (me->GetHealth() <= damage)
                {
                    damage = 0;
                    if (!_isFalling)
                    {
                        _isFalling = true;
                        me->GetMotionMaster()->MoveIdle();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        //At hit the ground
                        me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_DEATH);
                        me->GetMotionMaster()->MoveFall(POINT_FALL_GROUND);
                        me->SetDisplayId(MODEL_ID_INVISIBLE);
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId)
            {
                if (type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_FALL_GROUND:
                        me->RemoveAurasDueToSpell(SPELL_FROST_SPHERE);
                        DoCast(SPELL_PERMAFROST_VISUAL);
                        DoCast(SPELL_PERMAFROST);
                        me->SetObjectScale(2.0f);
                        break;
                }
            }

        private:
            bool _isFalling;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_frost_sphereAI(creature);
        };
};

class npc_anubarak_spike : public CreatureScript
{
public:
    npc_anubarak_spike() : CreatureScript("npc_anubarak_spike") { }

    struct npc_anubarak_spikeAI : public QuantumBasicAI
    {
        npc_anubarak_spikeAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 PhaseSwitchTimer;
        PursuingSpikesPhases m_Phase;

        void Reset()
        {
            m_Phase = PHASE_NO_MOVEMENT;
            PhaseSwitchTimer = 1;
            // make sure the spike has everyone on threat list
            me->SetInCombatWithZone();
        }

        bool CanAIAttack(Unit const* victim) const
        {
            return victim->GetTypeId() == TYPE_ID_PLAYER;
        }

        void EnterToBattle(Unit* who)
        {
            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
            {
                StartChase(target);
                Talk(EMOTE_SPIKE, who->GetGUID());
            }
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            damage = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                me->DisappearAndDie();
                return;
            }

            if (PhaseSwitchTimer)
            {
                if (PhaseSwitchTimer <= diff)
                {
                    switch (m_Phase)
                    {
                        case PHASE_NO_MOVEMENT:
                            DoCast(me, SPELL_SPIKE_SPEED1);
                            DoCast(me, SPELL_SPIKE_TRAIL);
                            m_Phase = PHASE_IMPALE_NORMAL;
                            if (Unit* target2 = SelectTarget(TARGET_RANDOM, 0))
                            {
                                StartChase(target2);
                                Talk(EMOTE_SPIKE, target2->GetGUID());
                            }
                            PhaseSwitchTimer = 7000;
                            return;
                        case PHASE_IMPALE_NORMAL:
                            DoCast(me, SPELL_SPIKE_SPEED2);
                            m_Phase = PHASE_IMPALE_MIDDLE;
                            PhaseSwitchTimer = 7000;
                            return;
                        case PHASE_IMPALE_MIDDLE:
                            DoCast(me, SPELL_SPIKE_SPEED3);
                            m_Phase = PHASE_IMPALE_FAST;
                            PhaseSwitchTimer = 0;
                            return;
                    }
                }
				else PhaseSwitchTimer -= diff;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who)
                return;

            if (who->GetEntry() != NPC_FROST_SPHERE)
                return;

            if (m_Phase == PHASE_NO_MOVEMENT)
                return;

            if (me->IsWithinDist(who, 7.0f))
            {
                switch (m_Phase)
                {
                    case PHASE_IMPALE_NORMAL:
                        me->RemoveAurasDueToSpell(SPELL_SPIKE_SPEED1);
                        break;
                    case PHASE_IMPALE_MIDDLE:
                        me->RemoveAurasDueToSpell(SPELL_SPIKE_SPEED2);
                        break;
                    case PHASE_IMPALE_FAST:
                        me->RemoveAurasDueToSpell(SPELL_SPIKE_SPEED3);
                        break;
                }

                me->CastSpell(me, SPELL_SPIKE_FAIL, true);

                who->ToCreature()->DespawnAfterAction(3000);

                // After the spikes hit the icy surface they can't move for about ~5 seconds
                m_Phase = PHASE_NO_MOVEMENT;
                PhaseSwitchTimer = 5000;
                SetCombatMovement(false);
                me->GetMotionMaster()->MoveIdle();
                me->GetMotionMaster()->Clear();
            }
        }

        void StartChase(Unit* who)
        {
            DoCast(who, SPELL_MARK);
            me->SetSpeed(MOVE_RUN, 0.5f);
            // make sure the Spine will really follow the one he should
            me->getThreatManager().clearReferences();
            me->SetInCombatWithZone();
            me->getThreatManager().addThreat(who, 10000.f);
            me->GetMotionMaster()->Clear(true);
            me->GetMotionMaster()->MoveChase(who);
            me->TauntApply(who);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubarak_spikeAI(creature);
    };
};

class spell_impale : public SpellScriptLoader
{
    public:
        spell_impale() : SpellScriptLoader("spell_impale") { }

        class spell_impale_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_impale_SpellScript);

            void HandleDamageCalc(SpellEffIndex /*effIndex*/)
            {
                Unit* target = GetHitUnit();
                uint32 permafrost = sSpellMgr->GetSpellIdForDifficulty(SPELL_PERMAFROST, target);

                // make sure Impale doesnt do damage if we are standing on permafrost
                if (target && target->HasAura(permafrost))
                    SetHitDamage(0);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_impale_SpellScript::HandleDamageCalc, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_impale_SpellScript();
        }
};

void AddSC_boss_anubarak_trial()
{
    new boss_anubarak_trial_of_the_crusader();
    new npc_swarm_scarab();
    new npc_nerubian_burrower();
    new npc_anubarak_spike();
    new npc_frost_sphere();
    new spell_impale();
}