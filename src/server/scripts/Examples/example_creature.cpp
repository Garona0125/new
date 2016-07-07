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

enum Yells
{
	SAY_AGGRO                   = 0,
	SAY_RANDOM                  = 1,
	SAY_BERSERK                 = 2,
	SAY_PHASE                   = 3,
	SAY_DANCE                   = 4,
	SAY_SALUTE                  = 5,
	SAY_EVADE                   = 6,
};

enum Spells
{
    SPELL_BUFF                 = 25661,
    SPELL_ONE                  = 12555,
    SPELL_ONE_ALT              = 24099,
    SPELL_TWO                  = 10017,
    SPELL_THREE                = 26027,
    SPELL_FRENZY               = 23537,
    SPELL_BERSERK              = 32965,
};

enum Worgens
{
    FACTION_WORGEN             = 24,
};

#define GOSSIP_ITEM "I'm looking for a fight"

class example_creature : public CreatureScript
{
    public:
		example_creature() : CreatureScript("example_creature") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->CLOSE_GOSSIP_MENU();
                creature->SetCurrentFaction(FACTION_WORGEN);
                creature->AI()->AttackStart(player);
            }
            return true;
		}

        struct example_creatureAI : public QuantumBasicAI
        {
            example_creatureAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 SayTimer; // Timer for random chat
            uint32 RebuffTimer;  // Timer for rebuffing
            uint32 Spell1Timer; // Timer for spell 1 when in combat
            uint32 Spell2Timer; // Timer for spell 1 when in combat
            uint32 Spell3Timer; // Timer for spell 1 when in combat
            uint32 BeserkTimer; // Timer until we go into Beserk (enraged) mode
            uint32 Phase; // The current battle phase we are in
            uint32 PhaseTimer; // Timer until phase transition

            // *** HANDLED FUNCTION ***
            //This is called after spawn and whenever the core decides we need to evade
            void Reset()
            {
				Phase = 1; // Start in phase 1
                PhaseTimer = 60000; // 60 seconds
                Spell1Timer = 5000; //  5 seconds
                Spell2Timer = urand(10000, 20000); // between 10 and 20 seconds
                Spell3Timer = 19000; // 19 seconds
                BeserkTimer = 120000; // 2 minutes

                me->RestoreFaction();
            }

            void EnterToBattle(Unit* who)
            {
				Talk(SAY_AGGRO, who->GetGUID());
            }

            // *** HANDLED FUNCTION ***
            // Attack Start is called when victim change (including at start of combat)
            // By default, attack who and start movement toward the victim.
            //void AttackStart(Unit* who)
            //{
            //    QuantumBasicAI::AttackStart(who);
            //}

            void EnterEvadeMode()
            {
                Talk(SAY_EVADE);
            }

            void ReceiveEmote(Player* /*player*/, uint32 uiTextEmote)
            {
                me->HandleEmoteCommand(uiTextEmote);

                switch (uiTextEmote)
                {
                    case TEXT_EMOTE_DANCE:
                        Talk(SAY_DANCE);
                        break;
                    case TEXT_EMOTE_SALUTE:
                        Talk(SAY_SALUTE);
                        break;
                }
			}

            void UpdateAI(const uint32 diff)
            {
                if (!me->GetVictim())
                {
                    if (SayTimer <= diff)
                    {
						Talk(SAY_RANDOM);

                        SayTimer = 45000;
                    }
                    else SayTimer -= diff;

                    if (RebuffTimer <= diff)
                    {
                        DoCast(me, SPELL_BUFF);
                        RebuffTimer = 900000;
                    }
                    else RebuffTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                if (Spell1Timer <= diff)
                {
                    if (rand()%50 > 10)
                        DoCastVictim(SPELL_ONE_ALT);
                    else if (me->IsWithinDist(me->GetVictim(), 25.0f))
                        DoCastVictim(SPELL_ONE);

                    Spell1Timer = 5000;
                }
				else Spell1Timer -= diff;

                if (Spell2Timer <= diff)
                {
                    DoCastVictim(SPELL_TWO);
                    Spell2Timer = 37000;
                }
                else
                    Spell2Timer -= diff;

                if (Phase > 1)
                {
                    if (Spell3Timer <= diff)
                    {
                        DoCastVictim(SPELL_THREE);
                        Spell3Timer = 19000;
                    }
                    else Spell3Timer -= diff;

                    if (BeserkTimer <= diff)
                    {
                        Talk(SAY_BERSERK, me->GetVictim() ? me->GetVictim()->GetGUID() : 0);
                        DoCastVictim(SPELL_BERSERK);
                        BeserkTimer = 12000;
                    }
                    else BeserkTimer -= diff;
                }
                else if (Phase == 1)
                {
                    if (PhaseTimer <= diff)
                    {
                        ++Phase;
                        Talk(SAY_PHASE);
                        DoCast(me, SPELL_FRENZY);
                    }
                    else PhaseTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new example_creatureAI(creature);
        }
};

void AddSC_example_creature()
{
    new example_creature();
}