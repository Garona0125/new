/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

// Need mind control for ghosts

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "black_temple.h"

#define HIGH 10.0f

enum Says
{
	SAY_INTRO                 = -1564037,
	SAY_AGGRO                 = -1564038,
	SAY_SLAY_1                = -1564039,
	SAY_SLAY_2                = -1564040,
	SAY_SPELL_1               = -1564041,
	SAY_SPELL_2               = -1564042,
	SAY_SPECIAL_1             = -1564043,
	SAY_SPECIAL_2             = -1564044,
	SAY_ENRAGE                = -1564045,
	SAY_DEATH                 = -1564046,
};

enum Spells
{
	SPELL_INCINERATE               = 40239,
	SPELL_CRUSHING_SHADOWS         = 40243,
	SPELL_SUMMON_DOOM_BLOSSOM      = 40188,
	SPELL_SHADOWBOLT               = 40185,
	SPELL_PASSIVE_SHADOWFORM       = 40326,
	SPELL_SHADOW_OF_DEATH          = 40251,
	SPELL_BERSERK                  = 45078,
	SPELL_ATROPHY                  = 40327,
	SPELL_SUMMON_SPIRIT            = 40266,
	SPELL_SPIRIT_VENGEANCE         = 40268,
};

class npc_doom_blossom : public CreatureScript
{
public:
    npc_doom_blossom() : CreatureScript("npc_doom_blossom") { }

    struct  npc_doom_blossomAI : public QuantumBasicAI
    {
        npc_doom_blossomAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CheckTeronTimer;
        uint32 ShadowBoltTimer;
        uint64 TeronGUID;

        void Reset()
        {
            CheckTeronTimer = 10000;
            ShadowBoltTimer = 2000;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            TeronGUID = 0;
            TeronGUID = instance->GetData64(DATA_TERONGOREFIEND);
            Creature* TeronGorefiend = NULL;
            if (TeronGUID!= 0)
                TeronGorefiend = Creature::GetCreature(*me,TeronGUID);

            if (TeronGorefiend)
            {
                me->SetCurrentFaction(TeronGorefiend->GetFaction());
                if (TeronGorefiend->GetVictim())
                {
                    me->Attack(TeronGorefiend->GetVictim(), false);
                    DoZoneInCombat();

                    float x, y, z;
                    me->GetPosition(x, y, z);
                    me->GetMotionMaster()->MovePoint(0, x, y, z+HIGH);
                }
				else
                    Despawn();
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void Despawn()
        {
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            me->RemoveCorpse();
            return;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTeronTimer <= diff)
            {
                if (TeronGUID)
                {
                    DoZoneInCombat();

                    Creature* Teron = Unit::GetCreature(*me, TeronGUID);
                    if ((!Teron->IsAlive() || !Teron->IsInCombatActive()))
                    {
                        Despawn();
                        return;
                    }
                }
                else
                {
                    Despawn();
                    return;
                }

                CheckTeronTimer = 5000;
            }
			else CheckTeronTimer -= diff;

            if (ShadowBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
				{
                    DoCast(target, SPELL_SHADOWBOLT);
					ShadowBoltTimer = 2000;
				}
            }
			else ShadowBoltTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_doom_blossomAI(creature);
    }
};

class npc_shadowy_construct : public CreatureScript
{
public:
    npc_shadowy_construct() : CreatureScript("npc_shadowy_construct") { }

    struct  npc_shadowy_constructAI : public QuantumBasicAI
    {
        npc_shadowy_constructAI(Creature* creature) : QuantumBasicAI(creature)
        {
             instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint64 GhostGUID;
        uint64 TeronGUID;

        uint32 CheckPlayerTimer;
        uint32 CheckTeronTimer;
        uint32 resetTimer;

        void Reset()
        {
            GhostGUID = 0;
            TeronGUID = 0;

            CheckPlayerTimer = 2000;
            CheckTeronTimer = 5000;
            resetTimer = 10000;

            me->CastSpell(me,40334,true);
        }

		void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* who)
        {
            //if (!who || (!who->IsAlive()) || (who->GetGUID() == GhostGUID))
            //    return;

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (resetTimer <= diff)
            {
                if (rand()%3 == 0)
                {
                    DoResetThreat();
                    if (Unit* target = SelectTarget(TARGET_FARTHEST, 1, 300, true))
                    {
                        me->Attack(target, true);
                        me->getThreatManager().addThreat(target, 5000.0f);
                    }
                }
                resetTimer = 10000;
            }
			else resetTimer -= diff;

            if (CheckPlayerTimer <= diff)
            {
                DoCastVictim(SPELL_ATROPHY);
                CheckPlayerTimer = 3000;
            }
			else CheckPlayerTimer -= diff;

            if (CheckTeronTimer <= diff)
            {
                Creature* Teron = (Unit::GetCreature(*me, TeronGUID));
            
                if (instance->GetData(DATA_TERONGOREFIENDEVENT) != IN_PROGRESS)
                {
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return;
                }
                DoResetThreat();
                CheckTeronTimer = 5000;
            }
			else CheckTeronTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowy_constructAI(creature);
    }
};

class boss_teron_gorefiend : public CreatureScript
{
public:
    boss_teron_gorefiend() : CreatureScript("boss_teron_gorefiend") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_teron_gorefiendAI(creature);
    }

    struct  boss_teron_gorefiendAI : public QuantumBasicAI
    {
        boss_teron_gorefiendAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 IncinerateTimer;
        uint32 SummonDoomBlossomTimer;
        uint32 EnrageTimer;
        uint32 CrushingShadowsTimer;
        uint32 ShadowOfDeathTimer;
        uint32 SummonShadowsTimer;
        uint32 RandomYellTimer;
        uint32 AggroTimer;
        uint32 pulseTimer;

        uint64 AggroTargetGUID;
        uint64 GhostGUID;                                       // Player that gets killed by Shadow of Death and gets turned into a ghost

        std::list<uint64> shadowtargets;

        bool Intro;

        void Reset()
        {
			instance->SetData(DATA_TERONGOREFIENDEVENT, NOT_STARTED);

            IncinerateTimer = urand(20000, 31000);
            SummonDoomBlossomTimer = 12000;
            EnrageTimer = 600000;
            CrushingShadowsTimer = 22000;
            SummonShadowsTimer = 1000;
            ShadowOfDeathTimer = 30000;
            RandomYellTimer = 50000;

            shadowtargets.clear();

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            // Start off unattackable so that the intro is done properly
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            AggroTimer = 20000;
            AggroTargetGUID = 0;
            Intro = false;

            pulseTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(DATA_TERONGOREFIENDEVENT, IN_PROGRESS);
        }

        void MoveInLineOfSight(Unit *who)
        {
            if (!who || (!who->IsAlive()))
                return;

            if (who->IsTargetableForAttack() && who->IsInAccessiblePlaceFor(me) && me->IsHostileTo(who))
            {
                float attackRadius = me->GetAttackDistance(who);

                if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && me->IsWithinLOSInMap(who))
                {
                    //if (who->HasStealthAura())
                    //    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                    me->AddThreat(who, 1.0f);
                }

                if (!me->IsInCombatActive() && !Intro && me->IsWithinDistInMap(who, 30.0f) && (who->GetTypeId() == TYPE_ID_PLAYER))
                {
					instance->SetData(DATA_TERONGOREFIENDEVENT, IN_PROGRESS);

                    me->GetMotionMaster()->Clear(false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    DoSendQuantumText(SAY_INTRO, me);
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
                    AggroTargetGUID = who->GetGUID();
                    Intro = true;
                    return;
                }
            }

            if (!Intro)
                QuantumBasicAI::MoveInLineOfSight(who);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_TERONGOREFIENDEVENT, DONE);

            DoSendQuantumText(SAY_DEATH, me);
        }

        float CalculateRandomLocation(float Loc, uint32 radius)
        {
            float coord = Loc;
            switch(rand()%2)
            {
                case 0:
                    coord += rand()%radius;
                    break;
                case 1:
                    coord -= rand()%radius;
                    break;
            }
            return coord;
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id != SPELL_SHADOW_OF_DEATH)
                return;

            if (target->GetTypeId() == TYPE_ID_PLAYER)
                shadowtargets.push_back(target->GetGUID());
        }

        void UpdateAI(const uint32 diff)
        {
            if (Intro)
            {
                if (AggroTimer <= diff)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    DoSendQuantumText(SAY_AGGRO, me);
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
                    Intro = false;
                    if (AggroTargetGUID)
                    {
                        Unit* unit = Unit::GetUnit(*me, AggroTargetGUID);
                        if (unit)
                            AttackStart(unit);

                        DoZoneInCombat();
                    }
                    else
                    {
                        EnterEvadeMode();
                        return;
                    }
                }
				else AggroTimer -= diff;
            }

            if (!UpdateVictim() || Intro)
                return;

            if (pulseTimer <= diff)
            {
                DoAttackerAreaInCombat(me->GetVictim(),50);
                pulseTimer = 10000;
            }
			else pulseTimer -= diff;

            if (SummonDoomBlossomTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_SUMMON_DOOM_BLOSSOM);
					SummonDoomBlossomTimer = 35000;
                }
            }
			else SummonDoomBlossomTimer -= diff;

            if (IncinerateTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 300, true))
                {
					DoCast(target, SPELL_INCINERATE);
					DoSendQuantumText(RAND(SAY_SPECIAL_1, SAY_SPECIAL_2), me);
					IncinerateTimer = 10000 + (rand()%10) * 1000;
                }
            }
			else IncinerateTimer -= diff;

            if (CrushingShadowsTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    SpellInfo *spellInfo = (SpellInfo*)sSpellMgr->GetSpellInfo(SPELL_CRUSHING_SHADOWS);
                    if (spellInfo)
                    {
                        spellInfo->MaxAffectedTargets = 5;
                        me->CastSpell(me,spellInfo,false);
                    }
                    CrushingShadowsTimer = 10000 + rand()%5000;
                }
            }
			else CrushingShadowsTimer -= diff;

            if (ShadowOfDeathTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 1, 300, true);

                    if (!target)
                       target = me->GetVictim();

                    if (target && target->IsAlive() && target->GetTypeId() == TYPE_ID_PLAYER && !target->HasAura(SPELL_SHADOW_OF_DEATH, 1))
                    {
                        // Custom ... becaus Effect is not needed
                        SpellInfo *spellInfo = (SpellInfo*)sSpellMgr->GetSpellInfo(SPELL_SHADOW_OF_DEATH);
                        if (spellInfo)
                        {
                            spellInfo->Effects[0].Effect = 0;
                            me->CastSpell(target, spellInfo, false);
                        }

                        //DoCast(target, SPELL_SHADOW_OF_DEATH);
                        //me->CastCustomSpell(target, SPELL_SHADOW_OF_DEATH, 0, 0, 0, false);
                        /*GhostGUID = target->GetGUID();*/
                        ShadowOfDeathTimer = 30000;
						//SummonShadowsTimer = 53000; // Make it VERY close but slightly less so that we can check if the aura is still on the player
                    }
                }
            }
			else ShadowOfDeathTimer -= diff;

            if (SummonShadowsTimer <= diff)
            {
                if (!shadowtargets.empty())
                {

                    std::list<uint64> del_player;
                    del_player.clear();
                    for(std::list<uint64>::iterator itr = shadowtargets.begin(); itr != shadowtargets.end(); ++itr)
                    {
                        Player* temp = Unit::GetPlayer(*me, *itr);
                        if (temp)
                        {
                            if (temp->IsDead() || !temp->HasAuraEffect(SPELL_SHADOW_OF_DEATH, 1))
                            {
                                float x,y,z;
                                temp->GetPosition(x,y,z);
                                Unit* target;
                                Creature* creature;

                                creature = me->SummonCreature(NPC_SHADOWY_CONSTRUCT, x+3 ,y+3 , z , 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 0);
                                target = SelectTarget(TARGET_RANDOM, 0);
                                if (!target)
									target = me->GetVictim();
                                if (target)
									creature->Attack(target, true);

                                creature = me->SummonCreature(NPC_SHADOWY_CONSTRUCT, x-3 ,y+3 , z , 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 0);
                                target = SelectTarget(TARGET_RANDOM, 0);
                                if (!target)
									target = me->GetVictim();
                                if (target)
									creature->Attack(target, true);

                                creature = me->SummonCreature(NPC_SHADOWY_CONSTRUCT, x+3 ,y-3 , z , 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 0);
                                target = SelectTarget(TARGET_RANDOM, 0);
                                if (!target)
									target = me->GetVictim();
                                if (target)
									creature->Attack(target, true);

                                creature = me->SummonCreature(NPC_SHADOWY_CONSTRUCT, x-3 ,y-3 , z , 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 0);
                                target = SelectTarget(TARGET_RANDOM, 0);
                                if (!target)
									target = me->GetVictim();
                                if (target)
									creature->Attack(target, true);

                                del_player.push_back(*itr);
                            }
                        }
                    }

                    for (std::list<uint64>::iterator itr = del_player.begin(); itr != del_player.end(); ++itr)
                    {
                        shadowtargets.remove(*itr);
                        Player* sacrifice = Unit::GetPlayer(*me, *itr);

                        if (sacrifice && sacrifice->IsAlive())
                            me->DealDamage(sacrifice, sacrifice->GetHealth());
                    }
                }
                SummonShadowsTimer = 1000;
            }
			else SummonShadowsTimer -= diff;

			if (RandomYellTimer <= diff)
            {
				DoSendQuantumText(RAND(SAY_SPELL_1, SAY_SPELL_2), me);
				RandomYellTimer = urand(50, 101) * 1000;
            }
			else RandomYellTimer -= diff;

            if (!me->HasAura(SPELL_BERSERK))
            {
				if (EnrageTimer <= diff)
				{
					DoCast(me, SPELL_BERSERK);
					DoSendQuantumText(SAY_ENRAGE, me);
				}
				else EnrageTimer -= diff;
			}

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_teron_gorefiend()
{
    new npc_doom_blossom();
    new npc_shadowy_construct();
    new boss_teron_gorefiend();
}