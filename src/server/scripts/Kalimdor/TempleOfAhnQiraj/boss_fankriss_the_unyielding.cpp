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
#include "temple_of_ahnqiraj.h"

enum Sounds
{
	SOUND_SENTENCE_YOU  = 8588,
	SOUND_SERVE_TO      = 8589,
	SOUND_LAWS          = 8590,
	SOUND_TRESPASS      = 8591,
	SOUND_WILL_BE       = 8592,
};

enum Spells
{
	SPELL_MORTAL_WOUND  = 28467,
	SPELL_ROOT          = 28858,
	SPELL_ENRAGE        = 28798,
};

class boss_fankriss_the_unyielding : public CreatureScript
{
public:
    boss_fankriss_the_unyielding() : CreatureScript("boss_fankriss_the_unyielding") { }

    struct boss_fankriss_the_unyieldingAI : public QuantumBasicAI
    {
        boss_fankriss_the_unyieldingAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MortalWoundTimer;
        uint32 SpawnHatchlingsTimer;
        uint32 SpawnSpawnsTimer;
        int Rand;
        float RandX;
        float RandY;

        Creature* Hatchling;
        Creature* Spawn;

        void Reset()
        {
            MortalWoundTimer = urand(10000, 15000);
            SpawnHatchlingsTimer = urand(6000, 12000);
            SpawnSpawnsTimer = urand(15000, 45000);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void SummonSpawn(Unit* victim)
        {
            if (!victim)
                return;

            Rand = 10 + (rand()%10);
            switch (rand()%2)
            {
                case 0:
					RandX = 0.0f - Rand;
					break;
                case 1:
					RandX = 0.0f + Rand;
					break;
            }

            Rand = 10 + (rand()%10);
            switch (rand()%2)
            {
                case 0:
					RandY = 0.0f - Rand;
					break;
                case 1:
					RandY = 0.0f + Rand;
					break;
            }
            Rand = 0;
            Spawn = DoSpawnCreature(NPC_SPAWN_OF_FANKRISS, RandX, RandY, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            if (Spawn)
                Spawn->AI()->AttackStart(victim);
        }

        void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(RAND(SOUND_SERVE_TO, SOUND_LAWS, SOUND_TRESPASS, SOUND_WILL_BE));
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MortalWoundTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_WOUND);
                MortalWoundTimer = urand(10000, 20000);
            }
			else MortalWoundTimer -= diff;

            if (SpawnSpawnsTimer <= diff)
            {
                switch (urand(0, 2))
                {
                    case 0:
                        SummonSpawn(SelectTarget(TARGET_RANDOM, 0));
                        break;
                    case 1:
                        SummonSpawn(SelectTarget(TARGET_RANDOM, 0));
                        SummonSpawn(SelectTarget(TARGET_RANDOM, 0));
                        break;
                    case 2:
                        SummonSpawn(SelectTarget(TARGET_RANDOM, 0));
                        SummonSpawn(SelectTarget(TARGET_RANDOM, 0));
                        SummonSpawn(SelectTarget(TARGET_RANDOM, 0));
                        break;
                }
                SpawnSpawnsTimer = urand(30000, 60000);
            }
			else SpawnSpawnsTimer -= diff;

            if (HealthAbovePct(3))
            {
                if (SpawnHatchlingsTimer <= diff)
                {
                    Unit* target = NULL;
                    target = SelectTarget(TARGET_RANDOM, 0);
                    if (target && target->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        DoCast(target, SPELL_ROOT);

                        if (DoGetThreat(target))
                            DoModifyThreatPercent(target, -100);

                        switch (urand(0, 2))
                        {
                            case 0:
                                DoTeleportPlayer(target, -8106.0142f, 1289.2900f, -74.419533f, 5.112f);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-3, target->GetPositionY()-3, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-3, target->GetPositionY()+3, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-5, target->GetPositionY()-5, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-5, target->GetPositionY()+5, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                break;
                            case 1:
                                DoTeleportPlayer(target, -7990.135354f, 1155.1907f, -78.849319f, 2.608f);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-3, target->GetPositionY()-3, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-3, target->GetPositionY()+3, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-5, target->GetPositionY()-5, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-5, target->GetPositionY()+5, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                break;
                            case 2:
                                DoTeleportPlayer(target, -8159.7753f, 1127.9064f, -76.868660f, 0.675f);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-3, target->GetPositionY()-3, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-3, target->GetPositionY()+3, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-5, target->GetPositionY()-5, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                Hatchling = me->SummonCreature(NPC_VEKNISS_HATCHING, target->GetPositionX()-5, target->GetPositionY()+5, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (Hatchling)
                                    Hatchling->AI()->AttackStart(target);
                                break;
                        }
                    }
                    SpawnHatchlingsTimer = urand(45000, 60000);
                }
				else SpawnHatchlingsTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fankriss_the_unyieldingAI(creature);
    }
};

void AddSC_boss_fankriss_the_unyielding()
{
    new boss_fankriss_the_unyielding();
}