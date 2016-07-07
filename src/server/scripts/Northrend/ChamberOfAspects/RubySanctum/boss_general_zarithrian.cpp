/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumEscortAI.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "ruby_sanctum.h"

enum Texts
{
	SAY_AGGRO  = -1666200,
	SAY_SLAY_1 = -1666201,
	SAY_SLAY_2 = -1666202,
	SAY_DEATH  = -1666203,
	SAY_SUMMON = -1666204,
};

enum Spells
{
    SPELL_CALL_FLAMECALLER  = 74398,
    SPELL_CLEAVE_ARMOR      = 74367,
    SPELL_IMTIMIDATING_ROAR = 74384,
    SPELL_LAVA_GOUT_10      = 74394,
	SPELL_LAVA_GOUT_25      = 74395,
    SPELL_BLAST_NOVA_10     = 74392,
	SPELL_BLAST_NOVA_25     = 74393,
};

enum Equipment
{
    EQUIP_MAIN     = 47156,
    EQUIP_OFFHAND  = 51812,
    EQUIP_RANGED   = EQUIP_NO_CHANGE,
    EQUIP_DONE     = EQUIP_NO_CHANGE,
};

struct Locations
{
    float x, y, z, o;
};

static Locations SpawnLoc[] =
{
    {3008.552734f, 530.471680f, 89.195290f, 6.16f}, // 0 - zarithrian start point, o = 6,16
    {3014.313477f, 486.453735f, 89.255096f, 6.16f}, // 1 - Where mob attacks 1
    {3025.324951f, 580.588501f, 88.593185f, 6.16f}, // 2 - Where mob attacks 2
	{3043.649902f, 452.039001f, 85.533600f, 6.16f},	// 3 - where mob spawns 1
	{3048.159912f, 601.634985f, 86.851196f, 6.16f},	// 4 - where mob spawns 2
};

class boss_general_zarithrian : public CreatureScript
{
public:
    boss_general_zarithrian() : CreatureScript("boss_general_zarithrian") { }

    struct boss_general_zarithrianAI : public QuantumBasicAI
    {
        boss_general_zarithrianAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();

            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);

			Reset();
        }

        InstanceScript* instance;
		std::list<Unit*> PlayerList;

        uint32 SummonTimer;
        uint32 CleaveTimer;
        uint32 ImtimidatingTimer;

        void Reset()
        {
			if (!instance)
				return;

			SummonTimer = 15*IN_MILLISECONDS;
            CleaveTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            ImtimidatingTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (me->IsAlive())
            {
                instance->SetData(TYPE_ZARITHRIAN, NOT_STARTED);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			}

			PlayerList.clear();
        }

		void EnterToBattle(Unit* /*who*/) 
        {
            if (!instance)
				return;

            SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);
            instance->SetData(TYPE_ZARITHRIAN, IN_PROGRESS);
            DoSendQuantumText(SAY_AGGRO, me);
        }
		
		void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustReachedHome()
        {
            if (!instance)
				return;

            instance->SetData(TYPE_ZARITHRIAN, FAIL);
        }

        void JustSummoned(Creature* summoned)
        {
			if (!instance || !summoned) return;

            summoned->SetInCombatWithZone();
			summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}

        void JustDied(Unit* killer)
        {
            if (!instance)
				return;

            instance->SetData(TYPE_ZARITHRIAN, DONE);
            DoSendQuantumText(SAY_DEATH, me);
        }

        void UpdateAI(const uint32 diff)
        {
			if (instance->GetData(TYPE_XERESTRASZA) == DONE && instance->GetData(TYPE_BALTHARUS) == DONE && instance->GetData(TYPE_RAGEFIRE) == DONE)
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			
			if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SummonTimer <= diff)
			{
				me->SummonCreature(NPC_ONYX_FLAMECALLER, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, SpawnLoc[3].o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				me->SummonCreature(NPC_ONYX_FLAMECALLER, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, SpawnLoc[4].o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				DoSendQuantumText(SAY_SUMMON, me);
				SummonTimer = 45*IN_MILLISECONDS;
				if (instance->instance->GetSpawnMode() == RAID_DIFFICULTY_25MAN_NORMAL || instance->instance->GetSpawnMode() == RAID_DIFFICULTY_25MAN_HEROIC)
				{
					switch (urand(0, 1))
					{
					    case 0:
							me->SummonCreature(NPC_ONYX_FLAMECALLER, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, SpawnLoc[3].o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
							break;
						case 1:
							me->SummonCreature(NPC_ONYX_FLAMECALLER, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, SpawnLoc[4].o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
							break;
					}
				}
            }
			else SummonTimer -= diff;
			
			if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE_ARMOR);
                CleaveTimer = urand(10*IN_MILLISECONDS,15*IN_MILLISECONDS);
            }
			else CleaveTimer -= diff;

            if (ImtimidatingTimer <= diff)
			{
				DoCastVictim(SPELL_IMTIMIDATING_ROAR);
				ImtimidatingTimer = urand(15*IN_MILLISECONDS,25*IN_MILLISECONDS);
			}
			else ImtimidatingTimer -= diff;
			
			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_general_zarithrianAI(creature);
    }
};

class npc_onyx_flamecaller : public CreatureScript
{
public:
    npc_onyx_flamecaller() : CreatureScript("npc_onyx_flamecaller") { }

    struct npc_onyx_flamecallerAI : public QuantumBasicAI
    {
        npc_onyx_flamecallerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        InstanceScript* instance;

		uint32 LavaGoutCount;
        uint32 LavaGoutTimer;
        uint32 BlastNovaTimer;
		uint8 nextPoint;
		uint8 stage;
		bool MovementStarted;

        void Reset()
        {
            if (!instance)
				return;

            LavaGoutTimer = urand(10*IN_MILLISECONDS,14*IN_MILLISECONDS);
			LavaGoutCount = 0;
            BlastNovaTimer = urand(10*IN_MILLISECONDS,25*IN_MILLISECONDS);
            me->SetRespawnDelay(7*DAY);
			nextPoint = 0;
			setStage(0);
        }

		void setStage(uint8 phase)
        {
            stage = phase;
        }

        uint8 getStage()
        {
            return stage;
        }

		void MovementInform(uint32 type, uint32 id)
		{
            if (!instance)
				return;

            if (type != POINT_MOTION_TYPE || !MovementStarted)
				return;

            if (id == nextPoint)
			{
				me->GetMotionMaster()->MovementExpired();
				MovementStarted = false;
			}
        }

		void StartMovement(uint32 id)
		{
            nextPoint = id;
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(id, SpawnLoc[id].x, SpawnLoc[id].y, SpawnLoc[id].z);
            MovementStarted = true;
        }
		
		void UpdateAI(const uint32 diff)
        {
			if (instance->GetData(TYPE_ZARITHRIAN) != IN_PROGRESS)
				me->DespawnAfterAction();

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			switch (getStage())
            {
			case 0:
				if (me->GetPositionY() < 500.0f) StartMovement(1);
				else
					StartMovement(2);
				setStage(1);
				break;
			case 1:
				if (MovementStarted)
					return;
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100.0f, true))
				{
					me->AddThreat(target, 100.0f);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					me->GetMotionMaster()->MoveChase(target);
					setStage(2);
				}
				break;
			case 2:
				if (LavaGoutTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (me->IsNonMeleeSpellCasted(false) && LavaGoutCount < 5)
						{
							if (GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
								DoCast(target,SPELL_LAVA_GOUT_10);
							else
								DoCast(target,SPELL_LAVA_GOUT_25);
							LavaGoutTimer = 2*IN_MILLISECONDS;
							LavaGoutCount += 1;
						}
						else 
						{
							LavaGoutTimer = urand(10*IN_MILLISECONDS,14*IN_MILLISECONDS);
							LavaGoutCount = 0;
						}
					}
				}
				else LavaGoutTimer -= diff;
				
				if (BlastNovaTimer <= diff)
				{
					if (GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
						DoCast(SPELL_BLAST_NOVA_10);
					else
						DoCast(SPELL_BLAST_NOVA_25);
					BlastNovaTimer = urand(10*IN_MILLISECONDS,25*IN_MILLISECONDS);
				}
				else BlastNovaTimer -= diff;

				DoMeleeAttackIfReady();

			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onyx_flamecallerAI(creature);
    }
};

void AddSC_boss_general_zarithrian()
{
    new boss_general_zarithrian();
    new npc_onyx_flamecaller();
}