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

enum Spells
{
	SPELL_REVIVE = 51918,
};

#define VALK_WHISPER "It is not yet your time, champion. Rise! Rise and fight once more!"

class npc_valkyr_battle_maiden : public CreatureScript
{
public:
    npc_valkyr_battle_maiden() : CreatureScript("npc_valkyr_battle_maiden") { }

    struct npc_valkyr_battle_maidenAI : public PassiveAI
    {
        npc_valkyr_battle_maidenAI(Creature* creature) : PassiveAI(creature) {}

        uint32 FlyBackTimer;
        float x, y, z;
        uint32 phase;

        void Reset()
        {
            me->SetActive(true);
            me->SetVisible(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetCanFly(true);

            FlyBackTimer = 500;

            phase = 0;

            me->GetPosition(x, y, z);
            z += 4.0f;
            x -= 3.5f;
            y -= 5.0f;
            me->GetMotionMaster()->Clear(false);
            me->SetPosition(x, y, z, 0.0f);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (FlyBackTimer <= diff)
            {
                Player* player = NULL;
                if (me->IsSummon())
				{
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
					{
                        if (summoner->GetTypeId() == TYPE_ID_PLAYER)
                            player = CAST_PLR(summoner);
					}
				}

                if (!player)
                    phase = 3;

                switch (phase)
                {
                    case 0:
                        me->SetWalk(false);
                        me->HandleEmoteCommand(EMOTE_STATE_FLY_GRAB_CLOSED);
                        FlyBackTimer = 500;
                        break;
                    case 1:
                        player->GetClosePoint(x, y, z, me->GetObjectSize());
                        z += 2.5f;
                        x -= 2.0f;
                        y -= 1.5f;
                        me->GetMotionMaster()->MovePoint(0, x, y, z);
                        me->SetTarget(player->GetGUID());
                        me->SetVisible(true);
                        FlyBackTimer = 4500;
                        break;
                    case 2:
                        if (!player->isRessurectRequested())
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_01);
                            DoCast(player, SPELL_REVIVE, true);
                            me->MonsterWhisper(VALK_WHISPER, player->GetGUID());
                        }
                        FlyBackTimer = 5000;
                        break;
                    case 3:
                        me->SetVisible(false);
                        FlyBackTimer = 3000;
                        break;
                    case 4:
                        me->DisappearAndDie();
                        break;
                    default:
                        break;
                }
                ++phase;
            }
			else FlyBackTimer-=diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valkyr_battle_maidenAI(creature);
    }
};

class npc_acherus_teleporter :  public CreatureScript
{
public:
    npc_acherus_teleporter() : CreatureScript("npc_acherus_teleporter") {}

    struct npc_acherus_teleporterAI : public QuantumBasicAI
    {
        npc_acherus_teleporterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint32 CheckTimer;
        uint32 Check2Timer;

        void Reset()
        {
            CheckTimer = 5000;
            Check2Timer = 300000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer < diff)
            {
                switch(me->GetEntry())
                {
                //Map 609
                case 29580:
                    if (!me->HasAuraEffect(54700, 0))
					{
						me->CastSpell(me, 54700, true);
					}
                    break;
                case 29581:
                    if (!me->HasAuraEffect(54724, 0))
					{
						me->CastSpell(me, 54724, true);
					}
                    break;
                //Map 0
                case 29589:
                    if (!me->HasAuraEffect(54745, 0))
					{
                        me->CastSpell(me, 54745, true);
					}
                    break;
                case 29588:
                    if (!me->HasAuraEffect(54742, 0))
					{
                        me->CastSpell(me, 54742, true);
					}
					break;
                }
                CheckTimer = 5000;
            }
			else CheckTimer -= diff;

            if (Check2Timer < diff)
            {
                me->RemoveAurasDueToSpell(54700);
                me->RemoveAurasDueToSpell(54745);
                me->RemoveAurasDueToSpell(54724);
                me->RemoveAurasDueToSpell(54744);
                Check2Timer = 300000;
            }
			else Check2Timer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature)
    {
        return new npc_acherus_teleporterAI(creature);
    }
};

void AddSC_the_scarlet_enclave()
{
    new npc_valkyr_battle_maiden();
    new npc_acherus_teleporter();
	// UPDATE creature_template SET scriptname = 'npc_acherus_teleporter' WHERE entry IN (29580, 29581, 29588, 29589);
}