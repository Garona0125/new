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
#include "mechanar.h"

enum Texts
{
    SAY_AGGRO_1                   = -1554006,
    SAY_HAMMER_1                  = -1554007,
    SAY_HAMMER_2                  = -1554008,
    SAY_SLAY_1                    = -1554009,
    SAY_SLAY_2                    = -1554010,
    SAY_DEATH_1                   = -1554011,
    EMOTE_HAMMER                  = -1554012,
};

enum Spells
{
    SPELL_SHADOW_POWER_5N         = 35322,
    SPELL_SHADOW_POWER_5H         = 39193,
    SPELL_HAMMER_PUNCH            = 35326,
    SPELL_JACKHAMMER_5N           = 35327,
    SPELL_JACKHAMMER_5H           = 39194,
    SPELL_STREAM_OF_MACHINE_FLUID = 35311,
};

class boss_gatewatcher_iron_hand : public CreatureScript
{
    public:
        boss_gatewatcher_iron_hand() : CreatureScript("boss_gatewatcher_iron_hand") { }

            struct boss_gatewatcher_iron_handAI : public QuantumBasicAI
            {
                boss_gatewatcher_iron_handAI(Creature* creature) : QuantumBasicAI(creature) {}

                uint32 ShadowPowerTimer;
                uint32 JackhammerTimer;
                uint32 StreamofMachineFluidTimer;

                void Reset()
                {
                    ShadowPowerTimer = 25000;
                    JackhammerTimer = 45000;
                    StreamofMachineFluidTimer = 55000;

					DoCast(me, SPELL_UNIT_DETECTION_ALL);
					me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
                }

                void EnterToBattle(Unit* who)
                {
					if (who->GetTypeId() == TYPE_ID_PLAYER)
						DoSendQuantumText(SAY_AGGRO_1, me);
                }

                void KilledUnit(Unit* victim)
                {
					if (victim->GetTypeId() == TYPE_ID_PLAYER)
						DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
                }

                void JustDied(Unit* /*Killer*/)
                {
                    DoSendQuantumText(SAY_DEATH_1, me);
                    //TODO: Add door check/open code
                }

                void UpdateAI(const uint32 diff)
                {
                    if (!UpdateVictim())
                        return;

					if (me->HasUnitState(UNIT_STATE_CASTING))
						return;

                    if (ShadowPowerTimer <= diff)
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_SHADOW_POWER_5N, SPELL_SHADOW_POWER_5H));
                        ShadowPowerTimer = urand(20000, 28000);
                    }
                    else ShadowPowerTimer -= diff;

                    if (JackhammerTimer <= diff)
                    {
                        //TODO: expect cast this about 5 times in a row (?), announce it by emote only once
                        DoSendQuantumText(EMOTE_HAMMER, me);
                        DoCastVictim(DUNGEON_MODE(SPELL_JACKHAMMER_5N, SPELL_JACKHAMMER_5H));
                        DoSendQuantumText(RAND(SAY_HAMMER_1, SAY_HAMMER_2), me);
                        JackhammerTimer = 30000;
                    }
                    else JackhammerTimer -= diff;

                    if (StreamofMachineFluidTimer <= diff)
                    {
                        DoCastVictim(SPELL_STREAM_OF_MACHINE_FLUID);
                        StreamofMachineFluidTimer = urand(35000, 50000);
                    }
                    else StreamofMachineFluidTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            };

            CreatureAI* GetAI(Creature* creature) const
            {
                return new boss_gatewatcher_iron_handAI(creature);
            }
};

void AddSC_boss_gatewatcher_iron_hand()
{
    new boss_gatewatcher_iron_hand();
}