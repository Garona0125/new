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

enum Yenniku
{
	QUEST_SAVING_YENNIKU  = 592,

	SPELL_YENNIKU_RELEASE = 3607,
};

class mob_yenniku : public CreatureScript
{
public:
    mob_yenniku() : CreatureScript("mob_yenniku") { }

    struct mob_yennikuAI : public QuantumBasicAI
    {
        mob_yennikuAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reseted = false;
        }

        uint32 ResetTimer;

        bool Reseted;

        void Reset()
        {
            ResetTimer = 0;
            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (caster->GetTypeId() == TYPE_ID_PLAYER) // Yenniku's Release
            {
                if (!Reseted && CAST_PLR(caster)->GetQuestStatus(QUEST_SAVING_YENNIKU) == QUEST_STATUS_INCOMPLETE && spell->Id == SPELL_YENNIKU_RELEASE)
                {
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STUN);
                    me->CombatStop();
                    me->DeleteThreatList();
                    me->SetCurrentFaction(83);
                    Reseted = true;
                    ResetTimer = 60000;
                }
            }
            return;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (Reseted)
            {
                if (ResetTimer <= diff)
                {
                    EnterEvadeMode();
                    Reseted = false;
                    me->SetCurrentFaction(28);                     //troll, bloodscalp
                    return;
                }
                else ResetTimer -= diff;

                if (me->IsInCombatActive() && me->GetVictim())
                {
                    if (me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        Unit* victim = me->GetVictim();

                        if (CAST_PLR(victim)->GetTeam() == HORDE)
                        {
                            me->CombatStop();
                            me->DeleteThreatList();
                        }
                    }
				}
			}

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_yennikuAI(creature);
    }
};

void AddSC_stranglethorn_vale()
{
    new mob_yenniku();
}