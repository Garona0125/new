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
#include "GameEventMgr.h"
#include "shadow_labyrinth.h"

enum Texts
{
	SAY_INTRO   = -1555028,
	SAY_AGGRO_1 = -1555029,
	SAY_AGGRO_2 = -1555030,
	SAY_AGGRO_3 = -1555031,
	SAY_HELP    = -1555032,
	SAY_SLAY_1  = -1555033,
	SAY_SLAY_2  = -1555034,
	SAY_DEATH   = -1555035,
};

enum Spells
{
	SPELL_RAIN_OF_FIRE_5N       = 33617,
	SPELL_RAIN_OF_FIRE_5H       = 39363,
	SPELL_DRAW_SHADOWS          = 33563,
	SPELL_SHADOWBOLT_VOLLEY     = 33841,
	SPELL_BANISH                = 38791,
	SPELL_SACRIFICE             = 33587,
	SPELL_SHADOW_NOVA           = 33846,
	SPELL_EMPOWERING_SHADOWS_5N = 33783,
	SPELL_EMPOWERING_SHADOWS_5H = 39364,
	SPELL_VOID_PORTAL_VISUAL    = 33569,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

float VorpilPosition[3] = {-252.8820f, -264.3030f, 17.1f };

const Position VoidPortalSpawnPos[7] =
{
	{-209.633f, -263.645f, 17.0864f, 3.12449f},
	{-261.425f, -297.32f, 17.0874f, 1.59689f},
	{-292.251f, -248.329f, 15.6303f, 5.68421f},
	{-307.587f, -256.851f, 12.6818f, 0.0441634f},
	{-295.945f, -269.158f, 12.6813f, 3.43799f},
	{-279.93f, -263.707f, 12.6812f, 0.0568528f},
	{-262.607f, -229.718f, 17.0871f, 4.83666f},
};

class npc_void_traveler : public CreatureScript
{
public:
    npc_void_traveler() : CreatureScript("npc_void_traveler") { }

    struct npc_void_travelerAI : public QuantumBasicAI
    {
        npc_void_travelerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MoveTimer;

        uint64 VorpilGUID;

        bool Sacrificed;

        void Reset()
        {
            VorpilGUID = 0;

            MoveTimer = 0;

            Sacrificed = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
				me->SetDisplayId(MODEL_CHRISTMAS_GRAND_MASTER_VORPIL);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(uint32 const diff)
        {
            if (!VorpilGUID)
            {
                me->Kill(me);
                return;
            }

            if (MoveTimer <= diff)
            {
                Creature* Vorpil = Unit::GetCreature(*me, VorpilGUID);
                if (!Vorpil)
                {
                    VorpilGUID = 0;
                    return;
                }

                if (Sacrificed)
                {
                    if (Vorpil->IsAlive())
                    {
                        me->AddAura(DUNGEON_MODE(SPELL_EMPOWERING_SHADOWS_5N, SPELL_EMPOWERING_SHADOWS_5H), Vorpil);
                        Vorpil->ModifyHealth(int32(Vorpil->CountPctFromMaxHealth(4)));
                    }
                    DoCast(me, SPELL_SHADOW_NOVA, true);
                    me->Kill(me);
                    return;
                }

                me->GetMotionMaster()->MoveFollow(Vorpil, 0, 0);

                if (me->IsWithinDist(Vorpil, 3))
                {
                    DoCast(me, SPELL_SACRIFICE, false);
                    Sacrificed = true;
                    MoveTimer = 500;
                    return;
                }

                if (!Vorpil->IsInCombatActive() || Vorpil->IsDead())
                {
                    me->Kill(me);
                    return;
                }
                MoveTimer = 1000;
            }
            else MoveTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_void_travelerAI(creature);
    }
};

class boss_grandmaster_vorpil : public CreatureScript
{
public:
    boss_grandmaster_vorpil() : CreatureScript("boss_grandmaster_vorpil") { }

    struct boss_grandmaster_vorpilAI : public QuantumBasicAI
    {
        boss_grandmaster_vorpilAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            Intro = false;
        }

        InstanceScript* instance;
		SummonList Summons;

        bool Intro;
		bool HelpYell;
        bool Sumportals;

        uint32 ShadowBoltVolleyTimer;
        uint32 DrawShadowsTimer;
        uint32 SummonTravelerTimer;
        uint32 BanishTimer;
        uint64 PortalsGuid[5];

        void Reset()
        {
            ShadowBoltVolleyTimer = 500;
			BanishTimer = 3000;
            DrawShadowsTimer = 5000;
            SummonTravelerTimer = 7000;

			Summons.DespawnAll();

			Intro = false;
            HelpYell = false;
			Sumportals = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);

			instance->SetData(DATA_GRAND_MASTER_VORPIL, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 60.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 60.0f))
			{
				DoSendQuantumText(SAY_INTRO, me);
				Intro = true;
			}
		}

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

			for (uint8 i = 0; i < 7; i++)
				me->SummonCreature(NPC_VOID_PORTAL, VoidPortalSpawnPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

			instance->SetData(DATA_GRAND_MASTER_VORPIL, IN_PROGRESS);
        }

		void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			Summons.DespawnAll();

			instance->SetData(DATA_GRAND_MASTER_VORPIL, DONE);
        }

		void JustSummoned(Creature* summoned)
		{
			switch (summoned->GetEntry())
			{
                case NPC_VOID_TRAVELER:
					CAST_AI(npc_void_traveler::npc_void_travelerAI, summoned->AI())->VorpilGUID = me->GetGUID();
					Summons.DoZoneInCombat();
					Summons.Summon(summoned);
				case NPC_VOID_PORTAL:
					summoned->CastSpell(summoned, SPELL_VOID_PORTAL_VISUAL, true);
					Summons.Summon(summoned);
					break;
				default:
					break;
			}
		}

        void SpawnVoidTraveler()
        {
			for (uint8 i = 0; i < 7; i++)
				me->SummonCreature(NPC_VOID_TRAVELER, VoidPortalSpawnPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

            if (!HelpYell)
            {
                DoSendQuantumText(SAY_HELP, me);
                HelpYell = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowBoltVolleyTimer <= diff)
            {
                DoCast(me, SPELL_SHADOWBOLT_VOLLEY);
                ShadowBoltVolleyTimer = urand(5000, 6000);
            }
			else ShadowBoltVolleyTimer -= diff;

            if (BanishTimer <= diff && IsHeroic())
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true))
                {
                    DoCast(target, SPELL_BANISH);
                    BanishTimer = 16000;
                }
            }
			else BanishTimer -= diff;

            if (DrawShadowsTimer <= diff)
            {
                Map* map = me->GetMap();
                Map::PlayerList const &PlayerList = map->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
				{
                    if (Player* player = i->getSource())
					{
                        if (player->IsAlive() && !player->HasAura(SPELL_BANISH))
                            player->TeleportTo(me->GetMapId(), VorpilPosition[0], VorpilPosition[1], VorpilPosition[2], 0, TELE_TO_NOT_LEAVE_COMBAT);
					}
				}

                me->SetPosition(VorpilPosition[0], VorpilPosition[1], VorpilPosition[2], 0.0f);
                DoCast(me, SPELL_DRAW_SHADOWS, true);

				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					DoCast(target, DUNGEON_MODE(SPELL_RAIN_OF_FIRE_5N, SPELL_RAIN_OF_FIRE_5H), true);

                ShadowBoltVolleyTimer = 6000;
                DrawShadowsTimer = 30000;
            }
			else DrawShadowsTimer -= diff;

            if (SummonTravelerTimer <= diff)
            {
                SpawnVoidTraveler();
                SummonTravelerTimer = 20000;

                if (HealthBelowPct(30))
                    SummonTravelerTimer = 10000;
            }
			else SummonTravelerTimer -=diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grandmaster_vorpilAI(creature);
    }
};

void AddSC_boss_grandmaster_vorpil()
{
    new boss_grandmaster_vorpil();
    new npc_void_traveler();
}