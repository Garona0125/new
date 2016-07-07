/* ScriptData
SDName: Boss_Supremus
SD%Complete: 95
SDComment: Need to implement molten punch
SDCategory: Black Temple
EndScriptData */

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
#include "black_temple.h"

enum Emotes
{
	EMOTE_NEW_TARGET           = -1564010,
	EMOTE_PUNCH_GROUND         = -1564011,
	EMOTE_GROUND_CRACK         = -1564012,
};

enum Spells
{
	SPELL_MOLTEN_PUNCH         = 40126,
	SPELL_HATEFUL_STRIKE       = 41926,
	SPELL_MOLTEN_FLAME         = 40980,
	SPELL_VOLCANIC_ERUPTION    = 40117,
	SPELL_VOLCANIC_SUMMON      = 40276,
	SPELL_BERSERK              = 45078,
	SPELL_CHARGE               = 41581,
};

class molten_flame : public CreatureScript
{
public:
    molten_flame() : CreatureScript("molten_flame") { }

    struct molten_flameAI : public NullCreatureAI
    {
        molten_flameAI(Creature* creature) : NullCreatureAI(creature)
        {
            float x, y, z;
            me->CastSpell(me, SPELL_MOLTEN_FLAME, true);
            me->GetNearPoint(me, x, y, z, 1.0f, 50.0f, float(M_PI*2*rand_norm()));
            me->GetMotionMaster()->MovePoint(0, x, y, z);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new molten_flameAI(creature);
    }
};

class boss_supremus : public CreatureScript
{
public:
    boss_supremus() : CreatureScript("boss_supremus") { }

    struct boss_supremusAI : public QuantumBasicAI
    {
        boss_supremusAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 SummonFlameTimer;
        uint32 SwitchTargetTimer;
        uint32 PhaseSwitchTimer;
        uint32 SummonVolcanoTimer;
        uint32 HatefulStrikeTimer;
        uint32 BerserkTimer;

        uint32 pulseTimer;

        bool Phase1;

        SummonList summons;

        void Reset()
        {
			if (me->IsAlive())
				instance->SetData(DATA_SUPREMUSEVENT, NOT_STARTED);

            HatefulStrikeTimer = 5000;
            SummonFlameTimer = 20000;
            SwitchTargetTimer = 90000;
            PhaseSwitchTimer = 60000;
            SummonVolcanoTimer = 5000;
            BerserkTimer = 900000;                              // 15 minute enrage

            pulseTimer = 10000;

            Phase1 = true;
            summons.DespawnAll();

            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();

			instance->SetData(DATA_SUPREMUSEVENT, IN_PROGRESS);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_SUPREMUSEVENT, DONE);

            summons.DespawnAll();
        }

        void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);
		}

        void SummonedCreatureDespawn(Creature *summon)
		{
			summons.Despawn(summon);
		}

        Unit* CalculateHatefulStrikeTarget()
        {
            uint32 health = 0;
            Unit* target = NULL;

            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            std::list<HostileReference*>::iterator i = m_threatlist.begin();
            for (i = m_threatlist.begin(); i!= m_threatlist.end();++i)
            {
                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                if (unit && me->IsWithinMeleeRange(unit))
                {
                    if (unit->GetHealth() > health)
                    {
                        health = unit->GetHealth();
                        target = unit;
                    }
                }
            }
            return target;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (pulseTimer <= diff)
            {
                if (me->GetVictim() && me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER)
                    DoAttackerGroupInCombat(((Player*)me->GetVictim()));
                else 
                    DoAttackerAreaInCombat(me->GetVictim(),50);
                pulseTimer = 5000;
            }
			else pulseTimer -= diff;

            if (!me->HasAura(SPELL_BERSERK, 0))
            {
				if (BerserkTimer <= diff)
				{
					DoCast(me, SPELL_BERSERK);
				}
				else BerserkTimer -= diff;
            }

            if (SummonFlameTimer <= diff)
            {
                DoCast(me, SPELL_MOLTEN_PUNCH);
				DoSendQuantumText(EMOTE_PUNCH_GROUND, me);
                SummonFlameTimer = 10000;
            }
			else SummonFlameTimer -= diff;

            if (Phase1)
            {
                if (HatefulStrikeTimer <= diff)
                {
                    if (Unit* target = CalculateHatefulStrikeTarget())
                    {
                        DoCast(target, SPELL_HATEFUL_STRIKE);
                        HatefulStrikeTimer = 5000;
                    }
                }
				else HatefulStrikeTimer -= diff;
            }

            if (!Phase1)
            {
                if (SwitchTargetTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                    {
                        if (me->GetDistance2d(me->GetVictim()) < 40)
                            me->CastSpell(me->GetVictim(),SPELL_CHARGE,false);

                        DoResetThreat();
                        me->AddThreat(target, 5000000.0f);
                        DoSendQuantumText(EMOTE_NEW_TARGET, me);
                        SwitchTargetTimer = 10000;
                    }
                }
				else SwitchTargetTimer -= diff;

                if (SummonVolcanoTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 999, true))
                    {
                        DoCast(target, SPELL_VOLCANIC_SUMMON);
                        DoSendQuantumText(EMOTE_GROUND_CRACK, me);
                        SummonVolcanoTimer = 10000;
                    }
                }
				else SummonVolcanoTimer -= diff;
            }

            if (PhaseSwitchTimer <= diff)
            {
                if (!Phase1)
                {
                    Phase1 = true;
                    DoResetThreat();
                    PhaseSwitchTimer = 60000;
                    me->SetSpeed(MOVE_RUN, 1.2f);
                    DoZoneInCombat();
                    me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
                }
                else
                {
                    Phase1 = false;
                    DoResetThreat();
                    SwitchTargetTimer = 10000;
                    SummonVolcanoTimer = 2000;
                    PhaseSwitchTimer = 60000;
                    me->SetSpeed(MOVE_RUN, 0.9f);
                    DoZoneInCombat();
                    me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
                }
            }
			else PhaseSwitchTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_supremusAI(creature);
    }
};

class npc_volcano : public CreatureScript
{
public:
    npc_volcano() : CreatureScript("npc_volcano") { }

    struct npc_volcanoAI : public QuantumBasicAI
    {
        npc_volcanoAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CheckTimer;

        bool Eruption;

        void Reset()
        {
            CheckTimer = 3000;
            Eruption = false;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            me->GetMotionMaster()->MoveIdle();
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/)
        {
            return; // paralyze the npc
        }

        void AttackStart(Unit* /*who*/)
        {
            return;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                uint64 SupremusGUID = instance->GetData64(DATA_SUPREMUS);
                Creature* Supremus = (Unit::GetCreature(*me, SupremusGUID));
                if (!Eruption && Supremus && ! CAST_AI(boss_supremus::boss_supremusAI,Supremus->AI())->Phase1)
                {
                    Eruption = true;
                    DoCast(me, SPELL_VOLCANIC_ERUPTION);
                }
                else if ((Eruption && Supremus && CAST_AI(boss_supremus::boss_supremusAI,Supremus->AI())->Phase1) || !Supremus)
                {
                    if (me->HasAura(SPELL_VOLCANIC_ERUPTION, 0))
                        me->RemoveAura(SPELL_VOLCANIC_ERUPTION, 0);
                }
                CheckTimer = 1500;
            }
			else CheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_volcanoAI(creature);
    }
};

void AddSC_boss_supremus()
{
    new boss_supremus();
    new molten_flame();
    new npc_volcano();
}