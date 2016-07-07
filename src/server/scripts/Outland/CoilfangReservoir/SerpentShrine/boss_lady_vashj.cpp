/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "Spell.h"
#include "serpent_shrine.h"

#define MIDDLE_X    30.134f
#define MIDDLE_Y    -923.65f
#define MIDDLE_Z    42.9f
#define SPOREBAT_X  30.977156f
#define SPOREBAT_Y  -925.297761f
#define SPOREBAT_Z  77.176567f
#define SPOREBAT_O  5.223932f

#define TEXT_ALREADY_DEACTIVATED "Already deactivated"

float ElementPos[8][4] =
{
    {8.3f, -835.3f, 21.9f, 5.0f},
    {53.4f, -835.3f, 21.9f, 4.5f},
    {96.0f, -861.9f, 21.8f, 4.0f},
    {96.0f, -986.4f, 21.4f, 2.5f},
    {54.4f, -1010.6f, 22, 1.8f},
    {9.8f, -1012, 21.7f, 1.4f},
    {-35.0f, -987.6f, 21.5f, 0.8f},
    {-58.9f, -901.6f, 21.5f, 6.0f},
};

float ElementWPPos[8][3] =
{
    {71.700752f, -883.905884f, 41.097168f},
    {45.039848f, -868.022827f, 41.097015f},
    {14.585141f, -867.894470f, 41.097061f},
    {-25.415508f, -906.737732f, 41.097061f},
    {-11.801594f, -963.405884f, 41.097067f},
    {14.556657f, -979.051514f, 41.097137f},
    {43.466549f, -979.406677f, 41.097027f},
    {69.945908f, -964.663940f, 41.097054f},
};

float SporebatWPPos[8][3] =
{
    {31.6f, -896.3f, 59.1f},
    {9.1f,  -913.9f, 56.0f},
    {5.2f,  -934.4f, 52.4f},
    {20.7f, -946.9f, 49.7f},
    {41.0f, -941.9f, 51.0f},
    {47.7f, -927.3f, 55.0f},
    {42.2f, -912.4f, 51.7f},
    {27.0f, -905.9f, 50.0f},
};

float CoilfangElitePos[3][4] =
{
    {28.84f, -923.28f, 42.9f, 6.0f},
    {31.183281f, -953.502625f, 41.523602f, 1.640957f},
    {58.895180f, -923.124268f, 41.545307f, 3.152848f},
};

float CoilfangStriderPos[3][4] =
{
    {66.427010f, -948.778503f, 41.262245f, 2.584220f},
    {7.513962f, -959.538208f, 41.300422f, 1.034629f},
    {-12.843201f, -907.798401f, 41.239620f, 6.087094f},
};

float ShieldGeneratorChannelPos[4][4] =
{
    {49.6262f, -902.181f, 43.0975f, 3.95683f},
    {10.988f, -901.616f, 42.5371f, 5.4373f},
    {10.3859f, -944.036f, 42.5446f, 0.779888f},
    {49.3126f, -943.398f, 42.5501f, 2.40174f},
};

enum Texts
{
	SAY_INTRO   = -1548042,
	SAY_AGGRO1  = -1548043,
	SAY_AGGRO2  = -1548044,
	SAY_AGGRO3  = -1548045,
	SAY_AGGRO4  = -1548046,
	SAY_PHASE1  = -1548047,
	SAY_PHASE2  = -1548048,
	SAY_PHASE3  = -1548049,
	SAY_SHOT_1  = -1548050,
	SAY_SHOT_2  = -1548051,
	SAY_SLAY1   = -1548052,
	SAY_SLAY2   = -1548053,
	SAY_SLAY3   = -1548054,
	SAY_DEATH   = -1548055,
};

enum Spells
{
	SPELL_STATIC_CHARGE_TRIGGER = 38280,
	SPELL_FORKED_LIGHTNING      = 40088,
	SPELL_SURGE                 = 38044,
	SPELL_MULTI_SHOT            = 38310,
	SPELL_SHOCK_BLAST           = 38509,
	SPELL_ENTANGLE              = 38316,
	SPELL_SHOOT                 = 40873,
	SPELL_POISON_BOLT           = 40095,
	SPELL_TOXIC_SPORES          = 38575,
	SPELL_MAGIC_BARRIER         = 38112,
};

class boss_lady_vashj : public CreatureScript
{
public:
    boss_lady_vashj() : CreatureScript("boss_lady_vashj") { }

    struct boss_lady_vashjAI : public QuantumBasicAI
    {
        boss_lady_vashjAI (Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            Intro = false;
            JustCreated = true;
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // set it only once on Creature create (no need do intro if wiped)
        }

        InstanceScript* instance;

        uint64 ShieldGeneratorChannel[4];

        uint32 AggroTimer;
        uint32 ShockBlastTimer;
        uint32 EntangleTimer;
        uint32 StaticChargeTimer;
        uint32 ForkedLightningTimer;
        uint32 CheckTimer;
        uint32 EnchantedElementalTimer;
        uint32 TaintedElementalTimer;
        uint32 CoilfangEliteTimer;
        uint32 CoilfangStriderTimer;
        uint32 SummonSporebatTimer;
        uint32 SummonSporebatStaticTimer;
        uint8 EnchantedElementalPos;
        uint8 Phase;

        bool Entangle;
        bool Intro;
        bool CanAttack;
        bool JustCreated;

        void Reset()
        {
			AggroTimer = 19000;
            ShockBlastTimer = 1+rand()%60000;
            EntangleTimer = 30000;
            StaticChargeTimer = 10000+rand()%15000;
            ForkedLightningTimer = 2000;
            CheckTimer = 15000;
            EnchantedElementalTimer = 5000;
            TaintedElementalTimer = 50000;
            CoilfangEliteTimer = 45000+rand()%5000;
            CoilfangStriderTimer = 60000+rand()%10000;
            SummonSporebatTimer = 10000;
            SummonSporebatStaticTimer = 30000;
            EnchantedElementalPos = 0;
            Phase = 0;

            Entangle = false;
            if (JustCreated)
            {
                CanAttack = false;
                JustCreated = false;
            }
			else CanAttack = true;
            
            for (uint8 i = 0; i < 4; ++i)
                if (Unit* remo = Unit::GetUnit(*me, ShieldGeneratorChannel[i]))
                    remo->setDeathState(JUST_DIED);

            if (instance)
                instance->SetData(DATA_LADYVASHJEVENT, NOT_STARTED);

            ShieldGeneratorChannel[0] = 0;
            ShieldGeneratorChannel[1] = 0;
            ShieldGeneratorChannel[2] = 0;
            ShieldGeneratorChannel[3] = 0;

            me->SetCorpseDelay(1000*60*60);
        }

        // Called when a tainted elemental dies
        void EventTaintedElementalDeath()
        {
            // the next will spawn 50 seconds after the previous one's death
            if (TaintedElementalTimer > 50000)
                TaintedElementalTimer = 50000;
        }
        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
        }

        void JustDied(Unit* /*victim*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(DATA_LADYVASHJEVENT, DONE);
        }

        void StartEvent()
        {
            DoSendQuantumText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3, SAY_AGGRO4), me);

            Phase = 1;

            if (instance)
                instance->SetData(DATA_LADYVASHJEVENT, IN_PROGRESS);
        }

        void EnterToBattle(Unit* who)
        {
            if (instance)
            {
                // remove old tainted cores to prevent cheating in phase 2
                Map* map = me->GetMap();
                Map::PlayerList const &PlayerList = map->GetPlayers();
                for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                    if (Player* player = itr->getSource())
                        player->DestroyItemCount(31088, 1, true);
            }
            StartEvent(); // this is EnterToBattle(), so were are 100% in combat, start the event

            if (Phase != 2)
                AttackStart(who);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!Intro)
            {
                Intro = true;
                DoSendQuantumText(SAY_INTRO, me);
            }

            if (!CanAttack)
                return;

            if (!who || me->GetVictim())
                return;

            if (me->CanCreatureAttack(who))
            {
                float attackRadius = me->GetAttackDistance(who);

                if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && me->IsWithinLOSInMap(who))
                {
                    if (!me->IsInCombatActive()) // AttackStart() sets UNIT_FLAG_IN_COMBAT, so this msut be before attacking
                        StartEvent();

                    if (Phase != 2)
                        AttackStart(who);
                }
            }
        }

        void CastShootOrMultishot()
        {
            switch (urand(0, 1))
            {
                case 0:
                    // Shoot
                    // Used in Phases 1 and 3 after Entangle or while having nobody in melee range. A shot that hits her target for 4097-5543 Physical damage.
                    DoCastVictim(SPELL_SHOOT);
                    break;
                case 1:
                    // Multishot
                    // Used in Phases 1 and 3 after Entangle or while having nobody in melee range. A shot that hits 1 person and 4 people around him for 6475-7525 physical damage.
                    DoCastVictim(SPELL_MULTI_SHOT);
                    break;
            }
            if (rand()%3)
            {
                DoSendQuantumText(RAND(SAY_SHOT_1, SAY_SHOT_2), me);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!CanAttack && Intro)
            {
                if (AggroTimer <= diff)
                {
                    CanAttack = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    AggroTimer=19000;
                }
                else
                {
                    AggroTimer-=diff;
                    return;
                }
            }
            // to prevent abuses during phase 2
            if (Phase == 2 && !me->GetVictim() && me->IsInCombatActive())
            {
                EnterEvadeMode();
                return;
            }

            if (!UpdateVictim())
                return;

            if (Phase == 1 || Phase == 3)
            {
                if (ShockBlastTimer <= diff)
                {
                    DoCastVictim(SPELL_SHOCK_BLAST);
                    me->TauntApply(me->GetVictim());

                    ShockBlastTimer = 1000+rand()%14000;
                }
				else ShockBlastTimer -= diff;

                if (StaticChargeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 200, true))
					{
						if (target && !target->HasAura(SPELL_STATIC_CHARGE_TRIGGER))
						{
							DoCast(target, SPELL_STATIC_CHARGE_TRIGGER); // cast Static Charge every 2 seconds for 20 seconds
							StaticChargeTimer = 10000+rand()%20000;
						}
					}
                }
				else StaticChargeTimer -= diff;

                if (EntangleTimer <= diff)
                {
                    if (!Entangle)
                    {
                        DoCastVictim(SPELL_ENTANGLE);
                        Entangle = true;
                        EntangleTimer = 10000;
                    }
                    else
                    {
                        CastShootOrMultishot();
                        Entangle = false;
                        EntangleTimer = 20000+rand()%5000;
                    }
                }
				else EntangleTimer -= diff;

                // Phase 1
                if (Phase == 1)
                {
                    // Start phase 2
                    if (HealthBelowPct(70))
                    {
                        Phase = 2;

                        me->GetMotionMaster()->Clear();
                        DoTeleportTo(MIDDLE_X, MIDDLE_Y, MIDDLE_Z);

                        for (uint8 i = 0; i < 4; ++i)
						{
                            if (Creature* creature = me->SummonCreature(NPC_SHIED_GENERATOR_CHANNEL, ShieldGeneratorChannelPos[i][0],  ShieldGeneratorChannelPos[i][1],  ShieldGeneratorChannelPos[i][2],  ShieldGeneratorChannelPos[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0))
                                ShieldGeneratorChannel[i] = creature->GetGUID();
						}

                        DoSendQuantumText(SAY_PHASE2, me);
                    }
                }
                // Phase 3
                else
                {
                    if (SummonSporebatTimer <= diff)
                    {
                        if (Creature* sporebat = me->SummonCreature(NPC_TOXIC_SPOREBAT, SPOREBAT_X, SPOREBAT_Y, SPOREBAT_Z, SPOREBAT_O, TEMPSUMMON_CORPSE_DESPAWN, 0))
						{
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                sporebat->AI()->AttackStart(target);
						}

                        // summon sporebats faster and faster
                        if (SummonSporebatStaticTimer > 1000)
                            SummonSporebatStaticTimer -= 1000;

                        SummonSporebatTimer = SummonSporebatStaticTimer;

                        if (SummonSporebatTimer < 5000)
                            SummonSporebatTimer = 5000;
                    }
					else SummonSporebatTimer -= diff;
                }

                // Melee attack
                DoMeleeAttackIfReady();

                if (CheckTimer <= diff)
                {
                    bool inMeleeRange = false;
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        if (target && target->IsWithinDistInMap(me, 5)) // if in melee range
                        {
                            inMeleeRange = true;
                            break;
                        }
                    }

                    if (!inMeleeRange)
                        CastShootOrMultishot();

                    CheckTimer = 5000;
                }
				else CheckTimer -= diff;
            }
            // Phase 2
            else
            {
                if (ForkedLightningTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0);

                    if (!target)
                        target = me->GetVictim();

                    DoCast(target, SPELL_FORKED_LIGHTNING);

                    ForkedLightningTimer = 2000+rand()%6000;
                }
				else ForkedLightningTimer -= diff;

                if (EnchantedElementalTimer <= diff)
                {
                    me->SummonCreature(NPC_ENCHANTED_ELEMENTAL, ElementPos[EnchantedElementalPos][0], ElementPos[EnchantedElementalPos][1], ElementPos[EnchantedElementalPos][2], ElementPos[EnchantedElementalPos][3], TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if (EnchantedElementalPos == 7)
                        EnchantedElementalPos = 0;
                    else ++EnchantedElementalPos;

                    EnchantedElementalTimer = 10000+rand()%5000;
                }
				else EnchantedElementalTimer -= diff;

                if (TaintedElementalTimer <= diff)
                {
                    uint32 pos = rand()%8;
                    me->SummonCreature(NPC_TAINTED_ELEMENTAL, ElementPos[pos][0], ElementPos[pos][1], ElementPos[pos][2], ElementPos[pos][3], TEMPSUMMON_DEAD_DESPAWN, 0);

                    TaintedElementalTimer = 120000;
                }
				else TaintedElementalTimer -= diff;

                if (CoilfangEliteTimer <= diff)
                {
                    uint32 pos = rand()%3;
                    Creature* coilfangElite = me->SummonCreature(NPC_COILFANG_ELITE, CoilfangElitePos[pos][0], CoilfangElitePos[pos][1], CoilfangElitePos[pos][2], CoilfangElitePos[pos][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                    if (coilfangElite)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            coilfangElite->AI()->AttackStart(target);

                        else if (me->GetVictim())
                            coilfangElite->AI()->AttackStart(me->GetVictim());
                    }
                    CoilfangEliteTimer = 45000+rand()%5000;
                }
				else CoilfangEliteTimer -= diff;

                if (CoilfangStriderTimer <= diff)
                {
                    uint32 pos = rand()%3;
                    if (Creature* CoilfangStrider = me->SummonCreature(NPC_COILFANG_STRIDER, CoilfangStriderPos[pos][0], CoilfangStriderPos[pos][1], CoilfangStriderPos[pos][2], CoilfangStriderPos[pos][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            CoilfangStrider->AI()->AttackStart(target);

                        else if (me->GetVictim())
							CoilfangStrider->AI()->AttackStart(me->GetVictim());
                    }
                    CoilfangStriderTimer = 60000+rand()%10000;
                }
				else CoilfangStriderTimer -= diff;

                if (CheckTimer <= diff)
                {
                    // Start Phase 3
                    if (instance && instance->GetData(DATA_CANSTARTPHASE3))
                    {
                        me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_50));
                        me->RemoveAurasDueToSpell(SPELL_MAGIC_BARRIER);
                        DoSendQuantumText(SAY_PHASE3, me);
                        Phase = 3;

                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                    CheckTimer = 1000;
                }
				else CheckTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_vashjAI(creature);
    }
};

class npc_enchanted_elemental : public CreatureScript
{
public:
    npc_enchanted_elemental() : CreatureScript("npc_enchanted_elemental") { }

    struct npc_enchanted_elementalAI : public QuantumBasicAI
    {
        npc_enchanted_elementalAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 Move;
        uint32 Phase;
        float X, Y, Z;

        uint64 VashjGUID;

        void Reset()
        {
            me->SetSpeed(MOVE_WALK, 0.6f); // walk
            me->SetSpeed(MOVE_RUN, 0.6f); // run
            Move = 0;
            Phase = 1;

            VashjGUID = 0;

            X = ElementWPPos[0][0];
            Y = ElementWPPos[0][1];
            Z = ElementWPPos[0][2];

            //search for nearest waypoint (up on stairs)
            for (uint32 i = 1; i < 8; ++i)
            {
                if (me->GetDistance(ElementWPPos[i][0], ElementWPPos[i][1], ElementWPPos[i][2]) < me->GetDistance(X, Y, Z))
                {
                    X = ElementWPPos[i][0];
                    Y = ElementWPPos[i][1];
                    Z = ElementWPPos[i][2];
                }
            }

            if (instance)
                VashjGUID = instance->GetData64(DATA_LADYVASHJ);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
                return;

            if (!VashjGUID)
                return;

            if (Move <= diff)
            {
                me->SetWalk(true);
                if (Phase == 1)
                    me->GetMotionMaster()->MovePoint(0, X, Y, Z);
                if (Phase == 1 && me->IsWithinDist3d(X, Y, Z, 0.1f))
                    Phase = 2;
                if (Phase == 2)
                {
                    me->GetMotionMaster()->MovePoint(0, MIDDLE_X, MIDDLE_Y, MIDDLE_Z);
                    Phase = 3;
                }
                if (Phase == 3)
                {
                    me->GetMotionMaster()->MovePoint(0, MIDDLE_X, MIDDLE_Y, MIDDLE_Z);
                    if (me->IsWithinDist3d(MIDDLE_X, MIDDLE_Y, MIDDLE_Z, 3))
                        DoCast(me, SPELL_SURGE);
                }
                if (Creature* vashj = Unit::GetCreature(*me, VashjGUID))
                    if (!vashj->IsInCombatActive() || CAST_AI(boss_lady_vashj::boss_lady_vashjAI, vashj->AI())->Phase != 2 || vashj->IsDead())
                        me->Kill(me);
                Move = 1000;
            }
			else Move -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enchanted_elementalAI(creature);
    }
};

class npc_tainted_elemental : public CreatureScript
{
public:
    npc_tainted_elemental() : CreatureScript("npc_tainted_elemental") { }

    struct npc_tainted_elementalAI : public QuantumBasicAI
    {
        npc_tainted_elementalAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 PoisonBoltTimer;
        uint32 DespawnTimer;

        void Reset()
        {
            PoisonBoltTimer = 5000+rand()%5000;
            DespawnTimer = 30000;
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                if (Creature* vashj = Unit::GetCreature(*me, instance->GetData64(DATA_LADYVASHJ)))
                    CAST_AI(boss_lady_vashj::boss_lady_vashjAI, vashj->AI())->EventTaintedElementalDeath();
        }

        void EnterToBattle(Unit* who)
        {
            me->AddThreat(who, 0.1f);
        }

        void UpdateAI(const uint32 diff)
        {
            // PoisonBoltTimer
            if (PoisonBoltTimer <= diff)
            {
                Unit* target = SelectTarget(TARGET_RANDOM, 0);

                if (target && target->IsWithinDistInMap(me, 30))
                    DoCast(target, SPELL_POISON_BOLT);

                PoisonBoltTimer = 5000+rand()%5000;
            }
			else PoisonBoltTimer -= diff;

            // DespawnTimer
            if (DespawnTimer <= diff)
            {
                // call Unsummon()
                me->setDeathState(DEAD);

                // to prevent crashes
                DespawnTimer = 1000;
            }
			else DespawnTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tainted_elementalAI(creature);
    }
};

class npc_toxic_sporebat : public CreatureScript
{
public:
    npc_toxic_sporebat() : CreatureScript("npc_toxic_sporebat") {}

    struct npc_toxic_sporebatAI : public QuantumBasicAI
    {
        npc_toxic_sporebatAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            EnterEvadeMode();
        }

        InstanceScript* instance;

        uint32 MovementTimer;
        uint32 ToxicSporeTimer;
        uint32 BoltTimer;
        uint32 CheckTimer;

        void Reset()
        {
            me->SetDisableGravity(true);
            me->SetCurrentFaction(16);
            MovementTimer = 0;
            ToxicSporeTimer = 5000;
            BoltTimer = 5500;
            CheckTimer = 1000;
        }

        void MoveInLineOfSight(Unit* /*who*/){}

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
                MovementTimer = 0;
        }

        void UpdateAI (const uint32 diff)
        {
            // Random movement
            if (MovementTimer <= diff)
            {
                uint32 rndpos = rand()%8;
                me->GetMotionMaster()->MovePoint(1, SporebatWPPos[rndpos][0], SporebatWPPos[rndpos][1], SporebatWPPos[rndpos][2]);
                MovementTimer = 6000;
            }
			else MovementTimer -= diff;

            // toxic spores
            if (BoltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (Creature* trig = me->SummonCreature(NPC_TOXIC_SPORES_TRIGGER, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30000))
                    {
                        trig->SetCurrentFaction(16);
                        trig->CastSpell(trig, SPELL_TOXIC_SPORES, true);
                    }
                }
                BoltTimer = 10000+rand()%5000;
            }
            else BoltTimer -= diff;

            // CheckTimer
            if (CheckTimer <= diff)
            {
                if (instance)
                {
                    // check if vashj is death
                    Unit* Vashj = Unit::GetUnit(*me, instance->GetData64(DATA_LADYVASHJ));
                    if (!Vashj || (Vashj && !Vashj->IsAlive()) || (Vashj && CAST_AI(boss_lady_vashj::boss_lady_vashjAI, CAST_CRE(Vashj)->AI())->Phase != 3))
                    {
                        // remove
                        me->setDeathState(DEAD);
                        me->RemoveCorpse();
                        me->SetCurrentFaction(35);
                    }
                }

                CheckTimer = 1000;
            }
			else CheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_toxic_sporebatAI(creature);
    }
};

class npc_shield_generator_channel : public CreatureScript
{
public:
    npc_shield_generator_channel() : CreatureScript("npc_shield_generator_channel") {}

    struct npc_shield_generator_channelAI : public QuantumBasicAI
    {
        npc_shield_generator_channelAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 CheckTimer;
        bool Casted;

        void Reset()
        {
            CheckTimer = 0;
            Casted = false;
            me->SetDisplayId(MODEL_ID_INVISIBLE); // invisible

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI (const uint32 diff)
        {
            if (!instance)
                return;

            if (CheckTimer <= diff)
            {
                Unit* vashj = Unit::GetUnit(*me, instance->GetData64(DATA_LADYVASHJ));

                if (vashj && vashj->IsAlive())
                {
                    if (!Casted || !vashj->HasAura(SPELL_MAGIC_BARRIER))
                    {
                        DoCast(vashj, SPELL_MAGIC_BARRIER, true);
                        Casted = true;
                    }
                }
                CheckTimer = 1000;
            }
			else CheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shield_generator_channelAI(creature);
    }
};

class item_tainted_core : public ItemScript
{
public:
    item_tainted_core() : ItemScript("item_tainted_core") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& targets)
    {
        InstanceScript* instance = player->GetInstanceScript();

        Creature* vashj = Unit::GetCreature((*player), instance->GetData64(DATA_LADYVASHJ));
        if (vashj && (CAST_AI(boss_lady_vashj::boss_lady_vashjAI, vashj->AI())->Phase == 2))
        {
            if (GameObject* go = targets.GetGOTarget())
            {
                uint32 identifier;
                uint8 channelIdentifier;
                switch (go->GetEntry())
                {
                    case 185052:
                        identifier = DATA_SHIELDGENERATOR1;
                        channelIdentifier = 0;
                        break;
                    case 185053:
                        identifier = DATA_SHIELDGENERATOR2;
                        channelIdentifier = 1;
                        break;
                    case 185051:
                        identifier = DATA_SHIELDGENERATOR3;
                        channelIdentifier = 2;
                        break;
                    case 185054:
                        identifier = DATA_SHIELDGENERATOR4;
                        channelIdentifier = 3;
                        break;
                    default:
                        return true;
                }

                if (instance->GetData(identifier))
                {
                    player->GetSession()->SendNotification(TEXT_ALREADY_DEACTIVATED);
                    return true;
                }
                
                if (Unit* channel = Unit::GetCreature(*vashj, CAST_AI(boss_lady_vashj::boss_lady_vashjAI, vashj->AI())->ShieldGeneratorChannel[channelIdentifier]))
                {
                    channel->setDeathState(JUST_DIED);
                }

                instance->SetData(identifier, 1);
                player->DestroyItemCount(31088, 1, true);
                return true;
            }
            else if (targets.GetUnitTarget()->GetTypeId() == TYPE_ID_UNIT)
                return false;
            else if (targets.GetUnitTarget()->GetTypeId() == TYPE_ID_PLAYER)
            {
                player->DestroyItemCount(31088, 1, true);
                player->CastSpell(targets.GetUnitTarget(), 38134, true);
                return true;
            }
        }
        return true;
    }
};

void AddSC_boss_lady_vashj()
{
    new boss_lady_vashj();
    new npc_enchanted_elemental();
    new npc_tainted_elemental();
    new npc_toxic_sporebat();
    new npc_shield_generator_channel();
    new item_tainted_core();
}