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
SDName: Boss_Fathomlord_Karathress
SD%Complete: 70
SDComment: Cyclone workaround
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumEscortAI.h"
#include "serpent_shrine.h"

#define SAY_GAIN_BLESSING_OF_TIDES      "Your overconfidence will be your undoing! Guards, lend me your strength!"
#define SOUND_GAIN_BLESSING_OF_TIDES    11278
#define SAY_MISC                        "Alana be'lendor!" // don't know what use this
#define SOUND_MISC                      11283

#define SAY_AGGRO                   -1548021
#define SAY_GAIN_BLESSING           -1548022
#define SAY_GAIN_ABILITY1           -1548023
#define SAY_GAIN_ABILITY2           -1548024
#define SAY_GAIN_ABILITY3           -1548025
#define SAY_SLAY1                   -1548026
#define SAY_SLAY2                   -1548027
#define SAY_SLAY3                   -1548028
#define SAY_DEATH                   -1548029

// Karathress spells
#define SPELL_CATACLYSMIC_BOLT          38441
#define SPELL_POWER_OF_SHARKKIS         38455
#define SPELL_POWER_OF_TIDALVESS        38452
#define SPELL_POWER_OF_CARIBDIS         38451
#define SPELL_ENRAGE                    24318
#define SPELL_SEAR_NOVA                 38445
#define SPELL_BLESSING_OF_THE_TIDES     38449
// Sharkkis spells
#define SPELL_LEECHING_THROW            29436
#define SPELL_THE_BEAST_WITHIN          38373
#define SPELL_MULTISHOT                 38366
#define SPELL_SUMMON_FATHOM_LURKER      38433
#define SPELL_SUMMON_FATHOM_SPOREBAT    38431
#define SPELL_PET_ENRAGE                19574
// Tidalvess spells
#define SPELL_FROST_SHOCK               38234
#define SPELL_SPITFIRE_TOTEM            38236
#define SPELL_POISON_CLEANSING_TOTEM    38306
#define SPELL_EARTHBIND_TOTEM           38304
#define SPELL_EARTHBIND_TOTEM_EFFECT    6474
#define SPELL_WINDFURY_WEAPON           38184
//Caribdis Spells
#define SPELL_WATER_BOLT_VOLLEY         38335
#define SPELL_TIDAL_SURGE               38358
#define SPELL_TIDAL_SURGE_FREEZE        38357
#define SPELL_HEAL                      38330
#define SPELL_SUMMON_CYCLONE            38337
#define SPELL_CYCLONE_CYCLONE           29538

#define SEER_OLUM              22820
#define OLUM_X                 446.78f
#define OLUM_Y                 -542.76f
#define OLUM_Z                 -7.54773f
#define OLUM_O                 0.401581f

#define MAX_ADVISORS 3
//Fathom-Lord Karathress AI
class boss_fathomlord_karathress : public CreatureScript
{
public:
    boss_fathomlord_karathress() : CreatureScript("boss_fathomlord_karathress") { }

    struct boss_fathomlord_karathressAI : public QuantumBasicAI
    {
        boss_fathomlord_karathressAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            Advisors[0] = 0;
            Advisors[1] = 0;
            Advisors[2] = 0;
        }

        InstanceScript* instance;

        uint32 CataclysmicBoltTimer;
        uint32 EnrageTimer;
        uint32 SearNovaTimer;

        bool BlessingOfTides;

        uint64 Advisors[MAX_ADVISORS];

        void Reset()
        {
            CataclysmicBoltTimer = 10000;
            EnrageTimer = 600000;                              //10 minutes
            SearNovaTimer = 20000+rand()%40000; // 20 - 60 seconds

            BlessingOfTides = false;

			uint64 RAdvisors[MAX_ADVISORS];
			RAdvisors[0] = instance->GetData64(DATA_SHARKKIS);
			RAdvisors[1] = instance->GetData64(DATA_TIDALVESS);
			RAdvisors[2] = instance->GetData64(DATA_CARIBDIS);
			//Respawn of the 3 Advisors
			Creature* pAdvisor = NULL;
			for (int i=0; i<MAX_ADVISORS; ++i)
			{
				if (RAdvisors[i])
				{
					pAdvisor = (Unit::GetCreature(*me, RAdvisors[i]));
					if (pAdvisor && !pAdvisor->IsAlive())
					{
						pAdvisor->Respawn();
						pAdvisor->AI()->EnterEvadeMode();
						pAdvisor->GetMotionMaster()->MoveTargetedHome();
					}
					instance->SetData(DATA_KARATHRESSEVENT, NOT_STARTED);
				}
			}
        }

        void EventSharkkisDeath()
        {
            DoSendQuantumText(SAY_GAIN_ABILITY1, me);
            DoCast(me, SPELL_POWER_OF_SHARKKIS);
        }

        void EventTidalvessDeath()
        {
            DoSendQuantumText(SAY_GAIN_ABILITY2, me);
            DoCast(me, SPELL_POWER_OF_TIDALVESS);
        }

        void EventCaribdisDeath()
        {
            DoSendQuantumText(SAY_GAIN_ABILITY3, me);
            DoCast(me, SPELL_POWER_OF_CARIBDIS);
        }

        void GetAdvisors()
        {
            Advisors[0] = instance->GetData64(DATA_SHARKKIS);
            Advisors[1] = instance->GetData64(DATA_TIDALVESS);
            Advisors[2] = instance->GetData64(DATA_CARIBDIS);
        }

        void StartEvent(Unit* who)
        {
            GetAdvisors();

            DoSendQuantumText(SAY_AGGRO, me);
            DoZoneInCombat();

            instance->SetData64(DATA_KARATHRESSEVENT_STARTER, who->GetGUID());
            instance->SetData(DATA_KARATHRESSEVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_FATHOMLORDKARATHRESSEVENT, DONE);

            //support for quest 10944
            me->SummonCreature(SEER_OLUM, OLUM_X, OLUM_Y, OLUM_Z, OLUM_O, TEMPSUMMON_TIMED_DESPAWN, 3600000);
        }

        void EnterToBattle(Unit* who)
        {
            StartEvent(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombatActive() && instance->GetData(DATA_KARATHRESSEVENT))
            {
                if (Unit* target = Unit::GetUnit(*me, instance->GetData64(DATA_KARATHRESSEVENT_STARTER)))
                {
                    AttackStart(target);
                    GetAdvisors();
                }
            }

            if (!UpdateVictim())
                return;

            if (!instance->GetData(DATA_KARATHRESSEVENT))
            {
                EnterEvadeMode();
                return;
            }

            if (CataclysmicBoltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_CATACLYSMIC_BOLT);
					CataclysmicBoltTimer = 10000;
				}
            }
			else CataclysmicBoltTimer -= diff;

            if (SearNovaTimer <= diff)
            {
                DoCastVictim(SPELL_SEAR_NOVA);
                SearNovaTimer = 20000+rand()%40000;
            }
			else SearNovaTimer -= diff;

            if (EnrageTimer <= diff)
            {
                DoCast(me, SPELL_ENRAGE);
                EnrageTimer = 90000;
            }
			else EnrageTimer -= diff;

            if (!HealthAbovePct(75) && !BlessingOfTides)
            {
                BlessingOfTides = true;
                bool continueTriggering = false;
                Creature* Advisor;
                for (uint8 i = 0; i < MAX_ADVISORS; ++i)
				{
                    if (Advisors[i])
                    {
                        Advisor = (Unit::GetCreature(*me, Advisors[i]));
                        if (Advisor && Advisor->IsAlive())
                        {
                            continueTriggering = true;
                            break;
                        }
                    }

					if (continueTriggering)
					{
						DoCast(me, SPELL_BLESSING_OF_THE_TIDES);
						me->MonsterYell(SAY_GAIN_BLESSING_OF_TIDES, LANG_UNIVERSAL, 0);
						DoPlaySoundToSet(me, SOUND_GAIN_BLESSING_OF_TIDES);
					}
				}
			}

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fathomlord_karathressAI(creature);
    }
};

class boss_fathomguard_sharkkis : public CreatureScript
{
public:
    boss_fathomguard_sharkkis() : CreatureScript("boss_fathomguard_sharkkis") { }

    struct boss_fathomguard_sharkkisAI : public QuantumBasicAI
    {
        boss_fathomguard_sharkkisAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 LeechingThrowTimer;
        uint32 TheBeastWithinTimer;
        uint32 MultishotTimer;
        uint32 PetTimer;

        bool pet;

        uint64 SummonedPet;

        void Reset()
        {
            LeechingThrowTimer = 20000;
            TheBeastWithinTimer = 30000;
            MultishotTimer = 15000;
            PetTimer = 10000;

            pet = false;

            if (Creature* Pet = Unit::GetCreature(*me, SummonedPet))
			{
				if (Pet->IsAlive())
					Pet->DealDamage(Pet, Pet->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
			}

            SummonedPet = 0;

			instance->SetData(DATA_KARATHRESSEVENT, NOT_STARTED);
        }

        void JustDied(Unit* /*victim*/)
        {
			Creature* Karathress = NULL;
			Karathress = (Unit::GetCreature(*me, instance->GetData64(DATA_KARATHRESS)));

			if (Karathress)
			{
				if (!me->IsAlive() && Karathress)
					CAST_AI(boss_fathomlord_karathress::boss_fathomlord_karathressAI, Karathress->AI())->EventSharkkisDeath();
            }
        }

        void EnterToBattle(Unit* who)
        {
			instance->SetData64(DATA_KARATHRESSEVENT_STARTER, who->GetGUID());
			instance->SetData(DATA_KARATHRESSEVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombatActive() && instance->GetData(DATA_KARATHRESSEVENT))
            {
                if (Unit* target = Unit::GetUnit(*me, instance->GetData64(DATA_KARATHRESSEVENT_STARTER)))
					AttackStart(target);
            }

            if (!UpdateVictim())
                return;

            if (!instance->GetData(DATA_KARATHRESSEVENT))
            {
                EnterEvadeMode();
                return;
            }

            if (LeechingThrowTimer <= diff)
            {
                DoCastVictim(SPELL_LEECHING_THROW);
                LeechingThrowTimer = 20000;
            }
			else LeechingThrowTimer -= diff;

            if (MultishotTimer <= diff)
            {
                DoCastVictim(SPELL_MULTISHOT);
                MultishotTimer = 20000;
            }
			else MultishotTimer -= diff;

            if (TheBeastWithinTimer <= diff)
            {
                DoCast(me, SPELL_THE_BEAST_WITHIN);

                if (Creature* Pet = Unit::GetCreature(*me, SummonedPet))
				{
					if (Pet->IsAlive())
						Pet->CastSpell(Pet, SPELL_PET_ENRAGE, true);
				}
                TheBeastWithinTimer = 30000;
            }
			else TheBeastWithinTimer -= diff;

            //PetTimer
            if (PetTimer <= diff && pet == false)
            {
                pet = true;
                //uint32 spell_id;
                uint32 pet_id;
                if (!urand(0, 1))
                {
                    //spell_id = SPELL_SUMMON_FATHOM_LURKER;
                    pet_id = NPC_FATHOM_LURKER;
                }
                else
                {
                    //spell_id = SPELL_SUMMON_FATHOM_SPOREBAT;
                    pet_id = NPC_FATHOM_SPOREBAT;
                }
                //DoCast(me, spell_id, true);
                Creature* Pet = DoSpawnCreature(pet_id, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    Pet->AI()->AttackStart(target);
                    SummonedPet = Pet->GetGUID();
                }
            }
			else PetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fathomguard_sharkkisAI(creature);
    }
};

class boss_fathomguard_tidalvess : public CreatureScript
{
public:
    boss_fathomguard_tidalvess() : CreatureScript("boss_fathomguard_tidalvess") { }

    struct boss_fathomguard_tidalvessAI : public QuantumBasicAI
    {
        boss_fathomguard_tidalvessAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 FrostShockTimer;
        uint32 SpitfireTimer;
        uint32 PoisonCleansingTimer;
        uint32 EarthbindTimer;

        void Reset()
        {
            FrostShockTimer = 25000;
            SpitfireTimer = 60000;
            PoisonCleansingTimer = 30000;
            EarthbindTimer = 45000;

			instance->SetData(DATA_KARATHRESSEVENT, NOT_STARTED);
        }

        void JustDied(Unit* /*killer*/)
        {
			Creature* Karathress = NULL;
			Karathress = (Unit::GetCreature(*me, instance->GetData64(DATA_KARATHRESS)));

			if (Karathress)
			{
				if (!me->IsAlive() && Karathress)
					CAST_AI(boss_fathomlord_karathress::boss_fathomlord_karathressAI, Karathress->AI())->EventTidalvessDeath();
            }
        }

        void EnterToBattle(Unit* who)
        {
			instance->SetData64(DATA_KARATHRESSEVENT_STARTER, who->GetGUID());
			instance->SetData(DATA_KARATHRESSEVENT, IN_PROGRESS);

            DoCast(me, SPELL_WINDFURY_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombatActive() && instance->GetData(DATA_KARATHRESSEVENT))
            {
                if (Unit* target = Unit::GetUnit(*me, instance->GetData64(DATA_KARATHRESSEVENT_STARTER)))
					AttackStart(target);
            }

            if (!UpdateVictim())
                return;

            if (!instance->GetData(DATA_KARATHRESSEVENT))
            {
                EnterEvadeMode();
                return;
            }

            if (!me->HasAura(SPELL_WINDFURY_WEAPON))
                DoCast(me, SPELL_WINDFURY_WEAPON);

            if (FrostShockTimer <= diff)
            {
                DoCastVictim(SPELL_FROST_SHOCK);
                FrostShockTimer = 25000+rand()%5000;
            }
			else FrostShockTimer -= diff;

            if (SpitfireTimer <= diff)
            {
                DoCast(me, SPELL_SPITFIRE_TOTEM);
                Unit* SpitfireTotem = Unit::GetUnit(*me, NPC_SPITFIRE_TOTEM);
                if (SpitfireTotem)
                    CAST_CRE(SpitfireTotem)->AI()->AttackStart(me->GetVictim());

                SpitfireTimer = 60000;
            }
			else SpitfireTimer -= diff;

            if (PoisonCleansingTimer <= diff)
            {
                DoCast(me, SPELL_POISON_CLEANSING_TOTEM);
                PoisonCleansingTimer = 30000;
            }
			else PoisonCleansingTimer -= diff;

            if (EarthbindTimer <= diff)
            {
                DoCast(me, SPELL_EARTHBIND_TOTEM);
                EarthbindTimer = 45000;
            }
			else EarthbindTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fathomguard_tidalvessAI(creature);
    }
};

class boss_fathomguard_caribdis : public CreatureScript
{
public:
    boss_fathomguard_caribdis() : CreatureScript("boss_fathomguard_caribdis") { }

    struct boss_fathomguard_caribdisAI : public QuantumBasicAI
    {
        boss_fathomguard_caribdisAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 WaterBoltVolleyTimer;
        uint32 TidalSurgeTimer;
        uint32 HealTimer;
        uint32 CycloneTimer;

        void Reset()
        {
            WaterBoltVolleyTimer = 35000;
            TidalSurgeTimer = 15000+rand()%5000;
            HealTimer = 55000;
            CycloneTimer = 30000+rand()%10000;

			instance->SetData(DATA_KARATHRESSEVENT, NOT_STARTED);
        }

        void JustDied(Unit* /*victim*/)
        {
			Creature* Karathress = NULL;
			Karathress = (Unit::GetCreature(*me, instance->GetData64(DATA_KARATHRESS)));

			if (Karathress)
			{
				if (!me->IsAlive() && Karathress)
					CAST_AI(boss_fathomlord_karathress::boss_fathomlord_karathressAI, Karathress->AI())->EventCaribdisDeath();
            }
        }

        void EnterToBattle(Unit* who)
        {
			instance->SetData64(DATA_KARATHRESSEVENT_STARTER, who->GetGUID());
			instance->SetData(DATA_KARATHRESSEVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombatActive() && instance->GetData(DATA_KARATHRESSEVENT))
            {
                Unit* target = Unit::GetUnit(*me, instance->GetData64(DATA_KARATHRESSEVENT_STARTER));

                if (target)
                    AttackStart(target);
            }

            if (!UpdateVictim())
                return;

            if (!instance->GetData(DATA_KARATHRESSEVENT))
            {
                EnterEvadeMode();
                return;
            }

            if (WaterBoltVolleyTimer <= diff)
            {
                DoCastVictim(SPELL_WATER_BOLT_VOLLEY);
                WaterBoltVolleyTimer = 30000;
            }
			else WaterBoltVolleyTimer -= diff;

            if (TidalSurgeTimer <= diff)
            {
                DoCastVictim(SPELL_TIDAL_SURGE);
                // Hacky way to do it - won't trigger elseways
                me->GetVictim()->CastSpell(me->GetVictim(), SPELL_TIDAL_SURGE_FREEZE, true);
                TidalSurgeTimer = 15000+rand()%5000;
            }
			else TidalSurgeTimer -= diff;

            if (CycloneTimer <= diff)
            {
                //DoCast(me, SPELL_SUMMON_CYCLONE); // Doesn't work
                CycloneTimer = 30000+rand()%10000;
                Creature* cyclone = me->SummonCreature(NPC_CYCLONE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), float(rand()%5), TEMPSUMMON_TIMED_DESPAWN, 15000);
                if (cyclone)
                {
                    CAST_CRE(cyclone)->SetObjectScale(3.0f);
                    cyclone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    cyclone->SetCurrentFaction(me->GetFaction());
                    cyclone->CastSpell(cyclone, SPELL_CYCLONE_CYCLONE, true);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        cyclone->AI()->AttackStart(target);
                }
            }
			else CycloneTimer -= diff;

            if (HealTimer <= diff)
            {
                // It can be cast on any of the mobs
                Unit* unit = NULL;

                while (unit == NULL || !unit->IsAlive())
                    unit = selectAdvisorUnit();

                if (unit->IsAlive())
                    DoCast(unit, SPELL_HEAL);

                HealTimer = 60000;
            }
			else HealTimer -= diff;

            DoMeleeAttackIfReady();
        }

		Unit* selectAdvisorUnit()
        {
            Unit* unit = NULL;
            if (instance)
            {
                switch (rand()%4)
                {
                    case 0:
						unit = Unit::GetUnit(*me, instance->GetData64(DATA_KARATHRESS));
						break;
					case 1:
						unit = Unit::GetUnit(*me, instance->GetData64(DATA_SHARKKIS));
						break;
					case 2:
						unit = Unit::GetUnit(*me, instance->GetData64(DATA_TIDALVESS));
						break;
					case 3:
						unit = me;
						break;
				}
            }
			else unit = me;
            return unit;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_fathomguard_caribdisAI(creature);
    }
};

void AddSC_boss_fathomlord_karathress()
{
    new boss_fathomlord_karathress();
    new boss_fathomguard_sharkkis();
    new boss_fathomguard_tidalvess();
    new boss_fathomguard_caribdis();
}