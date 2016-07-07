/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
#include "karazhan.h"

enum Texts
{
	SAY_AGGRO           = 0,
	SAY_SPECIAL         = 1,
	SAY_KILL            = 2,
	SAY_DEATH           = 3,
};

enum Spells
{
	SPELL_VANISH       = 29448,
	SPELL_GARROTE      = 37066,
	SPELL_BLIND        = 34694,
	SPELL_GOUGE        = 29425,
	SPELL_FRENZY       = 37023,
};

#define POS_Z               81.73f

float Locations[4][3] =
{
    {-10991.0f, -1884.33f, 0.614315f},
    {-10989.4f, -1885.88f, 0.904913f},
    {-10978.1f, -1887.07f, 2.035550f},
    {-10975.9f, -1885.81f, 2.253890f},
};

const uint32 Adds[6] =
{
    17007,
    19872,
    19873,
    19874,
    19875,
    19876,
};

class boss_moroes : public CreatureScript
{
public:
    boss_moroes() : CreatureScript("boss_moroes") { }

    struct boss_moroesAI : public QuantumBasicAI
    {
        boss_moroesAI(Creature* creature) : QuantumBasicAI(creature)
        {
			memset(AddId, 0, sizeof(AddId));
			memset(AddGUID, 0, sizeof(AddGUID));

            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint64 AddGUID[4];

        uint32 VanishTimer;
        uint32 BlindTimer;
        uint32 GougeTimer;
        uint32 WaitTimer;
        uint32 CheckAddsTimer;
        uint32 AddId[4];

        bool InVanish;
        bool Enrage;

        void Reset()
        {
            VanishTimer = 30000;
            BlindTimer = 35000;
            GougeTimer = 23000;
            WaitTimer = 0;
            CheckAddsTimer = 5000;

            Enrage = false;
            InVanish = false;

			if (me->GetHealth())
                SpawnAdds();

            if (instance)
                instance->SetData(TYPE_MOROES, NOT_STARTED);
        }

        void StartEvent()
        {
            if (instance)
                instance->SetData(TYPE_MOROES, IN_PROGRESS);

            DoZoneInCombat();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            StartEvent();

            Talk(SAY_AGGRO);
            AddsAttack();
            DoZoneInCombat();
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_KILL);
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);

            if (instance)
                instance->SetData(TYPE_MOROES, DONE);

            DeSpawnAdds();

            if (instance)
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GARROTE);
        }

        void SpawnAdds()
        {
            DeSpawnAdds();
            if (isAddlistEmpty())
            {
                Creature* creature = NULL;
                std::vector<uint32> AddList;

                for (uint8 i = 0; i < 6; ++i)
                    AddList.push_back(Adds[i]);

                while (AddList.size() > 4)
                    AddList.erase((AddList.begin())+(rand()%AddList.size()));

                uint8 i = 0;
                for (std::vector<uint32>::const_iterator itr = AddList.begin(); itr != AddList.end(); ++itr)
                {
                    uint32 entry = *itr;

                    creature = me->SummonCreature(entry, Locations[i][0], Locations[i][1], POS_Z, Locations[i][2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    if (creature)
                    {
                        AddGUID[i] = creature->GetGUID();
                        AddId[i] = entry;
                    }
                    ++i;
                }
            }
			else
            {
                for (uint8 i = 0; i < 4; ++i)
                {
                    Creature* creature = me->SummonCreature(AddId[i], Locations[i][0], Locations[i][1], POS_Z, Locations[i][2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    if (creature)
                        AddGUID[i] = creature->GetGUID();
                }
            }
        }

        bool isAddlistEmpty()
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                if (AddId[i] == 0)
                    return true;
            }
            return false;
        }

        void DeSpawnAdds()
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                Creature* Temp = NULL;
                if (AddGUID[i])
                {
                    Temp = Creature::GetCreature(*me, AddGUID[i]);
                    if (Temp && Temp->IsAlive())
                        Temp->DisappearAndDie();
                }
            }
        }

        void AddsAttack()
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                Creature* Temp = NULL;
                if (AddGUID[i])
                {
                    Temp = Creature::GetCreature(*me, AddGUID[i]);
                    if (Temp && Temp->IsAlive())
                    {
                        Temp->AI()->AttackStart(me->GetVictim());
                        DoZoneInCombat(Temp);
                    }
					else EnterEvadeMode();
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (instance && !instance->GetData(TYPE_MOROES))
            {
                EnterEvadeMode();
                return;
            }

            if (!Enrage && HealthBelowPct(30))
            {
                DoCast(me, SPELL_FRENZY);
                Enrage = true;
            }

            if (CheckAddsTimer <= diff)
            {
                for (uint8 i = 0; i < 4; ++i)
                {
                    Creature* Temp = NULL;
                    if (AddGUID[i])
                    {
                        Temp = Unit::GetCreature(*me, AddGUID[i]);
                        if (Temp && Temp->IsAlive())
						{
                            if (!Temp->GetVictim())
                                Temp->AI()->AttackStart(me->GetVictim());
						}
                    }
                }
                CheckAddsTimer = 5000;
            }
			else CheckAddsTimer -= diff;

            if (!Enrage)
            {
                if (VanishTimer <= diff)
                {
                    DoCast(me, SPELL_VANISH);
                    InVanish = true;
                    VanishTimer = 30000;
                    WaitTimer = 5000;
                }
				else VanishTimer -= diff;

                if (GougeTimer <= diff)
                {
                    DoCastVictim(SPELL_GOUGE);
                    GougeTimer = 40000;
                }
				else GougeTimer -= diff;

                if (BlindTimer <= diff)
                {
                    std::list<Unit*> targets;
                    SelectTargetList(targets, 5, TARGET_RANDOM, me->GetMeleeReach()*5, true);
                    for (std::list<Unit*>::const_iterator i = targets.begin(); i != targets.end(); ++i)
					{
                        if (!me->IsWithinMeleeRange(*i))
                        {
                            DoCast(*i, SPELL_BLIND);
                            break;
                        }
					}
                    BlindTimer = 40000;
                }
				else BlindTimer -= diff;
            }

            if (InVanish)
            {
                if (WaitTimer <= diff)
                {
					Talk(SAY_SPECIAL);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
					{
                        target->CastSpell(target, SPELL_GARROTE, true);
					}

                    InVanish = false;
                }
				else WaitTimer -= diff;
            }

            if (!InVanish)
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_moroesAI(creature);
    }
};

struct boss_moroes_guestAI : public QuantumBasicAI
{
    InstanceScript* instance;

    uint64 GuestGUID[4];

    boss_moroes_guestAI(Creature* creature) : QuantumBasicAI(creature)
    {
        for (uint8 i = 0; i < 4; ++i)
            GuestGUID[i] = 0;

        instance = creature->GetInstanceScript();
    }

    void Reset()
    {
        if (instance)
            instance->SetData(TYPE_MOROES, NOT_STARTED);
    }

	void AcquireGUID()
    {
        if (!instance)
            return;

        if (Creature* Moroes = Unit::GetCreature(*me, instance->GetData64(DATA_MOROES)))
		{
            for (uint8 i = 0; i < 4; ++i)
			{
                if (uint64 GUID = CAST_AI(boss_moroes::boss_moroesAI, Moroes->AI())->AddGUID[i])
                    GuestGUID[i] = GUID;
			}
		}
    }

    Unit* SelectGuestTarget()
    {
        uint64 TempGUID = GuestGUID[rand()%4];
        if (TempGUID)
        {
            Unit* unit = Unit::GetUnit(*me, TempGUID);
            if (unit && unit->IsAlive())
			{
                return unit;
			}
        }
        return me;
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (instance && !instance->GetData(TYPE_MOROES))
            EnterEvadeMode();

        DoMeleeAttackIfReady();
    }
};

#define SPELL_MANABURN         29405
#define SPELL_MIND_FLY         29570
#define SPELL_SHADOW_WORD_PAIN 34441
#define SPELL_SHADOWFORM       29406

#define SPELL_HAMMER_OF_JUSTICE     13005
#define SPELL_JUDGEMENT_OF_COMMAND  29386
#define SPELL_SEAL_OF_COMMAND       29385

#define SPELL_DISPEL_MAGIC          15090
#define SPELL_GREATER_HEAL          29564
#define SPELL_HOLYFIRE              29563
#define SPELL_POWER_WORD_SHIELD     29408

#define SPELL_CLEANSE                   29380
#define SPELL_GREATER_BLESSING_OF_MIGHT 29381
#define SPELL_HOLY_LIGHT                29562
#define SPELL_DIVINE_SHIELD             41367

#define SPELL_HAMSTRING         9080
#define SPELL_MORTAL_STRIKE     29572
#define SPELL_WHIRLWIND         29573

#define SPELL_DISARM            8379
#define SPELL_HEROIC_STRIKE     29567
#define SPELL_SHIELD_BASH       11972
#define SPELL_SHIELD_WALL       29390

class boss_baroness_dorothea_millstipe : public CreatureScript
{
public:
    boss_baroness_dorothea_millstipe() : CreatureScript("boss_baroness_dorothea_millstipe") { }

    struct boss_baroness_dorothea_millstipeAI : public boss_moroes_guestAI
    {
        boss_baroness_dorothea_millstipeAI(Creature* creature) : boss_moroes_guestAI(creature) {}

        uint32 ManaBurnTimer;
        uint32 MindFlayTimer;
        uint32 ShadowWordPainTimer;

        void Reset()
        {
            ManaBurnTimer = 1000;
            MindFlayTimer = 3000;
            ShadowWordPainTimer = 5000;

            DoCast(me, SPELL_SHADOWFORM, true);

            boss_moroes_guestAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            boss_moroes_guestAI::UpdateAI(diff);

            if (MindFlayTimer <= diff)
            {
                DoCastVictim(SPELL_MIND_FLY);
                MindFlayTimer = 4000;
            }
			else MindFlayTimer -= diff;

            if (ManaBurnTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    if (target && target->GetPowerType() == POWER_MANA)
					{
                        DoCast(target, SPELL_MANABURN);
						ManaBurnTimer = 6000;
					}
				}
            }
			else ManaBurnTimer -= diff;

            if (ShadowWordPainTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                {
                    DoCast(target, SPELL_SHADOW_WORD_PAIN);
                    ShadowWordPainTimer = 8000;
                }
            }
			else ShadowWordPainTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_baroness_dorothea_millstipeAI(creature);
    }
};

class boss_baron_rafe_dreuger : public CreatureScript
{
public:
    boss_baron_rafe_dreuger() : CreatureScript("boss_baron_rafe_dreuger") { }

    struct boss_baron_rafe_dreugerAI : public boss_moroes_guestAI
    {
        boss_baron_rafe_dreugerAI(Creature* creature) : boss_moroes_guestAI(creature){}

        uint32 HammerOfJusticeTimer;
        uint32 SealOfCommandTimer;
        uint32 JudgementOfCommandTimer;

        void Reset()
        {
            HammerOfJusticeTimer = 1000;
            SealOfCommandTimer = 7000;
            JudgementOfCommandTimer = SealOfCommandTimer + 29000;

            boss_moroes_guestAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            boss_moroes_guestAI::UpdateAI(diff);

            if (SealOfCommandTimer <= diff)
            {
                DoCast(me, SPELL_SEAL_OF_COMMAND);
                SealOfCommandTimer = 32000;
                JudgementOfCommandTimer = 29000;
            }
			else SealOfCommandTimer -= diff;

            if (JudgementOfCommandTimer <= diff)
            {
                DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
                JudgementOfCommandTimer = SealOfCommandTimer + 29000;
            }
			else JudgementOfCommandTimer -= diff;

            if (HammerOfJusticeTimer <= diff)
            {
                DoCastVictim(SPELL_HAMMER_OF_JUSTICE);
                HammerOfJusticeTimer = 12000;
            }
			else HammerOfJusticeTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_baron_rafe_dreugerAI(creature);
    }
};

class boss_lady_catriona_von_indi : public CreatureScript
{
public:
    boss_lady_catriona_von_indi() : CreatureScript("boss_lady_catriona_von_indi") { }

    struct boss_lady_catriona_von_indiAI : public boss_moroes_guestAI
    {
        //Holy Priest
        boss_lady_catriona_von_indiAI(Creature* creature) : boss_moroes_guestAI(creature) {}

        uint32 DispelMagicTimer;
        uint32 GreaterHealTimer;
        uint32 HolyFireTimer;
        uint32 PowerWordShieldTimer;

        void Reset()
        {
            DispelMagicTimer = 11000;
            GreaterHealTimer = 1500;
            HolyFireTimer = 5000;
            PowerWordShieldTimer = 1000;

            AcquireGUID();

            boss_moroes_guestAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            boss_moroes_guestAI::UpdateAI(diff);

            if (PowerWordShieldTimer <= diff)
            {
                DoCast(me, SPELL_POWER_WORD_SHIELD);
                PowerWordShieldTimer = 15000;
            }
			else PowerWordShieldTimer -= diff;

            if (GreaterHealTimer <= diff)
            {
                Unit* target = SelectGuestTarget();

                DoCast(target, SPELL_GREATER_HEAL);
                GreaterHealTimer = 17000;
            }
			else GreaterHealTimer -= diff;

            if (HolyFireTimer <= diff)
            {
                DoCastVictim(SPELL_HOLYFIRE);
                HolyFireTimer = 22000;
            }
			else HolyFireTimer -= diff;

            if (DispelMagicTimer <= diff)
            {
                if (Unit* target = RAND(SelectGuestTarget(), SelectTarget(TARGET_RANDOM, 0, 100, true)))
                    DoCast(target, SPELL_DISPEL_MAGIC);

                DispelMagicTimer = 25000;
            }
			else DispelMagicTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_catriona_von_indiAI(creature);
    }
};

class boss_lady_keira_berrybuck : public CreatureScript
{
public:
    boss_lady_keira_berrybuck() : CreatureScript("boss_lady_keira_berrybuck") { }

    struct boss_lady_keira_berrybuckAI : public boss_moroes_guestAI
    {
        //Holy Pally
        boss_lady_keira_berrybuckAI(Creature* creature) : boss_moroes_guestAI(creature)  {}

        uint32 CleanseTimer;
        uint32 GreaterBlessTimer;
        uint32 HolyLightTimer;
        uint32 DivineShieldTimer;

        void Reset()
        {
            CleanseTimer = 13000;
            GreaterBlessTimer = 1000;
            HolyLightTimer = 7000;
            DivineShieldTimer = 31000;
            AcquireGUID();

            boss_moroes_guestAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            boss_moroes_guestAI::UpdateAI(diff);

            if (DivineShieldTimer <= diff)
            {
                DoCast(me, SPELL_DIVINE_SHIELD);
                DivineShieldTimer = 31000;
            }
			else DivineShieldTimer -= diff;

            if (HolyLightTimer <= diff)
            {
                Unit* target = SelectGuestTarget();

                DoCast(target, SPELL_HOLY_LIGHT);
                HolyLightTimer = 10000;
            }
			else HolyLightTimer -= diff;

            if (GreaterBlessTimer <= diff)
            {
                Unit* target = SelectGuestTarget();
                DoCast(target, SPELL_GREATER_BLESSING_OF_MIGHT);
                GreaterBlessTimer = 50000;
            }
			else GreaterBlessTimer -= diff;

            if (CleanseTimer <= diff)
            {
                Unit* target = SelectGuestTarget();
                DoCast(target, SPELL_CLEANSE);
                CleanseTimer = 10000;

            }
			else CleanseTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_keira_berrybuckAI(creature);
    }
};

class boss_lord_robin_daris : public CreatureScript
{
public:
    boss_lord_robin_daris() : CreatureScript("boss_lord_robin_daris") { }

    struct boss_lord_robin_darisAI : public boss_moroes_guestAI
    {
        //Arms Warr
        boss_lord_robin_darisAI(Creature* creature) : boss_moroes_guestAI(creature) {}

        uint32 HamstringTimer;
        uint32 MortalStrikeTimer;
        uint32 WhirlWindTimer;

        void Reset()
        {
            HamstringTimer = 7000;
            MortalStrikeTimer = 10000;
            WhirlWindTimer = 21000;

            boss_moroes_guestAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            boss_moroes_guestAI::UpdateAI(diff);

            if (HamstringTimer <= diff)
            {
                DoCastVictim(SPELL_HAMSTRING);
                HamstringTimer = 12000;
            }
			else HamstringTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = 18000;
            }
			else MortalStrikeTimer -= diff;

            if (WhirlWindTimer <= diff)
            {
                DoCast(me, SPELL_WHIRLWIND);
                WhirlWindTimer = 21000;
            }
			else WhirlWindTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lord_robin_darisAI(creature);
    }
};

class boss_lord_crispin_ference : public CreatureScript
{
public:
    boss_lord_crispin_ference() : CreatureScript("boss_lord_crispin_ference") { }

    struct boss_lord_crispin_ferenceAI : public boss_moroes_guestAI
    {
        //Arms Warr
        boss_lord_crispin_ferenceAI(Creature* creature) : boss_moroes_guestAI(creature) {}

        uint32 DisarmTimer;
        uint32 HeroicStrikeTimer;
        uint32 ShieldBashTimer;
        uint32 ShieldWallTimer;

        void Reset()
        {
            DisarmTimer = 6000;
            HeroicStrikeTimer = 10000;
            ShieldBashTimer = 8000;
            ShieldWallTimer = 4000;
            boss_moroes_guestAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            boss_moroes_guestAI::UpdateAI(diff);

            if (DisarmTimer <= diff)
            {
                DoCastVictim(SPELL_DISARM);
                DisarmTimer = 12000;
            }
			else DisarmTimer -= diff;

            if (HeroicStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_HEROIC_STRIKE);
                HeroicStrikeTimer = 10000;
            }
			else HeroicStrikeTimer -= diff;

            if (ShieldBashTimer <= diff)
            {
                DoCastVictim(SPELL_SHIELD_BASH);
                ShieldBashTimer = 13000;
            }
			else ShieldBashTimer -= diff;

            if (ShieldWallTimer <= diff)
            {
                DoCast(me, SPELL_SHIELD_WALL);
                ShieldWallTimer = 21000;
            }
			else ShieldWallTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lord_crispin_ferenceAI(creature);
    }
};

void AddSC_boss_moroes()
{
    new boss_moroes();
    new boss_baroness_dorothea_millstipe();
    new boss_baron_rafe_dreuger();
    new boss_lady_catriona_von_indi();
    new boss_lady_keira_berrybuck();
    new boss_lord_robin_daris();
    new boss_lord_crispin_ference();
}