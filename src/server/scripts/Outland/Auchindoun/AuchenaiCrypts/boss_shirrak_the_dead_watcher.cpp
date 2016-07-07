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
#include "auchenai_crypts.h"

/* ScriptData
Name: Boss_Shirrak_the_dead_watcher
%Complete: 80
Comment: InhibitMagic should stack slower far from the boss
Category: Auchindoun, Auchenai Crypts
EndScriptData */

enum Texts
{
	EMOTE_FOCUSES_ON = -1557999,
};

enum Spells
{
	SPELL_INHIBIT_MAGIC       = 32264,
	SPELL_ATTRACT_MAGIC       = 32265,
	SPELL_CARNIVOROUS_BITE_5N = 36383,
	SPELL_CARNIVOROUS_BITE_5H = 39382,
	SPELL_FIERY_BLAST_5N      = 32302,
	SPELL_FIERY_BLAST_5H      = 38382,
	SPELL_FOCUS_FIRE          = 32300,
};

class boss_shirrak_the_dead_watcher : public CreatureScript
{
public:
    boss_shirrak_the_dead_watcher() : CreatureScript("boss_shirrak_the_dead_watcher") { }

    struct boss_shirrak_the_dead_watcherAI : public QuantumBasicAI
    {
		boss_shirrak_the_dead_watcherAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

        uint32 InhibitmagicTimer;
		uint32 CarnivorousBiteTimer;
		uint32 FocusFireTimer;
        uint32 AttractmagicTimer;

        uint64 FocusedTargetGUID;

		SummonList Summons;

        void Reset()
        {
            InhibitmagicTimer = 0;
            CarnivorousBiteTimer = 2000;
            FocusFireTimer = 4000;
			AttractmagicTimer = 8000;

            FocusedTargetGUID = 0;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/){}

		void JustSummoned(Creature* summon)
		{
            if (summon->GetEntry() == NPC_FOCUS_FIRE)
            {
                summon->CastSpell(summon, SPELL_FOCUS_FIRE, true);
                summon->SetCurrentFaction(me->GetFaction());
                summon->SetLevel(me->GetCurrentLevel());
                summon->AddUnitState(UNIT_STATE_ROOT);

                if (Unit* FocusedTarget = Unit::GetUnit(*me, FocusedTargetGUID))
				{
					summon->AI()->EnterToBattle(FocusedTarget);
                    summon->AI()->AttackStart(FocusedTarget);
				}

				Summons.Summon(summon);
            }
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(const uint32 diff)
        {
            if (InhibitmagicTimer <= diff)
            {
                float dist;
                Map* map = me->GetMap();
                Map::PlayerList const &PlayerList = map->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
				{
                    if (Player* player = i->getSource())
					{
                        if (player->IsAlive() && (dist = player->IsWithinDist(me, 45.0f)))
                        {
                            player->RemoveAurasDueToSpell(SPELL_INHIBIT_MAGIC);
                            me->AddAura(SPELL_INHIBIT_MAGIC, player);
                            if (dist < 35)
                                me->AddAura(SPELL_INHIBIT_MAGIC, player);
                            if (dist < 25)
                                me->AddAura(SPELL_INHIBIT_MAGIC, player);
                            if (dist < 15)
                                me->AddAura(SPELL_INHIBIT_MAGIC, player);
                        }
					}
				}
				InhibitmagicTimer = 4000;
			}
			else InhibitmagicTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CarnivorousBiteTimer <= diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_CARNIVOROUS_BITE_5N, SPELL_CARNIVOROUS_BITE_5H));
                CarnivorousBiteTimer = 6000;
            }
			else CarnivorousBiteTimer -= diff;

			if (FocusFireTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target->GetTypeId() == TYPE_ID_PLAYER && target->IsAlive())
					{
						FocusedTargetGUID = target->GetGUID();
						DoSendQuantumText(EMOTE_FOCUSES_ON, me, target);
						me->SummonCreature(NPC_FOCUS_FIRE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 5500);
						FocusFireTimer = 8000;
					}
				}
			}
			else FocusFireTimer -= diff;

            if (AttractmagicTimer <= diff)
            {
                DoCastAOE(SPELL_ATTRACT_MAGIC);
                AttractmagicTimer = 12000;
            }
			else AttractmagicTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_shirrak_the_dead_watcherAI(creature);
    }
};

class npc_focus_fire : public CreatureScript
{
public:
    npc_focus_fire() : CreatureScript("npc_focus_fire") { }

    struct npc_focus_fireAI : public QuantumBasicAI
    {
        npc_focus_fireAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 FieryBlastTimer;

        bool fiery1, fiery2;

        void Reset()
        {
            FieryBlastTimer = 500;

            fiery1 = fiery2 = true;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (fiery2 && FieryBlastTimer <= diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_FIERY_BLAST_5N, SPELL_FIERY_BLAST_5H));

                if (fiery1)
					fiery1 = false;

                else if (fiery2)
					fiery2 = false;

                FieryBlastTimer = 1000;
            }
			else FieryBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_focus_fireAI(creature);
    }
};

void AddSC_boss_shirrak_the_dead_watcher()
{
    new boss_shirrak_the_dead_watcher();
    new npc_focus_fire();
}