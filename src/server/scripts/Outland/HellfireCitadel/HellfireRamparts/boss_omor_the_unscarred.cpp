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
#include "hellfire_ramparts.h"

enum Says
{
    SAY_AGGRO_1 = -1543009,
    SAY_AGGRO_2 = -1543010,
    SAY_AGGRO_3 = -1543011,
    SAY_SUMMON  = -1543012,
    SAY_CURSE   = -1543013,
    SAY_KILL_1  = -1543014,
    SAY_DIE     = -1543015,
    SAY_WIPE    = -1543016,
};

enum Spells
{
    SPELL_ORBITAL_STRIKE        = 30637,
    SPELL_SHADOW_WHIP           = 30638,
    SPELL_TREACHEROUS_AURA_5N   = 30695,
	SPELL_TREACHEROUS_AURA_5H   = 37566,
    SPELL_DEMONIC_SHIELD        = 31901,
    SPELL_SHADOW_BOLT_5N        = 30686,
    SPELL_SHADOW_BOLT_5H        = 39297,
    SPELL_SUMMON_FIENDISH_HOUND = 30707,
};

class boss_omor_the_unscarred : public CreatureScript
{
    public:
        boss_omor_the_unscarred() : CreatureScript("boss_omor_the_unscarred") { }

        struct boss_omor_the_unscarredAI : public QuantumBasicAI
        {
            boss_omor_the_unscarredAI(Creature* creature) : QuantumBasicAI(creature)
            {
                SetCombatMovement(false);
            }

            uint32 OrbitalStrikeTimer;
            uint32 ShadowWhipTimer;
            uint32 AuraTimer;
            uint32 DemonicShieldTimer;
            uint32 ShadowboltTimer;
            uint32 SummonTimer;
            uint32 SummonedCount;

            uint64 PlayerGUID;

            bool CanPullBack;

            void Reset()
            {
                DoSendQuantumText(SAY_WIPE, me);

                OrbitalStrikeTimer = 25000;
                ShadowWhipTimer = 2000;
                AuraTimer = 10000;
                DemonicShieldTimer = 1000;
                ShadowboltTimer = 2000;
                SummonTimer = 10000;
                SummonedCount = 0;
                PlayerGUID = 0;
                CanPullBack = false;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                if (rand()%2)
                    return;

                DoSendQuantumText(SAY_KILL_1, me);
            }

            void JustSummoned(Creature* summoned)
            {
                DoSendQuantumText(SAY_SUMMON, me);

                if (Unit* random = SelectTarget(TARGET_RANDOM, 0))
                    summoned->AI()->AttackStart(random);

                ++SummonedCount;
            }

            void JustDied(Unit* /*Killer*/)
            {
                DoSendQuantumText(SAY_DIE, me);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //only two may be wrong, perhaps increase timer and spawn periodically instead.
                if (SummonedCount < 2)
                {
                    if (SummonTimer <= diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_SUMMON_FIENDISH_HOUND);
                        SummonTimer = 15000+rand()%15000;
                    }
                    else SummonTimer -= diff;
                }

                if (CanPullBack)
                {
                    if (ShadowWhipTimer <= diff)
                    {
                        if (Player* temp = Unit::GetPlayer(*me, PlayerGUID))
                        {
                            //if unit dosen't have this flag, then no pulling back (script will attempt cast, even if orbital strike was resisted)
                            if (temp->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR))
                            {
                                me->InterruptNonMeleeSpells(false);
                                DoCast(temp, SPELL_SHADOW_WHIP);
                            }
                        }
                        PlayerGUID = 0;
                        ShadowWhipTimer = 2000;
                        CanPullBack = false;
                    }
                    else ShadowWhipTimer -= diff;
                }
                else
				{
                    if (OrbitalStrikeTimer <= diff)
                    {
                        Unit* temp = NULL;
                        if (me->IsWithinMeleeRange(me->GetVictim()))
                            temp = me->GetVictim();
                        else temp = SelectTarget(TARGET_RANDOM, 0);

                        if (temp && temp->GetTypeId() == TYPE_ID_PLAYER)
                        {
                            DoCast(temp, SPELL_ORBITAL_STRIKE);
                            OrbitalStrikeTimer = 14000+rand()%2000;
                            PlayerGUID = temp->GetGUID();

                            if (PlayerGUID)
                                CanPullBack = true;
                        }
                    }
                    else OrbitalStrikeTimer -= diff;
				}

                if (HealthBelowPct(20))
                {
                    if (DemonicShieldTimer <= diff)
                    {
                        DoCast(me, SPELL_DEMONIC_SHIELD);
                        DemonicShieldTimer = 15000;
                    }
                    else DemonicShieldTimer -= diff;
                }

                if (AuraTimer <= diff)
                {
                    DoSendQuantumText(SAY_CURSE, me);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoCast(target, DUNGEON_MODE(SPELL_TREACHEROUS_AURA_5N, SPELL_TREACHEROUS_AURA_5H));
                        AuraTimer = 8000+rand()%8000;
                    }
                }
                else AuraTimer -= diff;

                if (ShadowboltTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
						DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
                        ShadowboltTimer = 4000+rand()%2500;
                    }
                }
                else ShadowboltTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_omor_the_unscarredAI(creature);
        }
};

void AddSC_boss_omor_the_unscarred()
{
    new boss_omor_the_unscarred();
}