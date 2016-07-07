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

/* ScriptData
SDName: Boss_Leotheras_The_Blind
SD%Complete: 90
SDComment: Possesion Support
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "serpent_shrine.h"

#define SPELL_WHIRLWIND         37640
#define SPELL_CHAOS_BLAST       37674
#define SPELL_BERSERK           26662
#define SPELL_INSIDIOUS_WHISPER 37676
#define SPELL_DUAL_WIELD        42459

#define BANISH_BEAM             38909
#define AURA_BANISH             37833

#define SPELL_EARTHSHOCK        39076
#define SPELL_MINDBLAST         37531

#define INNER_DEMON_ID          21857
#define AURA_DEMONIC_ALIGNMENT  37713
#define SPELL_SHADOWBOLT        39309
#define SPELL_SOUL_LINK         38007
#define SPELL_CONSUMING_MADNESS 37749

#define MODEL_DEMON             20125
#define MODEL_NIGHTELF          20514
#define DEMON_FORM              21875
#define INNER_DEMON_VICTIM      1

#define SAY_AGGRO               -1548009
#define SAY_SWITCH_TO_DEMON     -1548010
#define SAY_INNER_DEMONS        -1548011
#define SAY_DEMON_SLAY1         -1548012
#define SAY_DEMON_SLAY2         -1548013
#define SAY_DEMON_SLAY3         -1548014
#define SAY_NIGHTELF_SLAY1      -1548015
#define SAY_NIGHTELF_SLAY2      -1548016
#define SAY_NIGHTELF_SLAY3      -1548017
#define SAY_FINAL_FORM          -1548018
#define SAY_FREE                -1548019
#define SAY_DEATH               -1548020

class boss_leotheras_the_blind : public CreatureScript
{
public:
    boss_leotheras_the_blind() : CreatureScript("boss_leotheras_the_blind") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_leotheras_the_blindAI(creature);
    }

    struct boss_leotheras_the_blindAI : public QuantumBasicAI
    {
        boss_leotheras_the_blindAI(Creature* creature) : QuantumBasicAI(creature)
        {
            creature->GetPosition(x, y, z);
            instance = creature->GetInstanceScript();
            Demon = 0;

            for (uint8 i = 0; i < 3; ++i) //clear guids
				SpellBinderGUID[i] = 0;
        }

        InstanceScript* instance;

        uint32 WhirlwindTimer;
        uint32 ChaosBlastTimer;
        uint32 SwitchToDemonTimer;
        uint32 SwitchToHumanTimer;
        uint32 BerserkTimer;
        uint32 InnerDemonsTimer;
        uint32 BanishTimer;

        bool DealDamage;
        bool NeedThreatReset;
        bool DemonForm;
        bool IsFinalForm;
        bool EnrageUsed;
        float x, y, z;

        uint64 InnderDemon[5];
        uint32 InnerDemon_Count;
        uint64 Demon;
        uint64 SpellBinderGUID[3];

        void Reset()
        {
            KillCharmedPlayer();
            CheckChannelers();
            BanishTimer = 1000;
            WhirlwindTimer = 15000;
            ChaosBlastTimer = 1000;
            SwitchToDemonTimer = 45000;
            SwitchToHumanTimer = 60000;
            BerserkTimer = 600000;
            InnerDemonsTimer = 30000;
            me->SetCanDualWield(true);
            DealDamage = true;
            DemonForm = false;
            IsFinalForm = false;
            NeedThreatReset = false;
            EnrageUsed = false;
			memset(InnderDemon, 0, sizeof(InnderDemon));
            InnerDemon_Count = 0;
            me->SetSpeed(MOVE_RUN, 2.0f, true);
            me->SetDisplayId(MODEL_NIGHTELF);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID  , 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);
            DoCast(me, SPELL_DUAL_WIELD, true);
            me->SetCorpseDelay(1000*60*60);
            if (instance)
				instance->SetData(DATA_LEOTHERASTHEBLINDEVENT, NOT_STARTED);
        }

        void CheckChannelers(/*bool DoEvade = true*/)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                if (Creature* add = Unit::GetCreature(*me, SpellBinderGUID[i]))
                    add->DisappearAndDie();

                float nx = x;
                float ny = y;
                float o = 2.4f;
                if (i == 0) {nx += 10; ny -= 5; o=2.5f;}
                if (i == 1) {nx -= 8; ny -= 7; o=0.9f;}
                if (i == 2) {nx -= 3; ny += 9; o=5.0f;}
                Creature* binder = me->SummonCreature(NPC_SPELLBINDER, nx, ny, z, o, TEMPSUMMON_DEAD_DESPAWN, 0);
                if (binder)
                    SpellBinderGUID[i] = binder->GetGUID();

            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->HasAura(AURA_BANISH))
                return;

            if (!me->GetVictim() && me->CanCreatureAttack(who))
            {
                if (me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                    return;

                float attackRadius = me->GetAttackDistance(who);
                if (me->IsWithinDistInMap(who, attackRadius))
                {
                    // Check first that object is in an angle in front of this one before LoS check
                    if (me->HasInArc(M_PI/2.0f, who) && me->IsWithinLOSInMap(who))
                        AttackStart(who);
                }
            }
        }

        void StartEvent()
        {
            DoSendQuantumText(SAY_AGGRO, me);
            if (instance)
                instance->SetData(DATA_LEOTHERASTHEBLINDEVENT, IN_PROGRESS);
        }

        void CheckBanish()
        {
            uint8 AliveChannelers = 0;
            for (uint8 i = 0; i < 3; ++i)
            {
                Unit* add = Unit::GetUnit(*me, SpellBinderGUID[i]);
                if (add && add->IsAlive())
                    ++AliveChannelers;
            }

            // channelers == 0 remove banish aura
            if (AliveChannelers == 0 && me->HasAura(AURA_BANISH))
            {
                // removing banish aura
                me->RemoveAurasDueToSpell(AURA_BANISH);
                // Leotheras is getting immune again
                me->ApplySpellImmune(AURA_BANISH, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
                // changing model to bloodelf
                me->SetDisplayId(MODEL_NIGHTELF);
                // and reseting equipment
                me->LoadEquipment(me->GetEquipmentId());

                if (instance && instance->GetData64(DATA_LEOTHERAS_EVENT_STARTER))
                {
                    Unit* victim = NULL;
                    victim = Unit::GetUnit(*me, instance->GetData64(DATA_LEOTHERAS_EVENT_STARTER));
                    if (victim)
                        me->getThreatManager().addThreat(victim, 1);
					StartEvent();
                }
            }
            else if (AliveChannelers != 0 && !me->HasAura(AURA_BANISH))
            {
                // channelers != 0 apply banish aura
                // removing Leotheras banish immune to apply AURA_BANISH
                me->ApplySpellImmune(AURA_BANISH, IMMUNITY_MECHANIC, MECHANIC_BANISH, false);
                DoCast(me, AURA_BANISH);

                // changing model
                me->SetDisplayId(MODEL_DEMON);

                // and removing weapons
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID  , 0);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);
            }
        }

        //Despawn all Inner Demon summoned
        void DespawnDemon()
        {
            for (uint8 i=0; i<5; ++i)
            {
                if (InnderDemon[i])
                {
                        //delete creature
                        Creature* creature = Unit::GetCreature(*me, InnderDemon[i]);
                        if (creature->IsAlive())
                            creature->DespawnAfterAction();

						InnderDemon[i] = 0;
				}
            }
			
			InnerDemon_Count = 0;
		}
		
		void KillCharmedPlayer()
		{
			if (!me->GetMap()->IsDungeon())
				return;
		
			InstanceMap::PlayerList const &playerliste = ((InstanceMap*)me->GetMap())->GetPlayers();
			InstanceMap::PlayerList::const_iterator it;
			
			Map::PlayerList const &PlayerList = ((InstanceMap*)me->GetMap())->GetPlayers();
			for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
			{
				Player* player = i->getSource();
				if (player && player->IsAlive() && player->HasAuraEffect(SPELL_CONSUMING_MADNESS,0))
					player->DealDamage(player,player->GetHealth(),0,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NORMAL,0,false);
			}
		}
		
		void CastConsumingMadness()
		{
			for (uint8 i=0; i<5; ++i)
			{
				if (InnderDemon[i] > 0)
				{
					Creature* unit = Unit::GetCreature(*me, InnderDemon[i]);
					if (unit && unit->IsAlive())
					{
						Unit* unit_target = Unit::GetUnit(*unit, unit->AI()->GetGUID(INNER_DEMON_VICTIM));
						if (unit_target && unit_target->IsAlive())
						{
							unit->CastSpell(unit_target, SPELL_CONSUMING_MADNESS, true);
							DoModifyThreatPercent(unit_target, -100);
						}
					}
				}
			}
		}
		
		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (DemonForm)
            {
                DoSendQuantumText(RAND(SAY_DEMON_SLAY1, SAY_DEMON_SLAY2, SAY_DEMON_SLAY3), me);
            }
            else
                DoSendQuantumText(RAND(SAY_NIGHTELF_SLAY1, SAY_NIGHTELF_SLAY2, SAY_NIGHTELF_SLAY3), me);
		}

        void JustDied(Unit* /*victim*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            //despawn copy
            if (Demon)
            {
                if (Creature* pDemon = Unit::GetCreature(*me, Demon))
                    pDemon->DespawnAfterAction();
            }

            if (instance)
                instance->SetData(DATA_LEOTHERASTHEBLINDEVENT, DONE);
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (me->HasAura(AURA_BANISH))
				return;

            me->LoadEquipment(me->GetEquipmentId());
        }

        void UpdateAI(const uint32 diff)
        {
            if (me->HasAura(AURA_BANISH) || !UpdateVictim())
            {
                if (BanishTimer <= diff)
                {
                    CheckBanish();//no need to check every update tick
                    BanishTimer = 1000;
                }
				else BanishTimer -= diff;
                return;
            }
            if (me->HasAura(SPELL_WHIRLWIND))
            {
                if (WhirlwindTimer <= diff)
                {
                    Unit* newTarget = SelectTarget(TARGET_RANDOM, 0);
                    if (newTarget)
                    {
                        DoResetThreat();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(0, newTarget->GetPositionX(), newTarget->GetPositionY(), newTarget->GetPositionZ());
                    }
                    WhirlwindTimer = 2000;
                }
				else WhirlwindTimer -= diff;
            }

            // reseting after changing forms and after ending whirlwind
            if (NeedThreatReset && !me->HasAura(SPELL_WHIRLWIND))
            {
                // when changing forms seting timers (or when ending whirlwind - to avoid adding new variable i use WhirlwindTimer to countdown 2s while whirlwinding)
                if (DemonForm)
                    InnerDemonsTimer = 30000;
                else
                    WhirlwindTimer =  15000;

                NeedThreatReset = false;
                DoResetThreat();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(me->GetVictim());
            }

            //EnrageTimer (10 min)
            if (BerserkTimer <= diff && !EnrageUsed)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_BERSERK);
                EnrageUsed = true;
            }
			else BerserkTimer -= diff;

            if (!DemonForm)
            {
                //Whirldind Timer
                if (!me->HasAura(SPELL_WHIRLWIND))
                {
                    if (WhirlwindTimer <= diff)
                    {
                        DoCast(me, SPELL_WHIRLWIND);
                        // while whirlwinding this variable is used to countdown target's change
                        WhirlwindTimer = 2000;
                        NeedThreatReset = true;
                    }
					else
						WhirlwindTimer -= diff;
                }
                //SwitchTimer

                if (!IsFinalForm)
                {
                    if (SwitchToDemonTimer <= diff)
                    {
                        //switch to demon form
                        me->RemoveAurasDueToSpell(SPELL_WHIRLWIND, 0);
                        me->SetDisplayId(MODEL_DEMON);
                        DoSendQuantumText(SAY_SWITCH_TO_DEMON, me);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID  , 0);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);
                        DemonForm = true;
                        NeedThreatReset = true;
                        SwitchToDemonTimer = 45000;
                    }
					else
						SwitchToDemonTimer -= diff;
                }
                DoMeleeAttackIfReady();
            }
            else
            {
                //ChaosBlastTimer
                if (!me->GetVictim())
                    return;
                if (me->IsWithinDist(me->GetVictim(), 30))
                    me->StopMoving();
                if (ChaosBlastTimer <= diff)
                {
                    // will cast only when in range of spell
                    if (me->IsWithinDist(me->GetVictim(), 30))
                    {
                        //DoCastVictim(SPELL_CHAOS_BLAST, true);
                        int damage = 100;
                        me->CastCustomSpell(me->GetVictim(), SPELL_CHAOS_BLAST, &damage, NULL, NULL, false, NULL, NULL, me->GetGUID());
                    }
                    ChaosBlastTimer = 3000;
                }
				else
					ChaosBlastTimer -= diff;
                //Summon Inner Demon
                if (InnerDemonsTimer <= diff)
                {
                    std::list<HostileReference*>& ThreatList = me->getThreatManager().getThreatList();
                    std::vector<Unit*> TargetList;
                    for (std::list<HostileReference*>::const_iterator itr = ThreatList.begin(); itr != ThreatList.end(); ++itr)
                    {
                        Unit* tempTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        if (tempTarget && tempTarget->GetTypeId() == TYPE_ID_PLAYER && tempTarget->GetGUID() != me->GetVictim()->GetGUID() && TargetList.size()<5)
                            TargetList.push_back(tempTarget);
                    }
                    //SpellInfo* spell = GET_SPELL(SPELL_INSIDIOUS_WHISPER);
                    for (std::vector<Unit*>::const_iterator itr = TargetList.begin(); itr != TargetList.end(); ++itr)
                    {
                        if ((*itr) && (*itr)->IsAlive())
                        {
                            Creature* demon = me->SummonCreature(INNER_DEMON_ID, (*itr)->GetPositionX()+10, (*itr)->GetPositionY()+10, (*itr)->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                            if (demon)
                            {
                                demon->AI()->AttackStart((*itr));
                                demon->AI()->SetGUID((*itr)->GetGUID(), INNER_DEMON_VICTIM);
                                demon->getThreatManager().addThreat((*itr),1000000);

                                (*itr)->AddAura(SPELL_INSIDIOUS_WHISPER, *itr);

                                //Safe storing of creatures
                                InnderDemon[InnerDemon_Count] = demon->GetGUID();

                                //Update demon count
                                ++InnerDemon_Count;
                            }
                        }
                    }
                    DoSendQuantumText(SAY_INNER_DEMONS, me);

                    InnerDemonsTimer = 999999;
                }
				else
					InnerDemonsTimer -= diff;

                //SwitchTimer
                if (SwitchToHumanTimer <= diff)
                {
                    //switch to nightelf form
                    me->SetDisplayId(MODEL_NIGHTELF);
                    me->LoadEquipment(me->GetEquipmentId());

                    CastConsumingMadness();
                    DespawnDemon();

                    DemonForm = false;
                    NeedThreatReset = true;

                    SwitchToHumanTimer = 60000;
                }
				else
					SwitchToHumanTimer -= diff;
            }

            if (!IsFinalForm && HealthBelowPct(15))
            {
                //at this point he divides himself in two parts
                CastConsumingMadness();
                DespawnDemon();
                Creature* Copy = NULL;
                Copy = DoSpawnCreature(DEMON_FORM, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6000);
                if (Copy)
                {
                    Demon = Copy->GetGUID();
                    if (me->GetVictim())
                        Copy->AI()->AttackStart(me->GetVictim());
                }
                //set nightelf final form
                IsFinalForm = true;
                DemonForm = false;

                DoSendQuantumText(SAY_FINAL_FORM, me);
                me->SetDisplayId(MODEL_NIGHTELF);
                me->LoadEquipment(me->GetEquipmentId());
            }
        }
    };
};

class boss_leotheras_the_blind_demonform : public CreatureScript
{
public:
    boss_leotheras_the_blind_demonform() : CreatureScript("boss_leotheras_the_blind_demonform") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_leotheras_the_blind_demonformAI(creature);
    }

    struct boss_leotheras_the_blind_demonformAI : public QuantumBasicAI
    {
        boss_leotheras_the_blind_demonformAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ChaosBlastTimer;
        bool DealDamage;

        void Reset()
        {
            ChaosBlastTimer = 1000;
            DealDamage = true;
        }

        void StartEvent()
        {
            DoSendQuantumText(SAY_FREE, me);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() != TYPE_ID_PLAYER)
                return;

            DoSendQuantumText(RAND(SAY_DEMON_SLAY1, SAY_DEMON_SLAY2, SAY_DEMON_SLAY3), me);
        }

        void JustDied(Unit* /*victim*/)
        {
            DoCast(me, 8149, true);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            StartEvent();
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;
            //ChaosBlastTimer
            if (me->IsWithinDist(me->GetVictim(), 30))
                me->StopMoving();

            if (ChaosBlastTimer <= diff)
             {
                // will cast only when in range od spell
                if (me->IsWithinDist(me->GetVictim(), 30))
                {
					//DoCastVictim(SPELL_CHAOS_BLAST, true);
                    int damage = 100;
                    me->CastCustomSpell(me->GetVictim(), SPELL_CHAOS_BLAST, &damage, NULL, NULL, false, NULL, NULL, me->GetGUID());
                    ChaosBlastTimer = 3000;
				}
			}
			else
				ChaosBlastTimer -= diff;
        }
    };
};

class mob_inner_demon : public CreatureScript
{
public:
    mob_inner_demon() : CreatureScript("mob_inner_demon") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_inner_demonAI(creature);
    }

    struct mob_inner_demonAI : public QuantumBasicAI
    {
        mob_inner_demonAI(Creature* creature) : QuantumBasicAI(creature)
        {
            victimGUID = 0;
        }

        uint32 ShadowBoltTimer;

        uint32 LinkTimer;
        uint64 victimGUID;

        void Reset()
        {
            ShadowBoltTimer = 10000;
            LinkTimer = 1000;
        }

		void SetGUID(uint64 guid, int32 id/* = 0 */)
		{
			if (id == INNER_DEMON_VICTIM)
				victimGUID = guid;
		}

		uint64 GetGUID(int32 id/* = 0 */)
		{
			if (id == INNER_DEMON_VICTIM)
				return victimGUID;
			return 0;
		}

        void JustDied(Unit* /*victim*/)
        {
            Unit* unit = Unit::GetUnit(*me, victimGUID);
            if (unit && unit->HasAura(SPELL_INSIDIOUS_WHISPER))
                unit->RemoveAurasDueToSpell(SPELL_INSIDIOUS_WHISPER);
        }

        void DamageTaken(Unit* done_by, uint32 &damage)
        {
            if (done_by->GetGUID() != victimGUID && done_by->GetGUID() != me->GetGUID())
            {
                damage = 0;
                DoModifyThreatPercent(done_by, -100);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!victimGUID)
				return;
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (me->GetVictim()->GetGUID() != victimGUID)
            {
                DoModifyThreatPercent(me->GetVictim(), -100);
                Unit* owner = Unit::GetUnit(*me, victimGUID);
                if (owner && owner->IsAlive())
                {
                    me->AddThreat(owner, 999999);
                    AttackStart(owner);
                }
				else if (owner && owner->IsDead())
                {
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return;
                }
            }

            if (LinkTimer <= diff)
            {
                DoCastVictim(SPELL_SOUL_LINK, true);
                LinkTimer = 1000;
            }
			else
				LinkTimer -= diff;

            if (!me->HasAura(AURA_DEMONIC_ALIGNMENT))
                DoCast(me, AURA_DEMONIC_ALIGNMENT, true);

            if (ShadowBoltTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOWBOLT, false);
                ShadowBoltTimer = 10000;
            }
			else
				ShadowBoltTimer -= diff;

           DoMeleeAttackIfReady();
        }
    };
};

class npc_greyheart_spellbinder : public CreatureScript
{
public:
    npc_greyheart_spellbinder() : CreatureScript("npc_greyheart_spellbinder") { }

    struct npc_greyheart_spellbinderAI : public QuantumBasicAI
    {
        npc_greyheart_spellbinderAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            leotherasGUID = 0;
            AddedBanish = false;
        }

        InstanceScript* instance;

        uint64 leotherasGUID;

        uint32 MindblastTimer;
        uint32 EarthshockTimer;

        bool AddedBanish;

        void Reset()
        {
            MindblastTimer  = urand(3000, 8000);
            EarthshockTimer = urand(5000, 10000);

            if (instance)
            {
                instance->SetData64(DATA_LEOTHERAS_EVENT_STARTER, 0);
                Creature* leotheras = Unit::GetCreature(*me, leotherasGUID);
                if (leotheras && leotheras->IsAlive())
                    CAST_AI(boss_leotheras_the_blind::boss_leotheras_the_blindAI, leotheras->AI())->CheckChannelers(/*false*/);
            }
        }

        void EnterToBattle(Unit* who)
        {
            me->InterruptNonMeleeSpells(false);

            if (instance)
                instance->SetData64(DATA_LEOTHERAS_EVENT_STARTER, who->GetGUID());
        }

        void JustRespawned()
        {
            AddedBanish = false;
            Reset();
        }

        void CastChanneling()
        {
            if (!me->IsInCombatActive() && !me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            {
                if (leotherasGUID)
                {
                    Creature* leotheras = Unit::GetCreature(*me, leotherasGUID);
                    if (leotheras && leotheras->IsAlive())
                        DoCast(leotheras, BANISH_BEAM);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance)
            {
                if (!leotherasGUID)
                    leotherasGUID = instance->GetData64(DATA_LEOTHERAS);

                if (!me->IsInCombatActive() && instance->GetData64(DATA_LEOTHERAS_EVENT_STARTER))
                {
                    Unit* victim = NULL;
                    victim = Unit::GetUnit(*me, instance->GetData64(DATA_LEOTHERAS_EVENT_STARTER));
                    if (victim)
						AttackStart(victim);
                }
            }

            if (!UpdateVictim())
            {
                CastChanneling();
                return;
            }

            if (instance && !instance->GetData64(DATA_LEOTHERAS_EVENT_STARTER))
            {
                EnterEvadeMode();
                return;
            }

            if (MindblastTimer <= diff)
            {
                Unit* target = NULL;
                target = SelectTarget(TARGET_RANDOM, 0);

                if (target)DoCast(target, SPELL_MINDBLAST);

                MindblastTimer = urand(10000, 15000);
            }
			else
				MindblastTimer -= diff;

            if (EarthshockTimer <= diff)
            {
                Map* map = me->GetMap();
                Map::PlayerList const &PlayerList = map->GetPlayers();
                for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                    {
                        bool isCasting = false;
                        for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
                            if (player->GetCurrentSpell(i))
                                isCasting = true;

                        if (isCasting)
                        {
                            DoCast(player, SPELL_EARTHSHOCK);
                            break;
                        }
                    }
                }

                EarthshockTimer = urand(8000, 15000);
            }
			else
				EarthshockTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greyheart_spellbinderAI(creature);
    }
};

void AddSC_boss_leotheras_the_blind()
{
    new boss_leotheras_the_blind();
    new boss_leotheras_the_blind_demonform();
	new mob_inner_demon();
    new npc_greyheart_spellbinder();
}