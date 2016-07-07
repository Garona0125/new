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
#include "halls_of_lightning.h"

enum Texts
{
    SAY_AGGRO      = -1602018,
    SAY_INTRO_1    = -1602019,
    SAY_INTRO_2    = -1602020,
    SAY_SLAY_1     = -1602021,
    SAY_SLAY_2     = -1602022,
    SAY_SLAY_3     = -1602023,
    SAY_DEATH      = -1602024,
    SAY_NOVA_1     = -1602025,
    SAY_NOVA_2     = -1602026,
    SAY_NOVA_3     = -1602027,
    SAY_75_HEALTH  = -1602028,
    SAY_50_HEALTH  = -1602029,
    SAY_25_HEALTH  = -1602030,
    EMOTE_NOVA     = -1602031,
};

enum Spells
{
    SPELL_ARC_LIGHTNING          = 52921,
    SPELL_LIGHTNING_NOVA_5N      = 52960,
    SPELL_LIGHTNING_NOVA_5H      = 59835,
    SPELL_PULSING_SHOCKWAVE_5N   = 52961,
    SPELL_PULSING_SHOCKWAVE_5H   = 59836,
    SPELL_PULSING_SHOCKWAVE_AURA = 59414,
};

class boss_loken : public CreatureScript
{
public:
    boss_loken() : CreatureScript("boss_loken") { }

    struct boss_lokenAI : public QuantumBasicAI
    {
        boss_lokenAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        bool Charged;
		bool Greet;
		bool Intro;

        uint32 ArcLightningTimer;
        uint32 LightningNovaTimer;
        uint32 ResumePulsingShockwaveTimer;
        uint32 HealthAmountModifier;
		uint32 IntroTimer;
		uint32 IntroStep;

        void Reset()
        {
			IntroStep = 0;
            ArcLightningTimer = 15000;
            LightningNovaTimer = 20000;
            ResumePulsingShockwaveTimer = 1000;

			Intro = false;
			Greet = false;

            HealthAmountModifier = 1;

			instance->SetData(TYPE_LOKEN, NOT_STARTED);
			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMELY_DEATH_START_EVENT);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Greet)
				return;

			if (me->IsWithinDistInMap(who, 60.0f) && who->IsAlive() && who->GetTypeId() == TYPE_ID_PLAYER)
			{
				IntroStep = 1;
				IntroTimer = 100;
				IntroEvent();
				Greet = true;
			}
		}

		void IntroEvent()
		{
			if (!IntroStep)
				return;

			if (Intro)
				return;

			switch (IntroStep)
			{
			    case 1:
					DoSendQuantumText(SAY_INTRO_1, me);
					JumpIntro(19000);
					break;
				case 2:
					DoSendQuantumText(SAY_INTRO_2, me);
					JumpIntro(19000);
					break;
				case 3:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					Intro = true;
					break;
				default:
					break;
			}
		}

		void JumpIntro(uint32 TimeIntro)
		{
			IntroTimer = TimeIntro;
			IntroStep++;
		}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			instance->SetData(TYPE_LOKEN, IN_PROGRESS);
			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMELY_DEATH_START_EVENT);

            if (instance->GetData(TYPE_IONAR) != DONE)
            {
                Map* map = me->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();
                    if (PlayerList.isEmpty())
                        return;

                    float fDist=0;
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource() && i->getSource()->IsAlive() && !i->getSource()->IsGameMaster())
                            me->DealDamage(i->getSource(),i->getSource()->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(TYPE_LOKEN, DONE);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PULSING_SHOCKWAVE_AURA);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ARC_LIGHTNING && Charged)
			{
                if (rand()%100<DUNGEON_MODE(65, 80))
                {
                    Charged = false;
                    DoCast(target->FindPlayerWithDistance(10.0f), SPELL_ARC_LIGHTNING);
                }
			}
		}

        void UpdateAI(const uint32 diff)
        {
			if (IntroTimer <= diff)
				IntroEvent();
			else
				IntroTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ResumePulsingShockwaveTimer)
            {
                if (ResumePulsingShockwaveTimer <= diff)
                {
                    DoCast(me, SPELL_PULSING_SHOCKWAVE_AURA, true);

                    DoCast(me, DUNGEON_MODE(SPELL_PULSING_SHOCKWAVE_5N, SPELL_PULSING_SHOCKWAVE_5H));
                    ResumePulsingShockwaveTimer = 0;
                }
                else ResumePulsingShockwaveTimer -= diff;
            }

            if (ArcLightningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_ARC_LIGHTNING);
					ArcLightningTimer = 15000 + rand()%1000;
				}
				Charged = true;
            }
            else ArcLightningTimer -= diff;

            if (LightningNovaTimer <= diff)
            {
                DoSendQuantumText(RAND(SAY_NOVA_1, SAY_NOVA_2, SAY_NOVA_3), me);
                DoSendQuantumText(EMOTE_NOVA, me);
                DoCast(me, DUNGEON_MODE(SPELL_LIGHTNING_NOVA_5N, SPELL_LIGHTNING_NOVA_5H));

                me->RemoveAurasDueToSpell(DUNGEON_MODE<uint32>(SPELL_PULSING_SHOCKWAVE_5N, SPELL_PULSING_SHOCKWAVE_5H));
                ResumePulsingShockwaveTimer = DUNGEON_MODE(5000, 4000);
                LightningNovaTimer = 30000;
            }
            else LightningNovaTimer -= diff;

            if (HealthBelowPct(100 - 25 * HealthAmountModifier))
            {
                switch (HealthAmountModifier)
                {
                    case 1:
						DoSendQuantumText(SAY_75_HEALTH, me);
						break;
                    case 2:
						DoSendQuantumText(SAY_50_HEALTH, me);
						break;
                    case 3:
						DoSendQuantumText(SAY_25_HEALTH, me);
						break;
                }

                ++HealthAmountModifier;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lokenAI(creature);
    }
};

class spell_loken_pulsing_shockwave : public SpellScriptLoader
{
    public:
        spell_loken_pulsing_shockwave() : SpellScriptLoader("spell_loken_pulsing_shockwave") { }

        class spell_loken_pulsing_shockwave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_loken_pulsing_shockwave_SpellScript);

            void CalculateDamage(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                float distance = GetCaster()->GetDistance2d(GetHitUnit());
                if (distance > 1.0f)
                    SetHitDamage(int32(GetHitDamage() * distance));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_loken_pulsing_shockwave_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_loken_pulsing_shockwave_SpellScript();
        }
};

void AddSC_boss_loken()
{
    new boss_loken();
	new spell_loken_pulsing_shockwave();
}