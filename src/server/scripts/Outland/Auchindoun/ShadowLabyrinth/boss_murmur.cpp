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
#include "shadow_labyrinth.h"

enum Texts
{
	EMOTE_SONIC_BOOM = -1555036,
};

enum Spells
{
	SPELL_SONIC_BOOM_CAST_5N   = 33923,
	SPELL_SONIC_BOOM_CAST_5H   = 38796,
	SPELL_SONIC_BOOM_EFFECT_5N = 33666,
	SPELL_SONIC_BOOM_EFFECT_5H = 38795,
	SPELL_RESONANCE            = 33657,
	SPELL_MURMURS_TOUCH_5N     = 33711,
	SPELL_MURMURS_TOUCH_5H     = 38794,
	SPELL_MAGNETIC_PULL        = 28337,
	SPELL_SONIC_SHOCK          = 38797,
	SPELL_THUNDERING_STORM     = 39365,
};

class boss_murmur : public CreatureScript
{
public:
    boss_murmur() : CreatureScript("boss_murmur") { }

    struct boss_murmurAI : public QuantumBasicAI
    {
        boss_murmurAI(Creature* creature): QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			SetCombatMovement(false);
		}

		InstanceScript* instance;

        uint32 ResonanceTimer;
		uint32 MurmursTouchTimer;
        uint32 SonicShockTimer;
        uint32 ThunderingStormTimer;
		uint32 SonicBoomTimer;

        bool SonicBoom;

        void Reset()
        {
			ResonanceTimer = 500;
			MurmursTouchTimer = 2000;
			SonicShockTimer = 4000;
			ThunderingStormTimer = 6000;
            SonicBoomTimer = 30000;
            SonicBoom = false;

			me->SetFullHealth();
            me->ResetPlayerDamageReq();
			ResetBinderState();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_MURMUR, NOT_STARTED);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			instance->SetData(DATA_MURMUR, IN_PROGRESS);
		}

		void JustReachedHome()
		{
			instance->SetData(DATA_MURMUR, FAIL);
		}

		void JustDied(Unit* /*killer*/)
		{
			instance->SetData(DATA_MURMUR, DONE);
		}

		void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (target->IsAlive() && spell->Id == DUNGEON_MODE(SPELL_SONIC_BOOM_EFFECT_5N, SPELL_SONIC_BOOM_EFFECT_5H))
                me->DealDamage(target, (target->GetHealth()*90)/100, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NATURE, spell);
        }

        void SonicBoomEffect()
        {
            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
				Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
				if (target && target->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (target->HasAura(DUNGEON_MODE(SPELL_SONIC_BOOM_CAST_5N, SPELL_SONIC_BOOM_CAST_5H) && me->IsWithinDistInMap(target, 35.0f)))
						target->SetHealth(target->CountPctFromMaxHealth(HEALTH_PERCENT_20));
				}
			}
		}

		void ResetBinderState()
		{
			std::list<Creature*> TrashList;
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_CABAL_SPELLBINDER, 35.0f);

			if (!TrashList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
				{
					if (Creature* spellbinder = *itr)
						spellbinder->AI()->Reset();
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SonicBoom)
            {
                DoCast(me, DUNGEON_MODE(SPELL_SONIC_BOOM_EFFECT_5N, SPELL_SONIC_BOOM_EFFECT_5H), true);
                SonicBoomEffect();

                SonicBoom = false;
                ResonanceTimer = 1500;
            }

            if (SonicBoomTimer <= diff)
            {
                DoSendQuantumText(EMOTE_SONIC_BOOM, me);
				DoCastAOE(SPELL_MAGNETIC_PULL, true);
                DoCast(me, DUNGEON_MODE(SPELL_SONIC_BOOM_CAST_5N, SPELL_SONIC_BOOM_CAST_5H));
                SonicBoomTimer = 30000;
                SonicBoom = true;
                return;
            }
			else SonicBoomTimer -= diff;

            if (MurmursTouchTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 80, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_MURMURS_TOUCH_5N, SPELL_MURMURS_TOUCH_5H));
					MurmursTouchTimer = urand(25000, 35000);
				}
            }
			else MurmursTouchTimer -= diff;

            if (!me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (ResonanceTimer <= diff)
                {
                    DoCastAOE(SPELL_RESONANCE);
                    ResonanceTimer = 5000;
                }
				else ResonanceTimer -= diff;
            }

			if (ThunderingStormTimer <= diff && IsHeroic())
			{
				std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
				for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
				{
					if (Unit* target = Unit::GetUnit(*me, (*i)->getUnitGuid()))
					{
						if (target->IsAlive() && !me->IsWithinDist(target, 35, false))
						{
							DoCast(target, SPELL_THUNDERING_STORM, true);
							ThunderingStormTimer = 15000;
						}
					}
                }
			}
			else ThunderingStormTimer -= diff;

			if (SonicShockTimer <= diff && IsHeroic())
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 20, false))
				{
					if (target->IsAlive())
					{
						DoCast(target, SPELL_SONIC_SHOCK);
						SonicShockTimer = urand(6000, 7000);
					}
				}
			}
			else SonicShockTimer -= diff;

            if (!me->IsAttackReady())
                return;

            if (!me->IsWithinMeleeRange(me->GetVictim()))
            {
                std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
                for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
				{
                    if (Unit* target = Unit::GetUnit(*me, (*i)->getUnitGuid()))
					{
                        if (target->IsAlive() && me->IsWithinMeleeRange(target))
                        {
                            me->TauntApply(target);
                            break;
                        }
					}
				}
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_murmurAI(creature);
    }
};

void AddSC_boss_murmur()
{
    new boss_murmur();
}