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
#include "SpellAuraEffects.h"
#include "naxxramas.h"

enum Texts
{
    SAY_GREET       = -1533009,
    SAY_AGGRO_1     = -1533010,
    SAY_AGGRO_2     = -1533011,
    SAY_AGGRO_3     = -1533012,
    SAY_AGGRO_4     = -1533013,
    SAY_SLAY_1      = -1533014,
    SAY_SLAY_2      = -1533015,
    SAY_DEATH       = -1533016,
};

#define EMOTE_EMBRACE "Grand Widow Faerlina is affected by Widow's Embrace!"
#define EMOTE_FRENZY  "Grand Widow Faerlina goes into a frenzy!"

enum Spells
{
	SPELL_POISON_BOLT_VOLLEY_10 = 28796,
    SPELL_POISON_BOLT_VOLLEY_25 = 54098,
    SPELL_RAIN_OF_FIRE_10       = 28794,
    SPELL_RAIN_OF_FIRE_25       = 54099,
    SPELL_FRENZY_10             = 28798,
    SPELL_FRENZY_25             = 54100,
    SPELL_WIDOWS_EMBRACE_10     = 28732,
    SPELL_WIDOWS_EMBRACE_25     = 54097,
    SPELL_FIREBALL_10           = 54095,
    SPELL_FIREBALL_25           = 54096,
};

enum Events
{
    EVENT_NONE   = 0,
    EVENT_POISON = 1,
    EVENT_FIRE   = 2,
    EVENT_FRENZY = 3,
};

class boss_grand_widow_faerlina : public CreatureScript
{
public:
    boss_grand_widow_faerlina() : CreatureScript("boss_grand_widow_faerlina") { }

    struct boss_grand_widow_faerlinaAI : public BossAI
    {
        boss_grand_widow_faerlinaAI(Creature* creature) : BossAI(creature, BOSS_FAERLINA){}

        bool Greet;
        bool DoDelayFrenzy;
        bool Achievement;

        void Reset()
        {
            _Reset();

            DoDelayFrenzy = false;
            Achievement = true;
			Greet = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Greet || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 60.0f))
				return;

			if (instance && Greet == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 60.0f))
			{
				DoSendQuantumText(SAY_GREET, me);
				Greet = true;
			}

			BossAI::MoveInLineOfSight(who);
		}

		void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3, SAY_AGGRO_4), me);

            events.ScheduleEvent(EVENT_POISON, urand(10000, 15000));
            events.ScheduleEvent(EVENT_FIRE, urand(6000, 18000));
            events.ScheduleEvent(EVENT_FRENZY, urand(60000, 80000));
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

            DoSendQuantumText(SAY_DEATH, me);

            if (Achievement)
                instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_MOMMA_SAID_KNOCK_YOU_OUT_10, ACHIEVEMENT_MOMMA_SAID_KNOCK_YOU_OUT_25));
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
			if (spell->Id == RAID_MODE(SPELL_WIDOWS_EMBRACE_10, SPELL_WIDOWS_EMBRACE_25))
            {
				Achievement = false;
				DoDelayFrenzy = false;

				me->RemoveAurasDueToSpell(RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25));
				me->Kill(caster);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (DoDelayFrenzy && !me->HasAura(RAID_MODE(SPELL_WIDOWS_EMBRACE_10, SPELL_WIDOWS_EMBRACE_25)))
            {
                DoDelayFrenzy = false;
                DoCast(me, RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25), true);
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_POISON:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (!me->HasAura(RAID_MODE(SPELL_WIDOWS_EMBRACE_10, SPELL_WIDOWS_EMBRACE_25)))
							{
								me->MonsterTextEmote(EMOTE_EMBRACE, 0, true);
                                DoCastAOE(RAID_MODE(SPELL_POISON_BOLT_VOLLEY_10, SPELL_POISON_BOLT_VOLLEY_25));
							}
                            events.ScheduleEvent(EVENT_POISON, urand(8000,15000));
                        }
                        break;
                    case EVENT_FIRE:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, RAID_MODE(SPELL_RAIN_OF_FIRE_10, SPELL_RAIN_OF_FIRE_25));
								events.ScheduleEvent(EVENT_FIRE, urand(6000, 18000));
							}
                        }
                        break;
                    case EVENT_FRENZY:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
							me->MonsterTextEmote(EMOTE_FRENZY, 0, true);
							if (!me->HasAura(RAID_MODE(SPELL_WIDOWS_EMBRACE_10, SPELL_WIDOWS_EMBRACE_25)))
								DoCast(me, RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25));
							else
								DoDelayFrenzy = true;

                            events.ScheduleEvent(EVENT_FRENZY, urand(60000,80000));
                        }
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grand_widow_faerlinaAI (creature);
    }
};

class mob_faerlina_add : public CreatureScript
{
public:
    mob_faerlina_add() : CreatureScript("mob_faerlina_add") { }

    struct mob_faerlina_addAI : public QuantumBasicAI
    {
        mob_faerlina_addAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 FireBallTimer;

        void Reset()
        {
            if (GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, SPELL_EFFECT_BIND, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
            }

            FireBallTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FireBallTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_FIREBALL_10, SPELL_FIREBALL_25));
                FireBallTimer = urand(5000, 10000);
            }
			else FireBallTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
            {
                if (Creature* Faerlina = instance->instance->GetCreature(instance->GetData64(DATA_GRAND_WIDOW_FAERLINA)))
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCast(Faerlina, RAID_MODE(SPELL_WIDOWS_EMBRACE_10, SPELL_WIDOWS_EMBRACE_25), true);
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_faerlina_addAI (creature);
    }
};

void AddSC_boss_grand_widow_faerlina()
{
    new boss_grand_widow_faerlina();
    new mob_faerlina_add();
}