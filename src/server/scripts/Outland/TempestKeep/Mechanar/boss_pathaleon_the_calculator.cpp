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
    SAY_AGGRO                      = -1554020,
    SAY_DOMINATION_1               = -1554021,
    SAY_DOMINATION_2               = -1554022,
    SAY_SUMMON                     = -1554023,
    SAY_ENRAGE                     = -1554024,
    SAY_SLAY_1                     = -1554025,
    SAY_SLAY_2                     = -1554026,
    SAY_DEATH                      = -1554027,
};

enum Spells
{
    SPELL_MANA_TAP                 = 36021,
    SPELL_ARCANE_TORRENT           = 36022,
    SPELL_DOMINATION               = 35280,
    SPELL_ARCANE_EXPLOSION         = 15453,
    SPELL_FRENZY                   = 36992,
    SPELL_SUMMON_NETHER_WRAITH_1   = 35285,
    SPELL_SUMMON_NETHER_WRAITH_2   = 35286,
    SPELL_SUMMON_NETHER_WRAITH_3   = 35287,
    SPELL_SUMMON_NETHER_WRAITH_4   = 35288,
    SPELL_DETONATION               = 35058,
    SPELL_ARCANE_MISSILES          = 35034,
};

class boss_pathaleon_the_calculator : public CreatureScript
{
    public: boss_pathaleon_the_calculator() : CreatureScript("boss_pathaleon_the_calculator") { }
		
		struct boss_pathaleon_the_calculatorAI : public QuantumBasicAI
        {
            boss_pathaleon_the_calculatorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

            uint32 SummonTimer;
            uint32 ManaTapTimer;
            uint32 ArcaneTorrentTimer;
            uint32 DominationTimer;
            uint32 ArcaneExplosionTimer;
			uint32 Counter;

			SummonList Summons;

            bool Enraged;

            void Reset()
            {
                SummonTimer = 30000;
                ManaTapTimer = urand(12000, 20000);
                ArcaneTorrentTimer = urand(16000, 25000);
                DominationTimer = urand(25000, 40000);
                ArcaneExplosionTimer = urand(8000, 13000);

                Enraged = false;

                Counter = 0;

				Summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* who)
            {
				if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_AGGRO, me);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);

                Summons.DespawnAll();
            }

			void JustSummoned(Creature* summon)
			{
				if (summon->GetEntry() == NPC_NETHER_WRAITH)
				{
					summon->AI()->DoZoneInCombat();
					Summons.Summon(summon);
				}
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (SummonTimer <= diff)
                {
					DoSendQuantumText(SAY_SUMMON, me);
					DoCast(me, SPELL_SUMMON_NETHER_WRAITH_1, true);
					DoCast(me, SPELL_SUMMON_NETHER_WRAITH_2, true);
					DoCast(me, SPELL_SUMMON_NETHER_WRAITH_3, true);
					DoCast(me, SPELL_SUMMON_NETHER_WRAITH_4, true);
                    SummonTimer = urand(30000, 45000);
                }
                else SummonTimer -= diff;

                if (ManaTapTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (target && target->GetPowerType() == POWER_MANA)
						{
							DoCast(target, SPELL_MANA_TAP);
							ManaTapTimer = urand(14000, 22000);
						}
					}
                }
                else ManaTapTimer -= diff;

                if (ArcaneTorrentTimer <= diff)
                {
                    DoCastAOE(SPELL_ARCANE_TORRENT);
                    ArcaneTorrentTimer = urand(12000, 18000);
                }
                else ArcaneTorrentTimer -= diff;

                if (DominationTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                    {
                        DoSendQuantumText(RAND(SAY_DOMINATION_1, SAY_DOMINATION_2), me);
                        DoCast(target, SPELL_DOMINATION);
                    }
                    DominationTimer = urand(25000, 30000);
                }
                else DominationTimer -= diff;

				if (ArcaneExplosionTimer <= diff && IsHeroic())
				{
					DoCastAOE(SPELL_ARCANE_EXPLOSION);
					ArcaneExplosionTimer = urand(10000, 14000);
				}
				else ArcaneExplosionTimer -= diff;

                if (!Enraged && HealthBelowPct(30))
                {
                    DoCast(me, SPELL_FRENZY);
                    DoSendQuantumText(SAY_ENRAGE, me);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
                    Enraged = true;
                }

                DoMeleeAttackIfReady();
            }
        };

		CreatureAI* GetAI(Creature* creature) const
		{
			return new boss_pathaleon_the_calculatorAI(creature);
		}
};

class npc_nether_wraith : public CreatureScript
{
    public:
        npc_nether_wraith() : CreatureScript("npc_nether_wraith") { }

        struct npc_nether_wraithAI : public QuantumBasicAI
        {
            npc_nether_wraithAI(Creature* creature) : QuantumBasicAI(creature) { }

            uint32 ArcaneMissilesTimer;
            uint32 DetonationTimer;
            uint32 DieTimer;

            bool Detonation;

            void Reset()
            {
                ArcaneMissilesTimer = urand(1000, 4000);
                DetonationTimer = 20000;
                DieTimer = 2200;

                Detonation = false;
            }

            void EnterToBattle(Unit* /*who*/) {}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (ArcaneMissilesTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
					{
                        DoCast(target, SPELL_ARCANE_MISSILES);
						ArcaneMissilesTimer = urand(5000, 10000);
					}
                }
                else ArcaneMissilesTimer -=diff;

                if (!Detonation)
                {
                    if (DetonationTimer <= diff)
                    {
                        DoCast(me, SPELL_DETONATION);
                        Detonation = true;
                    }
                    else DetonationTimer -= diff;
                }

                if (Detonation)
                {
                    if (DieTimer <= diff)
                    {
                        me->setDeathState(JUST_DIED);
                        me->RemoveCorpse();
                    }
                    else DieTimer -= diff;
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_nether_wraithAI(creature);
        }
};

void AddSC_boss_pathaleon_the_calculator()
{
    new boss_pathaleon_the_calculator();
    new npc_nether_wraith();
}