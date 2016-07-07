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
#include "GameEventMgr.h"
#include "auchenai_crypts.h"

enum Texts
{
	SAY_INTRO                = -1558000,
	SAY_SUMMON               = -1558001,
	SAY_AGGRO_1              = -1558002,
	SAY_AGGRO_2              = -1558003,
	SAY_AGGRO_3              = -1558004,
	SAY_ROAR                 = -1558005,
	SAY_SOUL_CLEAVE          = -1558006,
	SAY_SLAY_1               = -1558007,
	SAY_SLAY_2               = -1558008,
	SAY_DEATH                = -1558009,
};

enum Spells
{
	SPELL_MOONFIRE           = 37328,
	SPELL_FIREBALL           = 37329,
	SPELL_MIND_FLAY          = 37330,
	SPELL_HEMORRHAGE         = 37331,
	SPELL_FROSTSHOCK         = 37332,
	SPELL_CURSE_OF_AGONY     = 37334,
	SPELL_MORTAL_STRIKE      = 37335,
	SPELL_FREEZING_TRAP      = 37368,
	SPELL_HAMMER_OF_JUSTICE  = 37369,
	SPELL_RIBBON_OF_SOULS    = 32422,
	SPELL_SOUL_SCREAM        = 32421,
	SPELL_STOLEN_SOUL        = 32346,
	SPELL_STOLEN_SOUL_VISUAL = 32395,
	SPELL_SUMMON_AVATAR      = 32424,
	SPELL_AV_MORTAL_STRIKE   = 16856,
	SPELL_AV_SUNDER_ARMOR    = 16145,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class npc_stolen_soul : public CreatureScript
{
public:
    npc_stolen_soul() : CreatureScript("npc_stolen_soul") { }

    struct npc_stolen_soulAI : public QuantumBasicAI
    {
        npc_stolen_soulAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 MyClass;

        uint32 ClassTimer;

        void Reset()
        {
            ClassTimer = 1000;
        }

        void EnterToBattle(Unit* /*who*/) { }

        void SetMyClass(uint8 MyClass)
        {
            MyClass = MyClass;
        }

        void UpdateAI (const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ClassTimer <= diff)
            {
                switch (MyClass)
                {
                    case CLASS_WARRIOR:
                        DoCastVictim(SPELL_MORTAL_STRIKE);
                        ClassTimer = 6000;
                        break;
                    case CLASS_PALADIN:
                        DoCastVictim(SPELL_HAMMER_OF_JUSTICE);
                        ClassTimer = 6000;
                        break;
                    case CLASS_HUNTER:
                        DoCastVictim(SPELL_FREEZING_TRAP);
                        ClassTimer = 20000;
                        break;
                    case CLASS_ROGUE:
                        DoCastVictim(SPELL_HEMORRHAGE);
                        ClassTimer = 10000;
                        break;
                    case CLASS_PRIEST:
                        DoCastVictim(SPELL_MIND_FLAY);
                        ClassTimer = 5000;
                        break;
                    case CLASS_SHAMAN:
                        DoCastVictim(SPELL_FROSTSHOCK);
                        ClassTimer = 8000;
                        break;
                    case CLASS_MAGE:
                        DoCastVictim(SPELL_FIREBALL);
                        ClassTimer = 5000;
                        break;
                    case CLASS_WARLOCK:
                        DoCastVictim(SPELL_CURSE_OF_AGONY);
                        ClassTimer = 20000;
                        break;
                    case CLASS_DRUID:
                        DoCastVictim(SPELL_MOONFIRE);
                        ClassTimer = 10000;
                        break;
                }
            }
			else ClassTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stolen_soulAI(creature);
    }
};

class boss_exarch_maladaar : public CreatureScript
{
public:
    boss_exarch_maladaar() : CreatureScript("boss_exarch_maladaar") { }

    struct boss_exarch_maladaarAI : public QuantumBasicAI
    {
        boss_exarch_maladaarAI(Creature* creature) : QuantumBasicAI(creature)
        {
            HasTaunted = false;
        }

        uint32 SoulModel;
        uint64 SoulHolder;
        uint8 SoulClass;

        uint32 FearTimer;
        uint32 RibbonofSoulsTimer;
        uint32 StolenSoulTimer;

        bool HasTaunted;
        bool AvatarSummoned;

        void Reset()
        {
            SoulModel = 0;
            SoulHolder = 0;
            SoulClass = 0;

            FearTimer = 15000 + rand()% 5000;
            RibbonofSoulsTimer = 5000;
            StolenSoulTimer = 25000 + rand()% 10000;

			AvatarSummoned = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
				me->SetDisplayId(MODEL_CHRISTMAS_EXARCH_MALADAAR);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!HasTaunted && me->IsWithinDistInMap(who, 150.0f))
            {
                DoSendQuantumText(SAY_INTRO, me);
                HasTaunted = true;
            }

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_STOLEN_SOUL)
            {
                //SPELL_STOLEN_SOUL_VISUAL has shapeshift effect, but not implemented feature in Trinity for this spell.
                summoned->CastSpell(summoned, SPELL_STOLEN_SOUL_VISUAL, false);
                summoned->SetDisplayId(SoulModel);
                summoned->SetCurrentFaction(me->GetFaction());

                if (Unit* target = Unit::GetUnit(*me, SoulHolder))
                {
					CAST_AI(npc_stolen_soul::npc_stolen_soulAI, summoned->AI())->SetMyClass(SoulClass);
					summoned->AI()->AttackStart(target);
				}
            }
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
            me->SummonCreature(NPC_DORE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (!AvatarSummoned && HealthBelowPct(30))
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(true);

                DoSendQuantumText(SAY_SUMMON, me);

                DoCast(me, SPELL_SUMMON_AVATAR);
                AvatarSummoned = true;
                StolenSoulTimer = 15000 + rand()% 15000;
            }

            if (StolenSoulTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (target->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        if (me->IsNonMeleeSpellCasted(false))
                            me->InterruptNonMeleeSpells(true);

                        uint32 i = urand(1, 2);
                        if (i == 1)
                            DoSendQuantumText(SAY_ROAR, me);
                        else
                            DoSendQuantumText(SAY_SOUL_CLEAVE, me);

                        SoulModel = target->GetDisplayId();
                        SoulHolder = target->GetGUID();
                        SoulClass = target->GetCurrentClass();

                        DoCast(target, SPELL_STOLEN_SOUL);
                        me->SummonCreature(NPC_STOLEN_SOUL, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);

                        StolenSoulTimer = 20000 + rand()% 10000;
                    }
					else StolenSoulTimer = 1000;
                }
            }
			else StolenSoulTimer -= diff;

            if (RibbonofSoulsTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_RIBBON_OF_SOULS);
					RibbonofSoulsTimer = 5000 + (rand()%20 * 1000);
				}
            }
			else RibbonofSoulsTimer -= diff;

            if (FearTimer <= diff)
            {
                DoCast(me, SPELL_SOUL_SCREAM);
                FearTimer = 15000 + rand()% 15000;
            }
			else FearTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_exarch_maladaarAI(creature);
    }
};

class npc_avatar_of_the_martyred : public CreatureScript
{
public:
    npc_avatar_of_the_martyred() : CreatureScript("npc_avatar_of_the_martyred") { }

    struct npc_avatar_of_the_martyredAI : public QuantumBasicAI
    {
        npc_avatar_of_the_martyredAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MortalStrikeTimer;

        void Reset()
        {
            MortalStrikeTimer = 2000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_AV_MORTAL_STRIKE);
                MortalStrikeTimer = urand(10, 30) * 1000;
            }
			else MortalStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_avatar_of_the_martyredAI(creature);
    }
};

void AddSC_boss_exarch_maladaar()
{
    new boss_exarch_maladaar();
    new npc_avatar_of_the_martyred();
    new npc_stolen_soul();
}