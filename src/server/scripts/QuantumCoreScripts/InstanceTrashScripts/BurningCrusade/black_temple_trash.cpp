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
#include "../../../scripts/Outland/BlackTemple/black_temple.h"

enum Factions
{
	FACTION_ASHTONGUE_DEATHSWORN_FRIENDLY = 1820,
	FACTION_ASHTONGUE_DEATHSWORN_HATED    = 1813,
};

struct npc_ashtongue_deathsworn_friendAI : public QuantumBasicAI
{
    npc_ashtongue_deathsworn_friendAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    uint32 FriendlyCheckTimer;

    void Reset()
    {
        FriendlyCheckTimer = 5000;

        if (AkamasShemenEventFinished())
        {
            if (me->GetFaction() != FACTION_ASHTONGUE_DEATHSWORN_FRIENDLY)
                me->SetCurrentFaction(FACTION_ASHTONGUE_DEATHSWORN_FRIENDLY);
        }
		else
        {
            if (me->GetFaction() != FACTION_ASHTONGUE_DEATHSWORN_HATED)
                me->SetCurrentFaction(FACTION_ASHTONGUE_DEATHSWORN_HATED);
        }

        QuantumBasicAI::Reset();
    }

    bool AkamasShemenEventFinished()
    {
        if (instance)
           return (instance->GetData(DATA_SHADEOFAKAMAEVENT) == DONE);
        return false;
    }

    void CheckAkamaEvent(const uint32 diff)
    {
        if (me->GetFaction() != FACTION_ASHTONGUE_DEATHSWORN_FRIENDLY)
        {
            if (FriendlyCheckTimer)
            {
                if (AkamasShemenEventFinished())
                {
                    me->SetCurrentFaction(FACTION_ASHTONGUE_DEATHSWORN_FRIENDLY);

                    if (me->IsInCombatActive())
						me->AI()->EnterEvadeMode();
                }
                FriendlyCheckTimer = 5000;
            }
			else FriendlyCheckTimer -=diff;
        }
    }
};

enum Spells
{
	SPELL_CRASHING_WAVE                    = 40100,
	SPELL_VILE_SLIME                       = 40099,
	SPELL_SLUDGE_NOVA                      = 40102,
	SPELL_MERGE                            = 40106,
	SPELL_FREE_FRIEND                      = 40081,
	SPELL_BOOMING_VOICE                    = 40080,
	SPELL_LIGHTNING_PROD                   = 40066,
	SPELL_ELECTRIC_SPUR                    = 40076,
	SPELL_TAIL_SWEEP                       = 40077,
	SPELL_POISON_SPIT                      = 40078,
	SPELL_DEBILITATIN_SPRAY                = 40079,
	SPELL_FORKED_LIGHTNING                 = 40088,
	SPELL_HURRICANE                        = 40090,
	SPELL_SUMMON_GEYSER                    = 40091,
	SPELL_GEYSER_DAMAGE                    = 40089,
	SPELL_HOLY_NOVA                        = 40096,
	SPELL_RESTORATION                      = 40097,
	SPELL_SHELL_SHIELD                     = 40087,
	SPELL_WATER_SPIT                       = 40086,
	SPELL_HARPOONERS_MARK                  = 40084,
	SPELL_HOOKED_NET                       = 40082,
	SPELL_SPEAR_THROW                      = 40083,
	SPELL_DISGRUNTLED                      = 40851,
	SPELL_FURY                             = 40845,
	SPELL_THROW_PICK                       = 40844,
	SPELL_SKY_STALKER_SHOOT                = 40873,
	SPELL_IMMOLATION_ARROW                 = 40872,
	SPELL_DOOMBOLT                         = 40876,
	SPELL_WIND_REAVER_FIREBOLT             = 40877,
	SPELL_CLEAVE                           = 15284,
	SPELL_FREEZE                           = 40875,
	SPELL_FIXATE                           = 40892,
	SPELL_JAB                              = 40895,
	SPELL_ILLIDARI_FLAMES                  = 40938,
	SPELL_ILLIDARI_FLAMES_TRIGGER          = 40939,
	SPELL_RAIN_OF_CHAOS                    = 40946,
	SPELL_RAIN_OF_CHAOS_TRIGGER            = 40948,
	SPELL_WAR_STOMP                        = 40936,
	SPELL_ILLI_CURSE_OF_MENDING            = 39647,
	SPELL_ILLI_FEAR                        = 41150,
	SPELL_ILLI_SHADOW_INFERNO              = 39645,
	SPELL_ILLI_SHADOW_INFERNO_DMG          = 39646,
	SPELL_ILLI_SUMMON_SHADOWFIEND_OPTIC    = 39649,
	SPELL_ILLI_SUMMON_SHADOWFIEND_EFFECT   = 41159,
	SPELL_CLOAK_OF_SHADOW                  = 39666,
	SPELL_ILLI_GOUGE                       = 24698,
	SPELL_SHADOWSTEP                       = 41176,
	SPELL_CONCUSSION_BLOW                  = 32588,
	SPELL_CONCUSSIVE_THROW                 = 41182,
	SPELL_FRENZY                           = 34970,
	SPELL_CHARGE_RAGE                      = 39575,
	SPELL_SPIRIT_BOND                      = 39578,
	SPELL_PRIMALIST_SHOOT                  = 41188,
	SPELL_MULTI_SHOOT                      = 41187,
	SPELL_WYVERN_STING                     = 41186,
	SPELL_SWEEPING_WING_CLIP               = 39584,
	SPELL_SUMMON_FERAL_SPIRIT              = 39535,
	SPELL_BLOODLUST                        = 41185,
	SPELL_CHAIN_HEAL                       = 41114,
	SPELL_FLAME_SHOCK                      = 41115,
	SPELL_FROST_SHOCK                      = 41116,
	SPELL_CYCLONE_TOTEM                    = 39589,
	SPELL_SEARING_TOTEM                    = 39588,
	SPELL_WINDFURY_TOTEM                   = 39586,
	SPELL_CYCLONE_TOTEM_EFFECT             = 39594,
	SPELL_SEARING_TOTEM_ATTACK             = 39593,
	SPELL_STEALTH                          = 34189,
	SPELL_BLIND                            = 34654,
	SPELL_MIND_NUMBING_POISON              = 41190,
	SPELL_INSTANT_POISON                   = 41189,
	SPELL_LIGHTNING_BOLT                   = 41184,
	SPELL_CHAIN_LIGHTNING                  = 41183,
	SPELL_LIGHTNING_SHIELD                 = 41151,
	SPELL_SUMMON_STORM_FURY                = 39534,
	SPELL_STORM_BLINK_OPTICAL              = 39581,
	SPELL_STORM_BLINK_DAMAGE               = 39582,
	SPELL_SONIC_STRIKE                     = 41168,
	SPELL_BANISH                           = 39674,
	SPELL_CURSE_OF_AGONY                   = 39672,
	SPELL_FEL_IMMOLATE                     = 39670,
	SPELL_RAIN_OF_CHAOS_ILLIDARI_DEFILER   = 39671,
	SPELL_SKELETON_SHOT                    = 41171,
	SPELL_SUMMON_SKELETON                  = 41174,
	SPELL_RAPID_SHOT                       = 41173,
	SPELL_CURSE_OF_THE_BLEAKHEART          = 41170,
	SPELL_CURSE_OF_THE_BLEAKHEART_PROC     = 41356,
	SPELL_ILLIDARI_HEARTSEEKER_SHOT        = 41169,
	SPELL_DEMENTIA                         = 41404,
	SPELL_CONFUSION                        = 41397,
	SPELL_DEMENTIA_WHIRLWIND               = 41399,
	SPELL_CURSE_OF_VITALITY                = 41351,
	SPELL_SHARED_BONDS_SCRIPT              = 41362,
	SPELL_SHARED_BONDS_EFFECT              = 41363,
	SPELL_LASH_OF_PAIN                     = 41353,
	SPELL_PAINFULL_RAGE                    = 41369,
	SPELL_SIS_SHADOW_WORD_PAIN             = 41355,
	SPELL_SHELL_OF_PAIN                    = 41371,
	SPELL_SIS_GREATER_HEAL                 = 41378,
	SPELL_SIS_HOLY_NOVA                    = 41380,
	SPELL_SHELL_OF_LIFE                    = 41381,
	SPELL_LOVE_TRAP                        = 41338,
	SPELL_CONC_POLYMORPH                   = 41334,
	SPELL_INFATUATION                      = 41345,
	SPELL_COURT_POISONOUS_THROW            = 41346,
	SPELL_KIDNEY_SHOT                      = 41389,
	SPELL_UPPERCUT                         = 41388,
	SPELL_ATTENDANT_KICK                   = 41395,
	SPELL_ATTENDANT_SLEEP                  = 41396,
	SPELL_SOUL_BLAST                       = 41245,
	SPELL_SOUL_ANGER                       = 41986,
	SPELL_CONSUMING_STRIKES                = 41248,
	SPELL_ILLI_BLIZZARD                    = 41382,
	SPELL_ILLI_FIREBALL                    = 41383,
	SPELL_ILLI_FLAMESTRIKE                 = 41379,
	SPELL_ILLI_FROSTBOLT                   = 41384,
	SPELL_ILLI_DIVINE_SHIELD               = 41367,
	SPELL_ILLI_HAMMER_OF_JUSTICE           = 13005,
	SPELL_ILLI_JUDGEMENT                   = 41368,
	SPELL_ILLI_VANISH                      = 39667,
	SPELL_ILLI_PARALYZING_POISON           = 3609,
	SPELL_ILLI_HEAL                        = 41372,
	SPELL_ILLI_HOLY_SMITE                  = 41370,
	SPELL_ILLI_SHIELD                      = 41373,
	SPELL_ILLI_SHADOW_FORM                 = 29406,
	SPELL_ILLI_MIND_BLAST                  = 41374,
	SPELL_ILLI_SHADOW_WORD_DEATH           = 41375,
	SPELL_L1_ARCANE_CHARGE                 = 41357,
	SPELL_L4_ARCANE_CHARGE                 = 41348,
	SPELL_L4_ARCANE_CHARGE_DMG             = 41349,
	SPELL_L5_ARCANE_CHARGE                 = 41360,
	SPELL_HAND_ENRAGE                      = 38166,
	SPELL_ATROPHY                          = 40327,
	SPELL_BLOOD_BOLT_SCRIPT                = 41072,
	SPELL_BLOOD_BOLT_EFFECT                = 41065,
	SPELL_BLOOD_SIPHON                     = 41068,
	SPELL_SOLDIER_STRIKE                   = 11976,
	SPELL_ABSORPTION                       = 41034,
	SPELL_CHAOTIC_CHARGE                   = 41033,
	SPELL_CHAOTIC_CHARGE_DMG               = 41039,
	SPELL_SM_DEATH_COIL                    = 41070,
	SPELL_SM_DEMON_ARMOR                   = 13787,
	SPELL_SM_SHADOW_BOLT                   = 41069,
	SPELL_SM_RAISE_DEATH                   = 41071,
	SPELL_SM_SUMMON_SKELETON               = 41074,
	SPELL_SM_WHIRLING_WEAPON               = 41053,
	SPELL_SM_CHAOTIC_LIGHT                 = 41063,
	SPELL_WHIRLING                         = 41056,
	SPELL_SMHM_FREEZING_TRAP               = 41085,
	SPELL_SMHM_SHOOT                       = 41093,
	SPELL_SMHM_SILENCING_SHOT              = 41084,
	SPELL_SMHM_RIDING_HOUND                = 39906,
	SPELL_SMHM_VOLLEY                      = 41091,
	SPELL_SMHM_WING_CLIP                   = 32908,
	SPELL_SMRH_CHARGE                      = 25821,
	SPELL_SMRH_CARNIVOROUS_BITE            = 41092,
	SPELL_SMRH_ENRAGE                      = 8599,
	SPELL_SMWM_BERSERKER_STANCE            = 41100,
	SPELL_SMWM_BERSERKER_AURA              = 41107,
	SPELL_SMWM_WHIRLWIND                   = 41097,
	SPELL_SMWM_MULTILATE                   = 41103,
	SPELL_SMWM_BATTLE_STANCE               = 41099,
	SPELL_SMWM_BATTLE_AURA                 = 41106,
	SPELL_SMWM_KNOCK_AWAY                  = 18813,
	SPELL_SMWM_DEFENSIVE_STANCE            = 41101,
	SPELL_SMWM_DEFENSIVE_AURA              = 41105,
	SPELL_SMWM_SHIELD_WALL                 = 41104,
	SPELL_BEHEMOTH_CHARGE                  = 41272,
	SPELL_BEHEMOTH_FEL_STOMP               = 41274,
	SPELL_BEHEMOTH_FIERY_COMET             = 41277,
	SPELL_BEHEMOTH_METEOR                  = 41276,
	SPELL_BEHEMOTH_FRENZY                  = 8269,
	SPELL_INTERVENE                        = 41198,
	SPELL_BCSD_SHIELD_BASH                 = 41197,
	SPELL_BCSD_SHIELD_WALL                 = 41196,
	SPELL_BCSD_SHIELD_THROW                = 41213,
	SPELL_BCBF_WHIRLWIND                   = 41194,
	SPELL_BCBP_BLOOD_DRAIN                 = 41238,
	SPELL_BCBP_BLOOD_BOLT                  = 41229,
	SPELL_BCBP_PROPHECY_BLOOD_1            = 41230,
	SPELL_BCBP_PROPHECY_BLOOD_2            = 41231,
	SPELL_BCBP_FRENZY                      = 8269,
	SPELL_CLOUD_OF_DISEASE                 = 41193,
	SPELL_DISEASE_CLOUD                    = 41290,
	SPELL_BCB_ENRAGE                       = 41254,
	SPELL_BCB_FRENZY                       = 8269,
	SPELL_BCC_ENRAGE                       = 41251,
	SPELL_BCC_FRENZY                       = 8269,
	SPELL_BCSP_CHARGE                      = 36140,
	SPELL_BCSP_CLEAVE                      = 40505,
	SPELL_BCSP_FRENZY                      = 8269,
	SPELL_BCSP_MORTAL_WOUND                = 25646,
	SPELL_BCSP_STRIKE                      = 13446,
	SPELL_BCSP_SUNDER_ARMOR                = 13444,
};

enum WarriorStance
{
    BATTLE_STANCE    = 0,
    BERSERKER_STANCE = 1,
    DEFENSIVE_STANCE = 2,
};

class npc_aqueous_lord : public CreatureScript
{
public:
	npc_aqueous_lord() : CreatureScript("npc_aqueous_lord") { }

	struct npc_aqueous_lordAI : public QuantumBasicAI
    {
        npc_aqueous_lordAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WaveTimer;
        uint32 SlimeTimer;
        uint32 SpawnTimer;

        void Reset()
        {
			WaveTimer = 15000;
            SlimeTimer = 7000;
            SpawnTimer = 20000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (WaveTimer < diff)
            {
                DoCast(me, SPELL_CRASHING_WAVE);
                WaveTimer = rand()%5000+15000;
            }
			else WaveTimer -= diff;

            if (SlimeTimer < diff)
            {
                DoCastVictim(SPELL_VILE_SLIME);
                SlimeTimer = rand()%5000+25000;
            }
			else SlimeTimer -= diff;

            if (SpawnTimer < diff)
            {
                Creature* creature;

                for (int i = 0; i<2 ; i++)
                {
					creature = DoSpawnCreature(NPC_AQUEOUS_SPAWN, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
					if (creature)
						creature->AI()->AttackStart(me->GetVictim());
                }
                SpawnTimer = 20000;
            }
			else SpawnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_aqueous_lordAI(creature);
	}
};

class npc_aqueous_spawn : public CreatureScript
{
public:
	npc_aqueous_spawn() : CreatureScript("npc_aqueous_spawn") { }

    struct npc_aqueous_spawnAI : public QuantumBasicAI
    {
        npc_aqueous_spawnAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 NovaTimer;
        uint32 MergeTimer;
        uint64 LordGuid;

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->HasAura(SPELL_MERGE))
                QuantumBasicAI::MoveInLineOfSight(who);
        }

        void AttackStart(Unit* who)
        {
            if (!me->HasAura(SPELL_MERGE))
                QuantumBasicAI::AttackStart(who);
        }

        void Reset()
        {
            NovaTimer = 5000 + rand()%20000;
            MergeTimer = rand()%50000;
            LordGuid = 0;
        }

		void EnterToBattle(Unit* /*who*/){}

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void UpdateAI(uint32 const diff)
        {
            if (me->HasAura(SPELL_MERGE, 0))
            {
                if (me->GetUInt64Value(UNIT_FIELD_TARGET) != LordGuid)
                    me->SetUInt64Value(UNIT_FIELD_TARGET, LordGuid);
                return;
            }

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (NovaTimer < diff)
            {
                DoCast(me, SPELL_SLUDGE_NOVA);
                NovaTimer = rand()%10000+10000;
            }
			else NovaTimer -= diff;

            if (MergeTimer < diff)
            {
                Creature* lord = FindCreatureWithDistance(NPC_AQUEOUS_LORD, 30);
                if (lord)
                {
                    if (rand()%2 == 0)
                    {
                        me->SetUInt64Value(UNIT_FIELD_TARGET, lord->GetGUID());
                        LordGuid = lord->GetGUID();
                        me->CastSpell(me,SPELL_MERGE, true);
                    }
                    //DoCast(spawn, SPELL_MERGE, true);
                }
                MergeTimer = rand()%20000 + 10000;

            }
			else MergeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_aqueous_spawnAI(creature);
	}
};

class npc_coilskar_general : public CreatureScript
{
public:
	npc_coilskar_general() : CreatureScript("npc_coilskar_general") { }

    struct npc_coilskar_generalAI : public QuantumBasicAI
    {
        npc_coilskar_generalAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FreeTimer;
        uint32 BoomingTimer;

        void Reset()
        {
            FreeTimer = 10000;
            BoomingTimer = 0;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FreeTimer < diff)
            {
                DoCast(me, SPELL_FREE_FRIEND);
                FreeTimer = rand()%10000+10000;
                BoomingTimer = 1000;
            }
			else FreeTimer -= diff;

            if (BoomingTimer)
            {
                if (BoomingTimer < diff)
                {
                    DoCast(me, SPELL_BOOMING_VOICE);
                    BoomingTimer = 0;
                }
				else BoomingTimer -= diff;

				DoMeleeAttackIfReady();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_coilskar_generalAI(creature);
	}
};

class npc_coilskar_wrangler : public CreatureScript
{
public:
	npc_coilskar_wrangler() : CreatureScript("npc_coilskar_wrangler") { }

    struct npc_coilskar_wranglerAI : public QuantumBasicAI
    {
        npc_coilskar_wranglerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;
        uint32 LightningTimer;
        uint32 SpurTimer;

        void Reset()
        {
            CleaveTimer = 10000;
            LightningTimer = 10000 + rand()%5000;
            SpurTimer = 20000;
        }

		void EnterToBattle(Unit* /*who*/){}

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CleaveTimer < diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 5000+rand()%10000;
            }
			else CleaveTimer -= diff;

            if (LightningTimer < diff)
            {
                DoCastVictim(SPELL_LIGHTNING_PROD);
                LightningTimer = 15000+rand()%10000;
            }
			else LightningTimer -= diff;

            if (SpurTimer < diff)
            {
                Creature* leviathan = FindCreatureWithDistance(NPC_LEVIATHAN, 30);

                if (leviathan && !leviathan->HasAura(SPELL_ELECTRIC_SPUR, 0))
				{
                    DoCast(leviathan, SPELL_ELECTRIC_SPUR);
				}
                SpurTimer = 30000;
            }
			else SpurTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_coilskar_wranglerAI(creature);
	}
};

class npc_leviathan : public CreatureScript
{
public:
	npc_leviathan() : CreatureScript("npc_leviathan") { }

    struct npc_leviathanAI : public QuantumBasicAI
    {
        npc_leviathanAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SprayTimer;
        uint32 SpitTimer;
        uint32 TailTimer;

        void Reset()
        {
			SprayTimer = 15000;
            SpitTimer = 5000;
            TailTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (TailTimer < diff)
            {
                DoCast(me, SPELL_TAIL_SWEEP, true);
                TailTimer = 5000;
            }
			else TailTimer -= diff;

            if (SprayTimer < diff)
            {
                DoCastVictim(SPELL_DEBILITATIN_SPRAY);
                SprayTimer = 5000+rand()%125000;
            }
			else SprayTimer -= diff;

            if (SpitTimer < diff)
            {
                DoCastVictim(SPELL_POISON_SPIT);
                SpitTimer = 5000+rand()%12500;
            }
			else SpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_leviathanAI(creature);
	}
};

class npc_coilskar_sea_caller : public CreatureScript
{
public:
	npc_coilskar_sea_caller() : CreatureScript("npc_coilskar_sea_caller") { }

    struct npc_coilskar_sea_callerAI : public QuantumBasicAI
    {
        npc_coilskar_sea_callerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 LightningTimer;
        uint32 HurricaneTimer;
        uint32 GeyserTimer;

        void Reset()
        {
			LightningTimer = 10000+rand()%10000;
            GeyserTimer = 10000+rand()%10000;
            HurricaneTimer = 20000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (LightningTimer < diff)
            {
                DoCastVictim(SPELL_FORKED_LIGHTNING);
                LightningTimer = 5000+rand()%10000;
            }
			else LightningTimer -= diff;

            if (HurricaneTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                {
                    DoCast(target, SPELL_HURRICANE);
                    HurricaneTimer = 20000+rand()%10000;
                }
				else HurricaneTimer = 4000;
            }
			else HurricaneTimer -= diff;

            if (GeyserTimer < diff)
            {
				DoCast(me, SPELL_SUMMON_GEYSER);
				GeyserTimer = 10000+rand()%10000;
            }
			else GeyserTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_coilskar_sea_callerAI(creature);
	}
};

class npc_coilskar_geyser : public CreatureScript
{
public:
	npc_coilskar_geyser() : CreatureScript("npc_coilskar_geyser") { }

    struct npc_coilskar_geyserAI : public QuantumBasicAI
    {
        npc_coilskar_geyserAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 GeyserTimer;

        void Reset()
		{
			GeyserTimer = 500;
		}

		void MoveInLineOfSight(Unit* /*who*/){}
        void AttackStart(Unit* /*who*/){}
		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
                return;

            if (GeyserTimer < diff)
            {
                DoCast(me, SPELL_GEYSER_DAMAGE);
                GeyserTimer = 50000;
            }
			else GeyserTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_coilskar_geyserAI(creature);
	}
};

class npc_coilskar_soothsayer : public CreatureScript
{
public:
	npc_coilskar_soothsayer() : CreatureScript("npc_coilskar_soothsayer") { }

	struct npc_coilskar_soothsayerAI : public QuantumBasicAI
    {
        npc_coilskar_soothsayerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 HolyNovaTimer;
        uint32 RestorationTimer;

        void Reset()
        {
            HolyNovaTimer = 10000+rand()%10000;
            RestorationTimer = 10000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HolyNovaTimer < diff)
            {
                DoCast(me, SPELL_HOLY_NOVA);
                HolyNovaTimer = 10000+rand()%10000;
            }
			else HolyNovaTimer -= diff;

            if (RestorationTimer < diff)
            {
                Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW);
                if (!target)
				{
                    target = me;
				}

                bool hasCast = false;

                if (!target->HasAura(SPELL_RESTORATION, 0))
                {
                    DoCast(target, SPELL_RESTORATION);
                    RestorationTimer = 10000+rand()%5000;
                }
				else RestorationTimer = 1000+rand()%5000;

            }
			else RestorationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_coilskar_soothsayerAI(creature);
	}
};

class npc_dragon_turtle : public CreatureScript
{
public:
	npc_dragon_turtle() : CreatureScript("npc_dragon_turtle") { }

    struct npc_dragon_turtleAI : public QuantumBasicAI
    {
        npc_dragon_turtleAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShieldTimer;
        uint32 SpitTimer;
        uint32 CheckTimer;

        uint64 MarkTarget;

        void Reset()
        {
            ShieldTimer = 5000;
            SpitTimer = 10000+rand()%10000;
            MarkTarget = 0;
            CheckTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void CheckForMark()
        {
            if (MarkTarget == 0)
            {
                Map *map = me->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* player = i->getSource())
						{
                            if (player->IsAlive())
                            {
								if (me->HasAura(SPELL_HARPOONERS_MARK, 0))
									MarkTarget = me->GetGUID();
                            }
						}
                    }
                }
            }

            if (MarkTarget != 0)
            {
                Player* target = Unit::GetPlayer(*me, MarkTarget);
                if (target)
                {
                    if (target->HasAura(SPELL_HARPOONERS_MARK, 0))
                    {
                        me->SetInCombatWith(target);
                        target->SetInCombatWith(me);
                        me->AddThreat(target, 50000.0f);
                    }
					else
                    {
						MarkTarget = 0;
						DoModifyThreatPercent(target, 0);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SpitTimer < diff)
            {
                DoCastVictim(SPELL_WATER_SPIT);
                SpitTimer = 10000+rand()%10000;
            }
			else SpitTimer -= diff;

            if (((me->GetHealth()*100) / me->GetMaxHealth()) < 50 )
            {
                if (ShieldTimer < diff)
                {
                    DoCast(me, SPELL_SHELL_SHIELD);
                    ShieldTimer = 20000+rand()%10000;
                }
				else ShieldTimer -= diff;
            }

            if (CheckTimer < diff)
            {
                CheckForMark();
                CheckTimer = 5000;
            }
			else CheckTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dragon_turtleAI(creature);
	}
};

class npc_coilskar_harpooner : public CreatureScript
{
public:
	npc_coilskar_harpooner() : CreatureScript("npc_coilskar_harpooner") { }

    struct npc_coilskar_harpoonerAI : public QuantumBasicAI
    {
        npc_coilskar_harpoonerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShootTimer;
        uint32 NetTimer;
        uint32 CheckTimer;

        bool MarkSet;
        uint64 MarkTarget;

        void Reset()
        {
            ShootTimer = 2500;
            NetTimer = 10000+rand()%10000;
            MarkSet = false;
            CheckTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
            QuantumBasicAI::AttackStartNoMove(who);
        }

        void CheckForMark()
        {
            if (MarkTarget == 0)
            {
                Map *map = me->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* player = i->getSource())
						{
                            if (player->IsAlive())
                            {
                                if (me->HasAura(SPELL_HARPOONERS_MARK, 0))
                                    MarkTarget = me->GetGUID();
							}
						}
					}
                }
            }

            if (MarkTarget != 0)
            {
                Player* target = Unit::GetPlayer(*me,MarkTarget);
                if (target)
                {
                    if (target->HasAura(SPELL_HARPOONERS_MARK, 0))
                    {
                        me->SetInCombatWith(target);
                        target->SetInCombatWith(me);
                        me->AddThreat(target, 50000.0f);
                    }
					else
                    {
                        MarkTarget = 0;
                        DoModifyThreatPercent(target,0);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (NetTimer < diff)
            {
                if (me->IsWithinMeleeRange(me->GetVictim()))
				{
                    DoCastVictim(SPELL_HOOKED_NET);
				}
                NetTimer = 10000+rand()%10000;
            }
			else NetTimer -= diff;

            if (!MarkSet)
			{
                if (((me->GetHealth()*100) / me->GetMaxHealth()) < 50 )
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
						MarkSet = true;
						DoCast(target, SPELL_HARPOONERS_MARK);
					}
				}
			}

            if (CheckTimer < diff)
            {
                CheckForMark();
                CheckTimer = 5000;
            }
			else CheckTimer -= diff;

            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
            else
            {
                if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

                if (ShootTimer < diff)
                {
                    DoCastVictim(SPELL_SPEAR_THROW);
                    ShootTimer = 2500;
                }
				else ShootTimer -= diff;

				DoMeleeAttackIfReady();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_coilskar_harpoonerAI(creature);
	}
};

class npc_bonechewer_taskmaster : public CreatureScript
{
public:
	npc_bonechewer_taskmaster() : CreatureScript("npc_bonechewer_taskmaster") {}

    struct npc_bonechewer_taskmasterAI : public QuantumBasicAI
    {
        npc_bonechewer_taskmasterAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset(){}

        void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DISGRUNTLED);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_taskmasterAI(creature);
	}
};

class npc_bonechewer_worker : public CreatureScript
{
public:
	npc_bonechewer_worker() : CreatureScript("npc_bonechewer_worker") { }

    struct npc_bonechewer_workerAI : public QuantumBasicAI
    {
        npc_bonechewer_workerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ThrowTimer;

        void Reset()
        {
            ThrowTimer = 1000 + rand()%15000;
        }

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

		void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            Creature* taskmaster = FindCreatureWithDistance(NPC_TASKMASTER, 40);
            if (taskmaster)
			{
				taskmaster->CastSpell(taskmaster, SPELL_FURY, false);
			}
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DISGRUNTLED)
			{
                me->SetHealth(me->GetMaxHealth());
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ThrowTimer < diff)
            {
                DoCastVictim(SPELL_THROW_PICK);
                ThrowTimer = 9999999;
            }
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_workerAI(creature);
	}
};

class npc_dragonmaw_sky_stalker : public CreatureScript
{
public:
	npc_dragonmaw_sky_stalker() : CreatureScript("npc_dragonmaw_sky_stalker") { }

    struct npc_dragonmaw_sky_stalkerAI : public QuantumBasicAI
    {
        npc_dragonmaw_sky_stalkerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShootTimer;
        uint32 ArrowTimer;

        void Reset()
        {
            ShootTimer = 2500;
            ArrowTimer = 10000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->Attack(who, true))
                DoStartMovement(who, 30, M_PI/2);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArrowTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_IMMOLATION_ARROW);
					ArrowTimer = 10000+rand()%10000;
				}
            }
			else ArrowTimer -= diff;

            if (ShootTimer < diff)
            {
                DoCastVictim(SPELL_SKY_STALKER_SHOOT);
                ShootTimer = 2500;
            }
			else ShootTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dragonmaw_sky_stalkerAI(creature);
	}
};

class npc_dragonmaw_wind_reaver : public CreatureScript
{
public:
	npc_dragonmaw_wind_reaver() : CreatureScript("npc_dragonmaw_wind_reaver") { }

    struct npc_dragonmaw_wind_reaverAI : public QuantumBasicAI
    {
        npc_dragonmaw_wind_reaverAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DoomBoltTimer;
        uint32 FireBoltTimer;
        uint32 FreezeTimer;

        void Reset()
        {
            FireBoltTimer = 3000;
            DoomBoltTimer = 5000+rand()%10000;
            FreezeTimer = 5000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
			if (!who)
				return;

            if (me->Attack(who, true))
                DoStartMovement(who, 30, me->GetAngle(who));
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FreezeTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (me->IsWithinMeleeRange(me->GetVictim()))
					{
                        DoCastVictim(SPELL_FREEZE);
						FreezeTimer = 10000+rand()%10000;
					}
				}
            }
			else FreezeTimer -= diff;

            if (DoomBoltTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_DOOMBOLT);
						DoomBoltTimer = 10000+rand()%10000;
					}
                }
            }
			else DoomBoltTimer -= diff;

            if (FireBoltTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCastVictim(SPELL_WIND_REAVER_FIREBOLT);
                    FireBoltTimer = 3000;
                }
            }
			else FireBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dragonmaw_wind_reaverAI(creature);
	}
};

class npc_dragonmaw_wyrmcaller : public CreatureScript
{
public:
	npc_dragonmaw_wyrmcaller() : CreatureScript("npc_dragonmaw_wyrmcaller") { }

    struct npc_dragonmaw_wyrmcallerAI : public QuantumBasicAI
    {
        npc_dragonmaw_wyrmcallerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;
        uint32 FixateTimer;
        uint32 CheckTimer;
        uint32 JabTimer;

        bool FixateSet;
        uint64 FixateTarget;

        void Reset()
        {
            CleaveTimer = 10000;
            FixateTimer = 20000;
            JabTimer = 5000;

            FixateTarget = 0;
            CheckTimer = 5000;

            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
        }

		void EnterToBattle(Unit* /*who*/){}

        void CheckForMark()
        {
            if (FixateTarget == 0)
            {
                Map *map = me->GetMap();
                if (map->IsDungeon())
                {
					Map::PlayerList const &PlayerList = map->GetPlayers();
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* player = i->getSource())
						{
                            if (player->IsAlive())
                            {
                                if (me->HasAura(SPELL_FIXATE,0))
                                    FixateTarget = me->GetGUID();
                            }
						}
                    }
                }
            }

            if (FixateTarget != 0)
            {
                Player* target = Unit::GetPlayer(*me,FixateTarget);
                if (target)
                {
                    if (target->HasAura(SPELL_FIXATE,0))
                    {
                        me->SetInCombatWith(target);
                        target->SetInCombatWith(me);
                        me->AddThreat(target, 50000.0f);
                    }
					else
                    {
                        FixateTarget = 0;
                        DoModifyThreatPercent(target,0);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FixateTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_FIXATE);
					FixateTimer = 30000;
				}
            }
			else FixateTimer -= diff;

            if (CheckTimer < diff)
            {
                CheckForMark();
                CheckTimer = 5000;
            }
			else CheckTimer -= diff;

            if (CleaveTimer < diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 5000 + rand()%5000;
            }
			else CleaveTimer -= diff;

            if (JabTimer < diff)
            {
                DoCastVictim(SPELL_JAB);
                JabTimer = 5000 + rand()%5000;
            }
			else JabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dragonmaw_wyrmcallerAI(creature);
	}
};

class npc_illidari_fearbringer : public CreatureScript
{
public:
	npc_illidari_fearbringer() : CreatureScript("npc_illidari_fearbringer") { }

    struct npc_illidari_fearbringerAI : public QuantumBasicAI
    {
        npc_illidari_fearbringerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FlamesTimer;
        uint32 RainTimer;
        uint32 StompTimer;

		bool IsFlaming;
        bool IsRaining;
        uint32 RainCount;

        uint64 RainTrigger;

        void Reset()
        {
            StompTimer = 5000;
            RainTimer = 6000;
            FlamesTimer = 5000 + rand()%10000;

            IsRaining = false;
            RainCount = 0;
            RainTrigger = 0;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (StompTimer < diff)
            {
                DoCast(me, SPELL_WAR_STOMP);
                StompTimer = 20000 + rand()%10000;
                if (rand()%2 == 0)
				{
                    RainTimer = 1000;
				}
            }
			else StompTimer -= diff;

            if (FlamesTimer < diff)
            {
                if (!IsFlaming)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCastVictim(SPELL_ILLIDARI_FLAMES);
                        FlamesTimer = 2500;
                        IsFlaming = true;
                    }
                }
                else
                {
                    if (!me->HasAura(SPELL_ILLIDARI_FLAMES, 0))
                    {
                        IsFlaming = false;
                        FlamesTimer = 10000 + rand()%10000;
                    }
					else
                    {
                        int32 damage = 2850 +rand()%300;
                        me->CastCustomSpell(me, SPELL_ILLIDARI_FLAMES_TRIGGER, &damage, NULL, NULL, true);
                        FlamesTimer = 1000;
                    }
                }
            }
			else FlamesTimer -= diff;

            if (RainTimer < diff)
            {
                if (IsRaining)
                {
                    if (Creature* CRainTrigger = Creature::GetCreature(*me, RainTrigger))
                    {
                        int32 damage = 3420 +rand()%360;
                        CRainTrigger->CastCustomSpell(CRainTrigger, SPELL_RAIN_OF_CHAOS_TRIGGER, &damage, NULL, NULL, false, 0, 0, me->GetGUID());

                    }
                    if (RainCount > 2)
                    {
                        RainTimer = 20000 + rand()%10000;
                        IsRaining = false;
                    }
					else
                    {
                        RainCount++;
                        RainTimer = 3000;
                    }
                }
				else
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        {
                            DoCast(target, SPELL_RAIN_OF_CHAOS);
                            float x, y, z;
                            target->GetPosition(x, y, z);
                            Creature* trigger = me->SummonTrigger(x, y, z, 0, 10000);
                            RainTrigger = trigger->GetGUID();
                        }

                        RainCount = 0;
                        IsRaining = true;
                        RainTimer = 3000;
                    }
                }
            }
			else RainTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_fearbringerAI(creature);
	}
};

class npc_illidari_nightlord : public CreatureScript
{
public:
	npc_illidari_nightlord() : CreatureScript("npc_illidari_nightlord") { }

    struct npc_illidari_nightlordAI : public QuantumBasicAI
    {
        npc_illidari_nightlordAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CurseTimer;
        uint32 FearTimer;
        uint32 ShadowfiendTimer;
        uint32 InfernoTimer;

        bool Inferno;

        uint32 InfernoTick;

        void Reset()
        {
            FearTimer = 10000+rand()%10000;
            CurseTimer = 10000+rand()%10000;
            ShadowfiendTimer = 30000;

            InfernoTimer = 20000 + rand()%20000;
            Inferno = false;
            InfernoTick = 0;

            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (Inferno)
            {
                if (InfernoTimer < diff)
                {
                    int damage = InfernoTick *1000;
					me->CastCustomSpell(me, SPELL_ILLI_SHADOW_INFERNO_DMG, &damage, NULL, NULL, false, NULL, NULL, me->GetGUID());
					InfernoTick++;
                    InfernoTimer = 1000;
                }
				else InfernoTimer -= diff;

                if (!me->HasAura(SPELL_ILLI_SHADOW_INFERNO, 0))
                {
                    InfernoTick = 0;
                    InfernoTimer = 20000+rand()%10000;
                    Inferno = false;
                }
                return;
            }
			else
            {
                if (CurseTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(me, SPELL_ILLI_CURSE_OF_MENDING);
					}
                    CurseTimer = 10000+rand()%15000;
                }
				else CurseTimer -= diff;

                if (FearTimer < diff)
                {
                    DoCast(me, SPELL_ILLI_FEAR);
                    FearTimer = 10000+rand()%10000;
                }
				else FearTimer -= diff;

                if (ShadowfiendTimer < diff)
                {
                    DoCast(me, SPELL_ILLI_SUMMON_SHADOWFIEND_OPTIC);

                    for (int i = 0 ; i < 5; i++)
                    {
                        DoCast(me, SPELL_ILLI_SUMMON_SHADOWFIEND_EFFECT, true);
                    }
                    ShadowfiendTimer = 20000+rand()%10000;
                }
				else ShadowfiendTimer -= diff;

                if (InfernoTimer < diff)
                {
                    DoCast(me, SPELL_ILLI_SHADOW_INFERNO);
                    Inferno = true;
                    InfernoTimer = 100;
                    InfernoTick = 1;
                    return;
                }
				else InfernoTimer -= diff;
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_nightlordAI(creature);
	}
};

class npc_illidari_boneslicer : public CreatureScript
{
public:
	npc_illidari_boneslicer() : CreatureScript("npc_illidari_boneslicer") { }

    struct npc_illidari_boneslicerAI : public QuantumBasicAI
    {
        npc_illidari_boneslicerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CloakTimer;
        uint32 GougeTimer;
        uint32 StepTimer;

        void Reset()
        {
            CloakTimer = 10000+rand()%10000;
            GougeTimer = 10000+rand()%10000;
            StepTimer = 10000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (StepTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(me, SPELL_SHADOWSTEP);
					StepTimer = 10000+rand()%10000;
				}
            }
			else StepTimer -= diff;

            if (CloakTimer < diff)
            {
                DoCast(me, SPELL_CLOAK_OF_SHADOW);
                CloakTimer = 10000+rand()%10000;
            }
			else CloakTimer -= diff;

            if (GougeTimer < diff)
            {
                DoCastVictim(SPELL_ILLI_GOUGE);
                GougeTimer = 10000+rand()%10000;
            }
			else GougeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_boneslicerAI(creature);
	}
};

class npc_ashtongue_battlelord : public CreatureScript
{
public:
	npc_ashtongue_battlelord() : CreatureScript("npc_ashtongue_battlelord") { }

    struct npc_ashtongue_battlelordAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_ashtongue_battlelordAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 CleaveTimer;
        uint32 BlowTimer;
        uint32 ThrowTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            CleaveTimer = 5000 + rand()%5000;
            BlowTimer = 10000+ rand()%5000;
            ThrowTimer = 12000+ rand()% 5000;
            FrenzyTimer = 20000 - rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CleaveTimer < diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 5000 + rand()%5000;
            }
			else CleaveTimer -= diff;

            if (BlowTimer < diff)
            {
                DoCastVictim(SPELL_CONCUSSION_BLOW);
                BlowTimer = 5000 + rand()%5000;
            }
			else BlowTimer -= diff;

            if (ThrowTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 40)
					{
						DoCast(target, SPELL_CONCUSSIVE_THROW);
						ThrowTimer = 10000 + rand()%10000;
					}
                }
            }
			else ThrowTimer -= diff;

            if (!me->HasAura(SPELL_FRENZY, 0))
			{
                if (FrenzyTimer < diff)
                {
                    DoCast(me, SPELL_FRENZY);
                    FrenzyTimer = 20000 + rand()%10000;
                }
				else FrenzyTimer -= diff;
			}

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_battlelordAI(creature);
	}
};

class npc_ashtongue_feral_spirit : public CreatureScript
{
public:
	npc_ashtongue_feral_spirit() : CreatureScript("npc_ashtongue_feral_spirit") { }

    struct npc_ashtongue_feral_spiritAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_ashtongue_feral_spiritAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 RageTimer;
        uint32 BondTimer;

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void Reset()
        {
            RageTimer = 10000 + rand()%5000;
            BondTimer = 10000+ rand()%10000;

            if (Creature* master = FindCreatureWithDistance(NPC_ASHTONGUE_PRIMALIST, 40))
                master->SetPetGUID(me->GetGUID());
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (RageTimer < diff)
            {
                DoCast(me, SPELL_CHARGE_RAGE);
                RageTimer = 20000 + rand()%20000;
            }
			else RageTimer -= diff;

            if (BondTimer < diff)
            {
				me->CastSpell(me, SPELL_SPIRIT_BOND, true);
                BondTimer = 20000 + rand()%20000;
            }
			else BondTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_feral_spiritAI(creature);
	}
};

class npc_ashtongue_primalist : public CreatureScript
{
public:
	npc_ashtongue_primalist() : CreatureScript("npc_ashtongue_primalist") { }

    struct npc_ashtongue_primalistAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_ashtongue_primalistAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 ShootTimer;
        uint32 StingTimer;
        uint32 ClipTimer;
        uint32 MultiTimer;

        void Reset()
        {
            ShootTimer = 2500;
            StingTimer = 10000+rand()%10000;
            ClipTimer = 10000+rand()%10000;
            MultiTimer = 10000+rand()%10000;
        }

        void AttackStart(Unit* who)
        {
            QuantumBasicAI::AttackStartNoMove(who);
        }

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (Creature* creature = FindCreatureWithDistance(NPC_FERAL_SPIRIT, 30))
			{
				if (!creature || creature->IsDead())
					DoCast(me, SPELL_SUMMON_FERAL_SPIRIT);
			}
        }

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (StingTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 35)
					{
						DoCast(target, SPELL_WYVERN_STING);
						StingTimer = 10000+rand()%10000;
					}
                }
            }
			else StingTimer -= diff;

            if (MultiTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_MULTI_SHOOT);
						MultiTimer = 10000+rand()%10000;
					}
                }
            }
			else MultiTimer -= diff;

            if (ClipTimer < diff)
            {
                if (me->GetDistance2d(me->GetVictim()) < 6)
				{
                    DoCastVictim(SPELL_SWEEPING_WING_CLIP);
					ClipTimer = 10000+rand()%10000;
				}
            }
			else ClipTimer -= diff;

            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
            else
            {
                if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

                if (ShootTimer < diff)
                {
                    DoCastVictim(SPELL_SKY_STALKER_SHOOT);
                    ShootTimer = 2500;
                }
				else ShootTimer -= diff;
			}

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_primalistAI(creature);
	}
};

class npc_ashtongue_mystic : public CreatureScript
{
public:
	npc_ashtongue_mystic() : CreatureScript("npc_ashtongue_mystic") { }

    struct npc_ashtongue_mysticAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_ashtongue_mysticAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 BloodlustTimer;
        uint32 HealTimer;
        uint32 TotemTimer;
        uint32 ShockTimer;

        void Reset()
        {
            BloodlustTimer = 20000 + rand()%20000;
            HealTimer = 10000+ rand()%10000;
            ShockTimer = 10000+ rand()%10000;
            TotemTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BloodlustTimer < diff)
            {
                DoCast(me, SPELL_BLOODLUST);
                BloodlustTimer = 20000 + rand()%20000;
            }
			else BloodlustTimer -= diff;

            if (ShockTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    switch(rand()%2)
                    {
                    case 0:
                        DoCast(target, SPELL_FLAME_SHOCK);
                        break;
                    case 1:
                        DoCast(target, SPELL_FROST_SHOCK);
                        break;
                    }
                }
                ShockTimer = 5000 + rand()%5000;
            }
			else ShockTimer -= diff;

            if (TotemTimer < diff)
            {
                Creature* totem;
                bool TotemSummoned = false;

                if (!TotemSummoned)
                {
                    if (totem = FindCreatureWithDistance(NPC_CYCLONE_TOTEM, 30))
					{
						if (!totem)
							DoCast(me, SPELL_CYCLONE_TOTEM);

						TotemSummoned = true;
						TotemTimer = 2000;
					}
                }

                if (!TotemSummoned)
                {
                    if (totem = FindCreatureWithDistance(NPC_SEARING_TOTEM, 30))
					{
						if (!totem)
							DoCast(me, SPELL_SEARING_TOTEM);

						TotemSummoned = true;
						TotemTimer = 2000;
					}
                }

                if (!TotemSummoned)
                {
                    if (totem = FindCreatureWithDistance(NPC_WINDFURY_TOTEM, 30))
					{
						if (!totem)
							DoCast(me, SPELL_WINDFURY_TOTEM);

						TotemSummoned = true;
						TotemTimer = 2000;
					}
				}

				if (!TotemSummoned)
					TotemTimer = 10000 + rand()%10000;
            }
			else TotemTimer -= diff;

            if (HealTimer < diff)
            {
                if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
				{
					DoCast(target, SPELL_CHAIN_HEAL);
					HealTimer = 10000 + rand()%5000;
				}
            }
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_mysticAI(creature);
	}
};

class npc_ashtongue_cyclone_totem : public CreatureScript
{
public:
	npc_ashtongue_cyclone_totem() : CreatureScript("npc_ashtongue_cyclone_totem") { }

    struct npc_ashtongue_cyclone_totemAI : public NullCreatureAI
    {
        npc_ashtongue_cyclone_totemAI(Creature* creature) : NullCreatureAI(creature) {}

        void Reset()
        {
            SpellInfo *spellInfo = (SpellInfo*)sSpellMgr->GetSpellInfo(SPELL_CYCLONE_TOTEM_EFFECT);

            if (spellInfo)
            {
                spellInfo->MaxAffectedTargets = 5;
                me->CastSpell(me, spellInfo, false);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_cyclone_totemAI(creature);
	}
};

class npc_ashtongue_searing_totem : public CreatureScript
{
public:
	npc_ashtongue_searing_totem() : CreatureScript("npc_ashtongue_searing_totem") { }

    struct npc_ashtongue_searing_totemAI : public NullCreatureAI
    {
        npc_ashtongue_searing_totemAI(Creature* creature) : NullCreatureAI(creature) {}

        uint32 AttackTimer;

        void Reset()
        {
			AttackTimer = 100;
        }

		void EnterToBattle(Unit* /*who*/){}
    
        void UpdateAI(uint32 const diff)
        {
            if (AttackTimer < diff)
            {
                DoCast(me, SPELL_SEARING_TOTEM_ATTACK);
                AttackTimer = 2500;
            }
			else AttackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_searing_totemAI(creature);
	}
};

class npc_ashtongue_stalker : public CreatureScript
{
public:
	npc_ashtongue_stalker() : CreatureScript("npc_ashtongue_stalker") { }

    struct npc_ashtongue_stalkerAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_ashtongue_stalkerAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 BlindTimer;
        uint32 MPoisonTimer;
        uint32 IPoisonTimer;

        void Reset()
        {
            BlindTimer = 10000 + rand()%5000;
            MPoisonTimer = 5000+ rand()%10000;
            IPoisonTimer = 5000+ rand()%10000;
            DoCast(me, SPELL_STEALTH);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->RemoveAurasDueToSpell(SPELL_STEALTH);
        }

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BlindTimer < diff)
            {
                DoCastVictim(SPELL_BLIND);
                DoModifyThreatPercent(me->GetVictim(), 50);
                BlindTimer = 15000 + rand()%10000;
            }
			else BlindTimer -= diff;

            if (MPoisonTimer < diff)
            {
                if (me->GetVictim()->GetPowerType() == POWER_MANA)
                {
                    DoCastVictim(SPELL_MIND_NUMBING_POISON);
					MPoisonTimer = 15000 + rand()%10000;
				}
            }
			else MPoisonTimer -= diff;

            if (IPoisonTimer < diff)
            {
                DoCastVictim(SPELL_INSTANT_POISON);
                IPoisonTimer = 10000 + rand()%5000;
            }
			else IPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_stalkerAI(creature);
	}
};

class npc_ashtongue_stormcaller : public CreatureScript
{
public:
	npc_ashtongue_stormcaller() : CreatureScript("npc_ashtongue_stormcaller") { }

    struct npc_ashtongue_stormcallerAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_ashtongue_stormcallerAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 BoltTimer;
        uint32 ChainTimer;
        uint32 ShieldTimer;

        void Reset()
        {
            BoltTimer = 2500;
            ChainTimer = 10000+rand()%10000;
            ShieldTimer = 1000;
        }

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (Creature* creature = FindCreatureWithDistance(NPC_STORM_FURY, 30))
			{
				if (!creature || creature->IsDead())
					DoCast(me, SPELL_SUMMON_STORM_FURY);
			}
        }

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShieldTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCast(me, SPELL_LIGHTNING_SHIELD);
                    ShieldTimer = 30000;
                }
            }
			else ShieldTimer -= diff;

            if (ChainTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCastVictim(SPELL_CHAIN_LIGHTNING);
                    ChainTimer = 10000+rand()%10000;
                }
            }
			else ChainTimer -= diff;

            if (BoltTimer < diff)
            {
                DoCastVictim(SPELL_LIGHTNING_BOLT);
                BoltTimer = 2500;
            }
			else BoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ashtongue_stormcallerAI(creature);
	}
};

class npc_storm_fury : public CreatureScript
{
public:
	npc_storm_fury() : CreatureScript("npc_storm_fury") { }

    struct npc_storm_furyAI : public npc_ashtongue_deathsworn_friendAI
    {
        npc_storm_furyAI(Creature* creature) : npc_ashtongue_deathsworn_friendAI(creature) {}

        uint32 BlinkTimer;

        void Reset()
        {
            BlinkTimer = 20000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			CheckAkamaEvent(diff);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BlinkTimer < diff)
            {
                DoCast(me, SPELL_STORM_BLINK_DAMAGE, true);
                DoCast(me, SPELL_STORM_BLINK_OPTICAL);
                BlinkTimer = 20000+rand()%10000;
            }
			else BlinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_storm_furyAI(creature);
	}
};

class npc_illidari_centurion : public CreatureScript
{
public:
	npc_illidari_centurion() : CreatureScript("npc_illidari_centurion") { }

    struct npc_illidari_centurionAI : public QuantumBasicAI
    {
        npc_illidari_centurionAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;
        uint32 SonicStrikeTimer;

        void Reset()
        {
            CleaveTimer = 10000 + rand()%5000;
            SonicStrikeTimer = 10000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CleaveTimer < diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 5000 + rand()%5000;
            }
			else CleaveTimer -= diff;

            if (SonicStrikeTimer < diff)
            {
                DoCastVictim(SPELL_SONIC_STRIKE);
                SonicStrikeTimer = 10000 + rand()%10000;
            }
			else SonicStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_centurionAI(creature);
	}
};

class npc_illidari_defiler : public CreatureScript
{
public:
	npc_illidari_defiler() : CreatureScript("npc_illidari_defiler") { }

    struct npc_illidari_defilerAI : public QuantumBasicAI
    {
        npc_illidari_defilerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 BanishTimer;
        uint32 CurseTimer;
        uint32 ImmolationTimer;
        uint32 RainTimer;

        void Reset()
        {
            BanishTimer = 10000 + rand()%10000;
            CurseTimer = 10000 + rand()%10000;
            ImmolationTimer = 10000 + rand()%5000;
            RainTimer = 30000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BanishTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_BANISH);
				}
                BanishTimer = 15000;
            }
			else BanishTimer -= diff;

            if (CurseTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && !target->HasAura(SPELL_CURSE_OF_AGONY, 0))
					{
						DoCast(target, SPELL_CURSE_OF_AGONY);
						CurseTimer = 10000 + rand()%10000;
					}
                }
            }
			else CurseTimer -= diff;

            if (ImmolationTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && !target->HasAura(SPELL_FEL_IMMOLATE, 0))
					{
						DoCast(target, SPELL_FEL_IMMOLATE);
						ImmolationTimer = 15000 + rand()%10000;
					}
                }
            }
			else ImmolationTimer -= diff;

            if (RainTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_RAIN_OF_CHAOS_ILLIDARI_DEFILER);
					RainTimer = 20000 + rand()%10000;
				}
            }
			else RainTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_defilerAI(creature);
	}
};

class npc_illidari_heartseeker : public CreatureScript
{
public:
	npc_illidari_heartseeker() : CreatureScript("npc_illidari_heartseeker") { }

    struct npc_illidari_heartseekerAI : public QuantumBasicAI
    {
        npc_illidari_heartseekerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SkeletonTimer;
        uint32 BleakheartTimer;
        uint32 ShotTimer;
        uint32 RapidTimer;

        void Reset()
        {
            ShotTimer = 2500;
            RapidTimer = 10000+rand()%10000;
            BleakheartTimer = 10000+rand()%10000;
            SkeletonTimer = 10000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
            QuantumBasicAI::AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (RapidTimer < diff)
            {
                DoCast(me, SPELL_RAPID_SHOT);
                RapidTimer = 20000+rand()%10000;
            }
			else RapidTimer -= diff;

            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
            else
            {
                if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

                if (ShotTimer < diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCastVictim(SPELL_ILLIDARI_HEARTSEEKER_SHOT);
                        ShotTimer = 2500;
                    }
                }
				else ShotTimer -= diff;
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_heartseekerAI(creature);
	}
};

class npc_priestess_of_dementia : public CreatureScript
{
public:
	npc_priestess_of_dementia() : CreatureScript("npc_priestess_of_dementia") { }

    struct npc_priestess_of_dementiaAI : public QuantumBasicAI
    {
        npc_priestess_of_dementiaAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DementiaTimer;
        uint32 ConfusionTimer;
        uint32 ImageTimer;

        void Reset()
        {
            DementiaTimer = 1000;
            ConfusionTimer = 10000 + rand()%5000;
            ImageTimer = 10000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DementiaTimer < diff)
            {
                DoCast(me, SPELL_DEMENTIA);
                DementiaTimer = 30000 + rand()%10000;
            }
			else DementiaTimer -= diff;

            if (ConfusionTimer < diff)
            {
                DoCast(me, SPELL_CONFUSION);
                ConfusionTimer = 10000 + rand()%10000;
            }
			else ConfusionTimer -= diff;

            if (ImageTimer < diff)
            {
                if (Creature* creature = DoSpawnCreature(NPC_IMAGE_OF_DEMENTIA, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 10000 + rand()%10000))
                {
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						creature->AI()->AttackStart(target);
						creature->CastSpell(creature, SPELL_DEMENTIA_WHIRLWIND, false);
						ImageTimer = 20000 + rand()%10000;
					}
                }
            }
			else ImageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_priestess_of_dementiaAI(creature);
	}
};

class npc_priestess_of_delight : public CreatureScript
{
public:
	npc_priestess_of_delight() : CreatureScript("npc_priestess_of_delight") { }

    struct npc_priestess_of_delightAI : public QuantumBasicAI
    {
        npc_priestess_of_delightAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CurseTimer;

        void Reset()
        {
			CurseTimer = 10000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CurseTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_CURSE_OF_VITALITY);
					CurseTimer = 10000 + rand()%5000;
				}
            }
			else CurseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_priestess_of_delightAI(creature);
	}
};

class npc_sister_of_pain : public CreatureScript
{
public:
	npc_sister_of_pain() : CreatureScript("npc_sister_of_pain") { }

    struct npc_sister_of_painAI : public QuantumBasicAI
    {
        npc_sister_of_painAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 LashTimer;
        uint32 RageTimer;
        uint32 ShadowWordTimer;
        uint32 ShellTimer;

		void Reset()
        {
            LashTimer = 5000 + rand()%10000;
            RageTimer = 10000 + rand()%10000;
            ShadowWordTimer = 10000 + rand()%5000;
            ShellTimer = 10000 + rand()%5000;
        }

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void EnterToBattle(Unit* who)
        {
            if (Creature* sister = FindCreatureWithDistance(NPC_SISTER_OF_PLEASURE, 40))
            {
                sister->AI()->AttackStart(who);
                DoCast(sister, SPELL_SHARED_BONDS_SCRIPT);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
        
            if (LashTimer < diff)
            {
                DoCastVictim(SPELL_LASH_OF_PAIN);
                LashTimer = 5000 + rand()%10000;
            }
			else LashTimer -= diff;

            if (RageTimer < diff)
            {
                DoCast(me, SPELL_PAINFULL_RAGE);
                RageTimer = 25000 + rand()%10000;
            }
			else RageTimer -= diff;

            if (ShadowWordTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_SIS_SHADOW_WORD_PAIN);
					ShadowWordTimer = 10000 + rand()%8000;
				}
            }
			else ShadowWordTimer -= diff;

            if (ShellTimer < diff)
            {
                DoCast(me, SPELL_SHELL_OF_PAIN);
                ShellTimer = 30000 + rand()%10000;
            }
			else ShellTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sister_of_painAI(creature);
	}
};

class npc_sister_of_pleasure : public CreatureScript
{
public:
	npc_sister_of_pleasure() : CreatureScript("npc_sister_of_pleasure") { }

    struct npc_sister_of_pleasureAI : public QuantumBasicAI
    {
        npc_sister_of_pleasureAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 HealTimer;
        uint32 NovaTimer;
        uint32 ShellTimer;

		void Reset()
        {
             HealTimer = 10000 + rand()%5000;
             NovaTimer = 10000 + rand()%5000;
             ShellTimer = 10000 + rand()%5000;
        }

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void EnterToBattle(Unit* who)
        {
            if (Creature* sister = FindCreatureWithDistance(NPC_SISTER_OF_PAIN, 40))
            {
                sister->AI()->AttackStart(who);
                DoCast(sister,SPELL_SHARED_BONDS_SCRIPT);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HealTimer < diff)
            {
                Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW);
                if (!target)
				{
                    target = me;
				}
                DoCast(target, SPELL_SIS_GREATER_HEAL);
                HealTimer = 20000 + rand()%10000;
            }
			else HealTimer -= diff;

            if (NovaTimer < diff)
            {
                DoCast(me,SPELL_SIS_HOLY_NOVA);
                NovaTimer = 20000 + rand()%10000;
            }
			else NovaTimer -= diff;

            if (ShellTimer < diff)
            {
                DoCast(me, SPELL_SHELL_OF_LIFE);
                ShellTimer = 30000 + rand()%10000;
            }
			else ShellTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sister_of_pleasureAI(creature);
	}
};

class npc_temple_concubine : public CreatureScript
{
public:
	npc_temple_concubine() : CreatureScript("npc_temple_concubine") { }

    struct npc_temple_concubineAI : public QuantumBasicAI
    {
        npc_temple_concubineAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 TrapTimer;
        uint32 PolymorphTimer;

        void Reset()
        {
			TrapTimer = 10000 + rand()%5000;
			PolymorphTimer = 10000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (TrapTimer < diff)
            {
                DoCastVictim(SPELL_LOVE_TRAP);
                TrapTimer = 5000 + rand()%10000;
            }
			else TrapTimer -= diff;

            if (PolymorphTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_CONC_POLYMORPH);
				}
                PolymorphTimer = 15000;
            }
			else PolymorphTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_temple_concubineAI(creature);
	}
};

class npc_charming_courtesan : public CreatureScript
{
public:
	npc_charming_courtesan() : CreatureScript("npc_charming_courtesan") { }

    struct npc_charming_courtesanAI : public QuantumBasicAI
    {
		npc_charming_courtesanAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 InfatuationTimer;
        uint32 ThrowTimer;

        void Reset()
        {
			InfatuationTimer = 20000 + rand()%10000;
			ThrowTimer = 10000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (InfatuationTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_INFATUATION);
					InfatuationTimer = 30000 + rand()%10000;
				}
            }
			else InfatuationTimer -= diff;

            if (ThrowTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_COURT_POISONOUS_THROW);
					ThrowTimer = 15000;
				}
            }
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_charming_courtesanAI(creature);
	}
};

class npc_enslaved_servant : public CreatureScript
{
public:
	npc_enslaved_servant() : CreatureScript("npc_enslaved_servant") { }

    struct npc_enslaved_servantAI : public QuantumBasicAI
    {
        npc_enslaved_servantAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShotTimer;
        uint32 UppercutTimer;

        void Reset()
        {
			ShotTimer = 15000 + rand()%10000;
			UppercutTimer = 15000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShotTimer < diff)
            {
                DoCastVictim(SPELL_KIDNEY_SHOT);
                ShotTimer = 15000 + rand()%10000;
            }
			else ShotTimer -= diff;

            if (UppercutTimer < diff)
            {
                DoCastVictim(SPELL_UPPERCUT);
                UppercutTimer = 15000 + rand()%10000;
            }
			else UppercutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_enslaved_servantAI(creature);
	}
};

class npc_spellbound_attendant : public CreatureScript
{
public:
	npc_spellbound_attendant() : CreatureScript("npc_spellbound_attendant") { }

    struct npc_spellbound_attendantAI : public QuantumBasicAI
    {
        npc_spellbound_attendantAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 KickTimer;
        uint32 SleepTimer;

        void Reset()
        {
			KickTimer = 15000 + rand()%10000;
			SleepTimer = 15000 + rand()%10000;
		}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (KickTimer < diff)
            {
                DoCastVictim(SPELL_ATTENDANT_KICK);
                KickTimer = 15000 + rand()%10000;
            }
			else KickTimer -= diff;

            if (SleepTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_ATTENDANT_SLEEP);
					SleepTimer = 15000 + rand()%10000;
				}
            }
			else SleepTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_spellbound_attendantAI(creature);
	}
};

class npc_suffering_soul_fragment : public CreatureScript
{
public:
	npc_suffering_soul_fragment() : CreatureScript("npc_suffering_soul_fragment") { }

    struct npc_suffering_soul_fragmentAI : public QuantumBasicAI
    {
        npc_suffering_soul_fragmentAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 BlastTimer;

        void Reset()
        {
			BlastTimer = 5000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
        {
			me->RemoveCorpse();
			me->SetRespawnTime(30);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BlastTimer < diff)
            {
                DoCastVictim(SPELL_SOUL_BLAST);
                BlastTimer = 10000 + rand()%5000;
            }
			else BlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_suffering_soul_fragmentAI(creature);
	}
};

class npc_angered_soul_fragment : public CreatureScript
{
public:
	npc_angered_soul_fragment() : CreatureScript("npc_angered_soul_fragment") { }

    struct npc_angered_soul_fragmentAI : public QuantumBasicAI
    {
        npc_angered_soul_fragmentAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 AngerTimer;

        void Reset()
        {
			AngerTimer = 5000 + rand()%5000;
			QuantumBasicAI::Reset();
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
        {
			me->RemoveCorpse();
			me->SetRespawnTime(30);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (AngerTimer < diff)
            {
                DoCastVictim(SPELL_SOUL_ANGER);
                AngerTimer = 10000 + rand()%5000;
            }
			else AngerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_angered_soul_fragmentAI(creature);
	}
};

class npc_hungering_soul_fragment : public CreatureScript
{
public:
	npc_hungering_soul_fragment() : CreatureScript("npc_hungering_soul_fragment") { }

    struct npc_hungering_soul_fragmentAI : public QuantumBasicAI
    {
        npc_hungering_soul_fragmentAI(Creature* creature) : QuantumBasicAI(creature) {}

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_CONSUMING_STRIKES, true);
        }

		void JustDied(Unit* /*killer*/)
        {
			me->RemoveCorpse();
			me->SetRespawnTime(30);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_hungering_soul_fragmentAI(creature);
	}
};

class npc_illidari_battlemage : public CreatureScript
{
public:
	npc_illidari_battlemage() : CreatureScript("npc_illidari_battlemage") { }

    struct npc_illidari_battlemageAI : public QuantumBasicAI
    {
        npc_illidari_battlemageAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FrostTimer;
        uint32 FireTimer;
        uint32 FlameTimer;
        uint32 BlizzardTimer;

        void Reset()
        {
			FrostTimer = 10000 + rand()%5000;
            FireTimer = 10000 + rand()%5000;
            BlizzardTimer = 15000 + rand()%10000;
            FlameTimer = 15000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->Attack(who, true))
				DoStartMovement(who, 20, me->GetAngle(who));
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FrostTimer < diff)
            {
                DoCastVictim(SPELL_ILLI_FROSTBOLT);
                FrostTimer = 5000 + rand()%10000;
            }
			else FrostTimer -= diff;

            if (FireTimer < diff)
            {
                DoCastVictim(SPELL_ILLI_FIREBALL);
                FireTimer = 5000 + rand()%10000;
            }
			else FireTimer -= diff;

            if (BlizzardTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_ILLI_BLIZZARD);
					BlizzardTimer = 20000 + rand()%10000;
				}
            }
			else BlizzardTimer -= diff;

            if (FlameTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_ILLI_FLAMESTRIKE);
					FlameTimer = 15000 + rand()%10000;
				}
            }
			else FlameTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_battlemageAI(creature);
	}
};

class npc_illidari_blood_lord : public CreatureScript
{
public:
	npc_illidari_blood_lord() : CreatureScript("npc_illidari_blood_lord") { }

    struct npc_illidari_blood_lordAI : public QuantumBasicAI
    {
        npc_illidari_blood_lordAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShieldTimer;
        uint32 HammerTimer;
        uint32 JudgementTimer;

        void Reset()
        {
			ShieldTimer = 10000 + rand()%5000;
            HammerTimer = 10000 + rand()%5000;
            JudgementTimer = 10000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShieldTimer < diff)
            {
                DoCast(me, SPELL_ILLI_DIVINE_SHIELD);
                ShieldTimer = 30000 + rand()%10000;
            }
			else ShieldTimer -= diff;

            if (HammerTimer < diff)
            {
                DoCastVictim(SPELL_ILLI_HAMMER_OF_JUSTICE);
                HammerTimer = 5000 + rand()%5000;
            }
			else HammerTimer -= diff;

            if (JudgementTimer < diff)
            {
                DoCastVictim(SPELL_ILLI_JUDGEMENT);
                JudgementTimer = 10000 + rand()%5000;
            }
			else JudgementTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_blood_lordAI(creature);
	}
};

class npc_illidari_assassin : public CreatureScript
{
public:
	npc_illidari_assassin() : CreatureScript("npc_illidari_assassin") { }

    struct npc_illidari_assassinAI : public QuantumBasicAI
    {
        npc_illidari_assassinAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 VanishTimer;
        uint32 PoisonTimer;

        void Reset()
        {
			VanishTimer = 10000 + rand()%10000;
            PoisonTimer = 10000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (VanishTimer < diff)
            {
                DoCast(me, SPELL_ILLI_VANISH);
                VanishTimer = 10000 + rand()%10000;
            }
			else VanishTimer -= diff;

            if (PoisonTimer < diff)
            {
                DoCastVictim(SPELL_ILLI_PARALYZING_POISON);
                PoisonTimer = 10000 + rand()%10000;
            }
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_assassinAI(creature);
	}
};

class npc_illidari_archon : public CreatureScript
{
public:
	npc_illidari_archon() : CreatureScript("npc_illidari_archon") { }

    struct npc_illidari_archonAI : public QuantumBasicAI
    {
        npc_illidari_archonAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool shadow;

        uint32 HealTimer;
        uint32 SmiteTimer;
        uint32 ShieldTimer;
        uint32 BlastTimer;
        uint32 DeathTimer;

        void Reset()
        {
			HealTimer = 10000 + rand()%10000;
            SmiteTimer = 10000 + rand()%5000;
            ShieldTimer = 10000 + rand()%10000;
            BlastTimer = 10000 + rand()%5000;
            DeathTimer = 20000 + rand()%10000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
             shadow = rand()%2 == 0;
             if (shadow)
			 {
                DoCast(me, SPELL_ILLI_SHADOW_FORM);
			 }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (shadow)
            {
                if (BlastTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_ILLI_MIND_BLAST);
						BlastTimer = 5000 + rand()%10000;
					}
                }
				else BlastTimer -= diff;

                if (DeathTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_ILLI_SHADOW_WORD_DEATH);
						DeathTimer = 30000 + rand()%10000;
					}
                }
				else DeathTimer -= diff;
			}
            else
            {
                if (ShieldTimer < diff)
                {
                    if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
					{
						DoCast(target, SPELL_ILLI_SHIELD);
						ShieldTimer = 30000 + rand()%10000;
					}
                }
				else ShieldTimer -= diff;

                if (HealTimer < diff)
                {
                    if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
					{
						DoCast(target, SPELL_ILLI_HEAL);
						HealTimer = 10000 + rand()%10000;
					}
                }
				else HealTimer -= diff;

                if (SmiteTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_ILLI_HOLY_SMITE);
						SmiteTimer = 5000 + rand()%5000;
					}
                }
				else SmiteTimer -= diff;
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_illidari_archonAI(creature);
	}
};

class npc_promenade_sentinel : public CreatureScript
{
public:
	npc_promenade_sentinel() : CreatureScript("npc_promenade_sentinel") { }

    struct npc_promenade_sentinelAI : public QuantumBasicAI
    {
        npc_promenade_sentinelAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ArcaneL1Timer;
        uint32 ArcaneL4Timer;
        uint32 ArcaneL5Timer;

        void Reset()
        {
			ArcaneL1Timer = 1000;
            ArcaneL4Timer = 10000+ rand()%10000;
            ArcaneL5Timer = 10000+ rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArcaneL1Timer < diff)
            {
				DoCast(me, SPELL_L1_ARCANE_CHARGE, true);
                ArcaneL1Timer = 1000;
            }
			else ArcaneL1Timer -= diff;

            if (ArcaneL4Timer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_L4_ARCANE_CHARGE);
					ArcaneL4Timer = 10000;
				}
            }
			else ArcaneL4Timer -= diff;

            if (ArcaneL5Timer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    int damage = target->GetMaxHealth();
                    me->CastCustomSpell(target, SPELL_L5_ARCANE_CHARGE, &damage, NULL, NULL, false, NULL, NULL, me->GetGUID());
					ArcaneL5Timer = 10000+ rand()%15000;
				}
            }
			else ArcaneL5Timer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_promenade_sentinelAI(creature);
	}
};

class npc_arcane_charge_L4 : public CreatureScript
{
public:
	npc_arcane_charge_L4() : CreatureScript("npc_arcane_charge_L4") { }

    struct npc_arcane_charge_L4AI : public QuantumBasicAI
    {
        npc_arcane_charge_L4AI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ChargeTimer;

        void MoveInLineOfSight(Unit* /*who*/){}
        void AttackStart(Unit* /*who*/){}
		void EnterToBattle(Unit* /*who*/){}

        void Reset()
        {
            ChargeTimer = 100;
        }

        void UpdateAI(uint32 const diff)
        {
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ChargeTimer < diff)
            {
                DoCast(me, SPELL_L4_ARCANE_CHARGE_DMG, true);
                ChargeTimer = 1000;
            }
			else ChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_arcane_charge_L4AI(creature);
	}
};

class npc_hand_of_gorefiend : public CreatureScript
{
public:
	npc_hand_of_gorefiend() : CreatureScript("npc_hand_of_gorefiend") { }

    struct npc_hand_of_gorefiendAI : public QuantumBasicAI
    {
        npc_hand_of_gorefiendAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 AtrophyTimer;

        void Reset()
        {
			AtrophyTimer = 3000;
            me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AtrophyTimer < diff)
            {
                DoCastVictim(SPELL_ATROPHY);
                AtrophyTimer = urand(4000, 5000);
            }
			else AtrophyTimer -= diff;

            if (me->GetHealth() > 0)
			{
                if (((me->GetMaxHealth() * 100) / me->GetHealth()) <= 20)
                {
                    if (!me->HasAura(SPELL_HAND_ENRAGE, 0))
					{
                        DoCast(me, SPELL_HAND_ENRAGE);
					}
                }
			}

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_hand_of_gorefiendAI(creature);
	}
};

class npc_shadowmoon_blood_mage : public CreatureScript
{
public:
	npc_shadowmoon_blood_mage() : CreatureScript("npc_shadowmoon_blood_mage") { }

    struct npc_shadowmoon_blood_mageAI : public QuantumBasicAI
    {
        npc_shadowmoon_blood_mageAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 BoltTimer;
        uint32 SiphonTimer;

        void Reset()
        {
			SiphonTimer = 20000 + rand()%10000;
            BoltTimer = 5000 + rand()%5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BoltTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_BLOOD_BOLT_SCRIPT);
					BoltTimer = 10000 + rand()%5000;
				}
            }
			else BoltTimer -= diff;

            if (SiphonTimer < diff)
            {
                DoCastVictim(SPELL_BLOOD_SIPHON);
                SiphonTimer = 30000 + rand()%10000;
            }
			else SiphonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_blood_mageAI(creature);
	}
};

class npc_shadowmoon_soldier : public CreatureScript
{
public:
	npc_shadowmoon_soldier() : CreatureScript("npc_shadowmoon_soldier") { }

    struct npc_shadowmoon_soldierAI : public QuantumBasicAI
    {
        npc_shadowmoon_soldierAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 StrikeTimer;

        void Reset()
        {
			StrikeTimer = 5000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (StrikeTimer < diff)
            {
                DoCastVictim(SPELL_SOLDIER_STRIKE);
                StrikeTimer = 5000 + rand()%5000;
            }
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_soldierAI(creature);
	}
};

class npc_shadowmoon_reaver : public CreatureScript
{
public:
	npc_shadowmoon_reaver() : CreatureScript("npc_shadowmoon_reaver") { }

    struct npc_shadowmoon_reaverAI : public QuantumBasicAI
    {
        npc_shadowmoon_reaverAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 AbsorptionTimer;
        uint8 AuraStack;

        void Reset()
        {
			AbsorptionTimer = 10000 + rand()%5000;
            AuraStack = 0;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (AbsorptionTimer < diff)
            {
                DoCast(me, SPELL_ABSORPTION);
                AbsorptionTimer = 20000 + rand()%10000;
            }
			else AbsorptionTimer -= diff;

            if (AuraStack > (me->HasAura(SPELL_CHAOTIC_CHARGE, 0) ? me->GetAura(SPELL_CHAOTIC_CHARGE, 0)->GetStackAmount() : 0))
			{
                int32 damage = rand()%374 + 2313;
                SpellInfo *spellInfo;
                spellInfo = (SpellInfo*)sSpellMgr->GetSpellInfo(SPELL_CHAOTIC_CHARGE_DMG);
                if (spellInfo)
                {
                    spellInfo->MaxAffectedTargets = AuraStack;
                    spellInfo->Effects[0].BasePoints = damage + 1000 *AuraStack;
                    me->CastSpell(me, spellInfo, false);
                }
                AuraStack = 0;
            }
			else
            {
                if (me->HasAura(SPELL_CHAOTIC_CHARGE, 0))
				{
                    AuraStack = me->GetAura(SPELL_CHAOTIC_CHARGE, 0)->GetStackAmount();
				}
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_reaverAI(creature);
	}
};

class npc_shadowmoon_deathshaper : public CreatureScript
{
public:
	npc_shadowmoon_deathshaper() : CreatureScript("npc_shadowmoon_deathshaper") { }

    struct npc_shadowmoon_deathshaperAI : public QuantumBasicAI
    {
        npc_shadowmoon_deathshaperAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CoilTimer;
        uint32 BoltTimer;
        uint32 DeathTimer;
        uint32 ArmorTimer;

        void Reset()
        {
			ArmorTimer = 500;
            CoilTimer = 10000 + rand()%5000;
            BoltTimer = 1000 + rand()%5000;
            DeathTimer = 10000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        Creature* GetNextFriendlyCorpse()
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
                return NULL;

            for (std::list<Creature*>::iterator i = templist.begin(); i != templist.end(); ++i)
            {
                if ((*i) && me->IsWithinDistInMap((*i),25) && (*i)->IsDead())
					return (*i);
            }
            return NULL;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!me->HasAura(SPELL_SM_DEMON_ARMOR, 0))
            {
                if (ArmorTimer < diff)
                {
                    DoCast(me, SPELL_SM_DEMON_ARMOR);
                    ArmorTimer = 20000;
                }
				else ArmorTimer -= diff;
            }

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CoilTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_SM_DEATH_COIL);
						CoilTimer = 10000 + rand()%10000;
					}
                }
            }
			else CoilTimer -= diff;

            if (BoltTimer < diff)
            {
                DoCastVictim(SPELL_SM_SHADOW_BOLT);
                BoltTimer = 5000;
            }
			else BoltTimer -= diff;

            if (DeathTimer < diff)
            {
                Creature* target = GetNextFriendlyCorpse();
                if (target && target->IsDead())
                {
                    DoCast(target, SPELL_SM_SUMMON_SKELETON, true);
                    target->RemoveCorpse();
                }
                DeathTimer = 20000 + rand()%20000;
            }
			else DeathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_deathshaperAI(creature);
	}
};

class npc_shadowmoon_champion : public CreatureScript
{
public:
	npc_shadowmoon_champion() : CreatureScript("npc_shadowmoon_champion") { }

    struct npc_shadowmoon_championAI : public QuantumBasicAI
    {
        npc_shadowmoon_championAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WeaponTimer;
        uint32 LightTimer;

        void Reset()
        {
			WeaponTimer = 20000 + rand()% 10000;
            LightTimer = 5000 + rand()% 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (LightTimer < diff)
            {
                if (!me->HasAuraType(SPELL_AURA_MOD_DISARM))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_SM_CHAOTIC_LIGHT);
						LightTimer = 5000 + rand()%5000;
					}
                }
            }
			else LightTimer -= diff;

            if (WeaponTimer < diff)
            {
				DoCastVictim(SPELL_SM_WHIRLING_WEAPON);
				WeaponTimer = 25000 + rand()% 10000;
            }
			else WeaponTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_championAI(creature);
	}
};

class npc_whirling_blade : public CreatureScript
{
public:
	npc_whirling_blade() : CreatureScript("npc_whirling_blade") { }

    struct npc_whirling_bladeAI : public QuantumBasicAI
    {
        npc_whirling_bladeAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WhirlwindTimer;
        uint32 DelTimer;

        void MoveInLineOfSight(Unit* /*who*/){}
		void EnterToBattle(Unit* /*who*/){}
        void AttackStart(Unit* /*who*/){}

        void Reset()
        {
            DelTimer = 14000;
            WhirlwindTimer = 100;
        }

        void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DelTimer < diff)
            {
                me->DealDamage(me, me->GetHealth());
                me->RemoveCorpse();
                DelTimer = 1*MINUTE*IN_MILLISECONDS;
            }
			else DelTimer -= diff;

            if (WhirlwindTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
					DoCast(me, SPELL_WHIRLING);
                    WhirlwindTimer = 2000;
                }
            }
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_whirling_bladeAI(creature);
	}
};

class npc_shadowmoon_houndmaster : public CreatureScript
{
public:
	npc_shadowmoon_houndmaster() : CreatureScript("npc_shadowmoon_houndmaster") { }

    struct npc_shadowmoon_houndmasterAI : public QuantumBasicAI
    {
        npc_shadowmoon_houndmasterAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShootTimer;
        uint32 TrapTimer;
        uint32 ClipTimer;
        uint32 ShotTimer;
        uint32 VolleyTimer;

		Creature* FindCreatureWithDistance(uint32 entry, float range)
		{
			Creature* creature = NULL;
			Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
			Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
			me->VisitNearbyObject(range, searcher);
			return creature;
		}

        void Reset()
        {
			ShootTimer = 2500;
            TrapTimer = 10000 + rand()% 10000;
            ClipTimer = 10000 + rand()% 10000;
            ShotTimer = 10000 + rand()% 10000;
            VolleyTimer = 20000 + rand()% 10000;
        }

        void EnterToBattle(Unit* who)
        {
            Creature* hound = FindCreatureWithDistance(NPC_SHADOWMOON_RIDING_HOUND, 30);
            if (!hound || hound->IsDead())
			{
                DoCast(me, SPELL_SMHM_RIDING_HOUND);
			}
            else hound->AI()->AttackStart(who);
        }

        void AttackStart(Unit* who)
        {
            QuantumBasicAI::AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ClipTimer < diff)
            {
                if (me->IsWithinMeleeRange(me->GetVictim()))
				{
                    DoCast(me, SPELL_SMHM_WING_CLIP);
					ClipTimer = 10000 + rand()% 10000;
				}
            }
			else ClipTimer -= diff;

            if (ShotTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_SMHM_SILENCING_SHOT);
					ShotTimer = 10000 + rand()% 10000;
				}
            }
			else ShotTimer -= diff;

            if (VolleyTimer < diff)
            {
                DoCastVictim(SPELL_SMHM_VOLLEY);
                VolleyTimer = 20000 + rand()% 10000;
            }
			else VolleyTimer -= diff;

            if (TrapTimer < diff)
            {
                DoCast(me, SPELL_SMHM_FREEZING_TRAP);
                TrapTimer = 30000 + rand()% 10000;
            }
			else TrapTimer -= diff;

            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
            else
            {
                if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

                if (ShootTimer < diff)
                {
                    DoCastVictim(SPELL_SMHM_SHOOT);
                    ShootTimer = 2500;
                }
				else ShootTimer -= diff;
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_houndmasterAI(creature);
	}
};

class npc_shadowmoon_riding_hound : public CreatureScript
{
public:
	npc_shadowmoon_riding_hound() : CreatureScript("npc_shadowmoon_riding_hound") { }

    struct npc_shadowmoon_riding_houndAI : public QuantumBasicAI
    {
        npc_shadowmoon_riding_houndAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ChargeTimer;
        uint32 BiteTimer;

        void Reset()
        {
			ChargeTimer = 1000 + rand()% 10000;
            BiteTimer = 10000 + rand()% 10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ChargeTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_SMRH_CHARGE);
					ChargeTimer = 10000 + rand()% 10000;
				}
            }
			else ChargeTimer -= diff;

            if (BiteTimer < diff)
            {
                DoCastVictim(SPELL_SMRH_CARNIVOROUS_BITE);
                BiteTimer = 10000 + rand()% 10000;
            }
			else BiteTimer -= diff;

            if ((me->GetHealth() * 100 / me->GetMaxHealth()) < 20)
            {
                if (!me->HasAura(SPELL_SMRH_ENRAGE, 0))
				{
                    DoCast(me, SPELL_SMRH_ENRAGE, true);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_riding_houndAI(creature);
	}
};

class npc_shadowmoon_weapon_master : public CreatureScript
{
public:
	npc_shadowmoon_weapon_master() : CreatureScript("npc_shadowmoon_weapon_master") { }

    struct npc_shadowmoon_weapon_masterAI : public QuantumBasicAI
    {
        npc_shadowmoon_weapon_masterAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 StanceTimer;
        uint32 SpellTimer;
        uint32 Spell2Timer;

        WarriorStance stance;

        void Reset()
        {
            StanceTimer = 100;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            stance = (WarriorStance)(rand()%3);
            switch(stance)
			{
                case BATTLE_STANCE:
                    DoCast(me, SPELL_SMWM_BATTLE_STANCE);
                    DoCast(me, SPELL_SMWM_BATTLE_AURA, true);
                    SpellTimer = 1000 + rand()%5000;
                    Spell2Timer = 5000 + rand()%10000;
                    break;
                case BERSERKER_STANCE:
                    DoCast(me, SPELL_SMWM_BERSERKER_STANCE);
                    DoCast(me, SPELL_SMWM_BERSERKER_AURA, true);
                    SpellTimer = 10000;
                    break;
                case DEFENSIVE_STANCE:
                    DoCast(me, SPELL_SMWM_DEFENSIVE_STANCE);
                    DoCast(me, SPELL_SMWM_DEFENSIVE_AURA, true);
                    SpellTimer = 5000;
                    break;
                }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (StanceTimer < diff)
            {
                me->RemoveAurasDueToSpell(SPELL_SMWM_BERSERKER_STANCE);
                me->RemoveAurasDueToSpell(SPELL_SMWM_BERSERKER_AURA);
                me->RemoveAurasDueToSpell(SPELL_SMWM_BATTLE_STANCE);
                me->RemoveAurasDueToSpell(SPELL_SMWM_BATTLE_AURA);
                me->RemoveAurasDueToSpell(SPELL_SMWM_DEFENSIVE_STANCE);
                me->RemoveAurasDueToSpell(SPELL_SMWM_DEFENSIVE_AURA);
                WarriorStance oldstance = stance;

                while (stance == oldstance) stance = (WarriorStance)(rand()%3);

                switch(stance)
                {
                case BATTLE_STANCE:
                    DoCast(me, SPELL_SMWM_BATTLE_STANCE);
                    DoCast(me, SPELL_SMWM_BATTLE_AURA, true);
                    SpellTimer = 1000 + rand()%5000;
                    Spell2Timer = 5000 + rand()%10000;
                    break;
                case BERSERKER_STANCE:
                    DoCast(me, SPELL_SMWM_BERSERKER_STANCE);
                    DoCast(me, SPELL_SMWM_BERSERKER_AURA, true);
                    SpellTimer = 10000;
                    break;
                case DEFENSIVE_STANCE:
                    DoCast(me, SPELL_SMWM_DEFENSIVE_STANCE);
                    DoCast(me, SPELL_SMWM_DEFENSIVE_AURA, true);
                    SpellTimer = 5000;
                    break;
                }
                StanceTimer = 30000;
            }
			else StanceTimer -= diff;

            if (SpellTimer < diff)
            {
                switch(stance)
                {
                case BATTLE_STANCE:
                    DoCastVictim(SPELL_SMWM_KNOCK_AWAY);
                    SpellTimer = 5000 + rand()%10000;
                    break;
                case BERSERKER_STANCE:
                    DoCast(me, SPELL_SMWM_WHIRLWIND);
                    SpellTimer = 30000;
                    break;
                case DEFENSIVE_STANCE:
                    DoCast(me, SPELL_SMWM_SHIELD_WALL);
                    SpellTimer = 15000;
                    break;
                }
            }
			else SpellTimer -= diff;

            if (stance == BATTLE_STANCE)
            {
                if (Spell2Timer < diff)
                {
                    DoCastVictim(SPELL_SMWM_MULTILATE);
                    Spell2Timer = 5000 + rand()%10000;
                }
				else Spell2Timer -= diff;
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowmoon_weapon_masterAI(creature);
	}
};

class npc_bonechewer_behemoth : public CreatureScript
{
public:
	npc_bonechewer_behemoth() : CreatureScript("npc_bonechewer_behemoth") { }

    struct npc_bonechewer_behemothAI : public QuantumBasicAI
    {
        npc_bonechewer_behemothAI(Creature* creature) : QuantumBasicAI(creature)
		{
			MeteorForm = rand()%2 == 0;
		}

        bool MeteorForm;

        uint32 ChargeTimer;
        uint32 StompTimer;
        uint32 CometTimer;
        uint32 MeteorTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            ChargeTimer = 10000 + rand()%5000;
            StompTimer = 10000 + rand()%5000;
            CometTimer = 10000 + rand()%5000;
            MeteorTimer = 10000 + rand()%5000;
            FrenzyTimer = 20000 + rand()%10000;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (MeteorForm)
            {
                if (CometTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
					{
						if (target && me->GetDistance(target) > 10)
						{
							DoCast(target, SPELL_BEHEMOTH_FIERY_COMET);
							CometTimer = 10000 + rand()%5000;
						}
                    }
                }
				else CometTimer -= diff;

                if (MeteorTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_BEHEMOTH_METEOR);
						MeteorTimer = 10000 + rand()%5000;
					}
                }
				else MeteorTimer -= diff;
            }
            else
            {
                if (ChargeTimer < diff)
                {
                    if (Unit* target = SelectTarget(TARGET_FARTHEST, 0))
                    {
                        DoCast(target, SPELL_BEHEMOTH_CHARGE);
                        ChargeTimer = 10000 + rand()%5000;
                    }
                }
				else ChargeTimer -= diff;

                if (StompTimer < diff)
                {
                    DoCast(me, SPELL_BEHEMOTH_FEL_STOMP);
                    StompTimer = 5000 + rand()%10000;
                }
				else StompTimer -= diff;
            }

            if (FrenzyTimer < diff)
            {
                DoCast(me, SPELL_BEHEMOTH_FRENZY);
                FrenzyTimer = 30000 + rand()%10000;
            }
			else FrenzyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_behemothAI(creature);
	}
};

class npc_bonechewer_shield_disciple : public CreatureScript
{
public:
	npc_bonechewer_shield_disciple() : CreatureScript("npc_bonechewer_shield_disciple") { }

    struct npc_bonechewer_shield_discipleAI : public QuantumBasicAI
    {
        npc_bonechewer_shield_discipleAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 InterveneTimer;
        uint32 BashTimer;
        uint32 WallTimer;
        uint32 ThrowTimer;

        void Reset()
        {
			InterveneTimer = 10000 + rand()%10000;
			BashTimer = 10000 + rand()%5000;
            WallTimer = 10000 + rand()%10000;
            ThrowTimer = 10000 + rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (InterveneTimer < diff)
            {
                Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW);
                if (target && me->GetDistance2d(target) > 10)
                {
					DoCast(target, SPELL_INTERVENE);
					InterveneTimer = 20000 + rand()%10000;
                }
            }
			else InterveneTimer -= diff;

            if (BashTimer < diff)
            {
                DoCastVictim(SPELL_BCSD_SHIELD_BASH);
                BashTimer = 5000 + rand()%5000;
            }
			else BashTimer -= diff;

            if (WallTimer < diff)
            {
                DoCast(me, SPELL_BCSD_SHIELD_WALL);
                WallTimer = 20000 + rand()%10000;
            }
			else WallTimer -= diff;

            if (ThrowTimer < diff)
            {
                if  (Unit* target = SelectTarget(TARGET_RANDOM, 1, 40, true))
				{
					if (target && me->GetDistance2d(target) > 5)
					{
						DoCast(me, SPELL_BCSD_SHIELD_THROW);
						ThrowTimer = 10000 + rand()%20000;
					}
                }
            }
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_shield_discipleAI(creature);
	}
};

class npc_bonechewer_blade_fury : public CreatureScript
{
public:
	npc_bonechewer_blade_fury() : CreatureScript("npc_bonechewer_blade_fury") { }

    struct npc_bonechewer_blade_furyAI : public QuantumBasicAI
    {
        npc_bonechewer_blade_furyAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WhirlwindTimer;

        void Reset()
        {
			WhirlwindTimer = 10000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (WhirlwindTimer < diff)
            {
                DoCast(me,SPELL_BCBF_WHIRLWIND);
                WhirlwindTimer = 20000 + rand()%10000;
            }
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_blade_furyAI(creature);
	}
};

class npc_bonechewer_blood_prophet : public CreatureScript
{
public:
	npc_bonechewer_blood_prophet() : CreatureScript("npc_bonechewer_blood_prophet") { }

    struct npc_bonechewer_blood_prophetAI : public QuantumBasicAI
    {
        npc_bonechewer_blood_prophetAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DrainTimer;
        uint32 BoltTimer;
        uint32 ProphecyTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            DrainTimer = 10000+rand()%10000;
            BoltTimer = 50000+rand()%10000;
            ProphecyTimer = 10000+rand()%10000;
            FrenzyTimer = 30000+rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DrainTimer < diff)
            {
                DoCastVictim(SPELL_BCBP_BLOOD_DRAIN);
                DrainTimer = 20000 + rand()%10000;
            }
			else DrainTimer -= diff;

            if (ProphecyTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && !(target->HasAura(SPELL_BCBP_PROPHECY_BLOOD_1, 0) || target->HasAura(SPELL_BCBP_PROPHECY_BLOOD_2, 0)))
					{
						DoCast(target, rand()%2 == 0 ? SPELL_BCBP_PROPHECY_BLOOD_1 : SPELL_BCBP_PROPHECY_BLOOD_2);
						ProphecyTimer = 5000 + rand()%5000;
					}
				}
            }
			else ProphecyTimer -= diff;

            if (BoltTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCastVictim(SPELL_BCBP_BLOOD_BOLT);
                    BoltTimer = 5000 + rand()%5000;
                }
            }
			else BoltTimer -= diff;

            if (FrenzyTimer < diff)
            {
                DoCast(me, SPELL_BCBP_FRENZY);
                FrenzyTimer = 30000+rand()%10000;
            }
			else FrenzyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_blood_prophetAI(creature);
	}
};

class npc_mutant_war_hound : public CreatureScript
{
public:
	npc_mutant_war_hound() : CreatureScript("npc_mutant_war_hound") { }

    struct npc_mutant_war_houndAI : public QuantumBasicAI
    {
        npc_mutant_war_houndAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            DoCast(me, SPELL_DISEASE_CLOUD, true);
        }

		void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* victim)
        {
            DoCast(me, SPELL_CLOUD_OF_DISEASE, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_mutant_war_houndAI(creature);
	}
};

class npc_bonechewer_brawler : public CreatureScript
{
public:
	npc_bonechewer_brawler() : CreatureScript("npc_bonechewer_brawler") { }

    struct npc_bonechewer_brawlerAI : public QuantumBasicAI
    {
        npc_bonechewer_brawlerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 EnrageTimer;
        bool Enraged;

        void Reset()
        {
			EnrageTimer = 10000 + rand()%10000;
            Enraged = false;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (EnrageTimer < diff)
            {
                DoCast(me, SPELL_BCB_ENRAGE);
                EnrageTimer = 20000+ rand()%10000;
            }
			else EnrageTimer -= diff;

            if (!Enraged)
			{
                if ((me->GetHealth() * 100 / me->GetMaxHealth()) < 20)
                {
                    Enraged = true;
                    DoCast(me, SPELL_BCB_FRENZY, true);
                }
			}

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_brawlerAI(creature);
	}
};

class npc_bonechewer_combatant : public CreatureScript
{
public:
	npc_bonechewer_combatant() : CreatureScript("npc_bonechewer_combatant") { }

    struct npc_bonechewer_combatantAI : public QuantumBasicAI
    {
        npc_bonechewer_combatantAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 EnrageTimer;
        uint32 Enraged;

        void Reset()
        {
            EnrageTimer = 100;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (EnrageTimer < diff)
            {
				if (!me->HasAura(SPELL_BCC_ENRAGE, 0))
				{
                    DoCast(me, SPELL_BCC_ENRAGE);
				}
                EnrageTimer = 30000;
            }
			else EnrageTimer -= diff;

            if (!Enraged)
			{
                if ((me->GetHealth() * 100 / me->GetMaxHealth()) < 20)
                {
                    Enraged = true;
                    DoCast(me, SPELL_BCC_FRENZY, true);
                }
			}

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_combatantAI(creature);
	}
};

class npc_bonechewer_spectator : public CreatureScript
{
public:
	npc_bonechewer_spectator() : CreatureScript("npc_bonechewer_spectator") { }

    struct npc_bonechewer_spectatorAI : public QuantumBasicAI
    {
        npc_bonechewer_spectatorAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 RandomTimer;

        void Reset()
        {
			RandomTimer = 10000+ rand()%10000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (RandomTimer < diff)
            {
                Unit* target;
                switch(rand()%6)
                {
                case 0:
                    if (target = SelectTarget(TARGET_RANDOM, 1, 30, true))
					{
						if (target && me->GetDistance2d(target) > 10)
						{
							DoCast(target, SPELL_BCSP_CHARGE);
						}
					}
                    break;
                case 1:
                    DoCastVictim(SPELL_BCSP_CLEAVE);
                    break;
                case 2:
                    DoCast(me, SPELL_BCSP_FRENZY);
                    break;
                case 3:
                    DoCastVictim(SPELL_BCSP_MORTAL_WOUND);
                    break;
                case 4:
                    DoCastVictim(SPELL_BCSP_STRIKE);
                    break;
                case 5:
                    DoCastVictim(SPELL_BCSP_SUNDER_ARMOR);
                    break;
                }
                RandomTimer = 10000+ rand()%10000;
            }
			else RandomTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bonechewer_spectatorAI(creature);
	}
};

void AddSC_black_temple_trash()
{
	new npc_aqueous_lord();
	new npc_aqueous_spawn();
	new npc_coilskar_general();
	new npc_coilskar_wrangler();
	new npc_leviathan();
	new npc_coilskar_sea_caller();
	new npc_coilskar_geyser();
	new npc_coilskar_soothsayer();
	new npc_dragon_turtle();
	new npc_coilskar_harpooner();
	new npc_bonechewer_taskmaster();
	new npc_bonechewer_worker();
	new npc_dragonmaw_sky_stalker();
	new npc_dragonmaw_wind_reaver();
	new npc_dragonmaw_wyrmcaller();
	new npc_illidari_fearbringer();
	new npc_illidari_nightlord();
	new npc_illidari_boneslicer();
	new npc_ashtongue_battlelord();
	new npc_ashtongue_feral_spirit();
	new npc_ashtongue_primalist();
	new npc_ashtongue_mystic();
	new npc_ashtongue_cyclone_totem();
	new npc_ashtongue_searing_totem();
	new npc_ashtongue_stalker();
	new npc_ashtongue_stormcaller();
	new npc_storm_fury();
	new npc_illidari_centurion();
	new npc_illidari_defiler();
	new npc_illidari_heartseeker();
	new npc_priestess_of_dementia();
	new npc_priestess_of_delight();
	new npc_sister_of_pain();
	new npc_sister_of_pleasure();
	new npc_temple_concubine();
	new npc_charming_courtesan();
	new npc_enslaved_servant();
	new npc_spellbound_attendant();
	new npc_suffering_soul_fragment();
	new npc_angered_soul_fragment();
	new npc_hungering_soul_fragment();
	new npc_illidari_battlemage();
	new npc_illidari_blood_lord();
	new npc_illidari_assassin();
	new npc_illidari_archon();
	new npc_promenade_sentinel();
	new npc_arcane_charge_L4();
	new npc_hand_of_gorefiend();
	new npc_shadowmoon_blood_mage();
	new npc_shadowmoon_soldier();
	new npc_shadowmoon_reaver();
	new npc_shadowmoon_deathshaper();
	new npc_shadowmoon_champion();
	new npc_whirling_blade();
	new npc_shadowmoon_houndmaster();
	new npc_shadowmoon_riding_hound();
	new npc_shadowmoon_weapon_master();
	new npc_bonechewer_behemoth();
	new npc_bonechewer_shield_disciple();
	new npc_bonechewer_blade_fury();
	new npc_bonechewer_blood_prophet();
	new npc_mutant_war_hound();
	new npc_bonechewer_brawler();
	new npc_bonechewer_combatant();
	new npc_bonechewer_spectator();
}