/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
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
#include "QuantumEscortAI.h"
#include "QuantumFollowerAI.h"

enum pus
{
    SAY_1   = -1901000,
    SAY_2   = -1901001,                     
    SAY_3   = -1901002,
    SAY_4   = -1901003,
    SAY_5   = -1901004,

    NPC_IMP = 13276,
};

enum Spells
{
    SPELL_BLAST_WAVE = 17145,
    SPELL_FIRE_BLAST = 14145,
    SPELL_FIREBALL   = 15228,
    SPELL_RUNN       = 22735,
};


const Position SpawnPosition1 =
{
	23.019f,-703.442f,-12.642f,3.23f
};

const Position SpawnPosition2 =
{
	22.646f,-691.948f,-12.642f,4.13f
};

const Position SpawnPosition3 =
{
	12.277f,-676.333f,-12.642f,4.97f
};

const Position SpawnPosition4 =
{
	24.416f,-680.573f,-12.642f,4.31f
};

#define GOSSIP_HELLO "Game? Are your crazy?"
#define Gossip_2 "Why you little..."
#define Gossip_3 "Mark my words, I will catch you, Imp. And when I do!"
#define Gossip_4 "DIE!"
#define Gossip_5 "Prepare to meet your Maker."

class npc_pusillin : public CreatureScript
{
public:
    npc_pusillin() : CreatureScript("npc_pusillin") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        npc_pusillinAI* pAI = CAST_AI(npc_pusillinAI, creature->AI());
        
        switch (pAI->GossipStep)
        {
        case 0:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(6877, creature->GetGUID());
            break;
        case 1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(6878, creature->GetGUID());
            break;
        case 2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(6879, creature->GetGUID());
            break;
        case 3:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->SEND_GOSSIP_MENU(6880, creature->GetGUID());
            break;
        case 4:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            player->SEND_GOSSIP_MENU(6881, creature->GetGUID());
            break;
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        npc_pusillinAI* pAI = CAST_AI(npc_pusillinAI,creature->AI());

        if (!pAI)
            return false;

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            CAST_AI(npc_pusillinAI, creature->AI())->Start(false, true, player->GetGUID());
            pAI->SetHoldState(false);
            pAI->Phase = 1;
        }

        if (action == GOSSIP_ACTION_INFO_DEF+2)
        {
            pAI->Phase = 2;
            pAI->SetHoldState(false);
        }
        
        if (action == GOSSIP_ACTION_INFO_DEF+3)
        {
            pAI->Phase = 3;
            pAI->SetHoldState(false);
        }
        
        if (action == GOSSIP_ACTION_INFO_DEF+4)
        {
            pAI->Phase = 4;
            pAI->SetHoldState(false);
        }
        
        if (action == GOSSIP_ACTION_INFO_DEF+5)
        {
            pAI->Phase = 5;
        }

        player->CLOSE_GOSSIP_MENU();
        pAI->SetDespawnAtEnd(false);
        pAI->SetDespawnAtFar(false);
        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        return true;
    }

    struct npc_pusillinAI : public npc_escortAI
    {
        npc_pusillinAI(Creature* creature) : npc_escortAI(creature) {}

        uint32 Phase;
        uint32 GossipStep;
        uint32 BlastWaveTimer;
        uint32 FireBlastTimer;
        uint32 FireBallTimer;
        uint32 SpiritOfRunnTimer;
        bool buffed;

        void Reset()
        {
            Phase = 0;
            GossipStep = 0;
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            BlastWaveTimer = urand(7000, 9000);
            FireBlastTimer = urand(3000, 6000);
            FireBallTimer = urand(11000, 13000);
            SpiritOfRunnTimer = urand(5000, 7000);
            buffed=false;
        }
        
        void SetHoldState(bool bOnHold)
        {
            SetEscortPaused(bOnHold);
        }
        
        void WaypointReached(uint32 pointId)
        {
            switch(pointId)
            {
            case 0:
                break;
            case 2:
                GossipStep = 1;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetHoldState(true);
                break;
            case 4:
                GossipStep = 2;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetHoldState(true);
                break;
            case 10:
                GossipStep = 3;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetHoldState(true);
                break;
            case 16:
                GossipStep = 4;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
                
            if (Phase)
            {
                switch(Phase)
                {
                case 1:
                    DoSendQuantumText(SAY_1, me);
                    Phase = 0;
                    break;
                case 2:   
                    DoSendQuantumText(SAY_2, me);
                    Phase = 0;
                    break;
                case 3:
                    DoSendQuantumText(SAY_3, me);
                    Phase = 0;
                    break;
                case 4:
                    DoSendQuantumText(SAY_4, me);
                    Phase = 0;
                    break;
                case 5:
                    DoSendQuantumText(SAY_5, me);
                    me->SetCurrentFaction(16);
                    float scale = me->GetFloatValue(OBJECT_FIELD_SCALE_X);
                    me->SetObjectScale(scale* 3);
                    Unit* target = SelectTarget(TARGET_NEAREST, 0);
                    me->AI()->AttackStart(target);
                    for(uint8 i = 0; i<5; i++)
                    {
                        switch(i)
                        {
                        case 1:
                            if (Creature* imp = me->SummonCreature(NPC_IMP, SpawnPosition1, TEMPSUMMON_CORPSE_DESPAWN, 120000))
							{
								if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									imp->AI()->AttackStart(target);
							}
                            break;
                        case 2:
                            if (Creature* imp = me->SummonCreature(NPC_IMP, SpawnPosition2, TEMPSUMMON_CORPSE_DESPAWN, 120000))
							{
								if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									imp->AI()->AttackStart(target);
							}
                            break;
                        case 3:
                            if (Creature* imp = me->SummonCreature(NPC_IMP, SpawnPosition3, TEMPSUMMON_CORPSE_DESPAWN, 120000))
							{
								if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									imp->AI()->AttackStart(target);
							}
                            break;
                        case 4:
                            if (Creature* imp = me->SummonCreature(NPC_IMP, SpawnPosition4, TEMPSUMMON_CORPSE_DESPAWN, 120000))
							{
								if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									imp->AI()->AttackStart(target);
							}
							break;
                        }
                    }
                    Phase=6;
                    break;
                }
            }
    
            if (Phase==6)
            {
                if (!UpdateVictim())
                    return;

                if (BlastWaveTimer <= diff)
                {
                    DoCastVictim(SPELL_BLAST_WAVE);
                    BlastWaveTimer = urand(7000, 9000);
                }
                else BlastWaveTimer -= diff;

                if (FireBlastTimer <= diff)
                {
                    DoCastVictim(SPELL_FIRE_BLAST);
                    FireBlastTimer = urand(3000, 6000);
                }
                else FireBlastTimer -= diff;

                if (FireBallTimer <= diff)
                {
                    DoCastVictim(SPELL_FIREBALL);
                    FireBallTimer  = urand(11000, 13000);
                }
                else FireBallTimer -= diff;

                if (SpiritOfRunnTimer <= diff && buffed==false)
                {
                    DoCast(me, SPELL_RUNN);
                    buffed = true;
                }
                else SpiritOfRunnTimer -= diff;         

                DoMeleeAttackIfReady();
            }
        }
    };
        
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pusillinAI(creature);
    }
};

void AddSC_npc_pusillin()
{
    new npc_pusillin();
}