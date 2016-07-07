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
#include "SpellScript.h"
#include "the_botanica.h"

enum Spells
{
    SPELL_ALLERGIC_REACTION    = 34697,
    SPELL_TELEPORT_SELF        = 34673,
    SPELL_SUMMON_LASHER_1      = 34681,
    SPELL_SUMMON_FLAYER_1      = 34682,
    SPELL_SUMMON_LASHER_2      = 34684,
    SPELL_SUMMON_FLAYER_2      = 34685,
    SPELL_SUMMON_LASHER_3      = 34686,
    SPELL_SUMMON_FLAYER_4      = 34687,
    SPELL_SUMMON_LASHER_4      = 34688,
    SPELL_SUMMON_FLAYER_3      = 34690,
};

enum Others
{
    EMOTE_SUMMON               = -1553006,

    MODEL_ID_DEFAULT           = 13109,
    MODEL_ID_ARCANE            = 14213,
    MODEL_ID_FIRE              = 13110,
    MODEL_ID_FROST             = 14112,
    MODEL_ID_NATURE            = 14214,
};

class boss_laj : public CreatureScript
{
    public:
        boss_laj(): CreatureScript("boss_laj"){}

        struct boss_lajAI : public BossAI
        {
            boss_lajAI(Creature* creature) : BossAI(creature, DATA_LAJ) { }

            bool CanSummon;
            uint32 TeleportTimer;
            uint32 SummonTimer;
            uint32 TransformTimer;
            uint32 AllergicTimer;

            void Reset()
            {
                me->SetDisplayId(MODEL_ID_DEFAULT);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);

                CanSummon = false;
                TeleportTimer = 20000;
                SummonTimer = 2500;
                TransformTimer = 30000;
                AllergicTimer = 5000;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void DoTransform()
            {
                switch (rand()%5)
                {
                    case 0:
                        me->SetDisplayId(MODEL_ID_DEFAULT);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                        break;
                    case 1:
                        me->SetDisplayId(MODEL_ID_ARCANE);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                        break;
                    case 2:
                        me->SetDisplayId(MODEL_ID_FIRE);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                        break;
                    case 3:
                        me->SetDisplayId(MODEL_ID_FROST);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                        break;
                    case 4:
                        me->SetDisplayId(MODEL_ID_NATURE);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, false);
                        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
                        break;
                }
            }

            void DoSummons()
            {
                switch (rand()%4)
                {
                    case 0:
                        DoCast(me, SPELL_SUMMON_LASHER_1, true);
                        DoCast(me, SPELL_SUMMON_FLAYER_1, true);
                        break;
                    case 1:
                        DoCast(me, SPELL_SUMMON_LASHER_2, true);
                        DoCast(me, SPELL_SUMMON_FLAYER_2, true);
                        break;
                    case 2:
                        DoCast(me, SPELL_SUMMON_LASHER_3, true);
                        DoCast(me, SPELL_SUMMON_FLAYER_3, true);
                        break;
                    case 3:
                        DoCast(me, SPELL_SUMMON_LASHER_4, true);
                        DoCast(me, SPELL_SUMMON_FLAYER_4, true);
                        break;
                }
                CanSummon = false;
            }

            void EnterToBattle(Unit* /*who*/){}

            void JustSummoned(Creature* summon)
            {
                if (summon && me->GetVictim())
                    summon->AI()->AttackStart(SelectTarget(TARGET_RANDOM, 0));
            }

			void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (CanSummon)
                {
                    if (SummonTimer <= diff)
                    {
                        DoSendQuantumText(EMOTE_SUMMON, me);
                        DoSummons();
                        SummonTimer = 2500;
                    }
                    else SummonTimer -= diff;
                }

                if (AllergicTimer <= diff)
                {
                    DoCastVictim(SPELL_ALLERGIC_REACTION);
                    AllergicTimer = 25000+rand()%15000;
                }
                else AllergicTimer -= diff;

                if (TeleportTimer <= diff)
                {
                    DoCast(me, SPELL_TELEPORT_SELF);
                    TeleportTimer = 30000+rand()%10000;
                    CanSummon = true;
                }
                else TeleportTimer -= diff;

                if (TransformTimer <= diff)
                {
                    DoTransform();
                    TransformTimer = 25000+rand()%15000;
                }
                else TransformTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_lajAI(creature);
        }
};

void AddSC_boss_laj()
{
    new boss_laj();
}