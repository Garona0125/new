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
#include "magisters_terrace.h"

struct Speech
{
    int32 id;
};

const float fOrientation = 4.98f;
const float fZLocation = -19.921f;

float LackeyLocations[4][2]=
{
    {123.77f, 17.6007f},
    {131.731f, 15.0827f},
    {121.563f, 15.6213f},
    {129.988f, 17.2355f},
};

const uint32 AddEntries[] =
{
    NPC_KAGANI,
    NPC_ELLRYS,
    NPC_ERAMAS,
    NPC_YAZZAI,
    NPC_SALARIS,
    NPC_GARAXXAS,
    NPC_APOKO,
    NPC_ZELFAN,
};

static Speech LackeyDeath[] =
{
	{1},
	{2},
	{3},
	{4},
};

static Speech PlayerDeath[] =
{
	{5},
	{6},
	{7},
	{8},
	{9},
};

enum Texts
{
    SAY_AGGRO        = 0,
    SAY_DEATH        = 10,
};

enum Spells
{
	SPELL_DISPEL_MAGIC      = 27609,
    SPELL_FLASH_HEAL        = 17843,
    SPELL_SW_PAIN_5N        = 14032,
    SPELL_SW_PAIN_5H        = 15654,
    SPELL_SHIELD            = 44291,
    SPELL_RENEW_5N          = 44174,
    SPELL_RENEW_5H          = 46192,
	SPELL_IMMUNITY          = 46227, // Need Implement
	SPELL_HEALING_POTION    = 15503,
	// Kagani
	SPELL_CRIPPLING_POISON  = 44289, // Need Implement
	SPELL_KIDNEY_SHOT       = 27615,
    SPELL_GOUGE             = 12540,
    SPELL_KICK              = 27613,
    SPELL_VANISH            = 44290,
    SPELL_BACKSTAB_5N       = 15657,
	SPELL_BACKSTAB_5H       = 15582,
    SPELL_EVISCERATE_5N     = 27611,
	SPELL_EVISCERATE_5H     = 46189,
	// Ellrys
	SPELL_IMMOLATE_5N        = 44267,
	SPELL_IMMOLATE_5H        = 46191,
    SPELL_SHADOW_BOLT_5N     = 12471,
	SPELL_SHADOW_BOLT_5H     = 15232,
    SPELL_SEED_OF_CORRUPTION = 44141,
    SPELL_CURSE_OF_AGONY_5N  = 14875,
	SPELL_CURSE_OF_AGONY_5H  = 46190,
    SPELL_FEAR               = 38595,
    SPELL_IMP_FIREBALL       = 44164,
    SPELL_SUMMON_IMP         = 44163,
	SPELL_DEATH_COIL         = 44142, // Need Implement
	// Eramas
	SPELL_FISTS_OF_ARCANE_FURY = 44120, // Need Implement
	SPELL_KNOCKDOWN_5N         = 11428,
	SPELL_KNOCKDOWN_5H         = 46183,
    SPELL_SNAP_KICK            = 46182,
	// Yazai
	SPELL_POLYMORPH          = 13323,
    SPELL_ICE_BLOCK          = 27619,
    SPELL_BLIZZARD_5N        = 44178,
	SPELL_BLIZZARD_5H        = 46195,
    SPELL_ICE_LANCE_5N       = 44176,
	SPELL_ICE_LANCE_5H       = 46194,
    SPELL_CONE_OF_COLD_5N    = 12611,
	SPELL_CONE_OF_COLD_5H    = 38384,
    SPELL_FROSTBOLT_5N       = 15043,
	SPELL_FROSTBOLT_5H       = 15530,
    SPELL_BLINK              = 14514,
	// Salaris
	SPELL_INTERCEPT          = 27577,
    SPELL_DISARM             = 27581,
    SPELL_PIERCING_HOWL      = 23600,
    SPELL_FRIGHTENING_SHOUT  = 19134,
    SPELL_HAMSTRING          = 27584,
    SPELL_BATTLE_SHOUT       = 27578,
    SPELL_MORTAL_STRIKE      = 44268,
	// Garaxxas
	SPELL_AIMED_SHOT         = 44271,
    SPELL_SHOOT_5N           = 15620,
	SPELL_SHOOT_5H           = 22907,
    SPELL_CONCUSSIVE_SHOT    = 27634,
    SPELL_MULTI_SHOT_5N      = 44285,
	SPELL_MULTI_SHOT_5H      = 31942,
    SPELL_WING_CLIP          = 44286,
    SPELL_FREEZING_TRAP      = 44136,
	SPELL_IMPROVED_WING_CLIP = 47168,
	// Apoko
	SPELL_WINDFURY_TOTEM      = 27621,
    SPELL_WAR_STOMP           = 46026,
    SPELL_PURGE               = 27626,
    SPELL_HEALING_WAVE_5N     = 44256,
	SPELL_HEALING_WAVE_5H     = 46181,
    SPELL_FROST_SHOCK_5N      = 21401,
	SPELL_FROST_SHOCK_5H      = 46180,
    SPELL_FIRE_NOVA_TOTEM     = 44257,
    SPELL_EARTHBIND_TOTEM     = 15786,
	// Zelfan
    SPELL_GOBLIN_DRAGON_GUN_5N = 44272,
	SPELL_GOBLIN_DRAGON_GUN_5H = 46185,
	SPELL_FEL_IRON_BOMB_5N     = 46024,
	SPELL_FEL_IRON_BOMB_5H     = 46184,
    SPELL_ROCKET_LAUNCH_5N     = 44138,
	SPELL_ROCKET_LAUNCH_5H     = 46188,
    SPELL_RECOMBOBULATE        = 44274,
	SPELL_HIGH_EXPLOSIVE_SHEEP = 44276,
};

enum Lackey
{
	MAX_ACTIVE_LACKEY = 4,
};

class boss_priestess_delrissa : public CreatureScript
{
public:
    boss_priestess_delrissa() : CreatureScript("boss_priestess_delrissa") { }

    struct boss_priestess_delrissaAI : public QuantumBasicAI
    {
        boss_priestess_delrissaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            memset(&LackeyGUID, 0, sizeof(LackeyGUID));
            LackeyEntryList.clear();
        }

        InstanceScript* instance;

        std::vector<uint32> LackeyEntryList;
        uint64 LackeyGUID[MAX_ACTIVE_LACKEY];
        uint8 PlayersKilled;
        uint32 HealTimer;
        uint32 RenewTimer;
        uint32 ShieldTimer;
        uint32 ShadoWordPainTimer;
        uint32 DispelTimer;
        uint32 ResetTimer;

        void Reset()
        {
            PlayersKilled = 0;
            HealTimer = 15000;
            RenewTimer = 10000;
            ShieldTimer = 2000;
            ShadoWordPainTimer = 5000;
            DispelTimer = 7500;
            ResetTimer = 5000;
            InitializeLackeys();
        }

        void JustReachedHome()
        {
            if (instance)
                 instance->SetData(DATA_DELRISSA_EVENT, FAIL);
        }

        void EnterToBattle(Unit* who)
        {
            Talk(SAY_AGGRO);

            for (uint8 i = 0; i < MAX_ACTIVE_LACKEY; ++i)
            {
                if (Unit* Add = Unit::GetUnit(*me, LackeyGUID[i]))
                {
                    if (!Add->GetVictim())
                    {
                        who->SetInCombatWith(Add);
                        Add->AddThreat(who, 0.0f);
                    }
                }
            }

            if (instance)
                instance->SetData(DATA_DELRISSA_EVENT, IN_PROGRESS);
        }

        void InitializeLackeys()
        {
            if (!me->IsAlive())
                return;

            uint8 j = 0;

            if (LackeyEntryList.empty())
            {
                LackeyEntryList.resize((sizeof(AddEntries) / sizeof(uint32)));

                for (uint8 i = 0; i < LackeyEntryList.size(); ++i)
                    LackeyEntryList[i] = AddEntries[i];

                while (LackeyEntryList.size() > MAX_ACTIVE_LACKEY)
                    LackeyEntryList.erase(LackeyEntryList.begin() + rand()%LackeyEntryList.size());

                for (std::vector<uint32>::const_iterator itr = LackeyEntryList.begin(); itr != LackeyEntryList.end(); ++itr)
                {
                    if (Creature* Add = me->SummonCreature((*itr), LackeyLocations[j][0], LackeyLocations[j][1], fZLocation, fOrientation, TEMPSUMMON_CORPSE_DESPAWN, 0))
                        LackeyGUID[j] = Add->GetGUID();

                    ++j;
                }
            }
            else
            {
                for (std::vector<uint32>::const_iterator itr = LackeyEntryList.begin(); itr != LackeyEntryList.end(); ++itr)
                {
                    Unit* Add = Unit::GetUnit(*me, LackeyGUID[j]);

                    if (!Add)
                    {
                        Add = me->SummonCreature((*itr), LackeyLocations[j][0], LackeyLocations[j][1], fZLocation, fOrientation, TEMPSUMMON_CORPSE_DESPAWN, 0);
                        if (Add)
                            LackeyGUID[j] = Add->GetGUID();
                    }
                    ++j;
                }
            }
        }

        void KilledUnit(Unit* who)
        {
            if (who->GetTypeId() != TYPE_ID_PLAYER)
                return;

			Talk(PlayerDeath[PlayersKilled].id);

            if (PlayersKilled < 4)
                ++PlayersKilled;
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);

            if (instance)
				instance->SetData(DATA_DELRISSA_EVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ResetTimer <= diff)
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                if (me->GetPositionZ() >= z+10)
                {
                    EnterEvadeMode();
                    return;
                }
                ResetTimer = 5000;
            }
			else ResetTimer -= diff;

            if (HealTimer <= diff)
            {
                uint32 health = me->GetHealth();
                Unit* target = me;
                for (uint8 i = 0; i < MAX_ACTIVE_LACKEY; ++i)
                {
                    if (Unit* Add = Unit::GetUnit(*me, LackeyGUID[i]))
                    {
                        if (Add->IsAlive() && Add->GetHealth() < health)
                            target = Add;
                    }
                }

                DoCast(target, SPELL_FLASH_HEAL);
                HealTimer = 15000;
            }
			else HealTimer -= diff;

            if (RenewTimer <= diff)
            {
                Unit* target = me;

                if (urand(0, 1))
                    if (Unit* Add = Unit::GetUnit(*me, LackeyGUID[rand()%MAX_ACTIVE_LACKEY]))
                        if (Add->IsAlive())
                            target = Add;

                DoCast(target, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
                RenewTimer = 5000;
            }
			else RenewTimer -= diff;

            if (ShieldTimer <= diff)
            {
                Unit* target = me;

                if (urand(0, 1))
                    if (Unit* Add = Unit::GetUnit(*me, LackeyGUID[rand()%MAX_ACTIVE_LACKEY]))
                        if (Add->IsAlive() && !Add->HasAura(SPELL_SHIELD))
                            target = Add;

                DoCast(target, SPELL_SHIELD);
                ShieldTimer = 7500;
            }
			else ShieldTimer -= diff;

            if (DispelTimer <= diff)
            {
                Unit* target = NULL;

                if (urand(0, 1))
                    target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                else
                {
                    if (urand(0, 1))
                        target = me;
                    else
                        if (Unit* Add = Unit::GetUnit(*me, LackeyGUID[rand()%MAX_ACTIVE_LACKEY]))
                            if (Add->IsAlive())
                                target = Add;
                }

                if (target)
                    DoCast(target, SPELL_DISPEL_MAGIC);

                DispelTimer = 12000;
            }
			else DispelTimer -= diff;

            if (ShadoWordPainTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SW_PAIN_5N, SPELL_SW_PAIN_5H));
					ShadoWordPainTimer = 10000;
				}
            }
			else ShadoWordPainTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_priestess_delrissaAI(creature);
    }
};

struct boss_priestess_lackey_commonAI : public QuantumBasicAI
{
    boss_priestess_lackey_commonAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
        memset(&LackeyGUIDs, 0, sizeof(LackeyGUIDs));
        AcquireGUIDs();
    }

    InstanceScript* instance;

    uint64 LackeyGUIDs[MAX_ACTIVE_LACKEY];
    uint32 ResetThreatTimer;

    bool UsedPotion;

    void Reset()
    {
        UsedPotion = false;

        ResetThreatTimer = urand(5000, 20000);

        if (Creature* delrissa = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_DELRISSA) : 0))
        {
            if (!delrissa->IsAlive())
                delrissa->Respawn();
        }
    }

    void EnterToBattle(Unit* who)
    {
        if (!who)
            return;

        if (instance)
        {
            for (uint8 i = 0; i < MAX_ACTIVE_LACKEY; ++i)
            {
                if (Unit* Add = Unit::GetUnit(*me, LackeyGUIDs[i]))
                {
                    if (!Add->GetVictim() && Add != me)
                    {
                        who->SetInCombatWith(Add);
                        Add->AddThreat(who, 0.0f);
                    }
                }
            }

            if (Creature* delrissa = Unit::GetCreature(*me, instance->GetData64(DATA_DELRISSA)))
            {
                if (delrissa->IsAlive() && !delrissa->GetVictim())
                {
                    who->SetInCombatWith(delrissa);
                    delrissa->AddThreat(who, 0.0f);
                }
            }
        }
    }

	void JustDied(Unit* /*killer*/){}

    void KilledUnit(Unit* who)
    {
        if (Creature* delrissa = Unit::GetCreature(*me, instance->GetData64(DATA_DELRISSA)))
            delrissa->AI()->KilledUnit(who);
    }

    void AcquireGUIDs()
    {
        if (Creature* delrissa = (Unit::GetCreature(*me, instance->GetData64(DATA_DELRISSA))))
        {
            for (uint8 i = 0; i < MAX_ACTIVE_LACKEY; ++i)
                LackeyGUIDs[i] = CAST_AI(boss_priestess_delrissa::boss_priestess_delrissaAI, delrissa->AI())->LackeyGUID[i];
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UsedPotion && HealthBelowPct(25))
        {
            DoCast(me, SPELL_HEALING_POTION);
            UsedPotion = true;
        }

        if (ResetThreatTimer <= diff)
        {
            DoResetThreat();
            ResetThreatTimer = urand(5000, 20000);
        }
		else ResetThreatTimer -= diff;
    }
};

class boss_kagani_nightstrike : public CreatureScript
{
public:
    boss_kagani_nightstrike() : CreatureScript("boss_kagani_nightstrike") { }

    struct boss_kagani_nightstrikeAI : public boss_priestess_lackey_commonAI
    {
        boss_kagani_nightstrikeAI(Creature* creature) : boss_priestess_lackey_commonAI(creature) {}

        uint32 GougeTimer;
        uint32 KickTimer;
        uint32 VanishTimer;
        uint32 EviscerateTimer;
        uint32 WaitTimer;
        bool InVanish;

        void Reset()
        {
            GougeTimer = 5500;
            KickTimer = 7000;
            VanishTimer = 2000;
            EviscerateTimer = 6000;
            WaitTimer = 5000;
            InVanish = false;
            me->SetVisible(true);

            boss_priestess_lackey_commonAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (VanishTimer <= diff)
            {
                DoCast(me, SPELL_VANISH);
                Unit* unit = SelectTarget(TARGET_RANDOM, 0);
                DoResetThreat();

                if (unit)
                    me->AddThreat(unit, 1000.0f);

                InVanish = true;
                VanishTimer = 30000;
                WaitTimer = 10000;
            }
			else VanishTimer -= diff;

            if (InVanish)
            {
                if (WaitTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_BACKSTAB_5N, SPELL_BACKSTAB_5H));
                    DoCastVictim(SPELL_KIDNEY_SHOT, true);
                    me->SetVisible(true);
                    InVanish = false;
                }
				else WaitTimer -= diff;
            }

            if (GougeTimer <= diff)
            {
                DoCastVictim(SPELL_GOUGE);
                GougeTimer = 5500;
            }
			else GougeTimer -= diff;

            if (KickTimer <= diff)
            {
                DoCastVictim(SPELL_KICK);
                KickTimer = 7000;
            }
			else KickTimer -= diff;

            if (EviscerateTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_EVISCERATE_5N, SPELL_EVISCERATE_5H));
                EviscerateTimer = 4000;
            }
			else EviscerateTimer -= diff;

            if (!InVanish)
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kagani_nightstrikeAI(creature);
    }
};

class boss_ellrys_duskhallow : public CreatureScript
{
public:
    boss_ellrys_duskhallow() : CreatureScript("boss_ellrys_duskhallow") { }

    struct boss_ellrys_duskhallowAI : public boss_priestess_lackey_commonAI
    {
        boss_ellrys_duskhallowAI(Creature* creature) : boss_priestess_lackey_commonAI(creature), Summons(me){}

        uint32 ImmolateTimer;
        uint32 ShadowBoltTimer;
        uint32 SeedofCorruptionTimer;
        uint32 CurseofAgonyTimer;
        uint32 FearTimer;
		uint32 DeathCoilTimer;

		SummonList Summons;

        void Reset()
        {
            ImmolateTimer = 6000;
            ShadowBoltTimer = 3000;
            SeedofCorruptionTimer = 2000;
            CurseofAgonyTimer = 1000;
            FearTimer = 10000;

			Summons.DespawnAll();

            boss_priestess_lackey_commonAI::Reset();
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_SUMMON_IMP, true);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_FIZZLE)
			{
				Summons.Summon(summon);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (ImmolateTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_IMMOLATE_5N, SPELL_IMMOLATE_5H));
                ImmolateTimer = 6000;
            }
			else ImmolateTimer -= diff;

            if (ShadowBoltTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
                ShadowBoltTimer = 5000;
            }
			else ShadowBoltTimer -= diff;

            if (SeedofCorruptionTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_SEED_OF_CORRUPTION);
					SeedofCorruptionTimer = 10000;
				}
            }
			else SeedofCorruptionTimer -= diff;

            if (CurseofAgonyTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_CURSE_OF_AGONY_5N, SPELL_CURSE_OF_AGONY_5H));
					CurseofAgonyTimer = 13000;
				}
            }
			else CurseofAgonyTimer -= diff;

            if (FearTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_FEAR);
					FearTimer = 10000;
				}
            }
			else FearTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ellrys_duskhallowAI(creature);
    }
};

class boss_eramas_brightblaze : public CreatureScript
{
public:
    boss_eramas_brightblaze() : CreatureScript("boss_eramas_brightblaze") { }

    struct boss_eramas_brightblazeAI : public boss_priestess_lackey_commonAI
    {
        boss_eramas_brightblazeAI(Creature* creature) : boss_priestess_lackey_commonAI(creature) {}

        uint32 KnockdownTimer;
        uint32 SnapKickTimer;

        void Reset()
        {
            KnockdownTimer = 6000;
            SnapKickTimer = 4500;

            boss_priestess_lackey_commonAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (KnockdownTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_KNOCKDOWN_5N, SPELL_KNOCKDOWN_5H));
                KnockdownTimer = 6000;
            }
			else KnockdownTimer -= diff;

            if (SnapKickTimer <= diff)
            {
                DoCastVictim(SPELL_SNAP_KICK);
                SnapKickTimer = 4500;
            }
			else SnapKickTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_eramas_brightblazeAI(creature);
    }
};

class boss_yazzai : public CreatureScript
{
public:
    boss_yazzai() : CreatureScript("boss_yazzai") { }

    struct boss_yazzaiAI : public boss_priestess_lackey_commonAI
    {
        boss_yazzaiAI(Creature* creature) : boss_priestess_lackey_commonAI(creature) {}

        bool HasIceBlocked;

        uint32 PolymorphTimer;
        uint32 IceBlockTimer;
        uint32 WaitTimer;
        uint32 BlizzardTimer;
        uint32 IceLanceTimer;
        uint32 ConeofColdTimer;
        uint32 FrostboltTimer;
        uint32 BlinkTimer;

        void Reset()
        {
            HasIceBlocked = false;

            PolymorphTimer = 1000;
            IceBlockTimer = 20000;
            WaitTimer = 10000;
            BlizzardTimer = 8000;
            IceLanceTimer = 12000;
            ConeofColdTimer = 10000;
            FrostboltTimer = 3000;
            BlinkTimer = 8000;

            boss_priestess_lackey_commonAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (PolymorphTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, SPELL_POLYMORPH);
                    PolymorphTimer = 20000;
                }
            }
			else PolymorphTimer -= diff;

            if (HealthBelowPct(35) && !HasIceBlocked)
            {
                DoCast(me, SPELL_ICE_BLOCK);
                HasIceBlocked = true;
            }

            if (BlizzardTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_BLIZZARD_5N, SPELL_BLIZZARD_5H));
					BlizzardTimer = 8000;
				}
            }
			else BlizzardTimer -= diff;

            if (IceLanceTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_ICE_LANCE_5N, SPELL_ICE_LANCE_5H));
                IceLanceTimer = 12000;
            }
			else IceLanceTimer -= diff;

            if (ConeofColdTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_CONE_OF_COLD_5N, SPELL_CONE_OF_COLD_5H));
                ConeofColdTimer = 10000;
            }
			else ConeofColdTimer -= diff;

            if (FrostboltTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
                FrostboltTimer = 8000;
            }
			else FrostboltTimer -= diff;

            if (BlinkTimer <= diff)
            {
                bool InMeleeRange = false;
                std::list<HostileReference*>& t_list = me->getThreatManager().getThreatList();
                for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    if (Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                    {
                        //if in melee range
                        if (target->IsWithinDistInMap(me, 5))
                        {
                            InMeleeRange = true;
                            break;
                        }
                    }
                }

                if (InMeleeRange)
                    DoCast(me, SPELL_BLINK);

                BlinkTimer = 8000;
            }
			else BlinkTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_yazzaiAI(creature);
    }
};

class boss_warlord_salaris : public CreatureScript
{
public:
    boss_warlord_salaris() : CreatureScript("boss_warlord_salaris") { }

    struct boss_warlord_salarisAI : public boss_priestess_lackey_commonAI
    {
        boss_warlord_salarisAI(Creature* creature) : boss_priestess_lackey_commonAI(creature) {}

        uint32 InterceptStunTimer;
        uint32 DisarmTimer;
        uint32 PiercingHowlTimer;
        uint32 FrighteningShoutTimer;
        uint32 HamstringTimer;
        uint32 MortalStrikeTimer;

        void Reset()
        {
            InterceptStunTimer = 500;
            DisarmTimer = 6000;
            PiercingHowlTimer = 10000;
            FrighteningShoutTimer = 18000;
            HamstringTimer = 4500;
            MortalStrikeTimer = 8000;

            boss_priestess_lackey_commonAI::Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_BATTLE_SHOUT);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (InterceptStunTimer <= diff)
            {
                bool InMeleeRange = false;
                std::list<HostileReference*>& t_list = me->getThreatManager().getThreatList();
                for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    if (Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                    {
                        //if in melee range
                        if (target->IsWithinDistInMap(me, ATTACK_DISTANCE))
                        {
                            InMeleeRange = true;
                            break;
                        }
                    }
                }

                if (!InMeleeRange)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_INTERCEPT);
						InterceptStunTimer = 10000;
					}
				}
            }
			else InterceptStunTimer -= diff;

            if (DisarmTimer <= diff)
            {
                DoCastVictim(SPELL_DISARM);
                DisarmTimer = 6000;
            }
			else DisarmTimer -= diff;

            if (HamstringTimer <= diff)
            {
                DoCastVictim(SPELL_HAMSTRING);
                HamstringTimer = 4500;
            }
			else HamstringTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = 4500;
            }
			else MortalStrikeTimer -= diff;

            if (PiercingHowlTimer <= diff)
            {
                DoCastVictim(SPELL_PIERCING_HOWL);
                PiercingHowlTimer = 10000;
            }
			else PiercingHowlTimer -= diff;

            if (FrighteningShoutTimer <= diff)
            {
                DoCastVictim(SPELL_FRIGHTENING_SHOUT);
                FrighteningShoutTimer = 18000;
            }
			else FrighteningShoutTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_warlord_salarisAI(creature);
    }
};

class boss_garaxxas : public CreatureScript
{
public:
    boss_garaxxas() : CreatureScript("boss_garaxxas") { }

    struct boss_garaxxasAI : public boss_priestess_lackey_commonAI
    {
        boss_garaxxasAI(Creature* creature) : boss_priestess_lackey_commonAI(creature), Summons(me){}

        uint32 AimedShotTimer;
        uint32 ShootTimer;
        uint32 ConcussiveShotTimer;
        uint32 MultiShotTimer;
        uint32 WingClipTimer;
        uint32 FreezingTrapTimer;

		SummonList Summons;

        void Reset()
        {
            AimedShotTimer = 6000;
            ShootTimer = 2500;
            ConcussiveShotTimer = 8000;
            MultiShotTimer = 10000;
            WingClipTimer = 4000;
            FreezingTrapTimer = 15000;

			Summons.DespawnAll();

            boss_priestess_lackey_commonAI::Reset();
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SLIVER)
			{
				Summons.Summon(summon);
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SummonCreature(NPC_SLIVER, 121.179f, 12.7981f, -20.0049f, 5.25911f, TEMPSUMMON_DEAD_DESPAWN, 30000);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (me->IsWithinDistInMap(me->GetVictim(), ATTACK_DISTANCE))
            {
                if (WingClipTimer <= diff)
                {
                    DoCastVictim(SPELL_WING_CLIP);
                    WingClipTimer = 4000;
                }
				else WingClipTimer -= diff;

                if (FreezingTrapTimer <= diff)
                {
                    GameObject* go = me->GetGameObject(SPELL_FREEZING_TRAP);

                    if (go)
                        FreezingTrapTimer = 2500;
                    else
                    {
                        DoCastVictim(SPELL_FREEZING_TRAP);
                        FreezingTrapTimer = 15000;
                    }
                }
				else FreezingTrapTimer -= diff;

                DoMeleeAttackIfReady();
            }
            else
            {
                if (ConcussiveShotTimer <= diff)
                {
                    DoCastVictim(SPELL_CONCUSSIVE_SHOT);
                    ConcussiveShotTimer = 8000;
                }
				else ConcussiveShotTimer -= diff;

                if (MultiShotTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_MULTI_SHOT_5N, SPELL_MULTI_SHOT_5H));
                    MultiShotTimer = 10000;
                }
				else MultiShotTimer -= diff;

                if (AimedShotTimer <= diff)
                {
                    DoCastVictim(SPELL_AIMED_SHOT);
                    AimedShotTimer = 6000;
                }
				else AimedShotTimer -= diff;

                if (ShootTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
                    ShootTimer = 2500;
                }
				else ShootTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_garaxxasAI(creature);
    }
};

class boss_apoko : public CreatureScript
{
public:
    boss_apoko() : CreatureScript("boss_apoko") { }

    struct boss_apokoAI : public boss_priestess_lackey_commonAI
    {
        boss_apokoAI(Creature* creature) : boss_priestess_lackey_commonAI(creature) {}

        uint32 TotemTimer;
        uint8 TotemAmount;
        uint32 WarStompTimer;
        uint32 PurgeTimer;
        uint32 HealingWaveTimer;
        uint32 FrostShockTimer;

        void Reset()
        {
            TotemTimer = 2000;
            TotemAmount = 1;
            WarStompTimer = 10000;
            PurgeTimer = 8000;
            HealingWaveTimer = 5000;
            FrostShockTimer = 7000;
            boss_priestess_lackey_commonAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (TotemTimer <= diff)
            {
                DoCast(me, RAND(SPELL_WINDFURY_TOTEM, SPELL_FIRE_NOVA_TOTEM, SPELL_EARTHBIND_TOTEM));
                ++TotemAmount;
                TotemTimer = TotemAmount*2000;
            }
			else TotemTimer -= diff;

            if (WarStompTimer <= diff)
            {
                DoCastAOE(SPELL_WAR_STOMP);
                WarStompTimer = 10000;
            }
			else WarStompTimer -= diff;

            if (PurgeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_PURGE);
					PurgeTimer = 15000;
				}
            }
			else PurgeTimer -= diff;

            if (FrostShockTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_FROST_SHOCK_5N, SPELL_FROST_SHOCK_5H));
                FrostShockTimer = 7000;
            }
			else FrostShockTimer -= diff;

            if (HealingWaveTimer <= diff)
            {
                DoCast(me, DUNGEON_MODE(SPELL_HEALING_WAVE_5N, SPELL_HEALING_WAVE_5H));
                HealingWaveTimer = 5000;
            }
			else HealingWaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_apokoAI(creature);
    }
};

class boss_zelfan : public CreatureScript
{
public:
    boss_zelfan() : CreatureScript("boss_zelfan") { }

    struct boss_zelfanAI : public boss_priestess_lackey_commonAI
    {
        boss_zelfanAI(Creature* creature) : boss_priestess_lackey_commonAI(creature) {}

        uint32 GoblinDragonGunTimer;
        uint32 RocketLaunchTimer;
        uint32 RecombobulateTimer;
        uint32 HighExplosiveSheepTimer;
        uint32 FelIronBombTimer;

        void Reset()
        {
            GoblinDragonGunTimer = 20000;
            RocketLaunchTimer = 7000;
            RecombobulateTimer = 4000;
            HighExplosiveSheepTimer = 10000;
            FelIronBombTimer = 15000;
            boss_priestess_lackey_commonAI::Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            boss_priestess_lackey_commonAI::UpdateAI(diff);

            if (GoblinDragonGunTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_GOBLIN_DRAGON_GUN_5N, SPELL_GOBLIN_DRAGON_GUN_5H));
                GoblinDragonGunTimer = 10000;
            }
			else GoblinDragonGunTimer -= diff;

            if (RocketLaunchTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_ROCKET_LAUNCH_5N, SPELL_ROCKET_LAUNCH_5H));
                RocketLaunchTimer = 9000;
            }
			else RocketLaunchTimer -= diff;

            if (FelIronBombTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_FEL_IRON_BOMB_5N, SPELL_FEL_IRON_BOMB_5H));
                FelIronBombTimer = 15000;
            }
			else FelIronBombTimer -= diff;

            if (RecombobulateTimer <= diff)
            {
                for (uint8 i = 0; i < MAX_ACTIVE_LACKEY; ++i)
                {
                    if (Unit* Add = Unit::GetUnit(*me, LackeyGUIDs[i]))
                    {
                        if (Add->IsPolymorphed())
                        {
                            DoCast(Add, SPELL_RECOMBOBULATE);
                            break;
                        }
                    }
                }
                RecombobulateTimer = 2000;
            }
			else RecombobulateTimer -= diff;

            if (HighExplosiveSheepTimer <= diff)
            {
                DoCast(me, SPELL_HIGH_EXPLOSIVE_SHEEP);
                HighExplosiveSheepTimer = 65000;
            }
			else HighExplosiveSheepTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_zelfanAI(creature);
    }
};

void AddSC_boss_priestess_delrissa()
{
    new boss_priestess_delrissa();
    new boss_kagani_nightstrike();
    new boss_ellrys_duskhallow();
    new boss_eramas_brightblaze();
    new boss_yazzai();
    new boss_warlord_salaris();
    new boss_garaxxas();
    new boss_apoko();
    new boss_zelfan();
}