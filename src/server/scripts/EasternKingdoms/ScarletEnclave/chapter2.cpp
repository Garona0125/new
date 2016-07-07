/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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
#include "Player.h"
#include "SpellInfo.h"
#include "CreatureTextMgr.h"

enum win_friends
{
    SAY_PERSUADE1                     = -1609501,
    SAY_PERSUADE2                     = -1609502,
    SAY_PERSUADE3                     = -1609503,
    SAY_PERSUADE4                     = -1609504,
    SAY_PERSUADE5                     = -1609505,
    SAY_PERSUADE6                     = -1609506,
    SAY_PERSUADE7                     = -1609507,
    SAY_CRUSADER1                     = -1609508,
    SAY_CRUSADER2                     = -1609509,
    SAY_CRUSADER3                     = -1609510,
    SAY_CRUSADER4                     = -1609511,
    SAY_CRUSADER5                     = -1609512,
    SAY_CRUSADER6                     = -1609513,
    SAY_PERSUADED1                    = -1609514,
    SAY_PERSUADED2                    = -1609515,
    SAY_PERSUADED3                    = -1609516,
    SAY_PERSUADED4                    = -1609517,
    SAY_PERSUADED5                    = -1609518,
    SAY_PERSUADED6                    = -1609519,

    SPELL_PERSUASIVE_STRIKE           = 52781,
	SPELL_THREAT_PULSE                = 58111,

	QUEST_HOW_TO_WIN_FRIENDS          = 12720,
};

class npc_crusade_persuaded : public CreatureScript
{
public:
    npc_crusade_persuaded() : CreatureScript("npc_crusade_persuaded") { }

    struct npc_crusade_persuadedAI : public QuantumBasicAI
    {
        npc_crusade_persuadedAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SpeechTimer;
        uint32 SpeechCounter;
        uint64 PlayerGUID;

        void Reset()
        {
            SpeechTimer = 0;
            SpeechCounter = 0;
            PlayerGUID = 0;
            me->SetReactState(REACT_AGGRESSIVE);
            me->RestoreFaction();
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_PERSUASIVE_STRIKE && caster->GetTypeId() == TYPE_ID_PLAYER && me->IsAlive() && !SpeechCounter)
            {
                if (CAST_PLR(caster)->GetQuestStatus(QUEST_HOW_TO_WIN_FRIENDS) == QUEST_STATUS_INCOMPLETE)
                {
                    PlayerGUID = caster->GetGUID();
                    SpeechTimer = 1000;
                    SpeechCounter = 1;
                    me->SetCurrentFaction(caster->GetFaction());
                    me->CombatStop(true);
                    me->GetMotionMaster()->MoveIdle();
                    me->SetReactState(REACT_PASSIVE);
                    DoCastAOE(SPELL_THREAT_PULSE, true);

                    DoSendQuantumText(RAND(SAY_PERSUADE1, SAY_PERSUADE2, SAY_PERSUADE3, SAY_PERSUADE4, SAY_PERSUADE5, SAY_PERSUADE6, SAY_PERSUADE7), caster);
                    DoSendQuantumText(RAND(SAY_CRUSADER1, SAY_CRUSADER2, SAY_CRUSADER3, SAY_CRUSADER4, SAY_CRUSADER5, SAY_CRUSADER6), me);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (SpeechCounter)
            {
                if (SpeechTimer <= diff)
                {
                    Player* player = Unit::GetPlayer(*me, PlayerGUID);
                    if (!player)
                    {
                        EnterEvadeMode();
                        return;
                    }

                    switch (SpeechCounter)
                    {
                        case 1:
							DoSendQuantumText(SAY_PERSUADED1, me);
							SpeechTimer = 8000;
							break;
                        case 2:
							DoSendQuantumText(SAY_PERSUADED2, me);
							SpeechTimer = 8000;
							break;
                        case 3: DoSendQuantumText(SAY_PERSUADED3, me);
							SpeechTimer = 8000;
							break;
                        case 4:
							DoSendQuantumText(SAY_PERSUADED4, me);
							SpeechTimer = 8000;
							break;
                        case 5: DoSendQuantumText(SAY_PERSUADED5, player);
							SpeechTimer = 8000;
							break;
                        case 6: DoSendQuantumText(SAY_PERSUADED6, me);
                            player->Kill(me);
                            //me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            SpeechCounter = 0;
                            player->GroupEventHappens(QUEST_HOW_TO_WIN_FRIENDS, me);
                            return;
                    }

                    ++SpeechCounter;
                    DoCastAOE(SPELL_THREAT_PULSE, true);
                }
				else SpeechTimer -= diff;
                return;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crusade_persuadedAI(creature);
    }
};

enum Koltira
{
    SAY_BREAKOUT_1                  = -1609561,
    SAY_BREAKOUT_2                  = -1609562,
    SAY_BREAKOUT_3                  = -1609563,
    SAY_BREAKOUT_4                  = -1609564,
    SAY_BREAKOUT_5                  = -1609565,
    SAY_BREAKOUT_6                  = -1609566,
    SAY_BREAKOUT_7                  = -1609567,
    SAY_BREAKOUT_8                  = -1609568,
    SAY_BREAKOUT_9                  = -1609569,
    SAY_BREAKOUT_10                 = -1609570,

    SPELL_KOLTIRA_TRANSFORM         = 52899,
    SPELL_ANTI_MAGIC_ZONE           = 52894,

    QUEST_BLOODY_BREAKOUT           = 12727,

    NPC_CRIMSON_ACOLYTE             = 29007,
    NPC_HIGH_INQUISITOR_VALROTH     = 29001,
    NPC_KOLTIRA_ALT                 = 28447,
    //NPC_DEATH_KNIGHT_MOUNT          = 29201,
    MODEL_DEATH_KNIGHT_MOUNT        = 25278,
};

class npc_koltira_deathweaver : public CreatureScript
{
public:
    npc_koltira_deathweaver() : CreatureScript("npc_koltira_deathweaver") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_BLOODY_BREAKOUT)
        {
            creature->SetStandState(UNIT_STAND_STATE_STAND);

            if (npc_escortAI* EscortAI = CAST_AI(npc_koltira_deathweaver::npc_koltira_deathweaverAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID());
        }
        return true;
    }

    struct npc_koltira_deathweaverAI : public npc_escortAI
    {
        npc_koltira_deathweaverAI(Creature* creature) : npc_escortAI(creature)
        {
            me->SetReactState(REACT_DEFENSIVE);
        }

        uint32 Wave;
        uint32 WaveTimer;
        uint64 ValrothGUID;

        void Reset()
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                Wave = 0;
                WaveTimer = 3000;
                ValrothGUID = 0;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->LoadEquipment(0, true);
                me->RemoveAura(SPELL_ANTI_MAGIC_ZONE);
            }
        }

        void WaypointReached(uint32 PointId)
        {
            switch (PointId)
            {
                case 0:
                    DoSendQuantumText(SAY_BREAKOUT_1, me);
                    break;
                case 1:
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    break;
                case 2:
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    //me->UpdateEntry(NPC_KOLTIRA_ALT); // unclear if we must update or not
                    DoCast(me, SPELL_KOLTIRA_TRANSFORM);
                    me->LoadEquipment(me->GetEquipmentId());
                    break;
                case 3:
                    SetEscortPaused(true);
					me->SetReactState(REACT_PASSIVE);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoSendQuantumText(SAY_BREAKOUT_2, me);
                    DoCast(me, SPELL_ANTI_MAGIC_ZONE);  // cast again that makes bubble up
                    break;
                case 4:
                    SetRun(true);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    break;
                case 9:
                    me->Mount(MODEL_DEATH_KNIGHT_MOUNT);
                    break;
                case 10:
                    me->RemoveMount();
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (Player* player = GetPlayerForEscort())
                summoned->AI()->AttackStart(player);

            if (summoned->GetEntry() == NPC_HIGH_INQUISITOR_VALROTH)
                ValrothGUID = summoned->GetGUID();

            summoned->AddThreat(me, 0.0f);
            summoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
        }

        void SummonAcolyte(uint32 amount)
        {
            for (uint32 i = 0; i < amount; ++i)
				me->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329f, -6045.818f, 127.583f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
				if (Wave >= 0 && Wave < 5)
				{
					if (!me->HasAuraEffect(SPELL_ANTI_MAGIC_ZONE, 0))
						DoCast(me, SPELL_ANTI_MAGIC_ZONE, true);
				}
				
				if (WaveTimer <= diff)
                {
                    switch (Wave)
                    {
                        case 0:
                            DoSendQuantumText(SAY_BREAKOUT_3, me);
                            SummonAcolyte(3);
                            WaveTimer = 20000;
                            break;
                        case 1:
                            DoSendQuantumText(SAY_BREAKOUT_4, me);
                            SummonAcolyte(3);
                            WaveTimer = 20000;
                            break;
                        case 2:
                            DoSendQuantumText(SAY_BREAKOUT_5, me);
                            SummonAcolyte(4);
                            WaveTimer = 20000;
                            break;
                        case 3:
                            DoSendQuantumText(SAY_BREAKOUT_6, me);
                            me->SummonCreature(NPC_HIGH_INQUISITOR_VALROTH, 1642.329f, -6045.818f, 127.583f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
                            WaveTimer = 1000;
                            break;
                        case 4:
                        {
							Creature* temp = Unit::GetCreature(*me, ValrothGUID);

                            if (!temp || !temp->IsAlive())
                            {
								DoSendQuantumText(SAY_BREAKOUT_8, me);
                                WaveTimer = 5000;
                            }
                            else
                            {
                                WaveTimer = 2500;
                                return;                         //return, we don't want Wave to increment now
                            }
                            break;
                        }
						case 5:
                            DoSendQuantumText(SAY_BREAKOUT_9, me);
                            me->RemoveAurasDueToSpell(SPELL_ANTI_MAGIC_ZONE);
							me->SetReactState(REACT_DEFENSIVE);
                            // i do not know why the armor will also be removed
                            WaveTimer = 2500;
                            break;
                        case 6:
                            DoSendQuantumText(SAY_BREAKOUT_10, me);
                            SetEscortPaused(false);
                            break;
                    }
                    ++Wave;
                }
                else WaveTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_koltira_deathweaverAI(creature);
    }
};

//Scarlet courier
enum ScarletCourierEnum
{
	SAY_TREE1             = 0,
	SAY_TREE2             = 1,

    SPELL_SHOOT           = 52818,
    GO_INCONSPICUOUS_TREE = 191144,
    NPC_SCARLET_COURIER   = 29076,
};

class npc_scarlet_courier : public CreatureScript
{
public:
    npc_scarlet_courier() : CreatureScript("npc_scarlet_courier") { }

    struct npc_scarlet_courierAI : public QuantumBasicAI
    {
        npc_scarlet_courierAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 Stage;
        uint32 StageTimer;

        void Reset()
        {
            me->Mount(14338); // not sure about this id
            Stage = 1;
            StageTimer = 3000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_TREE2);

            me->RemoveMount();

            Stage = 0;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
                Stage = 2;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Stage && !me->IsInCombatActive())
            {
                if (StageTimer <= diff)
                {
                    switch (Stage)
                    {
                    case 1:
                        me->SetWalk(true);
                        if (GameObject* tree = me->FindGameobjectWithDistance(GO_INCONSPICUOUS_TREE, 40.0f))
                        {
                            Talk(SAY_TREE1);
                            float x, y, z;
                            tree->GetContactPoint(me, x, y, z);
                            me->GetMotionMaster()->MovePoint(1, x, y, z);
                        }
                        break;
                    case 2:
                        if (GameObject* tree = me->FindGameobjectWithDistance(GO_INCONSPICUOUS_TREE, 40.0f))
						{
                            if (Unit* unit = tree->GetOwner())
							{
                                AttackStart(unit);
							}
						}
                        break;
                    }
                    StageTimer = 3000;
                    Stage = 0;
                }
				else StageTimer -= diff;
			}

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_courierAI(creature);
    }
};

enum InquisitorValroth
{
	SAY_VALROTH_OOC              = -1609581, // The Crusade will purge your kind from this world!
    SAY_VALROTH_AGGRO            = -1609582, // It seems that I'll need to deal with you myself. The High Inquisitor comes for you, Scourge!
    SAY_VALROTH_RAND_1           = -1609583, // You have come seeking deliverance? I have come to deliver! // SAY
    SAY_VALROTH_RAND_2           = -1609584, // LIGHT PURGE YOU! // SAY
    SAY_VALROTH_RAND_3           = -1609585, // Coward! // SAY
    SAY_VALROTH_DEATH            = -1609586, // High Inquisitor Valroth's remains fall to the ground! // BOSS EMOTE

    SPELL_RENEW                  = 38210,
    SPELL_INQUISITOR_PENANCE     = 52922,
    SPELL_VALROTH_SMITE          = 52926,
    SPELL_SUMMON_VALROTH_REMAINS = 52929
};

class npc_high_inquisitor_valroth : public CreatureScript
{
public:
    npc_high_inquisitor_valroth() : CreatureScript("npc_high_inquisitor_valroth") { }

    struct npc_high_inquisitor_valrothAI : public QuantumBasicAI
    {
        npc_high_inquisitor_valrothAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 Stage;
        uint32 InquisitorPenanceTimer;
        uint32 ValrothSmiteTimer;

        void Reset()
        {
            Stage = 1000;
            InquisitorPenanceTimer = 2000;
            ValrothSmiteTimer = 1000;
        }

        void EnterToBattle(Unit* who)
        {
			DoSendQuantumText(SAY_VALROTH_AGGRO, me);
            DoCast(who, SPELL_VALROTH_SMITE);
        }

		void JustDied(Unit* killer)
        {
            DoSendQuantumText(SAY_VALROTH_DEATH, me);
            killer->CastSpell(me, SPELL_SUMMON_VALROTH_REMAINS, true);
        }

		void Shout()
        {
            if (rand()%100 < 15)
				DoSendQuantumText(RAND(SAY_VALROTH_RAND_1, SAY_VALROTH_RAND_2, SAY_VALROTH_RAND_3), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Stage <= diff)
            {
                Shout();
                DoCast(me, SPELL_RENEW);
                Stage = urand(1000, 6000);
            }
			else Stage -= diff;

            if (InquisitorPenanceTimer <= diff)
            {
                Shout();
                DoCastVictim(SPELL_INQUISITOR_PENANCE);
                InquisitorPenanceTimer = urand(2000, 7000);
            }
			else InquisitorPenanceTimer -= diff;

            if (ValrothSmiteTimer <= diff)
            {
                Shout();
                DoCastVictim(SPELL_VALROTH_SMITE);
                ValrothSmiteTimer = urand(1000, 6000);
            }
			else ValrothSmiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_inquisitor_valrothAI(creature);
    }
};

enum SpecialSurprise
{
    SAY_EXEC_START_1            = 0,                 // speech for all
    SAY_EXEC_START_2            = 1,
    SAY_EXEC_START_3            = 2,
    SAY_EXEC_PROG_1             = 3,
    SAY_EXEC_PROG_2             = 4,
    SAY_EXEC_PROG_3             = 5,
    SAY_EXEC_PROG_4             = 6,
    SAY_EXEC_PROG_5             = 7,
    SAY_EXEC_PROG_6             = 8,
    SAY_EXEC_PROG_7             = 9,
    SAY_EXEC_NAME_1             = 10,
    SAY_EXEC_NAME_2             = 11,
    SAY_EXEC_RECOG_1            = 12,
    SAY_EXEC_RECOG_2            = 13,
    SAY_EXEC_RECOG_3            = 14,
    SAY_EXEC_RECOG_4            = 15,
    SAY_EXEC_RECOG_5            = 16,
    SAY_EXEC_RECOG_6            = 17,
    SAY_EXEC_NOREM_1            = 18,
    SAY_EXEC_NOREM_2            = 19,
    SAY_EXEC_NOREM_3            = 20,
    SAY_EXEC_NOREM_4            = 21,
    SAY_EXEC_NOREM_5            = 22,
    SAY_EXEC_NOREM_6            = 23,
    SAY_EXEC_NOREM_7            = 24,
    SAY_EXEC_NOREM_8            = 25,
    SAY_EXEC_NOREM_9            = 26,
    SAY_EXEC_THINK_1            = 27,
    SAY_EXEC_THINK_2            = 28,
    SAY_EXEC_THINK_3            = 29,
    SAY_EXEC_THINK_4            = 30,
    SAY_EXEC_THINK_5            = 31,
    SAY_EXEC_THINK_6            = 32,
    SAY_EXEC_THINK_7            = 33,
    SAY_EXEC_THINK_8            = 34,
    SAY_EXEC_THINK_9            = 35,
    SAY_EXEC_THINK_10           = 36,
    SAY_EXEC_LISTEN_1           = 37,
    SAY_EXEC_LISTEN_2           = 38,
    SAY_EXEC_LISTEN_3           = 39,
    SAY_EXEC_LISTEN_4           = 40,
    SAY_PLAGUEFIST              = 41,
    SAY_EXEC_TIME_1             = 42,
    SAY_EXEC_TIME_2             = 43,
    SAY_EXEC_TIME_3             = 44,
    SAY_EXEC_TIME_4             = 45,
    SAY_EXEC_TIME_5             = 46,
    SAY_EXEC_TIME_6             = 47,
    SAY_EXEC_TIME_7             = 48,
    SAY_EXEC_TIME_8             = 49,
    SAY_EXEC_TIME_9             = 50,
    SAY_EXEC_TIME_10            = 51,
    SAY_EXEC_WAITING            = 52,
    EMOTE_DIES                  = 53,

    NPC_PLAGUEFIST              = 29053
};

class npc_a_special_surprise : public CreatureScript
{
public:
    npc_a_special_surprise() : CreatureScript("npc_a_special_surprise") { }

    struct npc_a_special_surpriseAI : public QuantumBasicAI
    {
        npc_a_special_surpriseAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ExecuteSpeechTimer;
        uint32 ExecuteSpeechCounter;
        uint64 PlayerGUID;

        void Reset()
        {
            ExecuteSpeechTimer = 0;
            ExecuteSpeechCounter = 0;
            PlayerGUID = 0;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
        }

        bool MeetQuestCondition(Player* player)
        {
            switch (me->GetEntry())
            {
                case 29061:                                     // Ellen Stanbridge
                    if (player->GetQuestStatus(12742) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29072:                                     // Kug Ironjaw
                    if (player->GetQuestStatus(12748) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29067:                                     // Donovan Pulfrost
                    if (player->GetQuestStatus(12744) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29065:                                     // Yazmina Oakenthorn
                    if (player->GetQuestStatus(12743) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29071:                                     // Antoine Brack
                    if (player->GetQuestStatus(12750) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29032:                                     // Malar Bravehorn
                    if (player->GetQuestStatus(12739) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29068:                                     // Goby Blastenheimer
                    if (player->GetQuestStatus(12745) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29073:                                     // Iggy Darktusk
                    if (player->GetQuestStatus(12749) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29074:                                     // Lady Eonys
                    if (player->GetQuestStatus(12747) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
                case 29070:                                     // Valok the Righteous
                    if (player->GetQuestStatus(12746) == QUEST_STATUS_INCOMPLETE)
                        return true;
                    break;
            }

            return false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (PlayerGUID || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDist(me, INTERACTION_DISTANCE))
                return;

            if (MeetQuestCondition(who->ToPlayer()))
                PlayerGUID = who->GetGUID();
        }

        void UpdateAI(const uint32 diff)
        {
            if (PlayerGUID && !me->GetVictim() && me->IsAlive())
            {
                if (ExecuteSpeechTimer <= diff)
                {
                    Player* player = Unit::GetPlayer(*me, PlayerGUID);

                    if (!player)
                    {
                        Reset();
                        return;
                    }

                    //TODO: simplify text's selection

                    switch (player->GetCurrentRace())
                    {
                        case RACE_HUMAN:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_5, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_1, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_5, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_7, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_6, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_ORC:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_6, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_1, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_7, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_8, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_8, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_DWARF:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_2, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_2, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_3, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_2, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_5, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_2, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_3, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_NIGHT_ELF:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_5, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_1, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_6, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_2, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_7, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_UNDEAD_PLAYER:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_3, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_4, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_3, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_1, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_3, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_4, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_TAUREN:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_1, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_5, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_8, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_9, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_9, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_GNOME:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_4, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_1, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_4, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_6, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_5, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_TROLL:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_3, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_7, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_2, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_6, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_9, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_10, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_4, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_10, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_BLOOD_ELF:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_1, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_1, player->GetGUID());
									break;
                                //case 5: //unknown
                                case 6:
									Talk(SAY_EXEC_THINK_3, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_1, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                        case RACE_DRAENEI:
                            switch (ExecuteSpeechCounter)
                            {
                                case 0:
									Talk(SAY_EXEC_START_1, player->GetGUID());
									break;
                                case 1:
									me->SetStandState(UNIT_STAND_STATE_STAND);
									break;
                                case 2:
									Talk(SAY_EXEC_PROG_1, player->GetGUID());
									break;
                                case 3:
									Talk(SAY_EXEC_NAME_1, player->GetGUID());
									break;
                                case 4:
									Talk(SAY_EXEC_RECOG_2, player->GetGUID());
									break;
                                case 5:
									Talk(SAY_EXEC_NOREM_1, player->GetGUID());
									break;
                                case 6:
									Talk(SAY_EXEC_THINK_4, player->GetGUID());
									break;
                                case 7:
									Talk(SAY_EXEC_LISTEN_1, player->GetGUID());
									break;
                                case 8:
                                    if (Creature* Plaguefist = GetClosestCreatureWithEntry(me, NPC_PLAGUEFIST, 85.0f))
                                        Plaguefist->AI()->Talk(SAY_PLAGUEFIST, player->GetGUID());
                                    break;
                                case 9:
                                    Talk(SAY_EXEC_TIME_2, player->GetGUID());
                                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                                    break;
                                case 10:
                                    Talk(SAY_EXEC_WAITING, player->GetGUID());
                                    break;
                                case 11:
                                    Talk(EMOTE_DIES);
                                    me->setDeathState(JUST_DIED);
                                    me->SetHealth(0);
                                    return;
                            }
                            break;
                    }

                    if (ExecuteSpeechCounter >= 9)
                        ExecuteSpeechTimer = 15000;
                    else
                        ExecuteSpeechTimer = 7000;

                    ++ExecuteSpeechCounter;
                }
                else ExecuteSpeechTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_a_special_surpriseAI(creature);
    }
};

enum TheLichKingChapter2
{
	AREA_DEATHS_BREACHS    = 4356,

	SPELL_ICEBOUND_VISAGE  = 53274,

	SAY_LICH_KING_SHOUT_1  = -1453000,
	SAY_LICH_KING_SHOUT_2  = -1453001,
	SAY_LICH_KING_SHOUT_3  = -1453002,
	SAY_LICH_KING_SHOUT_4  = -1453003,
	SAY_LICH_KING_SHOUT_5  = -1453004,
	SAY_LICH_KING_SHOUT_6  = -1453005,
	SAY_LICH_KING_SHOUT_8  = -1453006,
	SAY_LICH_KING_SHOUT_9  = -1453007,
	SAY_LICH_KING_SHOUT_10 = -1453008,
	SAY_LICH_KING_SHOUT_11 = -1453009,
	SAY_LICH_KING_SHOUT_12 = -1453010,
	SAY_LICH_KING_SHOUT_13 = -1453011,
	SAY_LICH_KING_SHOUT_14 = -1453012,
	SAY_LICH_KING_SHOUT_15 = -1453013,
	SAY_LICH_KING_SHOUT_16 = -1453014,
	SAY_LICH_KING_SHOUT_17 = -1453015,
};

int32 RandomCommandSay[] =
{
	SAY_LICH_KING_SHOUT_1,
	SAY_LICH_KING_SHOUT_2,
	SAY_LICH_KING_SHOUT_3,
	SAY_LICH_KING_SHOUT_4,
	SAY_LICH_KING_SHOUT_5,
	SAY_LICH_KING_SHOUT_6,
	SAY_LICH_KING_SHOUT_8,
	SAY_LICH_KING_SHOUT_9,
	SAY_LICH_KING_SHOUT_10,
	SAY_LICH_KING_SHOUT_11,
	SAY_LICH_KING_SHOUT_12,
	SAY_LICH_KING_SHOUT_13,
	SAY_LICH_KING_SHOUT_14,
	SAY_LICH_KING_SHOUT_15,
	SAY_LICH_KING_SHOUT_16,
	SAY_LICH_KING_SHOUT_17,
};

Position const LichPos[1] =
{
	{2310.55f, -5742.19f, 161.539f, 0.760744f},
};

class npc_the_lich_king_eh_chapter2 : public CreatureScript
{
public:
    npc_the_lich_king_eh_chapter2() : CreatureScript("npc_the_lich_king_eh_chapter2") {}

    struct npc_the_lich_king_eh_chapter2AI : public QuantumBasicAI
    {
        npc_the_lich_king_eh_chapter2AI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CheckHomePosTimer;
		uint32 ShoutCommandTimer;

        void Reset()
		{
			CheckHomePosTimer = 0.5*IN_MILLISECONDS;
			ShoutCommandTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_ICEBOUND_VISAGE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (CheckHomePosTimer <= diff && me->GetAreaId() == AREA_DEATHS_BREACHS && !me->IsInCombatActive())
			{
				me->GetMotionMaster()->MovePoint(0, LichPos[0]);
				CheckHomePosTimer = 10*IN_MILLISECONDS;
			}
			else CheckHomePosTimer -= diff;

			// Out of Combat Timer
			if (ShoutCommandTimer <= diff && me->GetAreaId() == AREA_DEATHS_BREACHS && !me->IsInCombatActive())
			{
				int32 SayId = rand()% (sizeof(RandomCommandSay)/sizeof(int32));
				DoSendQuantumText(RandomCommandSay[SayId], me);
				ShoutCommandTimer = 40*IN_MILLISECONDS;
			}
			else ShoutCommandTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_lich_king_eh_chapter2AI(creature);
    }
};

void AddSC_the_scarlet_enclave_c2()
{
    new npc_crusade_persuaded();
    new npc_scarlet_courier();
    new npc_koltira_deathweaver();
    new npc_high_inquisitor_valroth();
    new npc_a_special_surprise();
	new npc_the_lich_king_eh_chapter2();
}