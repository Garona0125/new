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

enum Texts
{
    SAY_INTRO     = -1543017,
    SAY_WIPE      = -1543018,
    SAY_AGGRO_1   = -1543019,
    SAY_AGGRO_2   = -1543020,
    SAY_AGGRO_3   = -1543021,
    SAY_KILL_1    = -1543022,
    SAY_KILL_2    = -1543023,
    SAY_DIE       = -1543024,
    EMOTE         = -1543025,
};

enum Spells
{
    SPELL_FIREBALL                = 34653,
    SPELL_FIREBALL_H              = 36920,
    SPELL_CONE_OF_FIRE            = 30926,
    SPELL_CONE_OF_FIRE_H          = 36921,
    SPELL_SUMMON_LIQUID_FIRE      = 23971,
    SPELL_SUMMON_LIQUID_FIRE_H    = 30928,
    SPELL_BELLOWING_ROAR          = 39427,
    SPELL_REVENGE                 = 19130,
    SPELL_REVENGE_H               = 40392,
    SPELL_KIDNEY_SHOT             = 30621,
    SPELL_FIRE_NOVA_VISUAL        = 19823,
};

const float VazrudenMiddle[3] = {-1406.5f, 1746.5f, 81.2f};

const float VazrudenRing[2][3] =
{
    {-1430, 1705, 112},
    {-1377, 1760, 112},
};

class boss_nazan : public CreatureScript
{
    public:
		boss_nazan() : CreatureScript("boss_nazan") { }

        struct boss_nazanAI : public QuantumBasicAI
        {
            boss_nazanAI(Creature* creature) : QuantumBasicAI(creature)
            {
                VazrudenGUID = 0;
                Flight = true;
            }

            uint32 FireballTimer;
            uint32 ConeOfFireTimer;
            uint32 BellowingRoarTimer;
            uint32 FlyTimer;
            uint32 TurnTimer;
            bool Flight;
            uint64 VazrudenGUID;
            SpellInfo* liquid_fire;

            void Reset()
            {
                FireballTimer = 4000;
                FlyTimer = 45000;
                TurnTimer = 0;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/) {}

            void JustSummoned(Creature* Summoned)
            {
                if (Summoned->GetEntry() == NPC_LIQUID_FIRE)
                {
                    Summoned->SetLevel(me->GetCurrentLevel());
                    Summoned->SetCurrentFaction(me->GetFaction());
                    Summoned->CastSpell(Summoned, DUNGEON_MODE(SPELL_SUMMON_LIQUID_FIRE, SPELL_SUMMON_LIQUID_FIRE_H), true);
                    Summoned->CastSpell(Summoned, SPELL_FIRE_NOVA_VISUAL, true);
                }
            }

            void SpellHitTarget(Unit* target, const SpellInfo* entry)
            {
                if (target && entry->Id == uint32(SPELL_FIREBALL))
                    me->SummonCreature(NPC_LIQUID_FIRE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (FireballTimer <= diff)
                {
                    if (Unit* victim = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(victim, DUNGEON_MODE(SPELL_FIREBALL, SPELL_FIREBALL_H), true);
						FireballTimer = urand(4000, 7000);
					}
                }
                else FireballTimer -= diff;

                if (Flight) // Phase 1 - the Flight
                {
                    Creature* Vazruden = Unit::GetCreature(*me, VazrudenGUID);
                    if (FlyTimer < diff || !Vazruden->IsAlive() && Vazruden->HealthAbovePct(20))
                    {
                        Flight = false;
                        BellowingRoarTimer = 6000;
                        ConeOfFireTimer = 12000;
                        me->SetDisableGravity(false);
                        me->SetWalk(true);
                        me->GetMotionMaster()->Clear();
                        if (Unit* target = SelectTarget(TARGET_NEAREST, 0))
                            me->AI()->AttackStart(target);
                        DoStartMovement(me->GetVictim());
                        DoSendQuantumText(EMOTE, me);
                        return;
                    }
                    else FlyTimer -= diff;

                    if (TurnTimer <= diff)
                    {
                        uint32 Waypoint = (FlyTimer/10000)%2;
                        if (!me->IsWithinDist3d(VazrudenRing[Waypoint][0], VazrudenRing[Waypoint][1], VazrudenRing[Waypoint][2], 5))
                            me->GetMotionMaster()->MovePoint(0, VazrudenRing[Waypoint][0], VazrudenRing[Waypoint][1], VazrudenRing[Waypoint][2]);
                        TurnTimer = 10000;
                    }
                    else TurnTimer -= diff;
                }
                else
                {
                    if (ConeOfFireTimer <= diff)
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_CONE_OF_FIRE, SPELL_CONE_OF_FIRE_H));
                        ConeOfFireTimer = 12000;
                        FireballTimer = 4000;
                    }
                    else ConeOfFireTimer -= diff;

                    if (IsHeroic())
                    {
                        if (BellowingRoarTimer <= diff)
                        {
                            DoCast(me, SPELL_BELLOWING_ROAR);
                            BellowingRoarTimer = 45000;
                        }
                        else BellowingRoarTimer -= diff;
                    }

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_nazanAI(creature);
        }
};

class boss_vazruden : public CreatureScript
{
public:
	boss_vazruden() : CreatureScript("boss_vazruden") { }

	struct boss_vazrudenAI : public QuantumBasicAI
	{
		boss_vazrudenAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 RevengeTimer;
		uint32 UnsummonCheck;

		bool WipeSaid;

		void Reset()
		{
			RevengeTimer = 4000;
			UnsummonCheck = 2000;

			WipeSaid = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
		}

		void KilledUnit(Unit* victim)
		{
			if (victim->GetEntry() != NPC_VAZRUDEN)
				DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer != me)
				DoSendQuantumText(SAY_DIE, me);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
			{
				if (UnsummonCheck < diff && me->IsAlive())
				{
					if (!WipeSaid)
					{
						DoSendQuantumText(SAY_WIPE, me);
						WipeSaid = true;
					}
					me->DisappearAndDie();
				}
				else UnsummonCheck -= diff;

				return;
			}

			if (RevengeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_REVENGE, SPELL_REVENGE_H));
				RevengeTimer = 5000;
			}
			else RevengeTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_vazrudenAI(creature);
	}
};

class boss_vazruden_the_herald : public CreatureScript
{
public:
	boss_vazruden_the_herald() : CreatureScript("boss_vazruden_the_herald") { }

        struct boss_vazruden_the_heraldAI : public QuantumBasicAI
        {
            boss_vazruden_the_heraldAI(Creature* creature) : QuantumBasicAI(creature)
            {
				Summoned = false;
                SentryDown = false;
                NazanGUID = 0;
                VazrudenGUID = 0;
            }

            uint32 Phase;
            uint32 Waypoint;
            uint32 Check;

            uint64 NazanGUID;
            uint64 VazrudenGUID;

            bool Summoned;
			bool SentryDown;

            void Reset()
            {
                Phase = 0;
                Waypoint = 0;
                Check = 0;
                UnsummonAdds();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);

				me->SetCanFly(true);
				me->SetDisableGravity(true);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void UnsummonAdds()
            {
                if (Summoned)
                {
                    Creature* Nazan = Unit::GetCreature(*me, NazanGUID);
                    if (!Nazan)
                        Nazan = me->FindCreatureWithDistance(NPC_NAZAN, 1500.0f);
                    if (Nazan)
                    {
                        Nazan->DisappearAndDie();
                        NazanGUID = 0;
                    }

                    Creature* Vazruden = Unit::GetCreature(*me, VazrudenGUID);
                    if (!Vazruden)
                        Vazruden = me->FindCreatureWithDistance(NPC_VAZRUDEN, 1500.0f);
                    if (Vazruden)
                    {
                        Vazruden->DisappearAndDie();
                        VazrudenGUID = 0;
                    }
                    Summoned = false;
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    me->SetVisible(true);

					std::list<Creature*> TrashList;
					me->GetCreatureListWithEntryInGrid(TrashList, NPC_HELLFIRE_SENTRY, 500.0f);

					if (!TrashList.empty())
					{
						for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
						{
							if (Creature* sentry = *itr)
							{
								sentry->Respawn();
								sentry->GetMotionMaster()->MoveTargetedHome();
							}
						}
					}
                }
            }

            void SummonAdds()
            {
                if (!Summoned)
                {
                    if (Creature* Vazruden = me->SummonCreature(NPC_VAZRUDEN, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 6000000))
                        VazrudenGUID = Vazruden->GetGUID();
                    if (Creature* Nazan = me->SummonCreature(NPC_NAZAN, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 6000000))
                        NazanGUID = Nazan->GetGUID();
                    Summoned = true;
                    me->SetVisible(false);
                    me->AddUnitState(UNIT_STATE_ROOT);
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (Phase == 0)
                {
                    Phase = 1;
                    Check = 0;
                    DoSendQuantumText(SAY_INTRO, me);
                }
            }

            void JustSummoned(Creature* Summoned)
            {
                if (!Summoned)
                    return;

                Unit* target = me->GetVictim();
                if (Summoned->GetEntry() == NPC_NAZAN)
                {
                    CAST_AI(boss_nazan::boss_nazanAI, Summoned->AI())->VazrudenGUID = VazrudenGUID;
                    Summoned->SetDisableGravity(true);
                    Summoned->SetSpeed(MOVE_FLIGHT, 2.5f);
					Summoned->AI()->DoZoneInCombat();
                    if (target)
                        AttackStartNoMove(target);
                }
				else
					if (target)
						Summoned->AI()->AttackStart(target);
            }

            void SentryDownBy(Unit* killer)
            {
                if (SentryDown)
                {
                    AttackStartNoMove(killer);
                    SentryDown = false;
                }
                else SentryDown = true;
            }

            void UpdateAI(const uint32 diff)
            {
                switch (Phase)
                {
                case 0: // circle around the platform
					return;
                    break;
                case 1: // go to the middle and begin the fight
                    if (Check <= diff)
                    {
                        if (!me->IsWithinDist3d(VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2], 5))
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(0, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2]);
                            Check = 1000;
                        }
                        else
                        {
                            SummonAdds();
                            Phase = 2;
                            return;
                        }
                    }
                    else
                        Check -= diff;
                    break;
                default:
                    if (Check <= diff)
                    {
                        Creature* Nazan = Unit::GetCreature(*me, NazanGUID);
                        Creature* Vazruden = Unit::GetCreature(*me, VazrudenGUID);
                        if (Nazan->IsAlive() || Vazruden->IsAlive())
                        {
                            if (Nazan->GetVictim() || Vazruden->GetVictim())
                                return;
                            else
                            {
                                UnsummonAdds();
                                EnterEvadeMode();
                                return;
                            }
                        }
                        else
                        {
                            me->SummonGameObject(DUNGEON_MODE(GO_REINFORCED_FEL_IRON_CHEST_5N, GO_REINFORCED_FEL_IRON_CHEST_5H), -1432.31f, 1771.16f, 82.0504f, 5.41551f, 0, 0, 0.420357f, -0.907359f, 3600000);
                            me->SetLootRecipient(NULL);
                            me->DealDamage(me, me->GetHealth());
                            me->RemoveCorpse();
                        }
                        Check = 2000;
                    }
                    else Check -= diff;
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_vazruden_the_heraldAI(creature);
        }
};

class npc_hellfire_sentry : public CreatureScript
{
public:
	npc_hellfire_sentry() : CreatureScript("npc_hellfire_sentry") { }

	struct npc_hellfire_sentryAI : public QuantumBasicAI
	{
		npc_hellfire_sentryAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 KidneyShotTimer;

		void Reset()
		{
			KidneyShotTimer = urand(3000, 7000);
		}

		void EnterToBattle(Unit* /*who*/) {}

		void JustDied(Unit* who)
		{
			if (Creature* herald = me->FindCreatureWithDistance(NPC_VAZRUDEN_HERALD, 250.0f))
				CAST_AI(boss_vazruden_the_herald::boss_vazruden_the_heraldAI, herald->AI())->SentryDownBy(who);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (KidneyShotTimer <= diff)
			{
				DoCastVictim(SPELL_KIDNEY_SHOT);
				KidneyShotTimer = 20000;
			}
			else KidneyShotTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_hellfire_sentryAI(creature);
	}
};

void AddSC_boss_vazruden_the_herald()
{
    new boss_vazruden_the_herald();
    new boss_vazruden();
    new boss_nazan();
    new npc_hellfire_sentry();
}