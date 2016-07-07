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
#include "blackrock_depths.h"

enum Spells
{
    SPELL_SMELT_DARK_IRON              = 14891,
    SPELL_LEARN_SMELT                  = 14894,

    QUEST_SPECTRAL_CHALICE             = 4083,

    DATA_SKILLPOINT_MIN                = 230,
};

#define GOSSIP_ITEM_TEACH_1 "Teach me the art of smelting dark iron"
#define GOSSIP_ITEM_TEACH_2 "Continue..."
#define GOSSIP_ITEM_TEACH_3 "[PH] Continue..."
#define GOSSIP_ITEM_TRIBUTE "I want to pay tribute"

class boss_gloomrel : public CreatureScript
{
    public:
        boss_gloomrel() : CreatureScript("boss_gloomrel") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestRewardStatus(QUEST_SPECTRAL_CHALICE) == 1 && player->GetSkillValue(SKILL_MINING) >= DATA_SKILLPOINT_MIN && !player->HasSpell(SPELL_SMELT_DARK_IRON))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (player->GetQuestRewardStatus(QUEST_SPECTRAL_CHALICE) == 0 && player->GetSkillValue(SKILL_MINING) >= DATA_SKILLPOINT_MIN)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRIBUTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                    player->SEND_GOSSIP_MENU(2606, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 11:
                    player->CastSpell(player, SPELL_LEARN_SMELT, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
                    player->SEND_GOSSIP_MENU(2604, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 22:
                    if (InstanceScript* instance = creature->GetInstanceScript())
                        instance->DoRespawnGameObject(instance->GetData64(DATA_GO_CHALICE), MINUTE*5);

                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
            return true;
        }
};

enum DoomrelSpells
{
    SPELL_SHADOWBOLTVOLLEY           = 15245,
    SPELL_IMMOLATE                   = 12742,
    SPELL_CURSEOFWEAKNESS            = 12493,
    SPELL_DEMONARMOR                 = 13787,
    SPELL_SUMMON_VOIDWALKERS         = 15092,
};

#define GOSSIP_ITEM_CHALLENGE "Your bondage is at an end, Doom'rel. I challenge you!"
#define GOSSIP_SELECT_DOOMREL "[PH] Continue..."

class boss_doomrel : public CreatureScript
{
public:
    boss_doomrel() : CreatureScript("boss_doomrel") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(2601, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*Sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_DOOMREL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(2605, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CLOSE_GOSSIP_MENU();
                //start event here
                creature->SetCurrentFaction(FACTION_HOSTILE);
                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                creature->AI()->AttackStart(player);
                InstanceScript* instance = creature->GetInstanceScript();
                if (instance)
                    instance->SetData64(DATA_EVENSTARTER, player->GetGUID());
                break;
        }
        return true;
    }

    struct boss_doomrelAI : public QuantumBasicAI
    {
        boss_doomrelAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 ShadowVolleyTimer;
        uint32 ImmolateTimer;
        uint32 CurseOfWeaknessTimer;
        uint32 DemonArmorTimer;
        bool Voidwalkers;

        void Reset()
        {
            ShadowVolleyTimer = 10000;
            ImmolateTimer = 18000;
            CurseOfWeaknessTimer = 5000;
            DemonArmorTimer = 16000;
            Voidwalkers = false;

            me->SetCurrentFaction(FACTION_FRIEND);

            // was set before event start, so set again
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

            if (instance)
            {
                if (instance->GetData(DATA_GHOSTKILL) >= 7)
                    me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                else
                    me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void EnterEvadeMode()
        {
            me->RemoveAllAuras();
            me->DeleteThreatList();
            me->CombatStop(true);
            me->LoadCreaturesAddon();

            if (me->IsAlive())
                me->GetMotionMaster()->MoveTargetedHome();

            me->SetLootRecipient(NULL);

            if (instance)
                instance->SetData64(DATA_EVENSTARTER, 0);
        }

        void JustDied(Unit* /*who*/)
        {
            if (instance)
                instance->SetData(DATA_GHOSTKILL, 1);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowVolleyTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOWBOLTVOLLEY);
                ShadowVolleyTimer = 12000;
            }
			else ShadowVolleyTimer -= diff;

            if (ImmolateTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_IMMOLATE);
					ImmolateTimer = 25000;
				}
            }
			else ImmolateTimer -= diff;

            if (CurseOfWeaknessTimer <= diff)
            {
                DoCastVictim(SPELL_CURSEOFWEAKNESS);
                CurseOfWeaknessTimer = 45000;
            }
			else CurseOfWeaknessTimer -= diff;

            if (DemonArmorTimer <= diff)
            {
                DoCast(me, SPELL_DEMONARMOR);
                DemonArmorTimer = 300000;
            }
			else DemonArmorTimer -= diff;

            if (!Voidwalkers && HealthBelowPct(51))
            {
                DoCastVictim(SPELL_SUMMON_VOIDWALKERS, true);
                Voidwalkers = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_doomrelAI(creature);
    }
};

void AddSC_boss_tomb_of_seven()
{
    new boss_gloomrel();
    new boss_doomrel();
}