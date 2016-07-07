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
#include "sunwell_plateau.h"

enum Spells
{
    SPELL_ENRAGE                = 26662,
    SPELL_NEGATIVE_ENERGY       = 46009,
    SPELL_DARKNESS              = 45999,
    SPELL_OPEN_ALL_PORTALS      = 46177,
	SPELL_OPEN_PORTAL_TARGET    = 45976,
    SPELL_OPEN_PORTAL_VISUAL    = 45977,
    SPELL_SUMMON_BERSERKER      = 46037,
    SPELL_SUMNON_FURY_MAGE      = 46038,
    SPELL_SUMMON_VOID_SENTINEL  = 45988,
    SPELL_SUMMON_ENTROPIUS      = 46217,
	SPELL_VOID_ZONE_PERIODIC    = 46262,
    SPELL_DARKNESS_ENTROPIUS    = 46269,
    SPELL_SUMMON_BLACK_HOLE     = 46282,
    SPELL_NEGATIVE_ENERGY_E     = 46284,
    SPELL_ENTROPIUS_SPAWN       = 46223,
    SPELL_DARKFIEND_AOE         = 45944,
    SPELL_DARKFIEND_VISUAL      = 45936,
    SPELL_DARKFIEND_SKIN        = 45934,
    SPELL_BLACK_HOLE_VISUAL     = 46242,
    SPELL_BLACK_HOLE_GROW       = 46228,
	SPELL_DARKNESS_PROC         = 45996,
	SPELL_ENTROPIUS_BODY        = 46819,
};

enum BossTimers
{
    TIMER_HUMANOIDES            = 0,
    TIMER_PHASE                 = 1,
    TIMER_SENTINEL              = 2,
    TIMER_DARKNESS              = 3,
};

float DarkFiends[8][4] =
{
    {1819.9f, 609.80f, 69.74f, 1.94f},
    {1829.39f, 617.89f, 69.73f, 2.61f},
    {1801.98f, 633.62f, 69.74f, 5.71f},
    {1830.88f, 629.99f, 69.73f, 3.52f},
    {1800.38f, 621.41f, 69.74f, 0.22f},
    {1808.3f, 612.45f, 69.73f, 1.02f},
    {1823.9f, 639.69f, 69.74f, 4.12f},
    {1811.85f, 640.46f, 69.73f, 4.97f},
};

float Humanoides[6][5] =
{
    {NPC_FURY_MAGE, 1780.16f, 666.83f, 71.19f, 5.21f},
    {NPC_FURY_MAGE, 1847.93f, 600.30f, 71.30f, 2.57f},
    {NPC_BERSERKER, 1779.97f, 660.64f, 71.19f, 5.28f},
    {NPC_BERSERKER, 1786.2f, 661.01f, 71.19f, 4.51f},
    {NPC_BERSERKER, 1845.17f, 602.63f, 71.28f, 2.43f},
    {NPC_BERSERKER, 1842.91f, 599.93f, 71.23f, 2.44f},
};

uint32 EnrageTimer = 600000;

class boss_muru : public CreatureScript
{
public:
    boss_muru() : CreatureScript("boss_muru") { }

    struct boss_muruAI : public QuantumBasicAI
    {
        boss_muruAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint8 Phase;
        uint32 Timer[4];

        bool DarkFiend;

        void Reset()
        {
            DarkFiend = false;
            Phase = 1;

            EnrageTimer = 600000;
            Timer[TIMER_DARKNESS] = 45000;
            Timer[TIMER_HUMANOIDES] = 10000;
            Timer[TIMER_PHASE] = 2000;
            Timer[TIMER_SENTINEL] = 31500;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisible(true);

            Summons.DespawnAll();

			instance->SetData(DATA_MURU_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			DoCastAOE(SPELL_NEGATIVE_ENERGY, true);
			DoCastAOE(SPELL_DARKNESS, true);

			instance->SetData(DATA_MURU_EVENT, IN_PROGRESS);
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DARKNESS_PROC)
            {
                for (uint8 i = 0; i < 8; ++i)
                    me->SummonCreature(NPC_DARK_FIEND, DarkFiends[i][0], DarkFiends[i][1], DarkFiends[i][2], DarkFiends[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (damage >= me->GetHealth() && Phase == 1)
            {
                damage = 0;
                Phase = 2;
                me->RemoveAllAuras();
                DoCast(me, SPELL_OPEN_ALL_PORTALS, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
            if (Phase > 1 && Phase < 4)
                damage = 0;
        }

        void JustSummoned(Creature* summoned)
        {
            switch (summoned->GetEntry())
            {
                case NPC_ENTROPIUS:
                    me->SetVisible(false);
                    break;
                case NPC_DARK_FIEND:
                    summoned->CastSpell(summoned, SPELL_DARKFIEND_VISUAL, true);
                    break;
				case NPC_VOID_SPAWN:
					break;
            }
            summoned->AI()->AttackStart(SelectTarget(TARGET_RANDOM, 0, 85, true));
            Summons.Summon(summoned);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Phase == 3)
            {
                if (Timer[TIMER_PHASE] <= diff)
                {
                    if (!instance)
                        return;

                    switch (instance->GetData(DATA_MURU_EVENT))
                    {
                        case NOT_STARTED:
                            Reset();
                            break;
                        case DONE:
                            Phase = 4;
                            me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                            me->RemoveCorpse();
                            break;
                    }
                    Timer[TIMER_PHASE] = 3000;
                }
				else Timer[TIMER_PHASE] -= diff;
                return;
            }

            if (EnrageTimer < diff && !me->HasAura(SPELL_ENRAGE, 0))
            {
                DoCast(me, SPELL_ENRAGE, true);
            }
			else EnrageTimer -= diff;

            for (uint8 i = 0; i < 4; ++i)
            {
                if (Timer[i] <= diff)
                {
                    switch (i)
                    {
                        case TIMER_HUMANOIDES:
                            for (uint8 i = 0; i < 6; ++i)
                                me->SummonCreature(uint32(Humanoides[i][0]),Humanoides[i][1],Humanoides[i][2],Humanoides[i][3], Humanoides[i][4], TEMPSUMMON_CORPSE_DESPAWN, 0);
                            Timer[TIMER_HUMANOIDES] = 60000;
                            break;
                        case TIMER_PHASE:
                            me->RemoveAllAuras();
                            DoCast(me, SPELL_SUMMON_ENTROPIUS, true);
                            Timer[TIMER_PHASE] = 3000;
                            Phase = 3;
                            return;
                        case TIMER_SENTINEL:
							DoCastAOE(SPELL_DARKNESS, true);
                            DoCastAOE(SPELL_OPEN_PORTAL_TARGET, true);
                            Timer[TIMER_SENTINEL] = 30000;
                            break;
                    }
                    break;
                }
            }

            //Timer
            for (uint8 i = 0; i < 4; ++i)
            {
                if (i != TIMER_PHASE)
					Timer[i] -= diff;
                else if (Phase == 2)
					Timer[i] -= diff;
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_muruAI(creature);
    }
};

class boss_entropius : public CreatureScript
{
public:
    boss_entropius() : CreatureScript("boss_entropius") { }

    struct boss_entropiusAI : public QuantumBasicAI
    {
        boss_entropiusAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint32 BlackHoleSummonTimer;

        void Reset()
        {
            BlackHoleSummonTimer = 15000;
            DoCastAOE(SPELL_NEGATIVE_ENERGY_E, true);
			DoCast(me, SPELL_ENTROPIUS_SPAWN, true);

            Summons.DespawnAll();

			instance->SetData(DATA_MURU_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCastAOE(SPELL_NEGATIVE_ENERGY_E, true);
            DoCast(me, SPELL_ENTROPIUS_SPAWN, true);

			instance->SetData(DATA_MURU_EVENT, IN_PROGRESS);
        }

        void JustSummoned(Creature* summoned)
        {
            switch (summoned->GetEntry())
            {
                case NPC_DARK_FIEND:
                    summoned->CastSpell(summoned, SPELL_DARKFIEND_VISUAL, true);
                    break;
                case NPC_DARKNESS:
                    summoned->AddUnitState(UNIT_STATE_STUNNED);
                    float x, y, z, o;
                    summoned->GetHomePosition(x, y, z, o);
                    me->SummonCreature(NPC_DARK_FIEND, x, y, z, o, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    break;
            }
            summoned->AI()->AttackStart(SelectTarget(TARGET_RANDOM,0, 50, true));
            Summons.Summon(summoned);
        }

        void JustDied(Unit* /*killer*/)
        {
            Summons.DespawnAll();

			DoCast(me, SPELL_ENTROPIUS_BODY, true);
			DoCast(me, SPELL_ENTROPIUS_SPAWN, true);

			instance->SetData(DATA_MURU_EVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (EnrageTimer < diff && !me->HasAura(SPELL_ENRAGE, 0))
            {
                DoCast(me, SPELL_ENRAGE, true);
            }
			else EnrageTimer -= diff;

            if (BlackHoleSummonTimer <= diff)
            {
                Unit* random = SelectTarget(TARGET_RANDOM, 0, 100, true);
                if (!random)
                    return;

                DoCast(random, SPELL_DARKNESS_ENTROPIUS, true);

                random = SelectTarget(TARGET_RANDOM, 0, 100, true);
				if (!random)
					return;

                random->CastSpell(random, SPELL_SUMMON_BLACK_HOLE, true);
                BlackHoleSummonTimer = 15000;
            }
			else BlackHoleSummonTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_entropiusAI(creature);
    }
};

class npc_muru_portal : public CreatureScript
{
public:
    npc_muru_portal() : CreatureScript("npc_muru_portal") { }

    struct npc_muru_portalAI : public QuantumBasicAI
    {
        npc_muru_portalAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			instance = creature->GetInstanceScript();
			SetCombatMovement(false);
			creature->SetCanFly(true);
        }

        InstanceScript* instance;

        SummonList Summons;

        bool SummonSentinel;
        bool InAction;

        uint32 SummonTimer;

        void Reset()
        {
            SummonTimer = 5000;

            InAction = false;
            SummonSentinel = false;

            Summons.DespawnAll();
        }

		void EnterToBattle(Unit* /*who*/){}

        void JustSummoned(Creature* summoned)
        {
			if (Player* target = Unit::GetPlayer(*me, instance->GetData64(DATA_PLAYER_GUID)))
				summoned->AI()->AttackStart(target);

			Summons.Summon(summoned);
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
			float x, y, z, o;
            me->GetHomePosition(x, y, z, o);
            DoTeleportTo(x, y, z);

            InAction = true;

            switch (spell->Id)
            {
                case SPELL_OPEN_ALL_PORTALS:
                    DoCastAOE(SPELL_OPEN_PORTAL_TARGET, true);
                    break;
                case SPELL_OPEN_PORTAL_TARGET:
                    DoCastAOE(SPELL_OPEN_PORTAL_VISUAL, true);
                    SummonSentinel = true;
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!SummonSentinel)
            {
                if (InAction && instance && instance->GetData(DATA_MURU_EVENT) == NOT_STARTED)
                    Reset();
                return;
            }

            if (SummonTimer <= diff)
            {
                DoCastAOE(SPELL_SUMMON_VOID_SENTINEL, true);
                SummonTimer = 5000;
                SummonSentinel = false;
            }
			else SummonTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_muru_portalAI(creature);
    }
};

class npc_dark_fiend : public CreatureScript
{
public:
    npc_dark_fiend() : CreatureScript("npc_dark_fiend") { }

    struct npc_dark_fiendAI : public QuantumBasicAI
    {
        npc_dark_fiendAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WaitTimer;
        bool InAction;

        void Reset()
        {
            WaitTimer = 2000;
            InAction = false;

            me->AddUnitState(UNIT_STATE_STUNNED);
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, const SpellInfo* Spell)
        {
            for (uint8 i = 0; i < 3; ++i)
			{
                if (Spell->Effects[i].Effect == 38)
                {
                    me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    me->RemoveCorpse();
                }
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WaitTimer <= diff)
            {
                if (!InAction)
                {
                    me->ClearUnitState(UNIT_STATE_STUNNED);
                    DoCastAOE(SPELL_DARKFIEND_SKIN, true);
                    AttackStart(SelectTarget(TARGET_RANDOM, 0, 100, true));
                    InAction = true;
                    WaitTimer = 500;
                }
                else
                {
                    if (me->IsWithinDist(me->GetVictim(), 5.0f))
                    {
                        DoCastAOE(SPELL_DARKFIEND_AOE, true);
						me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
						me->RemoveCorpse();
                    }
                    WaitTimer = 500;
                }
            }
			else WaitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_fiendAI(creature);
    }
};

class npc_singularity : public CreatureScript
{
public:
    npc_singularity() : CreatureScript("npc_singularity") { }

    struct npc_singularityAI : public QuantumBasicAI
    {
        npc_singularityAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
			SetCombatMovement(false);
        }

		InstanceScript* instance;

        uint32 DespawnTimer;
        uint32 SpellTimer;
        uint8 Phase;
        uint8 NeedForAHack;

        void Reset()
        {
            DespawnTimer = 15000;
            SpellTimer = 5000;
            Phase = 0;

            DoCast(me, SPELL_BLACK_HOLE_VISUAL, true);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (SpellTimer <= diff)
            {
                Unit* Victim = Unit::GetUnit(*me, instance ? instance->GetData64(DATA_PLAYER_GUID) : 0);
                switch (NeedForAHack)
                {
                    case 0:
                        DoCastAOE(SPELL_BLACK_HOLE_GROW, true);
                        if (Victim)
                            AttackStart(Victim);
                        SpellTimer = 700;
                        NeedForAHack = 2;
                        break;
                    case 1:
                        me->AddAura(SPELL_BLACK_HOLE_GROW, me);
                        NeedForAHack = 2;
                        SpellTimer = 600;
                        break;
                    case 2:
                        SpellTimer = 400;
                        NeedForAHack = 3;
                        me->RemoveAura(SPELL_BLACK_HOLE_GROW, 1);
                        break;
                    case 3:
                        SpellTimer = urand(400,900);
                        NeedForAHack = 1;
                        if (Unit* Temp = me->GetVictim())
                        {
                            if (Temp->GetPositionZ() > 73 && Victim)
                                AttackStart(Victim);
                        }
						else
							return;
                }
            }
			else SpellTimer -= diff;

            if (DespawnTimer <= diff)
			{
				me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				me->RemoveCorpse();
			}
			else DespawnTimer -= diff;
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_singularityAI(creature);
    }
};

class npc_darkness : public CreatureScript
{
public:
    npc_darkness() : CreatureScript("npc_darkness") { }

    struct npc_darknessAI : public QuantumBasicAI
    {
        npc_darknessAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
        }

        uint32 DespawnTimer;

        void Reset()
        {
            DespawnTimer = 15000;

			DoCast(me, SPELL_BLACK_HOLE_VISUAL, true);
            DoCast(me, SPELL_VOID_ZONE_PERIODIC, true);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer <= diff)
			{
				me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				me->RemoveCorpse();
			}
			else DespawnTimer -= diff;
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darknessAI(creature);
    }
};

void AddSC_boss_muru()
{
    new boss_muru();
    new boss_entropius();
    new npc_muru_portal();
    new npc_dark_fiend();
    new npc_singularity();
	new npc_darkness();
}