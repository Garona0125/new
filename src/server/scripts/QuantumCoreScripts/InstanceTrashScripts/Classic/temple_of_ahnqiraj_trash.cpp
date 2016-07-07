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
#include "../../../scripts/Kalimdor/TempleOfAhnQiraj/temple_of_ahnqiraj.h"

enum Spells
{
	SPELL_MENDING_BUFF        = 2147,
	SPELL_KNOCK_BUFF          = 21737,
	SPELL_KNOCK               = 25778,
	SPELL_MANAB_BUFF          = 812,
	SPELL_MANAB               = 25779,
	SPELL_REFLECTAF_BUFF      = 13022,
	SPELL_REFLECTSFR_BUFF     = 19595,
	SPELL_THORNS_BUFF         = 25777,
	SPELL_THUNDER_BUFF        = 2834,
	SPELL_THUNDER             = 8732,
	SPELL_MS_TRIKE_BUFF       = 9347,
	SPELL_MS_TRIKE            = 24573,
	SPELL_STORM_BUFF          = 2148,
	SPELL_STORM               = 26546,
	SPELL_ENRAGE              = 8599,
	SPELL_MAGIC_REFLECTION    = 20223,
	SPELL_DRAIN_MANA          = 25671,
	SPELL_SHOCK_BLAST         = 26458,
	SPELL_BERSERKER_CHARGE    = 22886,
	SPELL_PIERCING_SHRIEK     = 26379,
	SPELL_ACID_SPIT           = 24334,
	SPELL_CLEAVE              = 40504,
	SPELL_HEAD_BUTT           = 25788,
	SPELL_DEBILITATING_CHARGE = 1906,
	SPELL_AGGRO_DRONES        = 25152,
	SPELL_WHIRLWIND           = 26038,
	SPELL_KNOCKBACK           = 26027,
	SPELL_SUMM_VEKNISS_BORER  = 6122,
	SPELL_VW_BERSERKER_CHARGE = 19471,
	SPELL_POISON              = 25605,
	SPELL_SF_ENRAGE           = 15716,
	SPELL_IMPALE              = 26025,
	SPELL_RETALIATION         = 22857,
	SPELL_VENOM_SPIT          = 25497,
	SPELL_STINGER_CHARGE      = 25190,
	SPELL_BOULDER             = 21832,
	SPELL_SHADOW_STORM        = 14297,
	SPELL_THUNDERCLAP         = 23931,
	SPELL_METEOR              = 24340,
	SPELL_PLAGUE              = 22997,
	SPELL_SHADOWFROST_REFLECT = 19595,
	SPELL_EXPLODE             = 25698,
	SPELL_NULLIFY             = 26552,
	SPELL_SILENCE             = 26069,
	SPELL_FEAR                = 26070,
	SPELL_ENTANGLING_ROOTS    = 26071,
	SPELL_DUST_CLOUD          = 26072,
	SPELL_FIRE_NOVA           = 26073,
	SPELL_FRIGHTENING_SHOUT   = 19134,
	SPELL_KNOCK_AWAY          = 11130,
	SPELL_VENGEANCE           = 25164,
	SPELL_MIND_FLAY           = 26044,
	SPELL_MIND_BLAST          = 26048,
	SPELL_MANA_BURN           = 26049,
	SPELL_CAUSE_INSANITY      = 26079,
	SPELL_QS_ACID_SPIT        = 26050,
	SPELL_PIERCE_ARMOR        = 6016,
	SPELL_QS_KNOCK_AWAY       = 10101,
	SPELL_QS_ENRAGE           = 26041,
	SPELL_BATTLE_SHOUT        = 26043,
	SPELL_WHIRLWIND_AURA      = 13736,
	SPELL_UPPERCUT            = 10966,
	SPELL_TRAMPLE             = 15550,
	SPELL_AW_THUNDERCLAP      = 15588,
	SPELL_SUNDER_ARMOR        = 25051,
	SPELL_POISON_BOLT         = 26601,
	SPELL_CRIPPLING_POISON    = 25809,
	SPELL_MIND_NUMBING_POISON = 25810,
};

class npc_anubisath_sentinel : public CreatureScript
{
public:
    npc_anubisath_sentinel() : CreatureScript("npc_anubisath_sentinel") { }

    struct aqsentinelAI : public QuantumBasicAI
    {
        uint32 SpellId;
        int abselected;

        void SelectAbility(int random)
        {
            switch (random)
            {
                case 0:
					SpellId = SPELL_MENDING_BUFF;
					break;
                case 1:
					SpellId = SPELL_KNOCK_BUFF;
					break;
                case 2:
					SpellId = SPELL_MANAB_BUFF;
					break;
                case 3:
					SpellId = SPELL_REFLECTAF_BUFF;
					break;
                case 4:
					SpellId = SPELL_REFLECTSFR_BUFF;
					break;
                case 5:
					SpellId = SPELL_THORNS_BUFF;
					break;
                case 6:
					SpellId = SPELL_THUNDER_BUFF;
					break;
                case 7:
					SpellId = SPELL_MS_TRIKE_BUFF;
					break;
                case 8:
					SpellId = SPELL_STORM_BUFF;
					break;
            }
        }

        aqsentinelAI(Creature* creature) : QuantumBasicAI(creature)
        {
            ClearBuddyList();
            abselected = 0;                                     // just initialization of variable
        }

        uint64 NearbyGUID[3];

        void ClearBuddyList()
        {
            NearbyGUID[0] = NearbyGUID[1] = NearbyGUID[2] = 0;
        }

        void AddBuddyToList(uint64 CreatureGUID)
        {
            if (CreatureGUID == me->GetGUID())
                return;

            for (int i=0; i<3; ++i)
            {
                if (NearbyGUID[i] == CreatureGUID)
                    return;
                if (!NearbyGUID[i])
                {
                    NearbyGUID[i] = CreatureGUID;
                    return;
                }
            }
        }

        void GiveBuddyMyList(Creature* creature)
        {
            aqsentinelAI* cai = CAST_AI(aqsentinelAI, (creature)->AI());
            for (int i=0; i<3; ++i)
                if (NearbyGUID[i] && NearbyGUID[i] != creature->GetGUID())
                    cai->AddBuddyToList(NearbyGUID[i]);
            cai->AddBuddyToList(me->GetGUID());
        }

        void SendMyListToBuddies()
        {
            for (int i=0; i<3; ++i)
			{
                if (Creature* pNearby = Unit::GetCreature(*me, NearbyGUID[i]))
                    GiveBuddyMyList(pNearby);
			}
        }

        void CallBuddiesToAttack(Unit* who)
        {
            for (int i=0; i<3; ++i)
            {
                Creature* creature = Unit::GetCreature(*me, NearbyGUID[i]);
                if (creature)
                {
                    if (!creature->IsInCombatActive())
                    {
                        creature->SetNoCallAssistance(true);
                        if (creature->AI())
                            creature->AI()->AttackStart(who);
                    }
                }
            }
        }

        void AddSentinelsNear(Unit* /*nears*/)
        {
            std::list<Creature*> assistList;
            me->GetCreatureListWithEntryInGrid(assistList, NPC_ANUBISATH_SENTINEL, 70.0f);

            if (assistList.empty())
                return;

            for (std::list<Creature*>::const_iterator iter = assistList.begin(); iter != assistList.end(); ++iter)
                AddBuddyToList((*iter)->GetGUID());
        }

        int pickAbilityRandom(bool *chosenAbilities)
        {
            for (int t = 0; t < 2; ++t)
            {
                for (int i = !t ? (rand()%9) : 0; i < 9; ++i)
                {
                    if (!chosenAbilities[i])
                    {
                        chosenAbilities[i] = true;
                        return i;
                    }
                }
            }
            return 0;
        }

        void GetOtherSentinels(Unit* who)
        {
            bool *chosenAbilities = new bool[9];
            memset(chosenAbilities, 0, 9*sizeof(bool));
            SelectAbility(pickAbilityRandom(chosenAbilities));

            ClearBuddyList();
            AddSentinelsNear(me);
            int bli;
            for (bli = 0; bli < 3; ++bli)
            {
                if (!NearbyGUID[bli])
                    break;

                Creature* pNearby = Unit::GetCreature(*me, NearbyGUID[bli]);
                if (!pNearby)
                    break;

                AddSentinelsNear(pNearby);
                CAST_AI(aqsentinelAI, pNearby->AI())->gatherOthersWhenAggro = false;
                CAST_AI(aqsentinelAI, pNearby->AI())->SelectAbility(pickAbilityRandom(chosenAbilities));
            }

            SendMyListToBuddies();
            CallBuddiesToAttack(who);

            delete[] chosenAbilities;

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
        }

        bool gatherOthersWhenAggro;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if (!me->IsDead())
            {
                for (int i=0; i<3; ++i)
                {
                    if (!NearbyGUID[i])
                        continue;

                    if (Creature* nearby = Unit::GetCreature(*me, NearbyGUID[i]))
                    {
                        if (nearby->IsDead())
                            nearby->Respawn();
                    }
                }
            }
            ClearBuddyList();
            gatherOthersWhenAggro = true;
        }

        void GainSentinelAbility(uint32 id)
        {
            me->AddAura(id, me);
        }

        void EnterToBattle(Unit* who)
        {
            if (gatherOthersWhenAggro)
                GetOtherSentinels(who);

            GainSentinelAbility(SpellId);
            DoZoneInCombat();
        }

        void JustDied(Unit* /*who*/)
        {
            for (int ni=0; ni<3; ++ni)
            {
				Creature* sent = Unit::GetCreature(*me, NearbyGUID[ni]);
                if (!sent)
                    continue;

                if (sent->IsDead())
                    continue;

                sent->ModifyHealth(int32(sent->CountPctFromMaxHealth(HEALTH_PERCENT_50)));
                CAST_AI(aqsentinelAI, sent->AI())->GainSentinelAbility(SpellId);
            }
        }

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new aqsentinelAI(creature);
    }
};

class npc_anubisath_swarmguard : public CreatureScript
{
public:
    npc_anubisath_swarmguard() : CreatureScript("npc_anubisath_swarmguard") { }

    struct npc_anubisath_swarmguardAI : public QuantumBasicAI
    {
        npc_anubisath_swarmguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubisath_swarmguardAI(creature);
    }
};

class npc_anubisath_defender : public CreatureScript
{
public:
    npc_anubisath_defender() : CreatureScript("npc_anubisath_defender") { }

    struct npc_anubisath_defenderAI : public QuantumBasicAI
    {
        npc_anubisath_defenderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
		}

		uint32 RandomAbilityTimer;
		uint32 PlagueTimer;

        void Reset()
        {
			RandomAbilityTimer = 0.5*IN_MILLISECONDS;
			PlagueTimer = 1500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SHADOWFROST_REFLECT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RandomAbilityTimer <= diff)
			{
				DoCast(me, RAND(SPELL_SHADOW_STORM, SPELL_THUNDERCLAP, SPELL_METEOR));
				RandomAbilityTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RandomAbilityTimer -= diff;

			if (PlagueTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PLAGUE);
					PlagueTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else PlagueTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			if (HealthBelowPct(HEALTH_PERCENT_20))
				DoCast(me, SPELL_EXPLODE);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubisath_defenderAI(creature);
    }
};

class npc_anubisath_warder : public CreatureScript
{
public:
    npc_anubisath_warder() : CreatureScript("npc_anubisath_warder") { }

    struct npc_anubisath_warderAI : public QuantumBasicAI
    {
        npc_anubisath_warderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
		}

		uint32 RandomAbilityTimer;

        void Reset()
        {
			RandomAbilityTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RandomAbilityTimer <= diff)
			{
				DoCast(me, RAND(SPELL_SILENCE, SPELL_FEAR, SPELL_ENTANGLING_ROOTS, SPELL_DUST_CLOUD, SPELL_FIRE_NOVA));
				RandomAbilityTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RandomAbilityTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubisath_warderAI(creature);
    }
};

class npc_anubisath_warrior : public CreatureScript
{
public:
    npc_anubisath_warrior() : CreatureScript("npc_anubisath_warrior") { }

    struct npc_anubisath_warriorAI : public QuantumBasicAI
    {
        npc_anubisath_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ThunderclapTimer;
		uint32 TrampleTimer;
		uint32 UppercutTimer;

        void Reset()
        {
			ThunderclapTimer = 2*IN_MILLISECONDS;
			TrampleTimer = 4*IN_MILLISECONDS;
			UppercutTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_AW_THUNDERCLAP);
				ThunderclapTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ThunderclapTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else UppercutTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubisath_warriorAI(creature);
    }
};

class npc_vekniss_soldier : public CreatureScript
{
public:
    npc_vekniss_soldier() : CreatureScript("npc_vekniss_soldier") { }

    struct npc_vekniss_soldierAI : public QuantumBasicAI
    {
        npc_vekniss_soldierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DebilitatingChargeTimer;
		uint32 CleaveTimer;

        void Reset()
        {
			DebilitatingChargeTimer = 200;
			CleaveTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AGGRO_DRONES, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DebilitatingChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEBILITATING_CHARGE);
					DebilitatingChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else DebilitatingChargeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vekniss_soldierAI(creature);
    }
};

class npc_vekniss_warrior : public CreatureScript
{
public:
    npc_vekniss_warrior() : CreatureScript("npc_vekniss_warrior") { }

    struct npc_vekniss_warriorAI : public QuantumBasicAI
    {
        npc_vekniss_warriorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BeserkerChargeTimer;
		uint32 SummonBorrerTimer;

		SummonList Summons;

        void Reset()
        {
			BeserkerChargeTimer = 200;
			SummonBorrerTimer = 1*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_VEKNISS_BORER)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BeserkerChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VW_BERSERKER_CHARGE);
					BeserkerChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else BeserkerChargeTimer -= diff;

			if (SummonBorrerTimer <= diff)
			{
				DoCastAOE(SPELL_SUMM_VEKNISS_BORER);
				SummonBorrerTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else SummonBorrerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vekniss_warriorAI(creature);
    }
};

class npc_vekniss_drone : public CreatureScript
{
public:
    npc_vekniss_drone() : CreatureScript("npc_vekniss_drone") { }

    struct npc_vekniss_droneAI : public QuantumBasicAI
    {
        npc_vekniss_droneAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vekniss_droneAI(creature);
    }
};

class npc_vekniss_guardian : public CreatureScript
{
public:
    npc_vekniss_guardian() : CreatureScript("npc_vekniss_guardian") { }

    struct npc_vekniss_guardianAI : public QuantumBasicAI
    {
        npc_vekniss_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ImpaleTimer;

        void Reset()
        {
			ImpaleTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImpaleTimer <= diff)
			{
				DoCastAOE(SPELL_IMPALE);
				ImpaleTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ImpaleTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vekniss_guardianAI(creature);
    }
};

class npc_vekniss_stinger : public CreatureScript
{
public:
    npc_vekniss_stinger() : CreatureScript("npc_vekniss_stinger") { }

    struct npc_vekniss_stingerAI : public QuantumBasicAI
    {
        npc_vekniss_stingerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StingerChargeTimer;
		uint32 BoulderTimer;

        void Reset()
        {
			StingerChargeTimer = 200;
			BoulderTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StingerChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_STINGER_CHARGE);
					StingerChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else StingerChargeTimer -= diff;

			if (BoulderTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BOULDER);
					BoulderTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else BoulderTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vekniss_stingerAI(creature);
    }
};

class npc_vekniss_hive_crawler : public CreatureScript
{
public:
    npc_vekniss_hive_crawler() : CreatureScript("npc_vekniss_hive_crawler") { }

    struct npc_vekniss_hive_crawlerAI : public QuantumBasicAI
    {
        npc_vekniss_hive_crawlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PoisonBoltTimer;
		uint32 SunderArmorTimer;
		uint32 DoublePoisonTimer;

        void Reset()
        {
			PoisonBoltTimer = 0.5*IN_MILLISECONDS;
			SunderArmorTimer = 1*IN_MILLISECONDS;
			DoublePoisonTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POISON_BOLT);
					PoisonBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else PoisonBoltTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SunderArmorTimer -= diff;

			if (DoublePoisonTimer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLING_POISON, true);
				DoCastVictim(SPELL_MIND_NUMBING_POISON, true);
				DoublePoisonTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DoublePoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vekniss_hive_crawlerAI(creature);
    }
};

class npc_hive_zara_drone : public CreatureScript
{
public:
    npc_hive_zara_drone() : CreatureScript("npc_hive_zara_drone") { }

    struct npc_hive_zara_droneAI : public QuantumBasicAI
    {
        npc_hive_zara_droneAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BeserkerChargeTimer;
		uint32 PoisonTimer;

        void Reset()
        {
			BeserkerChargeTimer = 200;
			PoisonTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BeserkerChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BERSERKER_CHARGE);
					BeserkerChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else BeserkerChargeTimer -= diff;

			if (PoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POISON);
					PoisonTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_droneAI(creature);
    }
};

class npc_hive_zara_soldier : public CreatureScript
{
public:
    npc_hive_zara_soldier() : CreatureScript("npc_hive_zara_soldier") { }

    struct npc_hive_zara_soldierAI : public QuantumBasicAI
    {
        npc_hive_zara_soldierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RetaliationTimer;
		uint32 VenomSpitTimer;

        void Reset()
        {
			RetaliationTimer = 0.5*IN_MILLISECONDS;
			VenomSpitTimer = 1500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RetaliationTimer <= diff)
			{
				DoCast(me, SPELL_RETALIATION);
				RetaliationTimer = urand(9*IN_MILLISECONDS, 1*IN_MILLISECONDS);
			}
			else RetaliationTimer -= diff;

			if (VenomSpitTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VENOM_SPIT);
					VenomSpitTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else VenomSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_soldierAI(creature);
    }
};

class npc_obsidian_eradicator : public CreatureScript
{
public:
    npc_obsidian_eradicator() : CreatureScript("npc_obsidian_eradicator") { }

    struct npc_obsidian_eradicatorAI : public QuantumBasicAI
    {
        npc_obsidian_eradicatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MagicReflectionTimer;
		uint32 DrainManaTimer;
		uint32 ShockBlastTimer;

        void Reset()
        {
			MagicReflectionTimer = 0.5*IN_MILLISECONDS;
			DrainManaTimer = 1*IN_MILLISECONDS;
			ShockBlastTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MagicReflectionTimer <= diff)
			{
				DoCast(me, SPELL_MAGIC_REFLECTION, true);
				MagicReflectionTimer = urand(12*IN_MILLISECONDS, 13*IN_MILLISECONDS);
			}
			else MagicReflectionTimer -= diff;

			if (DrainManaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_DRAIN_MANA);
						DrainManaTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
					}
				}
			}
			else DrainManaTimer -= diff;

			if (ShockBlastTimer <= diff)
			{
				DoCastAOE(SPELL_SHOCK_BLAST, true);
				ShockBlastTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShockBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_obsidian_eradicatorAI(creature);
    }
};

class npc_obsidian_nullifier : public CreatureScript
{
public:
    npc_obsidian_nullifier() : CreatureScript("npc_obsidian_nullifier") { }

    struct npc_obsidian_nullifierAI : public QuantumBasicAI
    {
        npc_obsidian_nullifierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DrainManaTimer;
		uint32 CleaveTimer;
		uint32 NullifyTimer;

        void Reset()
        {
			DrainManaTimer = 0.5*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;
			NullifyTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DrainManaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_DRAIN_MANA);
						DrainManaTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else DrainManaTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (NullifyTimer <= diff)
			{
				DoCastAOE(SPELL_NULLIFY, true);
				NullifyTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else NullifyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_obsidian_nullifierAI(creature);
    }
};

class npc_qiraji_champion : public CreatureScript
{
public:
    npc_qiraji_champion() : CreatureScript("npc_qiraji_champion") { }

    struct npc_qiraji_championAI : public QuantumBasicAI
    {
        npc_qiraji_championAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
		}

		uint32 CleaveTimer;
		uint32 FrighteningShoutTimer;
		uint32 KnockAwayTimer;
		uint32 VengeanceTimer;

        void Reset()
        {
			CleaveTimer = 1*IN_MILLISECONDS;
			FrighteningShoutTimer = 2*IN_MILLISECONDS;
			KnockAwayTimer = 4*IN_MILLISECONDS;
			VengeanceTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastAOE(SPELL_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrighteningShoutTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastAOE(SPELL_KNOCK_AWAY);
				KnockAwayTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else KnockAwayTimer -= diff;

			if (VengeanceTimer <= diff)
			{
				DoCast(me, SPELL_VENGEANCE);
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				VengeanceTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else VengeanceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_championAI(creature);
    }
};

class npc_qiraji_mindslayer : public CreatureScript
{
public:
    npc_qiraji_mindslayer() : CreatureScript("npc_qiraji_mindslayer") { }

    struct npc_qiraji_mindslayerAI : public QuantumBasicAI
    {
        npc_qiraji_mindslayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
		}

		uint32 MindFlayTimer;
		uint32 MindBlastTimer;
		uint32 ManaBurnTimer;
		uint32 CauseInsanityTimer;

        void Reset()
        {
			MindFlayTimer = 0.5*IN_MILLISECONDS;
			MindBlastTimer = 2*IN_MILLISECONDS;
			ManaBurnTimer = 4*IN_MILLISECONDS;
			CauseInsanityTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MindFlayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_FLAY);
					MindFlayTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindFlayTimer -= diff;

			if (MindBlastTimer <= diff)
			{
				DoCastAOE(SPELL_MIND_BLAST);
				MindBlastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MindBlastTimer -= diff;

			if (ManaBurnTimer <= diff)
			{
				DoCastAOE(SPELL_MANA_BURN);
				ManaBurnTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ManaBurnTimer -= diff;

			if (CauseInsanityTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CAUSE_INSANITY);
					CauseInsanityTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else CauseInsanityTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_mindslayerAI(creature);
    }
};

class npc_qiraji_brainwasher : public CreatureScript
{
public:
    npc_qiraji_brainwasher() : CreatureScript("npc_qiraji_brainwasher") { }

    struct npc_qiraji_brainwasherAI : public QuantumBasicAI
    {
        npc_qiraji_brainwasherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MindFlayTimer;
		uint32 ManaBurnTimer;
		uint32 CauseInsanityTimer;

        void Reset()
        {
			MindFlayTimer = 0.5*IN_MILLISECONDS;
			ManaBurnTimer = 2*IN_MILLISECONDS;
			CauseInsanityTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MindFlayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MIND_FLAY);
					MindFlayTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else MindFlayTimer -= diff;

			if (ManaBurnTimer <= diff)
			{
				DoCastAOE(SPELL_MANA_BURN);
				ManaBurnTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ManaBurnTimer -= diff;

			if (CauseInsanityTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CAUSE_INSANITY);
					CauseInsanityTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else CauseInsanityTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_brainwasherAI(creature);
    }
};

class npc_qiraji_scorpion : public CreatureScript
{
public:
    npc_qiraji_scorpion() : CreatureScript("npc_qiraji_scorpion") { }

    struct npc_qiraji_scorpionAI : public QuantumBasicAI
    {
        npc_qiraji_scorpionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PierceArmorTimer;
		uint32 AcidSpitTimer;

        void Reset()
        {
			PierceArmorTimer = 1*IN_MILLISECONDS;
			AcidSpitTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCE_ARMOR);
				PierceArmorTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			if (AcidSpitTimer <= diff)
			{
				DoCastVictim(SPELL_QS_ACID_SPIT);
				AcidSpitTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else AcidSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_scorpionAI(creature);
    }
};

class npc_qiraji_scarab : public CreatureScript
{
public:
    npc_qiraji_scarab() : CreatureScript("npc_qiraji_scarab") { }

    struct npc_qiraji_scarabAI : public QuantumBasicAI
    {
        npc_qiraji_scarabAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PierceArmorTimer;
		uint32 AcidSpitTimer;

        void Reset()
        {
			PierceArmorTimer = 1*IN_MILLISECONDS;
			AcidSpitTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCE_ARMOR);
				PierceArmorTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PierceArmorTimer -= diff;

			if (AcidSpitTimer <= diff)
			{
				DoCastVictim(SPELL_QS_ACID_SPIT);
				AcidSpitTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else AcidSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_scarabAI(creature);
    }
};

class npc_qiraji_slayer : public CreatureScript
{
public:
    npc_qiraji_slayer() : CreatureScript("npc_qiraji_slayer") { }

    struct npc_qiraji_slayerAI : public QuantumBasicAI
    {
        npc_qiraji_slayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BattleShoutTimer;
		uint32 WhirlwindTimer;
		uint32 EnrageTimer;
		uint32 KnockAwayTimer;

        void Reset()
        {
			BattleShoutTimer = 1*IN_MILLISECONDS;
			WhirlwindTimer = 2*IN_MILLISECONDS;
			EnrageTimer = 3*IN_MILLISECONDS;
			KnockAwayTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_BATTLE_SHOUT);
				BattleShoutTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else BattleShoutTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCast(me, SPELL_WHIRLWIND_AURA);
				WhirlwindTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			if (EnrageTimer <= diff)
			{
				DoCast(me, SPELL_QS_ENRAGE);
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				EnrageTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else EnrageTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_QS_KNOCK_AWAY);
				KnockAwayTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
			}
			else KnockAwayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_slayerAI(creature);
    }
};

class npc_qiraji_lasher : public CreatureScript
{
public:
    npc_qiraji_lasher() : CreatureScript("npc_qiraji_lasher") { }

    struct npc_qiraji_lasherAI : public QuantumBasicAI
    {
        npc_qiraji_lasherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WhirlwindTimer;
		uint32 KnockbackTimer;

        void Reset()
        {
			WhirlwindTimer = 2*IN_MILLISECONDS;
			KnockbackTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(25.0f);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			if (KnockbackTimer <= diff)
			{
				DoCastAOE(SPELL_KNOCKBACK);
				KnockbackTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else KnockbackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_lasherAI(creature);
    }
};

class npc_shrieker_scarab : public CreatureScript
{
public:
    npc_shrieker_scarab() : CreatureScript("npc_shrieker_scarab") { }

    struct npc_shrieker_scarabAI : public QuantumBasicAI
    {
        npc_shrieker_scarabAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BeserkerChargeTimer;
		uint32 PiercngShriekTimer;

        void Reset()
        {
			BeserkerChargeTimer = 200;
			PiercngShriekTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BeserkerChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BERSERKER_CHARGE);
					BeserkerChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else BeserkerChargeTimer -= diff;

			if (PiercngShriekTimer <= diff)
			{
				DoCastAOE(SPELL_PIERCING_SHRIEK);
				PiercngShriekTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else PiercngShriekTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shrieker_scarabAI(creature);
    }
};

class npc_spitting_scarab : public CreatureScript
{
public:
    npc_spitting_scarab() : CreatureScript("npc_spitting_scarab") { }

    struct npc_spitting_scarabAI : public QuantumBasicAI
    {
        npc_spitting_scarabAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BeserkerChargeTimer;
		uint32 AcidSpitTimer;

        void Reset()
        {
			BeserkerChargeTimer = 200;
			AcidSpitTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BeserkerChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BERSERKER_CHARGE);
					BeserkerChargeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else BeserkerChargeTimer -= diff;

			if (AcidSpitTimer <= diff)
			{
				DoCastVictim(SPELL_ACID_SPIT);
				AcidSpitTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else AcidSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spitting_scarabAI(creature);
    }
};

class npc_yauj_brood : public CreatureScript
{
public:
    npc_yauj_brood() : CreatureScript("npc_yauj_brood") { }

    struct npc_yauj_broodAI : public QuantumBasicAI
    {
        npc_yauj_broodAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 HeadButtTimer;

        void Reset()
        {
			HeadButtTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeadButtTimer <= diff)
			{
				DoCastVictim(SPELL_HEAD_BUTT);
				HeadButtTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else HeadButtTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_yauj_broodAI(creature);
    }
};

class npc_spawn_of_fankriss : public CreatureScript
{
public:
    npc_spawn_of_fankriss() : CreatureScript("npc_spawn_of_fankriss") { }

    struct npc_spawn_of_fankrissAI : public QuantumBasicAI
    {
        npc_spawn_of_fankrissAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_SF_ENRAGE, 0))
				{
					DoCast(me, SPELL_SF_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spawn_of_fankrissAI(creature);
    }
};

class npc_sarturas_royal_guard : public CreatureScript
{
public:
    npc_sarturas_royal_guard() : CreatureScript("npc_sarturas_royal_guard") { }

    struct npc_sarturas_royal_guardAI : public QuantumBasicAI
    {
        npc_sarturas_royal_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WhirlwindTimer;
		uint32 KnockbackTimer;

        void Reset()
        {
			WhirlwindTimer = 2*IN_MILLISECONDS;
			KnockbackTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(25.0f);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			if (KnockbackTimer <= diff)
			{
				DoCastAOE(SPELL_KNOCKBACK);
				KnockbackTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else KnockbackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sarturas_royal_guardAI(creature);
    }
};

class npc_swarmguard_needler : public CreatureScript
{
public:
    npc_swarmguard_needler() : CreatureScript("npc_swarmguard_needler") { }

    struct npc_swarmguard_needlerAI : public QuantumBasicAI
    {
        npc_swarmguard_needlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AGGRO_DRONES, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swarmguard_needlerAI(creature);
    }
};

void AddSC_temple_of_ahnqiraj_trash()
{
    new npc_anubisath_sentinel();
	new npc_anubisath_swarmguard();
	new npc_anubisath_defender();
	new npc_anubisath_warder();
	new npc_anubisath_warrior();
	new npc_vekniss_soldier();
	new npc_vekniss_warrior();
	new npc_vekniss_drone();
	new npc_vekniss_guardian();
	new npc_vekniss_stinger();
	new npc_vekniss_hive_crawler();
	new npc_hive_zara_drone();
	new npc_hive_zara_soldier();
	new npc_obsidian_eradicator();
	new npc_obsidian_nullifier();
	new npc_qiraji_champion();
	new npc_qiraji_mindslayer();
	new npc_qiraji_brainwasher();
	new npc_qiraji_scorpion();
	new npc_qiraji_scarab();
	new npc_qiraji_slayer();
	new npc_qiraji_lasher();
	new npc_shrieker_scarab();
	new npc_spitting_scarab();
	new npc_yauj_brood();
	new npc_spawn_of_fankriss();
	new npc_sarturas_royal_guard();
	new npc_swarmguard_needler();
}