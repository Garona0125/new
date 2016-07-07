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
#include "ahnkahet.h"

#define DATA_SPHERE_ANGLE_OFFSET 2.1f
#define DATA_GROUND_POSITION_Z   11.4f

enum Texts
{
    SAY_AGGRO           = -1619021,
    SAY_SLAY_1          = -1619022,
    SAY_SLAY_2          = -1619023,
	SAY_SLAY_3          = -1619024,
    SAY_DEATH           = -1619025,
    SAY_FEED_1          = -1619026,
    SAY_FEED_2          = -1619027,
    SAY_VANISH_1        = -1619028,
    SAY_VANISH_2        = -1619029,
};

enum Spells
{
    SPELL_BLOODTHIRST                         = 55968, //Trigger Spell + add aura
    SPELL_CONJURE_FLAME_SPHERE                = 55931,
    SPELL_FLAME_SPHERE_SUMMON_1               = 55895, // 1x 30106
    H_SPELL_FLAME_SPHERE_SUMMON_1             = 59511, // 1x 31686
    H_SPELL_FLAME_SPHERE_SUMMON_2             = 59512, // 1x 31687
    SPELL_FLAME_SPHERE_SPAWN_EFFECT           = 55891,
    SPELL_FLAME_SPHERE_VISUAL                 = 55928,
    SPELL_FLAME_SPHERE_PERIODIC_5N            = 55926,
    SPELL_FLAME_SPHERE_PERIODIC_5H            = 59508,
    SPELL_FLAME_SPHERE_DEATH_EFFECT           = 55947,
    SPELL_BEAM_VISUAL                         = 60342,
    SPELL_EMBRACE_OF_THE_VAMPYR_5N            = 55959,
    SPELL_EMBRACE_OF_THE_VAMPYR_5H            = 59513,
    SPELL_VANISH                              = 55964,
};

enum Misc
{
    DATA_EMBRACE_DMG_5N    = 20000,
    DATA_EMBRACE_DMG_5H    = 40000,
    DATA_SPHERE_DISTANCE   = 60,
};

enum CombatPhase
{
    PHASE_NORMAL                = 1,
    PHASE_CASTING_FLAME_SPHERES = 2,
    PHASE_JUST_VANISHED         = 3,
    PHASE_VANISHED              = 4,
    PHASE_FEEDING               = 5,
};

Position const MoveToHome[1] =
{
	{529.245f, -846.659f, 11.3081f, 1.73285f},
};

class boss_taldaram : public CreatureScript
{
public:
    boss_taldaram() : CreatureScript("boss_taldaram") { }

    struct boss_taldaramAI : public QuantumBasicAI
    {
        boss_taldaramAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);

            SpellInfo* spell = (SpellInfo*)sSpellMgr->GetSpellInfo(DUNGEON_MODE(SPELL_FLAME_SPHERE_PERIODIC_5N, SPELL_FLAME_SPHERE_PERIODIC_5H));
            if (spell)
                spell->Effects[0].Amplitude = 500;
        }

        uint32 BloodthirstTimer;
        uint32 VanishTimer;
        uint32 WaitTimer;
        uint32 EmbraceTimer;
        uint32 EmbraceTakenDamage;
        uint32 FlamesphereTimer;
        uint32 PhaseTimer;

        uint64 EmbraceTarget;

        CombatPhase Phase;

        InstanceScript* instance;

        void Reset()
        {
            BloodthirstTimer = 10*IN_MILLISECONDS;
            VanishTimer = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS);
            EmbraceTimer = 20*IN_MILLISECONDS;
            FlamesphereTimer = 5*IN_MILLISECONDS;

            EmbraceTakenDamage = 0;
            Phase = PHASE_NORMAL;
            PhaseTimer = 0;
            EmbraceTarget = 0;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_PRINCE_TALDARAM_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(DATA_PRINCE_TALDARAM_EVENT, IN_PROGRESS);

            DoSendQuantumText(SAY_AGGRO, me);
        }

		void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            Unit* pEmbraceTarget = GetEmbraceTarget();
            if (Phase == PHASE_FEEDING && pEmbraceTarget && victim == pEmbraceTarget)
            {
                Phase = PHASE_NORMAL;
                PhaseTimer = 0;
                EmbraceTarget = 0;
            }
			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		bool CheckSpheres()
        {
            if (!instance)
                return false;

            uint64 uiSphereGuids[2];
            uiSphereGuids[0] = instance->GetData64(DATA_SPHERE1);
            uiSphereGuids[1] = instance->GetData64(DATA_SPHERE2);

            for (uint8 i=0; i < 2; ++i)
            {
                GameObject* pSpheres = instance->instance->GetGameObject(uiSphereGuids[i]);
                if (!pSpheres)
                    return false;
                if (pSpheres->GetGoState() != GO_STATE_ACTIVE)
                    return false;
            }
            RemovePrison();
            return true;
        }

        Unit* GetEmbraceTarget()
        {
            if (!EmbraceTarget)
                return NULL;

            return Unit::GetUnit(*me, EmbraceTarget);
        }

        void RemovePrison()
        {
            if (!instance)
				return;

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveAurasDueToSpell(SPELL_BEAM_VISUAL);
            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
			me->GetMotionMaster()->MoveFall();
            uint64 prison_GUID = instance->GetData64(DATA_PRINCE_TALDARAM_PLATFORM);
            instance->HandleGameObject(prison_GUID, true);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_PRINCE_TALDARAM_EVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (PhaseTimer <= diff)
            {
                switch (Phase)
                {
                    case PHASE_CASTING_FLAME_SPHERES:
                    {
                        Creature* pSpheres[3];

                        pSpheres[0] = DoSpawnCreature(NPC_FLAME_SPHERE, 0, 0, 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10*IN_MILLISECONDS);
                        Unit* pSphereTarget = SelectTarget(TARGET_RANDOM, 0, 100, true);
                        if (pSphereTarget && pSpheres[0])
                        {
                            float angle, x, y;
                            angle = pSpheres[0]->GetAngle(pSphereTarget);
                            x = pSpheres[0]->GetPositionX() + DATA_SPHERE_DISTANCE * cos(angle);
                            y = pSpheres[0]->GetPositionY() + DATA_SPHERE_DISTANCE * sin(angle);
                            pSpheres[0]->GetMotionMaster()->MovePoint(0, x, y, pSpheres[0]->GetPositionZ());
                        }
                        if (IsHeroic())
                        {
                            pSpheres[1] = DoSpawnCreature(NPC_FLAME_SPHERE_1, 0, 0, 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10*IN_MILLISECONDS);
                            pSpheres[2] = DoSpawnCreature(NPC_FLAME_SPHERE_2, 0, 0, 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10*IN_MILLISECONDS);
                            if (pSphereTarget && pSpheres[1] && pSpheres[2])
                            {
                                float angle, x, y;
                                angle = pSpheres[1]->GetAngle(pSphereTarget) + DATA_SPHERE_ANGLE_OFFSET;
                                x = pSpheres[1]->GetPositionX() + DATA_SPHERE_DISTANCE/2 * cos(angle);
                                y = pSpheres[1]->GetPositionY() + DATA_SPHERE_DISTANCE/2 * sin(angle);
                                pSpheres[1]->GetMotionMaster()->MovePoint(0, x, y, pSpheres[1]->GetPositionZ());
                                angle = pSpheres[2]->GetAngle(pSphereTarget) - DATA_SPHERE_ANGLE_OFFSET;
                                x = pSpheres[2]->GetPositionX() + DATA_SPHERE_DISTANCE/2 * cos(angle);
                                y = pSpheres[2]->GetPositionY() + DATA_SPHERE_DISTANCE/2 * sin(angle);
                                pSpheres[2]->GetMotionMaster()->MovePoint(0, x, y, pSpheres[2]->GetPositionZ());
                            }
                        }

                        Phase = PHASE_NORMAL;
                        PhaseTimer = 0;
                        break;
                    }
                    case PHASE_JUST_VANISHED:
                        if (Unit* pEmbraceTarget = GetEmbraceTarget())
                        {
                            me->GetMotionMaster()->Clear();
                            me->SetSpeed(MOVE_WALK, 2.0f, true);
                            me->GetMotionMaster()->MoveChase(pEmbraceTarget);
                        }
                        Phase = PHASE_VANISHED;
                        PhaseTimer = 1300;
                        break;
                    case PHASE_VANISHED:
                        me->SetVisible(true);
                        if (Unit* pEmbraceTarget = GetEmbraceTarget())
                            DoCast(pEmbraceTarget, DUNGEON_MODE(SPELL_EMBRACE_OF_THE_VAMPYR_5N, SPELL_EMBRACE_OF_THE_VAMPYR_5H));
                        me->GetMotionMaster()->Clear();
                        me->SetSpeed(MOVE_WALK, 1.0f, true);
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                        Phase = PHASE_FEEDING;
                        PhaseTimer = 20*IN_MILLISECONDS;
                        break;
                    case PHASE_FEEDING:
                        Phase = PHASE_NORMAL;
                        PhaseTimer = 0;
                        EmbraceTarget = 0;
                        break;
                    case PHASE_NORMAL:
                        if (BloodthirstTimer <= diff)
                        {
                            DoCast(me, SPELL_BLOODTHIRST);
                            BloodthirstTimer = 10*IN_MILLISECONDS;
                        }
						else BloodthirstTimer -= diff;

                        if (FlamesphereTimer <= diff)
                        {
							DoCastVictim(SPELL_CONJURE_FLAME_SPHERE);
                            Phase = PHASE_CASTING_FLAME_SPHERES;
                            PhaseTimer = 3*IN_MILLISECONDS + diff;
                            FlamesphereTimer = 15*IN_MILLISECONDS;
                        }
						else FlamesphereTimer -= diff;

                        if (VanishTimer <= diff)
                        {
                            Unit* target = NULL;
                            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                            std::vector<Unit*> target_list;
                            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                            {
                                target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                                // exclude pets & totems
                                if (target && target->GetTypeId() == TYPE_ID_PLAYER && target->IsAlive())
                                    target_list.push_back(target);
                                target = NULL;
                            }
                            //He only vanishes if there are 3 or more alive players
                            if (target_list.size() > 2)
                            {
                                DoSendQuantumText(RAND(SAY_VANISH_1, SAY_VANISH_2), me);
                                DoCast(me, SPELL_VANISH);
								me->SetInCombatState(true);
                                me->SetVisible(false);
                                Phase = PHASE_JUST_VANISHED;
                                PhaseTimer = 500;

                                if (Unit* pEmbraceTarget = SelectTarget(TARGET_RANDOM, 0, 100, true))
								{
                                    EmbraceTarget = pEmbraceTarget->GetGUID();
									VanishTimer = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS);
								}
							}
                        }
						else VanishTimer -= diff;

                        DoMeleeAttackIfReady();
						break;
					}
				}
                else PhaseTimer -= diff;
        }

        void DamageTaken(Unit* /*doneby*/, uint32 &damage)
        {
            Unit* pEmbraceTarget = GetEmbraceTarget();

            if (Phase == PHASE_FEEDING && pEmbraceTarget && pEmbraceTarget->IsAlive())
            {
				EmbraceTakenDamage += damage;
				if (EmbraceTakenDamage > (uint32) DUNGEON_MODE(DATA_EMBRACE_DMG_5N, DATA_EMBRACE_DMG_5H))
				{
					Phase = PHASE_NORMAL;
					PhaseTimer = 0;
					EmbraceTarget = 0;
					me->CastStop();
				}
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_taldaramAI(creature);
    }
};

class npc_taldaram_flame_sphere : public CreatureScript
{
public:
    npc_taldaram_flame_sphere() : CreatureScript("npc_taldaram_flame_sphere") {}

    struct npc_taldaram_flame_sphereAI : public QuantumBasicAI
    {
        npc_taldaram_flame_sphereAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 DespawnTimer;
        InstanceScript* instance;

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->AddUnitMovementFlag(MOVEMENTFLAG_FLYING);
            me->SetCurrentFaction(16);
            me->SetObjectScale(1.0f);
            DoCast(me, SPELL_FLAME_SPHERE_VISUAL);
            DoCast(me, SPELL_FLAME_SPHERE_SPAWN_EFFECT);
            DoCast(me, DUNGEON_MODE(SPELL_FLAME_SPHERE_PERIODIC_5N, SPELL_FLAME_SPHERE_PERIODIC_5H));
            DespawnTimer = 10*IN_MILLISECONDS;
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void JustDied(Unit* /*who*/)
        {
            DoCast(me, SPELL_FLAME_SPHERE_DEATH_EFFECT);
        }

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer <= diff)
                me->DisappearAndDie();
            else
                DespawnTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_taldaram_flame_sphereAI(creature);
    }
};

class go_prince_taldaram_sphere : public GameObjectScript
{
public:
    go_prince_taldaram_sphere() : GameObjectScript("go_prince_taldaram_sphere") {}

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        Creature* taldaram = Unit::GetCreature(*go, instance ? instance->GetData64(DATA_PRINCE_TALDARAM) : 0);
        if (taldaram && taldaram->IsAlive())
        {
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            go->SetGoState(GO_STATE_ACTIVE);

            switch (go->GetEntry())
            {
                case GO_SPHERE1:
					instance->SetData(DATA_SPHERE1_EVENT, IN_PROGRESS);
					break;
                case GO_SPHERE2:
					instance->SetData(DATA_SPHERE2_EVENT, IN_PROGRESS);
					break;
            }

            CAST_AI(boss_taldaram::boss_taldaramAI, taldaram->AI())->CheckSpheres();
        }
        return true;
    }
};

void AddSC_boss_taldaram()
{
    new boss_taldaram();
    new npc_taldaram_flame_sphere();
    new go_prince_taldaram_sphere();
}