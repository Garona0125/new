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
#include "sethekk_halls.h"

enum Spells
{
	SPELL_BLINK               = 38194,
	SPELL_BLINK_TELEPORT      = 38203,
	SPELL_MANA_SHIELD         = 38151,
	SPELL_ARCANE_BUBBLE       = 9438,
	SPELL_SLOW                = 35032,
	SPELL_POLYMORPH_5N        = 38245,
	SPELL_POLYMORPH_5H        = 43309,
	SPELL_ARCANE_VOLLEY_5N    = 35059,
	SPELL_ARCANE_VOLLEY_5H    = 40424,
	SPELL_ARCANE_EXPLOSION_5N = 38197,
	SPELL_ARCANE_EXPLOSION_5H = 40425,
};

enum Says
{
	SAY_INTRO                  = -1556007,
	SAY_AGGRO_1                = -1556008,
	SAY_AGGRO_2                = -1556009,
	SAY_AGGRO_3                = -1556010,
	SAY_SLAY_1                 = -1556011,
	SAY_SLAY_2                 = -1556012,
	SAY_DEATH                  = -1556013,
	EMOTE_ARCANE_EXP           = -1556015,
};

class boss_talon_king_ikiss : public CreatureScript
{
public:
    boss_talon_king_ikiss() : CreatureScript("boss_talon_king_ikiss") { }

    struct boss_talon_king_ikissAI : public QuantumBasicAI
    {
        boss_talon_king_ikissAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 ArcaneVolleyTimer;
        uint32 SheepTimer;
        uint32 BlinkTimer;
        uint32 SlowTimer;

        bool ManaShield;
        bool Blink;
        bool Intro;

        void Reset()
        {
            ArcaneVolleyTimer = 1000;
            SheepTimer = 3000;
			SlowTimer = 5000;
            BlinkTimer = 35000;

			Blink = false;
            Intro = false;
            ManaShield = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->GetVictim() && me->CanCreatureAttack(who))
            {
                if (!Intro && me->IsWithinDistInMap(who, 100))
                {
                    Intro = true;
                    DoSendQuantumText(SAY_INTRO, me);
                }

                if (!me->CanFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                    return;

                float attackRadius = me->GetAttackDistance(who);
                if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
                {
                    //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    AttackStart(who);
                }
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        }

        void JustDied(Unit* /*Killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(DATA_IKISS_DOOR_EVENT, DONE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (Blink)
            {
                DoCast(me, DUNGEON_MODE(SPELL_ARCANE_EXPLOSION_5N, SPELL_ARCANE_EXPLOSION_5H));
                DoCast(me, SPELL_ARCANE_BUBBLE, true);
                Blink = false;
            }

            if (ArcaneVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_VOLLEY_5N, SPELL_ARCANE_VOLLEY_5H));
					ArcaneVolleyTimer = urand(4000, 5000);
				}
            }
			else ArcaneVolleyTimer -= diff;

            if (SheepTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_POLYMORPH_5N, SPELL_POLYMORPH_5H));
					SheepTimer = urand(6000, 7000);
				}
            }
			else SheepTimer -= diff;

			if (SlowTimer <= diff && IsHeroic())
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SLOW);
					SlowTimer = urand(8000, 9000);
				}
			}
			else SlowTimer -= diff;

            if (BlinkTimer <= diff)
            {
                DoSendQuantumText(EMOTE_ARCANE_EXP, me);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(false);

                    DoCast(target, SPELL_BLINK);
                    float X = target->GetPositionX();
                    float Y = target->GetPositionY();
                    float Z = target->GetPositionZ();
                    DoTeleportTo(X, Y, Z);
                    DoCast(target, SPELL_BLINK_TELEPORT);
                    Blink = true;
                }
                BlinkTimer = 35000;
            }
			else BlinkTimer -= diff;

			if (HealthBelowPct(20) && !ManaShield)
            {
                DoCast(me, SPELL_MANA_SHIELD);
                ManaShield = true;
            }

            if (!Blink)
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_talon_king_ikissAI(creature);
    }
};

void AddSC_boss_talon_king_ikiss()
{
    new boss_talon_king_ikiss();
}