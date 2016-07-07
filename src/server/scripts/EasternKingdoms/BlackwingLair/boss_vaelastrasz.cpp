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
#include "QuantumGossip.h"

enum Says
{
	SAY_LINE1           = 0,
	SAY_LINE2           = 1,
	SAY_LINE3           = 2,
	SAY_HALFLIFE        = 3,
	SAY_KILLTARGET      = 4,
};

#define GOSSIP_ITEM   "I cannot. Vaelastrasz! Surely something can be done ti heal you!"
#define GOSSIP_ITEM_1 "Vaelastrasz. no!!!" // need implement

enum Spells
{
   SPELL_ESSENCE_OF_THE_RED = 23513,
   SPELL_FLAME_BREATH       = 23461,
   SPELL_FIRE_NOVA          = 23462,
   SPELL_TAIL_SWIPE         = 15847,
   SPELL_BURNING_ADRENALINE = 23620,
   SPELL_CLEAVE             = 20684,
};

class boss_vaelastrasz : public CreatureScript
{
public:
    boss_vaelastrasz() : CreatureScript("boss_vaelastrasz") { }

    void SendDefaultMenu(Player* player, Creature* creature, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            CAST_AI(boss_vaelastrasz::boss_vaelAI, creature->AI())->BeginSpeech(player);
        }
    }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(907, creature->GetGUID());
        return true;
	}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
            SendDefaultMenu(player, creature, action);

        return true;
    }

    struct boss_vaelAI : public QuantumBasicAI
    {
        boss_vaelAI(Creature* creature) : QuantumBasicAI(creature)
        {
            creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            creature->SetCurrentFaction(35);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        uint64 PlayerGUID;
        uint32 SpeechTimer;
        uint32 SpeechNum;
        uint32 CleaveTimer;
        uint32 FlameBreathTimer;
        uint32 FireNovaTimer;
        uint32 BurningAdrenalineCasterTimer;
        uint32 BurningAdrenalineTankTimer;
        uint32 TailSwipeTimer;

        bool HasYelled;
        bool DoingSpeech;

        void Reset()
        {
            PlayerGUID = 0;
            SpeechTimer = 0;
            SpeechNum = 0;
            CleaveTimer = 8000;
            FlameBreathTimer = 11000;
            BurningAdrenalineCasterTimer = 15000;
            BurningAdrenalineTankTimer = 45000;
            FireNovaTimer = 5000;
            TailSwipeTimer = 20000;
            HasYelled = false;
            DoingSpeech = false;
        }

        void BeginSpeech(Unit* target)
        {
            PlayerGUID = target->GetGUID();
            //10 seconds
            Talk(SAY_LINE1);
            SpeechTimer = 10000;
            SpeechNum = 0;
            DoingSpeech = true;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void KilledUnit(Unit* victim)
        {
            if (rand()%5)
                return;

            Talk(SAY_KILLTARGET, victim->GetGUID());
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_ESSENCE_OF_THE_RED);

            DoZoneInCombat();

            me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_30));

            me->ResetPlayerDamageReq();
        }

        void UpdateAI(const uint32 diff)
        {
            if (DoingSpeech)
            {
                if (SpeechTimer <= diff)
                {
                    switch (SpeechNum)
                    {
                        case 0:
                            Talk(SAY_LINE2);
                            SpeechTimer = 16000;
                            ++SpeechNum;
                            break;
                        case 1:
                            Talk(SAY_LINE3);
                            SpeechTimer = 10000;
                            ++SpeechNum;
                            break;
                        case 2:
                            me->SetCurrentFaction(103);
                            if (PlayerGUID && Unit::GetUnit(*me, PlayerGUID))
                            {
                                AttackStart(Unit::GetUnit(*me, PlayerGUID));
                                DoCast(me, SPELL_ESSENCE_OF_THE_RED);
                            }
                            SpeechTimer = 0;
                            DoingSpeech = false;
                            break;
                    }
                }
				else SpeechTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (HealthBelowPct(HEALTH_PERCENT_15) && !HasYelled)
            {
                Talk(SAY_HALFLIFE);
                HasYelled = true;
            }

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 15000;
            }
			else CleaveTimer -= diff;

            if (FlameBreathTimer <= diff)
            {
                DoCastVictim(SPELL_FLAME_BREATH);
                FlameBreathTimer = urand(4000, 8000);
            }
			else FlameBreathTimer -= diff;

            if (BurningAdrenalineCasterTimer <= diff)
            {
                Unit* target = NULL;

                uint8 i = 0;
                while (i < 3)
                {
                    ++i;
                    target = SelectTarget(TARGET_RANDOM, 1, 100, true);
                    if (target && target->GetPowerType() == POWER_MANA)
						i = 3;
                }
                if (target)
                    target->CastSpell(target, SPELL_BURNING_ADRENALINE, 1);

                BurningAdrenalineCasterTimer = 15000;
            }
			else BurningAdrenalineCasterTimer -= diff;

            if (BurningAdrenalineTankTimer <= diff)
            {
                me->GetVictim()->CastSpell(me->GetVictim(), SPELL_BURNING_ADRENALINE, 1);
                BurningAdrenalineTankTimer = 45000;
            }
			else BurningAdrenalineTankTimer -= diff;

            if (FireNovaTimer <= diff)
            {
                DoCastVictim(SPELL_FIRE_NOVA);
                FireNovaTimer = 5000;
            }
			else FireNovaTimer -= diff;

            if (TailSwipeTimer <= diff)
            {
				DoCast(SPELL_TAIL_SWIPE);
                TailSwipeTimer = 20000;
            }
			else TailSwipeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vaelAI (creature);
    }
};

void AddSC_boss_vael()
{
    new boss_vaelastrasz();
}