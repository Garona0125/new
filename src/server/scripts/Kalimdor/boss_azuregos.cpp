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

enum Says
{
	SAY_TELEPORT       = 0,
};

enum Spells
{
    SPELL_MARKOFFROST  = 23182,
    SPELL_MANASTORM    = 21097,
    SPELL_CHILL        = 21098,
    SPELL_FROSTBREATH  = 21099,
    SPELL_REFLECT      = 22067,
    SPELL_CLEAVE       = 8255,
    SPELL_ENRAGE       = 23537,
};

class boss_azuregos : public CreatureScript
{
public:
    boss_azuregos() : CreatureScript("boss_azuregos") { }

    struct boss_azuregosAI : public QuantumBasicAI
    {
        boss_azuregosAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MarkOfFrostTimer;
        uint32 ManaStormTimer;
        uint32 ChillTimer;
        uint32 BreathTimer;
        uint32 TeleportTimer;
        uint32 ReflectTimer;
        uint32 CleaveTimer;
        uint32 EnrageTimer;

        bool Enraged;

        void Reset()
        {
            MarkOfFrostTimer = 35000;
            ManaStormTimer = urand(5000, 17000);
            ChillTimer = urand(10000, 30000);
            BreathTimer = urand(2000, 8000);
            TeleportTimer = 30000;
            ReflectTimer = urand(15000, 30000);
            CleaveTimer = 7000;
            EnrageTimer = 0;

            Enraged = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (TeleportTimer <= diff)
            {
				Talk(SAY_TELEPORT);
                std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
                std::list<HostileReference*>::const_iterator i = threatlist.begin();
                for (i = threatlist.begin(); i!= threatlist.end(); ++i)
                {
                    Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());

                    if (unit && (unit->GetTypeId() == TYPE_ID_PLAYER))
                        DoTeleportPlayer(unit, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()+3, unit->GetOrientation());
                }

                DoResetThreat();
                TeleportTimer = 30000;
            }
			else TeleportTimer -= diff;

            if (ChillTimer <= diff)
            {
                DoCastVictim(SPELL_CHILL);
                ChillTimer = urand(13000, 25000);
            }
			else ChillTimer -= diff;

            if (BreathTimer <= diff)
            {
                DoCastVictim(SPELL_FROSTBREATH);
                BreathTimer = urand(10000, 15000);
            }
			else BreathTimer -= diff;

            if (ManaStormTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_MANASTORM);
					ManaStormTimer = urand(7500, 12500);
				}
            }
			else ManaStormTimer -= diff;

            if (ReflectTimer <= diff)
            {
                DoCast(me, SPELL_REFLECT);
                ReflectTimer = urand(20000, 35000);
            }
			else ReflectTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 7000;
            }
			else CleaveTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_30) && !Enraged)
            {
                DoCast(me, SPELL_ENRAGE);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_azuregosAI (creature);
    }
};

void AddSC_boss_azuregos()
{
    new boss_azuregos();
}