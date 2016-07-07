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

/* ScriptData
SDName: Boss_Victor_Nefarius
SD%Complete: 75
SDComment: Missing some text, Vael beginning event, and spawns Nef in wrong place
SDCategory: Blackwing Lair
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"

#define GOSSIP_ITEM_1 "I've made no mistakes."
#define GOSSIP_ITEM_2 "You have lost your mind, Nefarius. You speak in riddles."
#define GOSSIP_ITEM_3 "Please do."

#define ADD_X1    -7591.151855f
#define ADD_X2    -7514.598633f
#define ADD_Y1    -1204.051880f
#define ADD_Y2    -1150.448853f
#define ADD_Z1    476.800476f
#define ADD_Z2    476.796570f

#define NEF_X     -7445
#define NEF_Y     -1332
#define NEF_Z     536

#define HIDE_X   -7592
#define HIDE_Y   -1264
#define HIDE_Z   481

enum Says
{
     SAY_GAMES_BEGIN_1       = 0,
	 SAY_GAMES_BEGIN_2       = 1,
	 SAY_VAEL_INTRO          = 2,
};

enum Creatures
{
    NPC_BRONZE_DRAKANOID       = 14263,
    NPC_BLUE_DRAKANOID         = 14261,
    NPC_RED_DRAKANOID          = 14264,
    NPC_GREEN_DRAKANOID        = 14262,
    NPC_BLACK_DRAKANOID        = 14265,

    NPC_CHROMATIC_DRAKANOID    = 14302,
    NPC_NEFARIAN               = 11583,
};

enum Spells
{
   SPELL_SHADOWBOLT = 21077,
   SPELL_FEAR       = 26070,
};

class boss_victor_nefarius : public CreatureScript
{
public:
    boss_victor_nefarius() : CreatureScript("boss_victor_nefarius") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(7134, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*Sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(7198, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(7199, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->CLOSE_GOSSIP_MENU();
                creature->AI()->Talk(SAY_GAMES_BEGIN_1);
                CAST_AI(boss_victor_nefarius::boss_victor_nefariusAI, creature->AI())->BeginEvent(player);
                break;
        }
        return true;
    }

    struct boss_victor_nefariusAI : public QuantumBasicAI
    {
        boss_victor_nefariusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NefarianGUID = 0;
            switch (urand(0, 19))
            {
                case 0:
                    DrakType1 = NPC_BRONZE_DRAKANOID;
                    DrakType2 = NPC_BLUE_DRAKANOID;
                    break;
                case 1:
                    DrakType1 = NPC_BRONZE_DRAKANOID;
                    DrakType2 = NPC_RED_DRAKANOID;
                    break;
                case 2:
                    DrakType1 = NPC_BRONZE_DRAKANOID;
                    DrakType2 = NPC_GREEN_DRAKANOID;
                    break;
                case 3:
                    DrakType1 = NPC_BRONZE_DRAKANOID;
                    DrakType2 = NPC_BLACK_DRAKANOID;
                    break;
                case 4:
                    DrakType1 = NPC_BLUE_DRAKANOID;
                    DrakType2 = NPC_BRONZE_DRAKANOID;
                    break;
                case 5:
                    DrakType1 = NPC_BLUE_DRAKANOID;
                    DrakType2 = NPC_RED_DRAKANOID;
                    break;
                case 6:
                    DrakType1 = NPC_BLUE_DRAKANOID;
                    DrakType2 = NPC_GREEN_DRAKANOID;
                    break;
                case 7:
                    DrakType1 = NPC_BLUE_DRAKANOID;
                    DrakType2 = NPC_BLACK_DRAKANOID;
                    break;
                case 8:
                    DrakType1 = NPC_RED_DRAKANOID;
                    DrakType2 = NPC_BRONZE_DRAKANOID;
                    break;
                case 9:
                    DrakType1 = NPC_RED_DRAKANOID;
                    DrakType2 = NPC_BLUE_DRAKANOID;
                    break;
                case 10:
                    DrakType1 = NPC_RED_DRAKANOID;
                    DrakType2 = NPC_GREEN_DRAKANOID;
                    break;
                case 11:
                    DrakType1 = NPC_RED_DRAKANOID;
                    DrakType2 = NPC_BLACK_DRAKANOID;
                    break;
                case 12:
                    DrakType1 = NPC_GREEN_DRAKANOID;
                    DrakType2 = NPC_BRONZE_DRAKANOID;
                    break;
                case 13:
                    DrakType1 = NPC_GREEN_DRAKANOID;
                    DrakType2 = NPC_BLUE_DRAKANOID;
                    break;
                case 14:
                    DrakType1 = NPC_GREEN_DRAKANOID;
                    DrakType2 = NPC_RED_DRAKANOID;
                    break;
                case 15:
                    DrakType1 = NPC_GREEN_DRAKANOID;
                    DrakType2 = NPC_BLACK_DRAKANOID;
                    break;
                case 16:
                    DrakType1 = NPC_BLACK_DRAKANOID;
                    DrakType2 = NPC_BRONZE_DRAKANOID;
                    break;
                case 17:
                    DrakType1 = NPC_BLACK_DRAKANOID;
                    DrakType2 = NPC_BLUE_DRAKANOID;
                    break;
                case 18:
                    DrakType1 = NPC_BLACK_DRAKANOID;
                    DrakType2 = NPC_GREEN_DRAKANOID;
                    break;
                case 19:
                    DrakType1 = NPC_BLACK_DRAKANOID;
                    DrakType2 = NPC_RED_DRAKANOID;
                    break;
            }
        }

        uint32 SpawnedAdds;
        uint32 AddSpawnTimer;
        uint32 ShadowBoltTimer;
        uint32 FearTimer;
        uint32 MindControlTimer;
        uint32 ResetTimer;
        uint32 DrakType1;
        uint32 DrakType2;
        uint64 NefarianGUID;
        uint32 NefCheckTime;

        void Reset()
        {
            SpawnedAdds = 0;
            AddSpawnTimer = 10*IN_MILLISECONDS;
            ShadowBoltTimer = 5*IN_MILLISECONDS;
            FearTimer = 8*IN_MILLISECONDS;
            ResetTimer = 900000;
            NefarianGUID = 0;
            NefCheckTime = 2*IN_MILLISECONDS;

            me->SetUInt32Value(UNIT_NPC_FLAGS, 1);
            me->SetCurrentFaction(35);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void BeginEvent(Player* target)
        {
            Talk(SAY_GAMES_BEGIN_2);

            //Trinity::Singleton<MapManager>::Instance().GetMap(me->GetMapId(), me)->GetPlayers().begin();
            /*
            list <Player*>::const_iterator i = sMapMgr->GetMap(me->GetMapId(), me)->GetPlayers().begin();

            for (i = sMapMgr->GetMap(me->GetMapId(), me)->GetPlayers().begin(); i != sMapMgr->GetMap(me->GetMapId(), me)->GetPlayers().end(); ++i)
            {
            AttackStart((*i));
            }
            */
            me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            me->SetCurrentFaction(103);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            AttackStart(target);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (who && who->GetTypeId() == TYPE_ID_PLAYER && me->IsHostileTo(who))
                me->AddThreat(who, 0.0f);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SpawnedAdds < 42)
            {
                if (ShadowBoltTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_SHADOWBOLT);

                    ShadowBoltTimer = urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS);
                }
				else ShadowBoltTimer -= diff;

                if (FearTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_FEAR);

                    FearTimer = 10*IN_MILLISECONDS + (rand()%10*IN_MILLISECONDS);
                }
				else FearTimer -= diff;

                if (AddSpawnTimer <= diff)
                {
                    uint32 CreatureID;
                    Creature* Spawned = NULL;
                    Unit* target = NULL;

                    if (urand(0, 2) == 0)
                        CreatureID = NPC_CHROMATIC_DRAKANOID;
                    else
                        CreatureID = DrakType1;

                    ++SpawnedAdds;

                    Spawned = me->SummonCreature(CreatureID, ADD_X1, ADD_Y1, ADD_Z1, 5.000f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                    target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target && Spawned)
                    {
                        Spawned->AI()->AttackStart(target);
                        Spawned->SetCurrentFaction(103);
                    }

                    if (urand(0, 2) == 0)
                        CreatureID = NPC_CHROMATIC_DRAKANOID;
                    else
                        CreatureID = DrakType2;

                    ++SpawnedAdds;

                    Spawned = me->SummonCreature(CreatureID, ADD_X2, ADD_Y2, ADD_Z2, 5.000f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                    target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target && Spawned)
                    {
                        Spawned->AI()->AttackStart(target);
                        Spawned->SetCurrentFaction(103);
                    }

                    if (SpawnedAdds >= 42)
                    {
                        //sMapMgr->GetMap(me->GetMapId(), me)->CreatureRelocation(me, 0, 0, -5000, 0);
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, 33356);
                        DoCast(me, 8149);
                        //Teleport self to a hiding spot (this causes errors in the Trinity log but no real issues)
                        DoTeleportTo(HIDE_X, HIDE_Y, HIDE_Z);
                        me->AddUnitState(UNIT_STATE_FLEEING);

                        Creature* Nefarian = me->SummonCreature(NPC_NEFARIAN, NEF_X, NEF_Y, NEF_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                        if (target && Nefarian)
                        {
                            Nefarian->AI()->AttackStart(target);
                            Nefarian->SetCurrentFaction(103);
                            NefarianGUID = Nefarian->GetGUID();
                        }
                        else sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: Blackwing Lair: Unable to spawn nefarian properly.");
                    }

                    AddSpawnTimer = 4*IN_MILLISECONDS;
                }
				else AddSpawnTimer -= diff;
            }
            else if (NefarianGUID)
            {
                if (NefCheckTime <= diff)
                {
                    Unit* Nefarian = Unit::GetCreature(*me, NefarianGUID);

                    if (!Nefarian || !Nefarian->IsAlive())
                    {
                        NefarianGUID = 0;
                        me->DespawnAfterAction();
                    }

                    NefCheckTime = 2*IN_MILLISECONDS;
                }
				else NefCheckTime -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_victor_nefariusAI(creature);
    }
};

void AddSC_boss_victor_nefarius()
{
    new boss_victor_nefarius();
}