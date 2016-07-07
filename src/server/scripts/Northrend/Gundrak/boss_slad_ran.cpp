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
#include "gundrak.h"

enum Yells
{
    SAY_AGGRO               = -1604000,
    SAY_SUMMON_SNAKES       = -1604001,
    SAY_SUMMON_CONSTRICTORS = -1604002,
    SAY_SLAY_1              = -1604003,
    SAY_SLAY_2              = -1604004,
    SAY_SLAY_3              = -1604005,
    SAY_DEATH               = -1604006,
    EMOTE_POISON_NOVA       = -1604007,
	EMOTE_ACTIVATE_ALTAR    = -1604032,
};

enum Spells
{
    SPELL_POISON_NOVA_5N     = 55081,
    SPELL_POISON_NOVA_5H     = 59842,
    SPELL_POWERFULL_BITE_5N  = 48287,
    SPELL_POWERFULL_BITE_5H  = 59840,
    SPELL_VENOM_BOLT_5N      = 54970,
    SPELL_VENOM_BOLT_5H      = 59839,
	SPELL_VENOMOUS_BITE_5N   = 54987,
    SPELL_VENOMOUS_BITE_5H   = 58996,
	SPELL_GRIP_OF_SLAD_RAN   = 55093,
    SPELL_SNAKE_WRAP         = 55099,
    SPELL_SNAKE_WRAP_STUN    = 55126,
};

enum Achievements
{
    ACHIEV_SNAKES = 2058,
};

static Position SpawnLoc[] =
{
  {1783.81f, 646.637f, 133.948f, 3.71755f},
  {1775.03f, 606.586f, 134.165f, 1.43117f},
  {1717.39f, 630.041f, 129.282f, 5.96903f},
  {1765.66f, 646.542f, 134.02f,  5.11381f},
  {1716.76f, 635.159f, 129.282f, 0.191986f}
};

class boss_slad_ran : public CreatureScript
{
public:
    boss_slad_ran() : CreatureScript("boss_slad_ran") { }

    struct boss_slad_ranAI : public QuantumBasicAI
    {
        boss_slad_ranAI(Creature* creature) : QuantumBasicAI(creature), lSummons(me)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 PoisonNovaTimer;
        uint32 PowerfullBiteTimer;
        uint32 VenomBoltTimer;
        uint32 SpawnTimer;

        uint8 Phase;

        std::set<uint64> lUnWrappedPlayers;

        SummonList lSummons;

        InstanceScript* instance;

        void Reset()
        {
            PoisonNovaTimer = 10*IN_MILLISECONDS;
            PowerfullBiteTimer = 3*IN_MILLISECONDS;
            VenomBoltTimer = 15*IN_MILLISECONDS;
            SpawnTimer = 5*IN_MILLISECONDS;
            Phase = 0;

            lSummons.DespawnAll();
            lUnWrappedPlayers.clear();

			instance->SetData(DATA_SLAD_RAN_EVENT, NOT_STARTED);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_SLAD_RAN_EVENT, IN_PROGRESS);

			Map::PlayerList const &players = instance->instance->GetPlayers();
			for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
				lUnWrappedPlayers.insert(itr->getSource()->GetGUID());
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
			DoSendQuantumText(EMOTE_ACTIVATE_ALTAR, me);

            AchievementEntry const *achievSnakes = sAchievementStore.LookupEntry(ACHIEV_SNAKES);
            if (achievSnakes && IsHeroic())
            {
                for (std::set<uint64>::const_iterator itr = lUnWrappedPlayers.begin(); itr != lUnWrappedPlayers.end(); ++itr)
                {
                    Player* player = Unit::GetPlayer(*me, *itr);
                    if (player && player->IsAlive() && (player->GetDistance2d(me) < 100))
                        player->CompletedAchievement(achievSnakes);
                }
            }

			instance->SetData(DATA_SLAD_RAN_EVENT, DONE);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            lSummons.Summon(summoned);
        }

        void GotWrapped(Unit* unit)
        {
            if (unit && unit->GetTypeId() == TYPE_ID_PLAYER)
                lUnWrappedPlayers.erase(unit->GetGUID());
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Phase)
            {
                if (SpawnTimer <= diff)
                {
                    if ((Phase == 1) || (Phase == 2))
                        for (uint8 i = 0; i < DUNGEON_MODE(1, 2); ++i)
                            me->SummonCreature(NPC_SNAKE, SpawnLoc[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN,1*IN_MILLISECONDS);
                    if (Phase == 2)
                        for (uint8 i = 0; i < DUNGEON_MODE(1, 2); ++i)
                            me->SummonCreature(NPC_CONSTRICTORS, SpawnLoc[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN,1*IN_MILLISECONDS);
                    SpawnTimer = 10000;
                }
				else SpawnTimer -= diff;
            }

            if (Phase == 0 && HealthBelowPct(90))
            {
                DoSendQuantumText(SAY_SUMMON_SNAKES, me);
                Phase = 1;
            }

            if (Phase == 1 && HealthBelowPct(75))
            {
                DoSendQuantumText(SAY_SUMMON_CONSTRICTORS, me);
                Phase = 2;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (PoisonNovaTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_POISON_NOVA_5N, SPELL_POISON_NOVA_5H));
                DoSendQuantumText(EMOTE_POISON_NOVA, me);
                PoisonNovaTimer = 23*IN_MILLISECONDS;
            }
			else PoisonNovaTimer -= diff;

            if (PowerfullBiteTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_POWERFULL_BITE_5N, SPELL_POWERFULL_BITE_5H));
                PowerfullBiteTimer = 10*IN_MILLISECONDS;
            }
			else PowerfullBiteTimer -= diff;

            if (VenomBoltTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_VENOM_BOLT_5N, SPELL_VENOM_BOLT_5H));
                VenomBoltTimer = 10*IN_MILLISECONDS;
            }
			else VenomBoltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_slad_ranAI(creature);
    }
};

class npc_slad_ran_constrictor : public CreatureScript
{
public:
    npc_slad_ran_constrictor() : CreatureScript("npc_slad_ran_constrictor") { }

    struct npc_slad_ran_constrictorAI : public QuantumBasicAI
    {
        npc_slad_ran_constrictorAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 WrapTarget;
        uint32 GripOfSladRanTimer;
        uint32 EnwrapTimer;
        bool bEnwrapping;

        void Reset()
        {
            GripOfSladRanTimer = 1*IN_MILLISECONDS;
            EnwrapTimer = 3*IN_MILLISECONDS;
            WrapTarget = 0;
            bEnwrapping = false;
        }

        void CastGrip(Unit* target) // workaround
        {
            uint8 stackcount = 0;

            if (target->HasAura(SPELL_GRIP_OF_SLAD_RAN)) //if aura exists
            {
                 if (Aura* pGripAura = target->GetAura(SPELL_GRIP_OF_SLAD_RAN))
                 {
                     stackcount = pGripAura->GetStackAmount();

                     pGripAura->SetStackAmount(stackcount + 1); // add one stack
                     pGripAura->SetDuration(pGripAura->GetMaxDuration()); // reset aura duration

                     //if now stacked 5 times
                     if (stackcount >= 4)
                     {
						 target->RemoveAurasDueToSpell(SPELL_GRIP_OF_SLAD_RAN);

						 me->AddUnitState(UNIT_STATE_ROOT);
						 DoCast(target, SPELL_SNAKE_WRAP);

						 bEnwrapping = true;
						 WrapTarget = target->GetGUID();
					 }
				 }
			}
			else DoCast(target, SPELL_GRIP_OF_SLAD_RAN); 
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (GripOfSladRanTimer <= diff)
            {
                Unit* target = me->GetVictim();

                CastGrip(target);

                GripOfSladRanTimer = 5*IN_MILLISECONDS;
            }
			else GripOfSladRanTimer -= diff;

            if (bEnwrapping)
            {
                if (EnwrapTimer <= diff)
                {
                    if (Unit* target = Unit::GetUnit(*me, WrapTarget))
                    {
                        target->CastSpell(target, SPELL_SNAKE_WRAP_STUN, true);

                        // replace with Unit::GetCreature(*me, instance ? instance->GetData64(DATA_SLADRAN) : 0) later
                        if (Creature* pSladran = GetClosestCreatureWithEntry(me, NPC_SLAD_RAN, 100.0f))
                            CAST_AI(boss_slad_ran::boss_slad_ranAI, pSladran->AI())->GotWrapped(target);

                        me->DisappearAndDie();
                        WrapTarget = 0;
                    }
                    bEnwrapping = false;
                    EnwrapTimer = 3*IN_MILLISECONDS;
                }
				else EnwrapTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slad_ran_constrictorAI(creature);
    }
};

class npc_slad_ran_viper : public CreatureScript
{
public:
    npc_slad_ran_viper() : CreatureScript("npc_slad_ran_viper") { }

    struct npc_slad_ran_viperAI : public QuantumBasicAI
    {
        npc_slad_ran_viperAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 VenomousBiteTimer;

        InstanceScript* instance;

        void Reset()
        {
            VenomousBiteTimer = 2*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (VenomousBiteTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_VENOMOUS_BITE_5N, SPELL_VENOMOUS_BITE_5H));
                VenomousBiteTimer = 10*IN_MILLISECONDS;
            }
			else VenomousBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slad_ran_viperAI(creature);
    }
};

class npc_snake_wrap : public CreatureScript
{
public:
    npc_snake_wrap() : CreatureScript("npc_snake_wrap") { }

    struct npc_snake_wrapAI : public QuantumBasicAI
    {
        npc_snake_wrapAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 WrapTargetGUID;

        void Reset()
        {
            WrapTargetGUID = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void JustDied(Unit *killer)
        {
            if (WrapTargetGUID)
            {
                Unit* target = Unit::GetUnit(*me, WrapTargetGUID);
                if (target)
                    target->RemoveAurasDueToSpell(SPELL_SNAKE_WRAP_STUN);
            }
            me->RemoveCorpse();
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!me->ToTempSummon())
                return;

            if (Unit* pSummoner = me->ToTempSummon()->GetSummoner())
                WrapTargetGUID = pSummoner->GetGUID();

            Unit* temp = Unit::GetUnit(*me, WrapTargetGUID);
            if ((temp && temp->IsAlive() && !temp->HasAura(SPELL_SNAKE_WRAP_STUN)) || !temp)
                me->DealDamage(me, me->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snake_wrapAI(creature);
    }
};

void AddSC_boss_slad_ran()
{
    new boss_slad_ran();
    new npc_slad_ran_constrictor();
    new npc_slad_ran_viper();
    new npc_snake_wrap();
}