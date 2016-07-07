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
#include "violet_hold.h"

enum Texts
{
    SAY_AGGRO           = -1608027,
    SAY_SLAY_1          = -1608028,
    SAY_SLAY_2          = -1608029,
    SAY_SLAY_3          = -1608030,
    SAY_DEATH           = -1608031,
    SAY_SPAWN           = -1608032,
    SAY_CHARGED         = -1608033,
    SAY_REPEAT_SUMMON_1 = -1608034,
    SAY_REPEAT_SUMMON_2 = -1608035,
    SAY_REPEAT_SUMMON_3 = -1608036,
};

enum Spells
{
    SPELL_ARCANE_BARRAGE_VOLLEY_5N = 54202,
    SPELL_ARCANE_BARRAGE_VOLLEY_5H = 59483,
    SPELL_SUMMON_ETHEREAL_SPHERE_1 = 54102,
    SPELL_SUMMON_ETHEREAL_SPHERE_2 = 54137,
    SPELL_SUMMON_ETHEREAL_SPHERE_3 = 54138,
	SPELL_ARCANE_TEMPEST_5N        = 35843,
	SPELL_ARCANE_TEMPEST_5H        = 49365,
	SPELL_ARCANE_POWER_5N          = 54160,
    SPELL_ARCANE_POWER_5H          = 59474,
    SPELL_ARCANE_BUFFED_5N         = 54226,
    SPELL_ARCANE_BUFFED_5H         = 59485,
    SPELL_SUMMON_PLAYERS           = 54164,
    SPELL_POWER_BALL_VISUAL        = 54141,
};

class boss_xevozz : public CreatureScript
{
public:
    boss_xevozz() : CreatureScript("boss_xevozz") { }

    struct boss_xevozzAI : public QuantumBasicAI
    {
        boss_xevozzAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 SummonEtherealSphereTimer;
        uint32 ArcaneBarrageVolleyTimer;
        uint32 ArcaneBuffetTimer;

		bool Intro;

        void Reset()
        {
			ArcaneBarrageVolleyTimer = 500;
			ArcaneBuffetTimer = 2000;
			SummonEtherealSphereTimer = 4000;

			Intro = false;

            DespawnSphere();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
				instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
				instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 25.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 25.0f))
			{
				DoSendQuantumText(SAY_SPAWN, me);
				Intro = true;
			}
		}

        void DespawnSphere()
        {
            std::list<Creature*> assistList;
            GetCreatureListWithEntryInGrid(assistList, me, NPC_ETHEREAL_SPHERE_N, 250.0f);

            if (assistList.empty())
                return;

            for (std::list<Creature*>::const_iterator iter = assistList.begin(); iter != assistList.end(); ++iter)
            {
                if (Creature* pSphere = *iter)
                    pSphere->Kill(pSphere, false);
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->SetSpeed(MOVE_RUN, 0.5f);

            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
            {
                summoned->AddThreat(target, 5.0f);
                summoned->AI()->AttackStart(target);
            }
        }

        void AttackStart(Unit* who)
        {
			if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER || UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			if (GameObject* Xcell = instance->instance->GetGameObject(instance->GetData64(DATA_XEVOZZ_CELL)))
			{
				if (Xcell->GetGoState() == GO_STATE_READY)
				{
					EnterEvadeMode();
					return;
				}

				if (instance->GetData(DATA_WAVE_COUNT) == 6)
					instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
				else if (instance->GetData(DATA_WAVE_COUNT) == 12)
					instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
			}
		}

		void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            DespawnSphere();

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
			{
				if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
					me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
				instance->SetData(DATA_WAVE_COUNT, 7);
			}
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
			{
				if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
					me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
				instance->SetData(DATA_WAVE_COUNT, 13);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArcaneBarrageVolleyTimer < diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_BARRAGE_VOLLEY_5N, SPELL_ARCANE_BARRAGE_VOLLEY_5H));
				ArcaneBarrageVolleyTimer = urand(3000, 4000);
            }
            else ArcaneBarrageVolleyTimer -= diff;

			if (ArcaneBuffetTimer < diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoSendQuantumText(SAY_CHARGED, me);
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BUFFED_5N, SPELL_ARCANE_BUFFED_5H));
					ArcaneBuffetTimer = urand(5000, 6000);
                }
            }
			else ArcaneBuffetTimer -= diff;

            if (SummonEtherealSphereTimer < diff)
            {
				DoSendQuantumText(RAND(SAY_REPEAT_SUMMON_1, SAY_REPEAT_SUMMON_2, SAY_REPEAT_SUMMON_3), me);
                DoCast(me, RAND(SPELL_SUMMON_ETHEREAL_SPHERE_1, SPELL_SUMMON_ETHEREAL_SPHERE_2, SPELL_SUMMON_ETHEREAL_SPHERE_3), true);

                if (IsHeroic())
                    me->SummonCreature(NPC_ETHEREAL_SPHERE_N, me->GetPositionX()-5+rand()%10, me->GetPositionY()-5+rand()%10, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 40000);

                SummonEtherealSphereTimer = urand(8000, 9000);
            }
            else SummonEtherealSphereTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_xevozzAI(creature);
    }
};

class npc_ethereal_sphere : public CreatureScript
{
public:
    npc_ethereal_sphere() : CreatureScript("npc_ethereal_sphere") { }

    struct npc_ethereal_sphereAI : public QuantumBasicAI
    {
        npc_ethereal_sphereAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

		uint32 RangeCheckTimer;
        uint32 SummonPlayersTimer;

        void Reset()
        {
            RangeCheckTimer = 500;
			SummonPlayersTimer = 5000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!me->HasAura(SPELL_POWER_BALL_VISUAL))
                DoCast(me, SPELL_POWER_BALL_VISUAL, true);

            if (RangeCheckTimer < diff)
            {
				if (Creature* pXevozz = Unit::GetCreature(*me, instance->GetData64(DATA_XEVOZZ)))
				{
					float fDistance = me->GetDistance2d(pXevozz);
					if (fDistance <= 3)
						DoCast(pXevozz, DUNGEON_MODE(SPELL_ARCANE_POWER_5N, SPELL_ARCANE_POWER_5H));
					else
						DoCast(me, DUNGEON_MODE(SPELL_ARCANE_TEMPEST_5N, SPELL_ARCANE_TEMPEST_5H));
				}
				RangeCheckTimer = 1000;
            }
            else RangeCheckTimer -= diff;

            if (SummonPlayersTimer < diff)
            {
                DoCastAOE(SPELL_SUMMON_PLAYERS);

                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (!PlayerList.isEmpty())
					{
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
						{
                            if (i->getSource()->IsAlive())
                                DoTeleportPlayer(i->getSource(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), i->getSource()->GetOrientation());
						}
					}
                }

                SummonPlayersTimer = 10000;
            }
            else SummonPlayersTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_sphereAI(creature);
    }
};

void AddSC_boss_xevozz()
{
    new boss_xevozz();
    new npc_ethereal_sphere();
}