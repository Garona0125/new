/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "karazhan.h"

enum Texts
{
	SAY_SLAY                    = 1,
	SAY_DEATH                   = 2,
	SAY_AGGRO                   = 3,
	SAY_SACRIFICE               = 4,
	SAY_SUMMON                  = 5,
};

enum Spells
{
	SPELL_SUMMON_DEMONCHAINS   = 30120,
	SPELL_DEMON_CHAINS         = 30206,
	SPELL_ENRAGE               = 23537,
	SPELL_SHADOW_BOLT          = 30055,
	SPELL_SACRIFICE            = 30115,
	SPELL_BERSERK              = 32965,
	SPELL_SUMMON_FIENDISIMP    = 30184,
	SPELL_SUMMON_IMP           = 30066,
	SPELL_FIENDISH_PORTAL      = 30171,
	SPELL_FIENDISH_PORTAL_1    = 30179,
	SPELL_FIREBOLT             = 30050,
	SPELL_BROKEN_PACT          = 30065,
	SPELL_AMPLIFY_FLAMES       = 30053
};

class mob_kilrek : public CreatureScript
{
public:
    mob_kilrek() : CreatureScript("mob_kilrek") { }

    struct mob_kilrekAI : public QuantumBasicAI
    {
        mob_kilrekAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint64 TerestianGUID;
        uint32 AmplifyTimer;

        void Reset()
        {
            TerestianGUID = 0;
            AmplifyTimer = 2000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!instance)
                return;
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
            {
                uint64 TerestianGUID = instance->GetData64(DATA_TERESTIAN);
                if (TerestianGUID)
                {
                    Unit* Terestian = Unit::GetUnit(*me, TerestianGUID);
                    if (Terestian && Terestian->IsAlive())
                        DoCast(Terestian, SPELL_BROKEN_PACT, true);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (AmplifyTimer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCastVictim(SPELL_AMPLIFY_FLAMES);

                AmplifyTimer = urand(10000, 20000);
            }
			else AmplifyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_kilrekAI (creature);
    }
};

class mob_demon_chain : public CreatureScript
{
public:
    mob_demon_chain() : CreatureScript("mob_demon_chain") { }

    struct mob_demon_chainAI : public QuantumBasicAI
    {
        mob_demon_chainAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 SacrificeGUID;

        void Reset()
        {
            SacrificeGUID = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void JustDied(Unit* /*killer*/)
        {
            if (SacrificeGUID)
            {
                Unit* Sacrifice = Unit::GetUnit(*me, SacrificeGUID);
                if (Sacrifice)
                    Sacrifice->RemoveAurasDueToSpell(SPELL_SACRIFICE);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_demon_chainAI(creature);
    }
};

class mob_fiendish_portal : public CreatureScript
{
public:
    mob_fiendish_portal() : CreatureScript("mob_fiendish_portal") { }

    struct mob_fiendish_portalAI : public PassiveAI
    {
        mob_fiendish_portalAI(Creature* creature) : PassiveAI(creature), summons(me){}

        SummonList summons;

        void Reset()
        {
            DespawnAllImp();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            DoZoneInCombat(summon);
        }

        void DespawnAllImp()
        {
            summons.DespawnAll();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_fiendish_portalAI (creature);
    }
};

class mob_fiendish_imp : public CreatureScript
{
public:
    mob_fiendish_imp() : CreatureScript("mob_fiendish_imp") { }

    struct mob_fiendish_impAI : public QuantumBasicAI
    {
        mob_fiendish_impAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FireboltTimer;

        void Reset()
        {
            FireboltTimer = 2000;

            me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (FireboltTimer <= diff)
            {
                DoCastVictim(SPELL_FIREBOLT);
                FireboltTimer = 2200;
            }
			else FireboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_fiendish_impAI (creature);
    }
};

class boss_terestian_illhoof : public CreatureScript
{
public:
    boss_terestian_illhoof() : CreatureScript("boss_terestian_illhoof") { }

    struct boss_terestianAI : public QuantumBasicAI
    {
        boss_terestianAI(Creature* creature) : QuantumBasicAI(creature)
        {
            for (uint8 i = 0; i < 2; ++i)
                PortalGUID[i] = 0;
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint64 PortalGUID[2];
        uint8 PortalsCount;

        uint32 SacrificeTimer;
        uint32 ShadowboltTimer;
        uint32 SummonTimer;
        uint32 BerserkTimer;

        bool SummonedPortals;
        bool Berserk;

        void Reset()
        {
            for (uint8 i = 0; i < 2; ++i)
            {
                if (PortalGUID[i])
                {
                    if (Creature* pPortal = Unit::GetCreature(*me, PortalGUID[i]))
                    {
                        CAST_AI(mob_fiendish_portal::mob_fiendish_portalAI, pPortal->AI())->DespawnAllImp();
                        pPortal->DespawnAfterAction();
                    }

                    PortalGUID[i] = 0;
                }
            }

            PortalsCount = 0;
            SacrificeTimer = 30000;
            ShadowboltTimer =  5000;
            SummonTimer = 10000;
            BerserkTimer = 600000;

            SummonedPortals = false;
            Berserk = false;

            if (instance)
                instance->SetData(TYPE_TERESTIAN, NOT_STARTED);

            me->RemoveAurasDueToSpell(SPELL_BROKEN_PACT);

            if (Minion* Kilrek = me->GetFirstMinion())
            {
                if (!Kilrek->IsAlive())
                {
                    Kilrek->UnSummon();
                    DoCast(me, SPELL_SUMMON_IMP, true);
                }
            }
            else DoCast(me, SPELL_SUMMON_IMP, true);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_AGGRO);
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_PORTAL)
            {
                PortalGUID[PortalsCount] = summoned->GetGUID();
                ++PortalsCount;

                if (summoned->GetUInt32Value(UNIT_CREATED_BY_SPELL) == SPELL_FIENDISH_PORTAL_1)
                {
					Talk(SAY_SUMMON);
                    SummonedPortals = true;
                }
            }
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/)
        {
            for (uint8 i = 0; i < 2; ++i)
            {
                if (PortalGUID[i])
                {
                    if (Creature* pPortal = Unit::GetCreature(*me, PortalGUID[i]))
                        pPortal->DespawnAfterAction();

                    PortalGUID[i] = 0;
                }
            }

            Talk(SAY_DEATH);

            if (instance)
                instance->SetData(TYPE_TERESTIAN, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SacrificeTimer <= diff)
            {
                Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true);
                if (target && target->IsAlive())
                {
                    DoCast(target, SPELL_SACRIFICE, true);
                    DoCast(target, SPELL_SUMMON_DEMONCHAINS, true);

                    if (Creature* chains = me->FindCreatureWithDistance(NPC_DEMON_CHAINS, 500.0f))
                    {
                        CAST_AI(mob_demon_chain::mob_demon_chainAI, chains->AI())->SacrificeGUID = target->GetGUID();
                        chains->CastSpell(chains, SPELL_DEMON_CHAINS, true);
						Talk(SAY_SACRIFICE);
                        SacrificeTimer = 30000;
                    }
                }
            }
			else SacrificeTimer -= diff;

            if (ShadowboltTimer <= diff)
            {
                DoCast(SelectTarget(TARGET_TOP_AGGRO, 0), SPELL_SHADOW_BOLT);
                ShadowboltTimer = 10000;
            }
			else ShadowboltTimer -= diff;

            if (SummonTimer <= diff)
            {
                if (!PortalGUID[0])
                    DoCastVictim(SPELL_FIENDISH_PORTAL, false);

                if (!PortalGUID[1])
                    DoCastVictim(SPELL_FIENDISH_PORTAL_1, false);

                if (PortalGUID[0] && PortalGUID[1])
                {
                    if (Creature* portal = Unit::GetCreature(*me, PortalGUID[urand(0, 1)]))
                        portal->CastSpell(me->GetVictim(), SPELL_SUMMON_FIENDISIMP, false);
                    SummonTimer = 5000;
                }
            }
			else SummonTimer -= diff;

            if (!Berserk)
            {
                if (BerserkTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK);
                    Berserk = true;
                }
				else BerserkTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_terestianAI(creature);
    }
};

void AddSC_boss_terestian_illhoof()
{
    new boss_terestian_illhoof();
    new mob_fiendish_imp();
    new mob_fiendish_portal();
    new mob_kilrek();
    new mob_demon_chain();
}