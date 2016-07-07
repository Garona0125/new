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
#include "WorldPacket.h"
#include "magisters_terrace.h"

#define LOCATION_Z -16.727455f

float KaelLocations[3][2]=
{
    {148.744659f, 181.377426f},
    {140.823883f, 195.403046f},
    {156.574188f, 195.650482f},
};

enum Texts
{
	SAY_AGGRO                   = 0,
	SAY_PHOENIX                 = 1,
	SAY_FLAMESTRIKE             = 2,
	SAY_GRAVITY_LAPSE           = 3,
	SAY_TIRED                   = 4,
	SAY_RECAST_GRAVITY          = 5,
	SAY_DEATH                   = 6,
};

enum Spells
{
	// Phase 1
	SPELL_FIREBALL_5N            = 44189,
	SPELL_FIREBALL_5H            = 46164,
	SPELL_PHOENIX                = 44194,
	SPELL_PHOENIX_BURN           = 44197,
	SPELL_REBIRTH_DMG            = 44196,
	SPELL_FLAMESTRIKE_1_5N       = 44190,
	SPELL_FLAMESTRIKE_1_5H       = 46163,
	SPELL_FLAMESTRIKE2           = 44191,
	SPELL_FLAMESTRIKE3           = 44192,
	SPELL_SHOCK_BARRIER          = 46165,
	SPELL_PYROBLAST              = 36819,
	// Phase 2
	SPELL_GRAVITY_LAPSE_INITIAL  = 44224,
	SPELL_GRAVITY_LAPSE_CHANNEL  = 44251,
	SPELL_TELEPORT_CENTER        = 44218,
	SPELL_GRAVITY_LAPSE_FLY      = 44227,
	SPELL_GRAVITY_LAPSE_DOT      = 44226,
	SPELL_ARCANE_SPHERE_PASSIVE  = 44263,
	SPELL_POWER_FEEDBACK         = 44233,
};

class boss_felblood_kaelthas : public CreatureScript
{
public:
    boss_felblood_kaelthas() : CreatureScript("boss_felblood_kaelthas") { }

    struct boss_felblood_kaelthasAI : public QuantumBasicAI
    {
        boss_felblood_kaelthasAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint32 FireballTimer;
        uint32 PhoenixTimer;
        uint32 FlameStrikeTimer;
        uint32 CombatPulseTimer;
        uint32 PyroblastTimer;
        uint32 GravityLapseTimer;
        uint32 GravityLapsePhase;

        bool FirstGravityLapse;
        bool HasTaunted;

        uint8 Phase;

        void Reset()
        {
            FireballTimer = 1000;
            PhoenixTimer = 10000;
            FlameStrikeTimer = 25000;
            CombatPulseTimer = 0;
            PyroblastTimer = 60000;
            GravityLapseTimer = 0;
            GravityLapsePhase = 0;

            FirstGravityLapse = true;
            HasTaunted = false;

            Summons.DespawnAll();

            Phase = 0;

            if (instance)
            {
                instance->SetData(DATA_KAELTHAS_EVENT, NOT_STARTED);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);
            Summons.DespawnAll();

            if (!instance)
                return;

            instance->SetData(DATA_KAELTHAS_EVENT, DONE);

			if (GameObject* escapeOrb = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_ESCAPE_ORB)))
				escapeOrb->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }

        void JustSummoned(Creature *summon)
        {
            Summons.Summon(summon);
        }

        void DamageTaken(Unit* /*doneby*/, uint32 &damage)
        {
            if (damage > me->GetHealth())
				RemoveGravityLapse();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!instance)
                return;

			instance->SetData(DATA_KAELTHAS_EVENT, IN_PROGRESS);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!HasTaunted && me->IsWithinDistInMap(who, 40.0f))
            {
                Talk(SAY_AGGRO);
                HasTaunted = true;
            }

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void SetThreatList(Creature* SummonedUnit)
        {
            if (!SummonedUnit)
                return;

            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
            {
                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                if (unit && unit->IsAlive())
                {
                    float threat = me->getThreatManager().getThreat(unit);
                    SummonedUnit->AddThreat(unit, threat);
                }
            }
        }

        void TeleportPlayersToSelf()
        {
            float x = KaelLocations[0][0];
            float y = KaelLocations[0][1];
            me->SetPosition(x, y, LOCATION_Z, 0.0f);
            //me->SendMonsterMove(x, y, LOCATION_Z, 0, 0, 0);
            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
            {
                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                if (unit && (unit->GetTypeId() == TYPE_ID_PLAYER))
                    unit->CastSpell(unit, SPELL_TELEPORT_CENTER, true);
            }
            DoCast(me, SPELL_TELEPORT_CENTER, true);
        }

        void CastGravityLapseKnockUp()
        {
            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
            {
                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                if (unit && (unit->GetTypeId() == TYPE_ID_PLAYER))
                    unit->CastSpell(unit, SPELL_GRAVITY_LAPSE_DOT, true, 0, 0, me->GetGUID());
            }
        }

        void CastGravityLapseFly()
        {
            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
            {
                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                if (unit && (unit->GetTypeId() == TYPE_ID_PLAYER))
                {
                    unit->CastSpell(unit, SPELL_GRAVITY_LAPSE_FLY, true, 0, 0, me->GetGUID());
					WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                    data.append(unit->GetPackGUID());
                    data << uint32(0);
                    unit->SendMessageToSet(&data, true);
                }
            }
        }

        void RemoveGravityLapse()
        {
            std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
            {
                Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                if (unit && (unit->GetTypeId() == TYPE_ID_PLAYER))
                {
                    unit->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
                    unit->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT);

					WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                    data.append(unit->GetPackGUID());
                    data << uint32(0);
                    unit->SendMessageToSet(&data, true);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            switch (Phase)
            {
                case 0:
                {
                    if (IsHeroic())
                    {
                        if (PyroblastTimer <= diff)
                        {
                            me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            DoCast(me, SPELL_SHOCK_BARRIER, true);
                            DoCastVictim(SPELL_PYROBLAST);
                            PyroblastTimer = 60000;
                        } else PyroblastTimer -= diff;
                    }

                    if (FireballTimer <= diff)
                    {
						if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
						{
							DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
							FireballTimer = urand(2000, 6000);
						}
                    }
					else FireballTimer -= diff;

                    if (PhoenixTimer <= diff)
                    {
                        Unit* target = SelectTarget(TARGET_RANDOM, 1);

                        uint8 random = urand(1, 2);
                        float x = KaelLocations[random][0];
                        float y = KaelLocations[random][1];

                        Creature* Phoenix = me->SummonCreature(NPC_PHOENIX, x, y, LOCATION_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
                        if (Phoenix)
                        {
                            Phoenix->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
                            SetThreatList(Phoenix);
                            if (target)
                                Phoenix->AI()->AttackStart(target);
                            else
                                Phoenix->AI()->AttackStart(me->GetVictim());
                        }

                        Talk(SAY_PHOENIX);

                        PhoenixTimer = 60000;
                    }
					else PhoenixTimer -= diff;

                    if (FlameStrikeTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        {
                            me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            DoCast(target, SPELL_FLAMESTRIKE3, true);
                            Talk(SAY_FLAMESTRIKE);
                        }
                        FlameStrikeTimer = urand(15000, 25000);
                    }
					else FlameStrikeTimer -= diff;

                    // Below 50%
                    if (HealthBelowPct(50))
                    {
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        GravityLapseTimer = 0;
                        GravityLapsePhase = 0;
                        Phase = 1;
                    }

                    DoMeleeAttackIfReady();
                }
                break;
				case 1:
                {
                    if (GravityLapseTimer <= diff)
                    {
                        switch (GravityLapsePhase)
                        {
                            case 0:
                                if (FirstGravityLapse)
                                {
									Talk(SAY_GRAVITY_LAPSE);
                                    FirstGravityLapse = false;

                                    if (instance)
                                        instance->SetData(DATA_KAELTHAS_STATUES, 1);
                                }
                                else
									Talk(SAY_RECAST_GRAVITY);

                                DoCast(me, SPELL_GRAVITY_LAPSE_INITIAL);
                                GravityLapseTimer = 2000 + diff;
                                GravityLapsePhase = 1;
                                break;
                            case 1:
                                TeleportPlayersToSelf();
                                GravityLapseTimer = 1000;
                                GravityLapsePhase = 2;
                                break;
                            case 2:
                                CastGravityLapseKnockUp();
                                GravityLapseTimer = 1000;
                                GravityLapsePhase = 3;
                                break;
                            case 3:
                                CastGravityLapseFly();
                                GravityLapseTimer = 30000;
                                GravityLapsePhase = 4;
                                for (uint8 i = 0; i < 3; ++i)
                                {
                                    Unit* target = NULL;
                                    target = SelectTarget(TARGET_RANDOM, 0);

                                    Creature* Orb = DoSpawnCreature(NPC_ARCANE_SPHERE, 5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                                    if (Orb && target)
                                    {
                                        Orb->SetSpeed(MOVE_RUN, 0.5f);
                                        Orb->AddThreat(target, 1000000.0f);
                                        Orb->AI()->AttackStart(target);
                                    }
                                }

                                DoCast(me, SPELL_GRAVITY_LAPSE_CHANNEL);
                                break;

                            case 4:
                                me->InterruptNonMeleeSpells(false);
								Talk(SAY_TIRED);
                                DoCast(me, SPELL_POWER_FEEDBACK);
                                RemoveGravityLapse();
                                GravityLapseTimer = 10000;
                                GravityLapsePhase = 0;
                                break;
                        }
                    }
					else GravityLapseTimer -= diff;
                }
                break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_felblood_kaelthasAI(creature);
    }
};

class npc_felkael_flamestrike : public CreatureScript
{
public:
    npc_felkael_flamestrike() : CreatureScript("npc_felkael_flamestrike") { }

    struct npc_felkael_flamestrikeAI : public QuantumBasicAI
    {
        npc_felkael_flamestrikeAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 FlameStrikeTimer;

        void Reset()
        {
            FlameStrikeTimer = 5000;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetCurrentFaction(16);

            DoCast(me, SPELL_FLAMESTRIKE2, true);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (FlameStrikeTimer <= diff)
            {
                DoCast(me, DUNGEON_MODE(SPELL_FLAMESTRIKE_1_5N, SPELL_FLAMESTRIKE_1_5H));
                me->Kill(me);
            }
			else FlameStrikeTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felkael_flamestrikeAI(creature);
    }
};

class npc_felkael_phoenix : public CreatureScript
{
public:
    npc_felkael_phoenix() : CreatureScript("npc_felkael_phoenix") { }

    struct npc_felkael_phoenixAI : public QuantumBasicAI
    {
        npc_felkael_phoenixAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 BurnTimer;
        uint32 DeathTimer;
        bool Rebirth;
        bool FakeDeath;

        void Reset()
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
            me->SetDisableGravity(true);
            DoCast(me, SPELL_PHOENIX_BURN, true);
            BurnTimer = 2000;
            DeathTimer = 3000;
            Rebirth = false;
            FakeDeath = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void DamageTaken(Unit* /*killer*/, uint32 &damage)
        {
            if (damage < me->GetHealth())
                return;

            if (FakeDeath)
            {
                damage = 0;
                return;
            }

            if (instance && instance->GetData(DATA_KAELTHAS_EVENT) == 0)
            {
                damage = 0;
                FakeDeath = true;

                me->InterruptNonMeleeSpells(false);
                me->SetHealth(0);
                me->StopMoving();
                me->ClearComboPointHolders();
                me->RemoveAllAurasOnDeath();
                me->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
                me->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->ClearAllReactives();
                me->SetTarget(0);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
                me->SetStandState(UNIT_STAND_STATE_DEAD);
           }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->SummonCreature(NPC_PHOENIX_EGG, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (FakeDeath)
            {
                if (!Rebirth)
                {
                    DoCast(me, SPELL_REBIRTH_DMG);
                    Rebirth = true;
                }

                if (Rebirth)
                {

                    if (DeathTimer <= diff)
                    {
                        me->SummonCreature(NPC_PHOENIX_EGG, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
                        me->DisappearAndDie();
                        Rebirth = false;
                    } else DeathTimer -= diff;
                }
            }

            if (!UpdateVictim())
                return;

            if (BurnTimer <= diff)
            {
                uint16 dmg = urand(1650, 2050);
                me->DealDamage(me, dmg, 0, DOT, SPELL_SCHOOL_MASK_FIRE, NULL, false);
                BurnTimer += 2000;
            }
			BurnTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felkael_phoenixAI(creature);
    }
};

class npc_felkael_phoenix_egg : public CreatureScript
{
public:
    npc_felkael_phoenix_egg() : CreatureScript("npc_felkael_phoenix_egg") { }

    struct npc_felkael_phoenix_eggAI : public QuantumBasicAI
    {
        npc_felkael_phoenix_eggAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 HatchTimer;

        void Reset()
        {
            HatchTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (HatchTimer <= diff)
            {
                me->SummonCreature(NPC_PHOENIX, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
                me->Kill(me);
            }
			else HatchTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felkael_phoenix_eggAI(creature);
    }
};

class npc_arcane_sphere : public CreatureScript
{
public:
    npc_arcane_sphere() : CreatureScript("npc_arcane_sphere") { }

    struct npc_arcane_sphereAI : public QuantumBasicAI
    {
        npc_arcane_sphereAI(Creature* creature) : QuantumBasicAI(creature) { Reset(); }

        uint32 DespawnTimer;
        uint32 ChangeTargetTimer;

        void Reset()
        {
            DespawnTimer = 30000;
            ChangeTargetTimer = urand(6000, 12000);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetDisableGravity(true);
            me->SetCurrentFaction(16);

            DoCast(me, SPELL_ARCANE_SPHERE_PASSIVE, true);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer <= diff)
                me->Kill(me);
            else
                DespawnTimer -= diff;

            if (!UpdateVictim())
                return;

            if (ChangeTargetTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                {
                    me->AddThreat(target, 1.0f);
                    me->TauntApply(target);
                    AttackStart(target);
                }

                ChangeTargetTimer = urand(5000, 15000);
            }
			else ChangeTargetTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_sphereAI(creature);
    }
};

void AddSC_boss_felblood_kaelthas()
{
    new boss_felblood_kaelthas();
    new npc_felkael_phoenix_egg();
    new npc_felkael_flamestrike();
	new npc_felkael_phoenix();
	new npc_arcane_sphere();
}