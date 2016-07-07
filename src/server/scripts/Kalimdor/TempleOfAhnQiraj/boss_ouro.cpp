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

enum Spells
{
	SPELL_OURO_SUBMERGE      = 26063,
	SPELL_SPAWN              = 26262,
	SPELL_SWEEP              = 26103,
	SPELL_SANDBLAST          = 26102,
	SPELL_GROUND_RUPTURE     = 26100,   
	SPELL_DIRTMOUND_PASSIVE  = 26092,
	SPELL_SUMMON_OURO_MOUNDS = 26058,
};

class boss_ouro : public CreatureScript
{
public:
    boss_ouro() : CreatureScript("boss_ouro") { }

    struct boss_ouroAI : public QuantumBasicAI
    {
        boss_ouroAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SweepTimer;
        uint32 SandBlastTimer;
        uint32 SubmergeTimer;
        uint32 BackTimer;
        uint32 ChangeTargetTimer;
        uint32 SpawnTimer;

		bool Spawned;
        bool Enrage;
        bool Submerged;

        void Reset()
        {
            SweepTimer = urand(5000, 10000);
            SandBlastTimer = urand(20000, 35000);
            SubmergeTimer = urand(90000, 150000);
            BackTimer = urand(30000, 45000);
            ChangeTargetTimer = urand(5000, 8000);
            SpawnTimer = urand(10000, 20000);

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_OURO_SUBMERGE, true);

			Spawned = false;
            Enrage = false;
            Submerged = false;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (Spawned || who->GetTypeId() != TYPE_ID_PLAYER || who->ToPlayer()->IsGameMaster() || !who->IsWithinDistInMap(me, 35.0f))
				return;

			if (Spawned == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 35.0f))
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				me->RemoveAurasDueToSpell(SPELL_OURO_SUBMERGE);
				DoCast(me, SPELL_SPAWN);
				Spawned = true;
			}
		}

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!Submerged && SweepTimer <= diff)
            {
                DoCastVictim(SPELL_SWEEP);
                SweepTimer = urand(15000, 30000);
            }
			else SweepTimer -= diff;

            if (!Submerged && SandBlastTimer <= diff)
            {
                DoCastVictim(SPELL_SANDBLAST);
                SandBlastTimer = urand(20000, 35000);
            }
			else SandBlastTimer -= diff;

            if (!Submerged && SubmergeTimer <= diff)
            {
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
				me->SetReactState(REACT_PASSIVE);
				DoCast(me, SPELL_OURO_SUBMERGE, true);
                DoCast(me, SPELL_DIRTMOUND_PASSIVE, true);

                Submerged = true;
                BackTimer = urand(30000, 45000);
            }
			else SubmergeTimer -= diff;

            if (Submerged && ChangeTargetTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
					ChangeTargetTimer = urand(10000, 20000);
				}
            }
			else ChangeTargetTimer -= diff;

            if (Submerged && BackTimer <= diff)
            {
				me->RemoveAurasDueToSpell(SPELL_OURO_SUBMERGE);
				me->RemoveAurasDueToSpell(SPELL_DIRTMOUND_PASSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->SetReactState(REACT_AGGRESSIVE);

                DoCastVictim(SPELL_GROUND_RUPTURE);

                Submerged = false;
                SubmergeTimer = urand(60000, 120000);
            }
			else BackTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ouroAI(creature);
    }
};

void AddSC_boss_ouro()
{
    new boss_ouro();
}