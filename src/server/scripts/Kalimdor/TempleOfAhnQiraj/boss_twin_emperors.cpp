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
#include "temple_of_ahnqiraj.h"

enum Spells
{
	SPELL_HEAL_BROTHER         = 7393,
	SPELL_TWIN_TELEPORT        = 800,
	SPELL_TWIN_TELEPORT_VISUAL = 26638,
	SPELL_EXPLODE_BUG          = 804,
	SPELL_MUTATE_BUG           = 802,
	SPELL_SHADOWBOLT           = 26006,
	SPELL_BLIZZARD             = 26607,
	SPELL_ARCANE_BURST         = 568,
	SPELL_UPPERCUT             = 26007,
	SPELL_UNBALANCING_STRIKE   = 26613,
	SPELL_BERSERK              = 26662,
	SPELL_BEAM                 = 39123,
	SPELL_EYE_LIGHT            = 53104,
};

enum Sounds
{
	SOUND_VN_DEATH              = 8660,
	SOUND_VN_AGGRO              = 8661,
	SOUND_VN_KILL               = 8662,
	SOUND_VL_AGGRO              = 8657,
	SOUND_VL_KILL               = 8658,
	SOUND_VL_DEATH              = 8659,
};

#define PULL_RANGE                  50
#define ABUSE_BUG_RANGE             20
#define TELEPORT_TIME               30000
#define VEKLOR_DIST                 20
#define HEAL_BROTHER_AMOUNT         30000.0f

struct boss_twin_emperorsAI : public QuantumBasicAI
{
    boss_twin_emperorsAI(Creature* creature): QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    uint32 HealTimer;
    uint32 TeleportTimer;
    uint32 AfterTeleportTimer;
    uint32 AbuseBugTimer, BugsTimer;
    uint32 EnrageTimer;
	uint32 CheckBrotherDeathTimer;

	uint64 TwinsGUIDs[2];

	bool tspellcasted;
	bool AfterTeleport;
	bool DontYellWhenDead;
	bool LoadedGUIDs;

    virtual bool IAmVeklor() = 0;
    virtual void Reset() = 0;
    virtual void CastSpellOnBug(Creature* target) = 0;

    void TwinReset()
    {
        HealTimer = 0;
		CheckBrotherDeathTimer = 1000;
        TeleportTimer = TELEPORT_TIME;
        AfterTeleport = false;
        tspellcasted = false;
        AfterTeleportTimer = 0;
        AbuseBugTimer = urand(10000, 17000);
        BugsTimer = 2000;
        me->ClearUnitState(UNIT_STATE_STUNNED);
        DontYellWhenDead = false;
        EnrageTimer = 15*60000;

		RespawnNearbies();

		me->GetMotionMaster()->MoveTargetedHome();

		DoCast(me, SPELL_UNIT_DETECTION_ALL);

		me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

		instance->HandleGameObject(instance->GetData64(DATA_TWINS_DOOR), true);
		instance->HandleGameObject(instance->GetData64(DATA_CTUN_DOOR), false);

		for (uint8 i = 0; i < 2; ++i)
			TwinsGUIDs[i] = 0;

        LoadedGUIDs = false;
    }

	void EnterToBattle(Unit* who)
    {
		RespawnNearbies();

		me->InterruptSpell(CURRENT_CHANNELED_SPELL);

        DoZoneInCombat();

        Creature* pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            QuantumBasicAI* otherAI = CAST_AI(QuantumBasicAI, pOtherBoss->AI());
            if (!pOtherBoss->IsInCombatActive())
            {
                DoPlaySoundToSet(me, IAmVeklor() ? SOUND_VL_AGGRO : SOUND_VN_AGGRO);
                otherAI->AttackStart(who);
                otherAI->DoZoneInCombat();

				if (!LoadedGUIDs)
					LoadGUIDs();
            }
        }

		instance->HandleGameObject(instance->GetData64(DATA_TWINS_DOOR), false);
		instance->HandleGameObject(instance->GetData64(DATA_CTUN_DOOR), false);
    }

	void KilledUnit(Unit* /*victim*/)
    {
        DoPlaySoundToSet(me, IAmVeklor() ? SOUND_VL_KILL : SOUND_VN_KILL);
    }

	void JustDied(Unit* /*killer*/)
    {
		DespawnNearbies();

        Creature* pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
            CAST_AI(boss_twin_emperorsAI, pOtherBoss->AI())->DontYellWhenDead = true;

        if (!DontYellWhenDead)
            DoPlaySoundToSet(me, IAmVeklor() ? SOUND_VL_DEATH : SOUND_VN_DEATH);

		instance->HandleGameObject(instance->GetData64(DATA_TWINS_DOOR), true);
		instance->HandleGameObject(instance->GetData64(DATA_CTUN_DOOR), true);
    }

	void LoadGUIDs()
    {
        TwinsGUIDs[0] = instance->GetData64(DATA_VEKNILASH);
        TwinsGUIDs[1] = instance->GetData64(DATA_VEKLOR);

        LoadedGUIDs = true;
    }

    Creature* GetOtherBoss()
    {
        if (instance)
            return Unit::GetCreature(*me, instance->GetData64(IAmVeklor() ? DATA_VEKNILASH : DATA_VEKLOR));
        else
            return NULL;
    }

	void DamageTaken(Unit* DoneBy, uint32 &damage)
    {
        if (DoneBy == me)
            return;

        //damage /= 4;
        for (uint8 i = 0; i < 2; ++i)
        {
            if (Creature* unit = Unit::GetCreature(*me, TwinsGUIDs[i]))
			{
                if (unit != me && unit->IsAlive())
                {
                    if (damage <= unit->GetHealth())
                    {
                        unit->LowerPlayerDamageReq(damage);
                        unit->SetHealth(unit->GetHealth() - damage);
                    }
                    else
                    {
                        unit->LowerPlayerDamageReq(damage);
                        unit->Kill(unit, false);
                    }
				}
			}
		}
	}

    void SpellHit(Unit* caster, const SpellInfo* entry)
    {
        if (caster == me)
            return;

        Creature* pOtherBoss = GetOtherBoss();
        if (entry->Id != SPELL_HEAL_BROTHER || !pOtherBoss)
            return;

        // add health so we keep same percentage for both brothers
        uint32 mytotal = me->GetMaxHealth(), histotal = pOtherBoss->GetMaxHealth();
        float mult = ((float)mytotal) / ((float)histotal);
        if (mult < 1)
            mult = 1.0f/mult;
        uint32 largerAmount = (uint32)((HEAL_BROTHER_AMOUNT * mult) - HEAL_BROTHER_AMOUNT);

        if (mytotal > histotal)
        {
            uint32 h = me->GetHealth()+largerAmount;
            me->SetHealth(std::min(mytotal, h));
        }
        else
        {
            uint32 h = pOtherBoss->GetHealth()+largerAmount;
            pOtherBoss->SetHealth(std::min(histotal, h));
        }
    }

    void TryHealBrother(uint32 diff)
    {
        if (IAmVeklor())                                    // this spell heals caster and the other brother so let VN cast it
            return;

        if (HealTimer <= diff)
        {
            Unit* pOtherBoss = GetOtherBoss();
            if (pOtherBoss && pOtherBoss->IsWithinDist(me, 60))
            {
                DoCast(pOtherBoss, SPELL_HEAL_BROTHER);
                HealTimer = 1000;
            }
        }
		else HealTimer -= diff;
    }

    void TeleportToMyBrother()
    {
        if (!instance)
            return;

        TeleportTimer = TELEPORT_TIME;

        if (IAmVeklor())
            return;

        Creature* pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            Position thisPos;
            thisPos.Relocate(me);
            Position otherPos;
            otherPos.Relocate(pOtherBoss);
            pOtherBoss->SetPosition(thisPos);
            me->SetPosition(otherPos);

            SetAfterTeleport();
            CAST_AI(boss_twin_emperorsAI,  pOtherBoss->AI())->SetAfterTeleport();
        }
    }

    void SetAfterTeleport()
    {
        me->InterruptNonMeleeSpells(false);
        me->AttackStop();
        DoResetThreat();
        DoCast(me, SPELL_TWIN_TELEPORT_VISUAL);
        me->AddUnitState(UNIT_STATE_STUNNED);
        AfterTeleport = true;
        AfterTeleportTimer = 2000;
        tspellcasted = false;
    }

    bool TryActivateAfterTTelep(uint32 diff)
    {
        if (AfterTeleport)
        {
            if (!tspellcasted)
            {
                me->ClearUnitState(UNIT_STATE_STUNNED);
                DoCast(me, SPELL_TWIN_TELEPORT);
                me->AddUnitState(UNIT_STATE_STUNNED);
            }

            tspellcasted = true;

            if (AfterTeleportTimer <= diff)
            {
                AfterTeleport = false;
                me->ClearUnitState(UNIT_STATE_STUNNED);
                if (Unit* nearu = me->SelectNearestTarget(100))
                {
                    AttackStart(nearu);
                    me->AddThreat(nearu, 10000);
                }
                return true;
            }
            else
            {
                AfterTeleportTimer -= diff;

                if (EnrageTimer > diff)
                    EnrageTimer -= diff;
                else
                    EnrageTimer = 0;
                if (TeleportTimer > diff)
                    TeleportTimer -= diff;
                else
                    TeleportTimer = 0;
                return false;
            }
        }
        else
        {
			return true;
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who || me->GetVictim())
            return;

        if (me->CanCreatureAttack(who))
        {
            float attackRadius = me->GetAttackDistance(who);
            if (attackRadius < PULL_RANGE)
                attackRadius = PULL_RANGE;

            if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= /*CREATURE_Z_ATTACK_RANGE*/7 /*there are stairs*/)
                AttackStart(who);
        }
    }

	void RespawnNearbies()
	{
		std::list<Creature*> TrashList;
		me->GetCreatureListWithEntryInGrid(TrashList, NPC_QUIRAJI_SCARAB, 350.0f);
		me->GetCreatureListWithEntryInGrid(TrashList, NPC_QUIRAJI_SCORPION, 350.0f);

		if (!TrashList.empty())
		{
			for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
			{
				if (Creature* trash = *itr)
					trash->Respawn();
			}
		}
	}

	void DespawnNearbies()
	{
		std::list<Creature*> TrashList;
		me->GetCreatureListWithEntryInGrid(TrashList, NPC_QUIRAJI_SCARAB, 350.0f);
		me->GetCreatureListWithEntryInGrid(TrashList, NPC_QUIRAJI_SCORPION, 350.0f);

		if (!TrashList.empty())
		{
			for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
			{
				if (Creature* trash = *itr)
					trash->Kill(trash);
			}
		}
	}

    Creature* RespawnNearbyBugsAndGetOne()
    {
        std::list<Creature*> UnitList;
        me->GetCreatureListWithEntryInGrid(UnitList, NPC_QUIRAJI_SCARAB, 350.0f);
        me->GetCreatureListWithEntryInGrid(UnitList, NPC_QUIRAJI_SCORPION, 350.0f);

        if (UnitList.empty())
            return NULL;

        Creature* nearb = NULL;

        for (std::list<Creature*>::const_iterator iter = UnitList.begin(); iter != UnitList.end(); ++iter)
        {
            Creature* creature = *iter;
            if (creature)
            {
                if (creature->IsDead())
                {
                    creature->Respawn();
                    creature->RemoveAllAuras();
                }
                if (creature->IsWithinDistInMap(me, ABUSE_BUG_RANGE))
                {
                    if (!nearb || (rand()%4) == 0)
                        nearb = creature;
                }
            }
        }
        return nearb;
    }

    void HandleBugs(uint32 diff)
    {
        if (BugsTimer < diff || AbuseBugTimer <= diff)
        {
            Creature* creature = RespawnNearbyBugsAndGetOne();
            if (AbuseBugTimer <= diff)
            {
                if (creature)
                {
                    CastSpellOnBug(creature);
                    AbuseBugTimer = urand(10000, 17000);
                }
                else
                {
                    AbuseBugTimer = 1000;
                }
            }
            else
            {
                AbuseBugTimer -= diff;
            }
            BugsTimer = 2000;
        }
        else
        {
            BugsTimer -= diff;
            AbuseBugTimer -= diff;
        }
    }

    void CheckEnrage(uint32 diff)
    {
        if (EnrageTimer <= diff)
        {
            if (!me->IsNonMeleeSpellCasted(true))
            {
                DoCast(me, SPELL_BERSERK);
                EnrageTimer = 60*60000;
            }
			else EnrageTimer = 0;
        }
		else EnrageTimer-=diff;
    }

	void CheckBrotherDeath(uint32 diff)
	{
		if (CheckBrotherDeathTimer <= diff)
        {
			Creature* Veklor = Unit::GetCreature(*me, instance->GetData64(DATA_VEKLOR));
			Creature* Veknilash = Unit::GetCreature(*me, instance->GetData64(DATA_VEKNILASH));

			if (!Veklor->IsAlive())
			{
				me->Kill(me);
				me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
			}

			if (!Veknilash->IsAlive())
			{
				me->Kill(me);
				me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
			}

			CheckBrotherDeathTimer = 1000;
		}
		else CheckBrotherDeathTimer -= diff;
	}
};

class boss_veknilash : public CreatureScript
{
public:
    boss_veknilash() : CreatureScript("boss_veknilash") { }

    struct boss_veknilashAI : public boss_twin_emperorsAI
    {
        bool IAmVeklor()
		{
			return false;
		}

        boss_veknilashAI(Creature* creature) : boss_twin_emperorsAI(creature) {}

		uint32 PurpleBeamTimer;
        uint32 UpperCutTimer;
        uint32 UnbalancingStrikeTimer;
        uint32 ScarabsTimer;

        int Rand;
        int RandX;
        int RandY;

        Creature* Summoned;

        void Reset()
        {
			TwinReset();
			PurpleBeamTimer = 500;
            UpperCutTimer = urand(14000, 29000);
            UnbalancingStrikeTimer = urand(8000, 18000);
            ScarabsTimer = urand(7000, 14000);

			me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);

			if (Creature* Veklor = Unit::GetCreature(*me, instance->GetData64(DATA_VEKLOR)))
			{
				if (!Veklor->IsAlive())
					Veklor->Respawn();
			}
        }

        void CastSpellOnBug(Creature* target)
        {
            target->AI()->AttackStart(me->getThreatManager().getHostilTarget());
            target->AddAura(SPELL_MUTATE_BUG, target);
            target->SetFullHealth();
        }

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* Eye = me->FindCreatureWithDistance(NPC_THE_MASTERS_EYE, 550.0f))
				{
					me->SetFacingToObject(Eye);
					Eye->CastSpell(Eye, SPELL_EYE_LIGHT, true);
					DoCast(Eye, SPELL_BEAM, true);
					PurpleBeamTimer = 120000; // 2 Minutes
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

            if (!TryActivateAfterTTelep(diff))
                return;

            if (UnbalancingStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_UNBALANCING_STRIKE);
                UnbalancingStrikeTimer = 8000+rand()%12000;
            }
			else UnbalancingStrikeTimer -= diff;

            if (UpperCutTimer <= diff)
            {
                if (Unit* randomMelee = SelectTarget(TARGET_RANDOM, 0, NOMINAL_MELEE_RANGE, true))
				{
                    DoCast(randomMelee, SPELL_UPPERCUT);
					UpperCutTimer = 15000+rand()%15000;
				}
            }
			else UpperCutTimer -= diff;

            HandleBugs(diff);

            TryHealBrother(diff);

            if (TeleportTimer <= diff)
            {
                TeleportToMyBrother();
            }
			else TeleportTimer -= diff;

            CheckEnrage(diff);

			CheckBrotherDeath(diff);

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_veknilashAI(creature);
    }
};

class boss_veklor : public CreatureScript
{
public:
    boss_veklor() : CreatureScript("boss_veklor") { }

    struct boss_veklorAI : public boss_twin_emperorsAI
    {
        bool IAmVeklor()
		{
			return true;
		}

        boss_veklorAI(Creature* creature) : boss_twin_emperorsAI(creature) {}

		uint32 PurpleBeamTimer;
        uint32 ShadowBoltTimer;
        uint32 BlizzardTimer;
        uint32 ArcaneBurstTimer;
        uint32 ScorpionsTimer;

        int Rand;
        int RandX;
        int RandY;

        Creature* Summoned;

        void Reset()
        {
			TwinReset();
			PurpleBeamTimer = 500;
            ShadowBoltTimer = 0;
            BlizzardTimer = urand(15000, 20000);
            ArcaneBurstTimer = 1000;
            ScorpionsTimer = urand(7000, 14000);

            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
            me->SetBaseWeaponDamage(BASE_ATTACK, MIN_DAMAGE, 0);
            me->SetBaseWeaponDamage(BASE_ATTACK, MAX_DAMAGE, 0);

			if (Creature* Veknilash = Unit::GetCreature(*me, instance->GetData64(DATA_VEKNILASH)))
			{
				if (!Veknilash->IsAlive())
					Veknilash->Respawn();
			}
		}

        void CastSpellOnBug(Creature* target)
        {
            target->AddAura(SPELL_EXPLODE_BUG, target);
            target->SetFullHealth();
		}

		void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (who->IsTargetableForAttack())
            {
                if (me->Attack(who, false))
                {
                    me->GetMotionMaster()->MoveChase(who, VEKLOR_DIST, 0);
                    me->AddThreat(who, 0.0f);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* Eye = me->FindCreatureWithDistance(NPC_THE_MASTERS_EYE, 550.0f))
				{
					me->SetFacingToObject(Eye);
					DoCast(Eye, SPELL_BEAM, true);
					PurpleBeamTimer = 120000; // 2 Minutes
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

            if (AfterTeleport)
                ArcaneBurstTimer = 5000;

            if (!TryActivateAfterTTelep(diff))
                return;

            if (ShadowBoltTimer <= diff)
            {
                if (!me->IsWithinDist(me->GetVictim(), 45.0f))
                    me->GetMotionMaster()->MoveChase(me->GetVictim(), VEKLOR_DIST, 0);
                else
                    DoCastVictim(SPELL_SHADOWBOLT);
                ShadowBoltTimer = 2000;
            }
			else ShadowBoltTimer -= diff;

            if (BlizzardTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 45, true))
				{
					DoCast(target, SPELL_BLIZZARD);
					BlizzardTimer = 15000+rand()%15000;
				}
            }
			else BlizzardTimer -= diff;

            if (ArcaneBurstTimer <= diff)
            {
                Unit* mvic;
                if ((mvic = SelectTarget(TARGET_NEAREST, 0, NOMINAL_MELEE_RANGE, true)) != NULL)
                {
                    DoCast(mvic, SPELL_ARCANE_BURST);
                    ArcaneBurstTimer = 5000;
                }
            } 
			else ArcaneBurstTimer -= diff;

            HandleBugs(diff);

            TryHealBrother(diff);

            if (TeleportTimer <= diff)
            {
                TeleportToMyBrother();
            }
			else TeleportTimer -= diff;

            CheckEnrage(diff);

			CheckBrotherDeath(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_veklorAI(creature);
    }
};

void AddSC_boss_twin_emperors()
{
    new boss_veknilash();
    new boss_veklor();
}