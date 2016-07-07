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
#include "mana_tombs.h"

enum PrinceTexts
{
    SAY_INTRO                    = -1557000,
    SAY_AGGRO_1                  = -1557001,
    SAY_AGGRO_2                  = -1557002,
    SAY_AGGRO_3                  = -1557003,
    SAY_SLAY_1                   = -1557004,
    SAY_SLAY_2                   = -1557005,
    SAY_SUMMON                   = -1557006,
    SAY_DEAD                     = -1557007,
};

enum Spells
{
    SPELL_BLINK                  = 34605,
    SPELL_FROSTBOLT              = 32364,
    SPELL_FIREBALL               = 32363,
    SPELL_FROSTNOVA              = 32365,
    SPELL_ETHEREAL_BEACON        = 32371,
    SPELL_ETHEREAL_BEACON_VISUAL = 32368,
	SPELL_ARCANE_BOLT            = 15254,
    SPELL_ETHEREAL_APPRENTICE    = 32372,
};

class boss_nexusprince_shaffar : public CreatureScript
{
public:
    boss_nexusprince_shaffar() : CreatureScript("boss_nexusprince_shaffar") { }

    struct boss_nexusprince_shaffarAI : public QuantumBasicAI
    {
        boss_nexusprince_shaffarAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
		{
			HasTaunted = false;
		}

        uint32 BlinkTimer;
        uint32 BeaconTimer;
        uint32 FireBallTimer;
        uint32 FrostboltTimer;
        uint32 FrostNovaTimer;

        SummonList summons;

        bool HasTaunted;
        bool CanBlink;

        void Reset()
        {
            BlinkTimer = 1500;
            BeaconTimer = 10000;
            FireBallTimer = 8000;
            FrostboltTimer = 4000;
            FrostNovaTimer = 15000;

            CanBlink = false;

            float dist = 8.0f;
            float posX, posY, posZ, angle;
            me->GetHomePosition(posX, posY, posZ, angle);

            me->SummonCreature(NPC_BEACON, posX - dist, posY - dist, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
            me->SummonCreature(NPC_BEACON, posX - dist, posY + dist, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
            me->SummonCreature(NPC_BEACON, posX + dist, posY, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
        }

        void EnterEvadeMode()
        {
            summons.DespawnAll();
            QuantumBasicAI::EnterEvadeMode();
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!HasTaunted && who->GetTypeId() == TYPE_ID_PLAYER && me->IsWithinDistInMap(who, 100.0f))
            {
                DoSendQuantumText(SAY_INTRO, me);
                HasTaunted = true;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
            DoZoneInCombat();
            summons.DoZoneInCombat();
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_BEACON)
            {
                summoned->CastSpell(summoned, SPELL_ETHEREAL_BEACON_VISUAL, false);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    summoned->AI()->AttackStart(target);
            }

            summons.Summon(summoned);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            summons.Despawn(summon);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEAD, me);
            summons.DespawnAll();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FrostNovaTimer <= diff)
            {
				if (me->IsNonMeleeSpellCasted(false))
					me->InterruptNonMeleeSpells(true);

                DoCastAOE(SPELL_FROSTNOVA);
                FrostNovaTimer  = urand(17500, 25000);
                CanBlink = true;
            }
			else FrostNovaTimer -= diff;

            if (FrostboltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = urand(4500, 6000);
				}
            }
			else FrostboltTimer -= diff;

            if (FireBallTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL);
					FireBallTimer = urand(4500, 6000);
				}
            }
			else FireBallTimer -= diff;

            if (CanBlink)
            {
                if (BlinkTimer <= diff)
                {
                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(true);

                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                        me->GetMotionMaster()->MovementExpired();

                    DoCast(me, SPELL_BLINK);
                    BlinkTimer = urand(1000, 2500);
                    CanBlink = false;
                }
				else BlinkTimer -= diff;
            }

            if (BeaconTimer <= diff)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(true);

                if (!urand(0, 3))
                    DoSendQuantumText(SAY_SUMMON, me);

                DoCast(me, SPELL_ETHEREAL_BEACON, true);
                BeaconTimer = 10000;
            }
			else BeaconTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_nexusprince_shaffarAI(creature);
	}
};

class npc_ethereal_beacon : public CreatureScript
{
public:
    npc_ethereal_beacon() : CreatureScript("npc_ethereal_beacon") { }

    struct npc_ethereal_beaconAI : public QuantumBasicAI
    {
        npc_ethereal_beaconAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 ApprenticeTimer;
        uint32 ArcaneBoltTimer;
        uint32 CheckTimer;

        void KillSelf()
        {
            me->Kill(me);
        }

        void Reset()
        {
            ApprenticeTimer = DUNGEON_MODE(20000, 10000);
            ArcaneBoltTimer = 1000;
            CheckTimer = 1000;
        }

        void EnterToBattle(Unit* who)
        {
            Creature* Shaffar = me->FindCreatureWithDistance(NPC_SHAFFAR, 100.0f);
            if (!Shaffar || Shaffar->IsDead())
            {
                KillSelf();
                return;
            }

            if (!Shaffar->IsInCombatActive())
                Shaffar->AI()->AttackStart(who);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me->GetVictim());
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CheckTimer <= diff)
            {
                Creature* Shaffar = me->FindCreatureWithDistance(NPC_SHAFFAR, 100);
                if (!Shaffar || Shaffar->IsDead() || !Shaffar->IsInCombatActive())
                {
                    KillSelf();
                    return;
                }
                CheckTimer = 1000;
            }
			else CheckTimer -= diff;

            if (ArcaneBoltTimer <= diff)
            {
                DoCastVictim(SPELL_ARCANE_BOLT);
                ArcaneBoltTimer = urand(2000, 4500);
            }
			else ArcaneBoltTimer -= diff;

            if (ApprenticeTimer <= diff)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(true);

                DoCast(me, SPELL_ETHEREAL_APPRENTICE, true);
                me->DespawnAfterAction();
                return;
            }
			else ApprenticeTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_beaconAI(creature);
    }
};

enum Ethereal
{
    SPELL_ETHEREAL_APPRENTICE_FIREBOLT  = 32369,
    SPELL_ETHEREAL_APPRENTICE_FROSTBOLT = 32370,
};

class npc_ethereal_apprentice : public CreatureScript
{
public:
    npc_ethereal_apprentice() : CreatureScript("npc_ethereal_apprentice") { }

    struct npc_ethereal_apprenticeAI : public QuantumBasicAI
    {
        npc_ethereal_apprenticeAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CastTimer;

        bool isFireboltTurn;

        void Reset()
        {
            CastTimer = 3000;
            isFireboltTurn = true;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CastTimer <= diff)
            {
                if (isFireboltTurn)
                {
                    DoCastVictim(SPELL_ETHEREAL_APPRENTICE_FIREBOLT, true);
                    isFireboltTurn = false;
                }
				else
				{
                    DoCastVictim(SPELL_ETHEREAL_APPRENTICE_FROSTBOLT, true);
                    isFireboltTurn = true;
                }
                CastTimer = 3000;
            }
			else CastTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_apprenticeAI(creature);
    }
};

void AddSC_boss_nexusprince_shaffar()
{
    new boss_nexusprince_shaffar();
    new npc_ethereal_beacon();
    new npc_ethereal_apprentice();
}