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

enum Texts
{
    SAY_CONFLAGRATION          = -1580044,
    SAY_SISTER_SACROLASH_DEAD  = -1580045,
    SAY_ALY_KILL_1             = -1580046,
    SAY_ALY_KILL_2             = -1580047,
    SAY_ALY_DEAD               = -1580048,
    SAY_BERSERK                = -1580049,
    SAY_SHADOW_NOVA            = -1580050,
    SAY_SISTER_ALYTHESS_DEAD   = -1580051,
    SAY_SAC_KILL_1             = -1580052,
    SAY_SAC_KILL_2             = -1580053,
    SAY_SAC_DEAD               = -1580054,
    SAY_ENRAGE                 = -1580055,
    SAY_INTRO_SAC_1            = -1580056,
    SAY_INTRO_ALY_2            = -1580057,
    SAY_INTRO_SAC_3            = -1580058,
    SAY_INTRO_ALY_4            = -1580059,
    SAY_INTRO_SAC_5            = -1580060,
    SAY_INTRO_ALY_6            = -1580061,
    SAY_INTRO_SAC_7            = -1580062,
    SAY_INTRO_ALY_8            = -1580063,
    EMOTE_SHADOW_NOVA          = -1580064,
    EMOTE_CONFLAGRATION        = -1580065,
};

enum Spells
{
    SPELL_DARK_TOUCHED      = 45347,
    SPELL_SHADOW_BLADES     = 45248,
    SPELL_DARK_STRIKE       = 45271,
    SPELL_SHADOW_NOVA       = 45329,
    SPELL_CONFOUNDING_BLOW  = 45256,
    SPELL_SHADOW_FURY       = 45270,
    SPELL_IMAGE_VISUAL      = 45263,
    SPELL_ENRAGE            = 46587,
    SPELL_EMPOWER           = 45366,
    SPELL_DARK_FLAME        = 45345,
    SPELL_PYROGENICS        = 45230,
    SPELL_FLAME_TOUCHED     = 45348,
    SPELL_CONFLAGRATION     = 45342,
    SPELL_BLAZE             = 45235,
    SPELL_FLAME_SEAR        = 46771,
    SPELL_BLAZE_SUMMON      = 45236,
    SPELL_BLAZE_BURN        = 45246,
};

struct boss_eredar_twinAI : public QuantumBasicAI
{
    boss_eredar_twinAI(Creature* creature) : QuantumBasicAI(creature){}

    uint32 AreaInCombatTimer;

    void Reset()
    {
        AreaInCombatTimer = 5000;
    }

    void SendAttacker(Unit* target) // Exploit Fix
    {
        std::list<Creature*> templist;
        float x, y, z;
        me->GetPosition(x, y, z);

        {
            CellCoord pair(Trinity::ComputeCellCoord(x, y));
            Cell cell(pair);
            cell.SetNoCreate();
            Trinity::AllFriendlyCreaturesInGrid check(me);
            Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(me, templist, check);
            TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid>, GridTypeMapContainer> cSearcher(searcher);
            cell.Visit(pair, cSearcher, *(me->GetMap()), *me, SIZE_OF_GRIDS);
        }

        if (!templist.size())
            return;

        for(std::list<Creature*>::iterator i = templist.begin(); i != templist.end(); ++i)
        {
            if ((*i) && me->IsWithinDistInMap((*i),10))
            {
                if (!(*i)->IsInCombatActive() && !me->GetVictim())
                    (*i)->AI()->AttackStart(target);
            }
        }
    }

    void EnterToBattle(Unit *who)
    {
        DoAttackerAreaInCombat(who, 100);

        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
			SendAttacker(target);
    }

    void KilledUnit(Unit* /*victim*/){}

    void JustDied(Unit* /*killer*/){}

    void CheckRadianceAura()
    {
        if (!me->HasAura(SPELL_SUNWELL_RADIANCE, 0))
            DoCast(me, SPELL_SUNWELL_RADIANCE, true);
    }

    void DoAggroPuls(const uint32 diff)
    {
        if (AreaInCombatTimer <= diff)
        {
            DoAttackerAreaInCombat(me->GetVictim(), 100);
            AreaInCombatTimer = 5000;
        }
		else AreaInCombatTimer -= diff;
    }

    void HandleTouchedSpells(Unit* target, uint32 TouchedType)
    {
        switch(TouchedType)
        {
        case SPELL_FLAME_TOUCHED:
            if (!target->HasAura(SPELL_DARK_FLAME, 0))
            {
                if (target->HasAura(SPELL_DARK_TOUCHED, 0))
                {
                    target->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
                    target->CastSpell(target, SPELL_DARK_FLAME, true, 0, 0, me->GetGUID());
                }
				else
                    target->CastSpell(target, SPELL_FLAME_TOUCHED, true, 0, 0, me->GetGUID());
            }
            break;
        case SPELL_DARK_TOUCHED:
            if (!target->HasAura(SPELL_DARK_FLAME, 0))
            {
                if (target->HasAura(SPELL_FLAME_TOUCHED, 0))
                {
                    target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                    target->CastSpell(target, SPELL_DARK_FLAME, true, 0, 0, me->GetGUID());
                }
				else target->CastSpell(target, SPELL_DARK_TOUCHED, true, 0, 0, me->GetGUID());
            }
            break;
        }
    }

    void UpdateAI(const uint32 diff){}
};

class boss_lady_sacrolash : public CreatureScript
{
public:
    boss_lady_sacrolash() : CreatureScript("boss_lady_sacrolash") {}

    struct boss_lady_sacrolashAI : public boss_eredar_twinAI
    {
		boss_lady_sacrolashAI(Creature* creature) : boss_eredar_twinAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

        InstanceScript* instance;

        bool InCombat;
        bool SisterDeath;
        bool Enraged;

        uint32 ShadowbladesTimer;
        uint32 ShadowNovaTimer;
        uint32 ConfoundingBlowTimer;
        uint32 ShadowimageTimer;
        uint32 ConflagrationTimer;
        uint32 EnrageTimer;

        void Reset()
        {
            InCombat = false;
            Enraged = false;

            if (instance)
            {
                Unit* Temp = Unit::GetUnit(*me, instance->GetData64(DATA_ALYTHESS));
                if (Temp)
				{
                    if (Temp->IsDead())
					{
                        CAST_CRE(Temp)->Respawn();
					}
					else
                    {
                        if (Temp->GetVictim())
                        {
                            me->getThreatManager().addThreat(Temp->GetVictim(),0.0f);
                            InCombat = true;
                        }
					}
				}
			}

            if (!InCombat)
            {
                ShadowbladesTimer = 10000;
                ShadowNovaTimer = 30000;
                ConfoundingBlowTimer = 25000;
                ShadowimageTimer = 20000;
                ConflagrationTimer = 30000;
                EnrageTimer = 360000;

                SisterDeath = false;
            }

			instance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* who)
        {
            boss_eredar_twinAI::EnterToBattle(who);

            if (instance)
            {
                Unit* Temp =  Unit::GetUnit(*me, instance->GetData64(DATA_ALYTHESS));
                if (Temp && Temp->IsAlive() && !(Temp->GetVictim()))
                    ((Creature*)Temp)->AI()->AttackStart(who);
            }

			instance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SAC_KILL_1, SAY_SAC_KILL_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (SisterDeath)
            {
                DoSendQuantumText(SAY_SAC_DEAD, me);

				instance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            switch(spell->Id)
            {
            case SPELL_SHADOW_BLADES:
            case SPELL_SHADOW_NOVA:
            case SPELL_CONFOUNDING_BLOW:
            case SPELL_SHADOW_FURY:
                HandleTouchedSpells(target, SPELL_DARK_TOUCHED);
                break;
            case SPELL_CONFLAGRATION:
                HandleTouchedSpells(target, SPELL_FLAME_TOUCHED);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            CheckRadianceAura();

            if (!SisterDeath)
            {
                if (instance)
                {
                    Creature* Temp = Creature::GetCreature(*me, instance->GetData64(DATA_ALYTHESS));
                    if (Temp && Temp->IsDead())
                    {
                        DoSendQuantumText(SAY_SISTER_ALYTHESS_DEAD, me);
                        me->InterruptSpell(CURRENT_GENERIC_SPELL);
						DoCast(me, SPELL_EMPOWER);
						SisterDeath = true;
                    }
                }
            }

            if (!UpdateVictim())
                return;

            DoAggroPuls(diff);

            if (SisterDeath)
            {
                if (ConflagrationTimer <= diff)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 300, true))
					{
						DoCast(target, SPELL_CONFLAGRATION);
						ConflagrationTimer = 30000+(rand()%5000);
					}
                }
				else ConflagrationTimer -= diff;
            }
            else
            {
                if (ShadowNovaTimer <= diff)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 300, true))
					{
						DoCast(target, SPELL_SHADOW_NOVA);

						if (!SisterDeath)
						{
							DoSendQuantumText(EMOTE_SHADOW_NOVA, me, target);
							DoSendQuantumText(SAY_SHADOW_NOVA, me);
						}
					}
					ShadowNovaTimer = 30000+(rand()%5000);
                }
				else ShadowNovaTimer -= diff;
            }

            if (ConfoundingBlowTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 10, true))
				{
					DoCast(target, SPELL_CONFOUNDING_BLOW);
					ConfoundingBlowTimer = 20000 + (rand()%5000);
				}
            }
			else ConfoundingBlowTimer -= diff;

            if (ShadowimageTimer <= diff)
            {
                Creature* temp = NULL;

                for (int i = 0; i<3; i++)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						temp = DoSpawnCreature(NPC_SHADOW_IMAGE, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 10000);

						if (temp && target)
						{
							temp->AI()->AttackStart(target);
							temp->getThreatManager().addThreat(target, 1000.0f);
						}
                    }
                }
                ShadowimageTimer = 20000;
            }
			else ShadowimageTimer -= diff;

            if (ShadowbladesTimer <= diff)
            {
				DoCast(me, SPELL_SHADOW_BLADES);
				ShadowbladesTimer = 10000;
            }
			else ShadowbladesTimer -= diff;

            if (EnrageTimer <= diff && !Enraged)
            {
                me->InterruptSpell(CURRENT_GENERIC_SPELL);
                DoSendQuantumText(SAY_ENRAGE, me);
                DoCast(me, SPELL_ENRAGE);
                if (me->HasAura(SPELL_ENRAGE,0))
                    Enraged = true;
            }
			else EnrageTimer -= diff;

            if (me->IsAttackReady() && !me->IsNonMeleeSpellCasted(false))
            {
                if (me->IsWithinMeleeRange(me->GetVictim()))
                {
                    HandleTouchedSpells(me->GetVictim(), SPELL_DARK_TOUCHED);
                    me->AttackerStateUpdate(me->GetVictim());
                    me->ResetAttackTimer();
                }
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_sacrolashAI(creature);
    }
};

class boss_grand_warlock_alythess : public CreatureScript
{
public:
    boss_grand_warlock_alythess() : CreatureScript("boss_grand_warlock_alythess") {}

    struct boss_grand_warlock_alythessAI : public boss_eredar_twinAI
    {
        boss_grand_warlock_alythessAI(Creature* creature) : boss_eredar_twinAI(creature)
		{
            instance = creature->GetInstanceScript();
            IntroStepCounter = 10;
        }

        InstanceScript* instance;

        bool InCombat;
        bool SisterDeath;
        bool Enraged;

        uint32 IntroStepCounter;
        uint32 IntroYellTimer;

        uint32 ConflagrationTimer;
        uint32 BlazeTimer;
        uint32 PyrogenicsTimer;
        uint32 ShadowNovaTimer;
        uint32 FlamesearTimer;
        uint32 EnrageTimer;

        void Reset()
        {
            InCombat = false;
            Enraged = false;

            if (instance)
            {
                Unit* Temp =  Unit::GetUnit(*me, instance->GetData64(DATA_SACROLASH));
                if (Temp)
				{
                    if (Temp->IsDead())
                    {
                        ((Creature*)Temp)->Respawn();
                    }
					else
                    {
                        if (Temp->GetVictim())
                        {
                            me->getThreatManager().addThreat(Temp->GetVictim(), 0.0f);
                            InCombat = true;
                        }
					}
				}
			}

            if (!InCombat)
            {
                ConflagrationTimer = 45000;
                BlazeTimer = 100;
                PyrogenicsTimer = 15000;
                ShadowNovaTimer = 40000;
                EnrageTimer = 360000;
                FlamesearTimer = 15000;
                IntroYellTimer = 10000;
                SisterDeath = false;
            }

			instance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* who)
        {
            boss_eredar_twinAI::EnterToBattle(who);

            if (instance)
            {
                Unit* Temp =  Unit::GetUnit(*me, instance->GetData64(DATA_SACROLASH));
                if (Temp && Temp->IsAlive() && !(Temp->GetVictim()))
                    ((Creature*)Temp)->AI()->AttackStart(who);
            }

			instance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
        }

        void AttackStart(Unit *who)
        {
            QuantumBasicAI::AttackStartNoMove(who);
        }

        void MoveInLineOfSight(Unit *who)
        {
            if (!who || me->GetVictim())
                return;

            if (me->CanCreatureAttack(who))
            {
                float attackRadius = me->GetAttackDistance(who);
                if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && me->IsWithinLOSInMap(who))
                {
                    if (!InCombat)
                    {
                        DoStartNoMovement(who);
                        EnterToBattle(who);
                        InCombat = true;
                    }
                }
            }
            else if (IntroStepCounter == 10 && me->IsWithinLOSInMap(who)&& me->IsWithinDistInMap(who, 25))
				IntroStepCounter = 0;
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ALY_KILL_1, SAY_ALY_KILL_2), me);
        }

        void JustDied(Unit* /*Killer*/)
        {
            if (SisterDeath)
            {
                DoSendQuantumText(SAY_ALY_DEAD, me);

				instance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            switch(spell->Id)
            {
            case SPELL_BLAZE:
                target->CastSpell(target, SPELL_BLAZE_SUMMON, true, 0, 0, me->GetGUID());
            case SPELL_CONFLAGRATION:
            case SPELL_FLAME_SEAR:
            case SPELL_BLAZE_BURN:
                HandleTouchedSpells(target, SPELL_FLAME_TOUCHED);
                break;
            case SPELL_SHADOW_NOVA:
                HandleTouchedSpells(target, SPELL_DARK_TOUCHED);
                break;
            }
        }

        uint32 IntroStep(uint32 step)
        {
            Creature* Sacrolash = (Creature*)Unit::GetUnit(*me, instance->GetData64(DATA_SACROLASH));
            switch (step)
            {
            case 0:
				return 0;
            case 1:
                if (Sacrolash)
                    DoSendQuantumText(SAY_INTRO_SAC_1, Sacrolash);
                return 1000;
            case 2: DoSendQuantumText(SAY_INTRO_ALY_2, me);
				return 1000;
            case 3:
                if (Sacrolash)
                    DoSendQuantumText(SAY_INTRO_SAC_3, Sacrolash);
				return 2000;
            case 4: DoSendQuantumText(SAY_INTRO_ALY_4, me);
				return 1000;
            case 5:
                if (Sacrolash)
                    DoSendQuantumText(SAY_INTRO_SAC_5, Sacrolash);
                return 2000;
            case 6: DoSendQuantumText(SAY_INTRO_ALY_6, me);
				return 1000;
            case 7:
                if (Sacrolash)
                    DoSendQuantumText(SAY_INTRO_SAC_7, Sacrolash);
                return 3000;
            case 8: DoSendQuantumText(SAY_INTRO_ALY_8, me);
				return 900000;
            }
            return 10000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (IntroStepCounter < 9)
            {
                if (IntroYellTimer <= diff)
					IntroYellTimer = IntroStep(++IntroStepCounter);

				else IntroYellTimer -= diff;
            }

            CheckRadianceAura();

            if (!SisterDeath)
            {
                if (instance)
                {
                    Creature* Temp = Creature::GetCreature(*me, instance->GetData64(DATA_SACROLASH));
                    if (Temp && Temp->IsDead())
                    {
                        DoSendQuantumText(SAY_SISTER_SACROLASH_DEAD, me);
                        me->InterruptSpell(CURRENT_GENERIC_SPELL);
						DoCast(me, SPELL_EMPOWER);
						SisterDeath = true;
                    }
                }
            }

            if (!UpdateVictim())
                return;

            DoAggroPuls(diff);

            if (SisterDeath)
            {
                if (ShadowNovaTimer <= diff)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 300, true))
					{
						DoCast(target, SPELL_SHADOW_NOVA);
						ShadowNovaTimer= 30000+(rand()%5000);
					}
                }
				else ShadowNovaTimer -= diff;
            }
            else
            {
                if (ConflagrationTimer <= diff)
                {
					me->InterruptSpell(CURRENT_GENERIC_SPELL);

					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 300, true))
					{
						DoCast(target, SPELL_CONFLAGRATION);
						ConflagrationTimer = 30000+(rand()%5000);

						if (!SisterDeath)
						{
							DoSendQuantumText(EMOTE_CONFLAGRATION, me, target);
							DoSendQuantumText(SAY_CONFLAGRATION, me);
						}
					}
					BlazeTimer = 4000;
				}
				else ConflagrationTimer -= diff;
            }

            if (FlamesearTimer <= diff)
            {
				DoCast(me, SPELL_FLAME_SEAR);
				FlamesearTimer = 15000;
            }
			else FlamesearTimer -= diff;

            if (PyrogenicsTimer <= diff)
            {
				DoCast(me, SPELL_PYROGENICS, true);
				PyrogenicsTimer = 15000;
            }
			else PyrogenicsTimer -= diff;

            if (BlazeTimer <= diff)
            {
				DoCastVictim(SPELL_BLAZE);
				BlazeTimer = 3800;
            }
			else BlazeTimer -= diff;

            if (EnrageTimer <= diff && !Enraged)
            {
                me->InterruptSpell(CURRENT_GENERIC_SPELL);
                DoSendQuantumText(SAY_BERSERK, me);
				DoCast(me, SPELL_ENRAGE);
				Enraged = true;
            }
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grand_warlock_alythessAI(creature);
    }
};

class npc_shadow_image : public CreatureScript
{
public:
    npc_shadow_image() : CreatureScript("npc_shadow_image") {}

    struct npc_shadow_imageAI : public QuantumBasicAI
    {
        npc_shadow_imageAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowfuryTimer;
        uint32 KillTimer;
        uint32 DarkstrikeTimer;

        void Reset()
        {
            ShadowfuryTimer = 5000 + (rand()%15000);
            DarkstrikeTimer = 3000;
            KillTimer = 15000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            switch(spell->Id)
            {
            case SPELL_SHADOW_FURY:
            case SPELL_DARK_STRIKE:
                if (!target->HasAura(SPELL_DARK_FLAME, 0))
                {
                    if (target->HasAura(SPELL_FLAME_TOUCHED, 0))
                    {
                        target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                        target->CastSpell(target, SPELL_DARK_FLAME, true);
                    }
					else target->CastSpell(target,SPELL_DARK_TOUCHED,true);
                }
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasAura(SPELL_IMAGE_VISUAL, 0))
                DoCast(me, SPELL_IMAGE_VISUAL);

            if (KillTimer <= diff)
            {
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                KillTimer = 9999999;
            }
			else KillTimer -= diff;

            if (!UpdateVictim())
                return;

            if (ShadowfuryTimer <= diff)
            {
                DoCast(me, SPELL_SHADOW_FURY);
                ShadowfuryTimer = 10000;
            }
			else ShadowfuryTimer -= diff;

            if (DarkstrikeTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (me->IsWithinMeleeRange(me->GetVictim()))
                        DoCastVictim(SPELL_DARK_STRIKE);
                }
                DarkstrikeTimer = 3000;
            }
            else DarkstrikeTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadow_imageAI(creature);
    }
};

void AddSC_boss_eredar_twins()
{
    new boss_lady_sacrolash();
    new boss_grand_warlock_alythess();
    new npc_shadow_image();
}