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
#include "QuantumGossip.h"

enum Spells
{
	SPELL_DUAL_WIELD = 674,
};

enum ThrallWarchief
{
    QUEST_WHAT_THE_WIND    = 6566,
    SPELL_CHAIN_LIGHTNING  = 16033,
    SPELL_FLAME_SHOCK      = 59684,
	SPELL_BLOODLUST        = 37599,
	SPELL_SHOCK_BARRIER    = 46165,
	SPELL_LIGHTNING_SHIELD = 59025,
	THRALL_SOUND_AGGRO     = 5880,
};

#define GOSSIP_HTW   "Please share your wisdom with me, Warchief."
#define GOSSIP_STW_1 "What discoveries?"
#define GOSSIP_STW_2 "Usurper?"
#define GOSSIP_STW_3 "With all due respect, Warchief - why not allow them to be destroyed? Does this not strengthen our position?"
#define GOSSIP_STW_4 "I... I did not think of it that way, Warchief."
#define GOSSIP_STW_5 "I live only to serve, Warchief! My life is empty and meaningless without your guidance."
#define GOSSIP_STW_6 "Of course, Warchief!"

class boss_thrall_warchief : public CreatureScript
{
public:
    boss_thrall_warchief() : CreatureScript("boss_thrall_warchief") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_WHAT_THE_WIND) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HTW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(5733, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(5734, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(5735, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(5736, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                player->SEND_GOSSIP_MENU(5737, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
                player->SEND_GOSSIP_MENU(5738, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_WHAT_THE_WIND);
                break;
        }
        return true;
    }

    struct boss_thrall_warchiefAI : public QuantumBasicAI
    {
        boss_thrall_warchiefAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ChainLightningTimer;
        uint32 FlameShockTimer;
		uint32 ShockBarrierTimer;

        void Reset()
        {
			ChainLightningTimer = 500;
            FlameShockTimer = 2000;
			ShockBarrierTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 1);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(THRALL_SOUND_AGGRO);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

			DoCast(me, SPELL_BLOODLUST, true);
		}

        void UpdateAI(uint32 const diff)
        {
			if (!me->HasAura(SPELL_LIGHTNING_SHIELD) && !me->IsInCombatActive())
				DoCast(me, SPELL_LIGHTNING_SHIELD);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_LIGHTNING_SHIELD))
			{
				DoCast(me, SPELL_BLOODLUST, true);
				DoCast(me, SPELL_LIGHTNING_SHIELD, true);
			}

            if (ChainLightningTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CHAIN_LIGHTNING, true);
					ChainLightningTimer = urand(3000, 4000);
				}
            }
			else ChainLightningTimer -= diff;

            if (FlameShockTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_SHOCK, true);
					FlameShockTimer = urand(5000, 6000);
				}
            }
			else FlameShockTimer -= diff;

			if (ShockBarrierTimer <= diff)
            {
				DoCast(me, SPELL_SHOCK_BARRIER, true);
				ShockBarrierTimer = urand(10000, 11000);
            }
			else ShockBarrierTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_thrall_warchiefAI(creature);
    }
};

enum Voljin
{
	SPELL_VEIL_OF_SHADOW  = 17820,
	SPELL_VJ_SHADOW_WP    = 17146,
	SPELL_VJ_SHADOW_SHOCK = 17289,
	SPELL_VJ_HEX          = 16097,
	SPELL_VJ_ENRAGE       = 15716,
	VOLJIN_SOUND_AGGRO    = 5881,
};

class boss_voljin : public CreatureScript
{
public:
    boss_voljin() : CreatureScript("boss_voljin") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_voljinAI : public QuantumBasicAI
    {
        boss_voljinAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VeilOfShadowTimer;
		uint32 ShadowWordPainTimer;
		uint32 ShadowFlameShockTimer;
		uint32 HexTimer;

        void Reset()
        {
			VeilOfShadowTimer = 500;
			ShadowWordPainTimer = 1500;
			ShadowFlameShockTimer = 3000;
			HexTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(VOLJIN_SOUND_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VeilOfShadowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VEIL_OF_SHADOW);
					VeilOfShadowTimer = urand(3000, 4000);
				}
			}
			else VeilOfShadowTimer -= diff;

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VJ_SHADOW_WP);
					ShadowWordPainTimer = urand(5000, 6000);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (ShadowFlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VJ_SHADOW_SHOCK);
					ShadowFlameShockTimer = urand(7000, 8000);
				}
			}
			else ShadowFlameShockTimer -= diff;

			if (HexTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VJ_HEX);
					HexTimer = urand(9000, 10000);
				}
			}
			else HexTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_VJ_ENRAGE, 0))
				{
					DoCast(me, SPELL_VJ_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_voljinAI(creature);
    }
};

enum CairneBloodhoof
{
    SPELL_BERSERKER_CHARGE = 16636,
    SPELL_CLEAVE           = 16044,
    SPELL_MORTAL_STRIKE    = 16856,
    SPELL_THUNDERCLAP      = 23931,
    SPELL_UPPERCUT         = 22916,
	QUEST_CAIRNE_HOOFPRINT = 925,
	SPELL_CAIRNE_HOOFPRINT = 23123,
	BLOODHOOF_SOUND_AGGRO  = 5884,
};

#define GOSSIP_HCB "I know this is rather silly but a young ward who is a bit shy would like your hoofprint."

class boss_cairne_bloodhoof : public CreatureScript
{
public:
    boss_cairne_bloodhoof() : CreatureScript("boss_cairne_bloodhoof") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_CAIRNE_HOOFPRINT) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HCB, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO);

        player->SEND_GOSSIP_MENU(7013, creature->GetGUID());
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_SENDER_INFO)
        {
            player->CastSpell(player, SPELL_CAIRNE_HOOFPRINT, false);
            player->SEND_GOSSIP_MENU(7014, creature->GetGUID());
        }
        return true;
    }

    struct boss_cairne_bloodhoofAI : public QuantumBasicAI
    {
        boss_cairne_bloodhoofAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 BerserkerChargeTimer;
        uint32 CleaveTimer;
        uint32 MortalStrikeTimer;
        uint32 ThunderclapTimer;
        uint32 UppercutTimer;

        void Reset()
        {
            BerserkerChargeTimer = 12000;
            CleaveTimer = 2000;
            MortalStrikeTimer = 4000;
            ThunderclapTimer = 6000;
            UppercutTimer = 8000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(BLOODHOOF_SOUND_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (BerserkerChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_BERSERKER_CHARGE);
					BerserkerChargeTimer = 12000;
				}
            }
			else BerserkerChargeTimer -= diff;

            if (UppercutTimer <= diff)
            {
                DoCastVictim(SPELL_UPPERCUT);
                UppercutTimer = 10000;
            }
			else UppercutTimer -= diff;

            if (ThunderclapTimer <= diff)
            {
                DoCastVictim(SPELL_THUNDERCLAP);
                ThunderclapTimer = 8000;
            }
			else ThunderclapTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = 6000;
            }
			else MortalStrikeTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 4000;
            }
			else CleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new boss_cairne_bloodhoofAI (creature);
    }
};

enum SylvanasMisc
{
    QUEST_JOURNEY_TO_UNDERCITY = 9180,

	EMOTE_LAMENT_END           = 0,
	SAY_LAMENT_END             = 1,

    SOUND_CREDIT               = 10896,
	NPC_SKELETON               = 6412,
    NPC_HIGHBORNE_LAMENTER     = 21628,
    NPC_HIGHBORNE_BUNNY        = 21641,
    SPELL_HIGHBORNE_AURA       = 37090,
    SPELL_SYLVANAS_CAST        = 36568,
	SPELL_RIBBON_OF_SOULS      = 34432, // the real one to use might be 37099
    SPELL_FADE                 = 20672,
    SPELL_BLACK_ARROW          = 59712,
    SPELL_MULTI_SHOT           = 59713,
    SPELL_SHOOT                = 59710,
    SPELL_SUMMON_SKELETON      = 59711,
	SAY_AGGRO                  = 0,
	SAY_DEATH                  = 1,
	SYLVANAS_SOUND_AGGRO       = 5886,
};

float HighborneLoc[4][3] =
{
    {1285.41f, 312.47f, 0.51f},
    {1286.96f, 310.40f, 1.00f},
    {1289.66f, 309.66f, 1.52f},
    {1292.51f, 310.50f, 1.99f},
};

#define HIGHBORNE_LOC_Y     -61.00f
#define HIGHBORNE_LOC_Y_NEW -55.50f

class boss_lady_sylvanas_windrunner : public CreatureScript
{
public:
    boss_lady_sylvanas_windrunner() : CreatureScript("boss_lady_sylvanas_windrunner") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnQuestReward(Player* /*player*/, Creature* creature, const Quest* quest, uint32 /*slot*/)
    {
        if (quest->GetQuestId() == QUEST_JOURNEY_TO_UNDERCITY)
        {
            CAST_AI(boss_lady_sylvanas_windrunner::boss_lady_sylvanas_windrunnerAI, creature->AI())->LamentEvent = true;
            CAST_AI(boss_lady_sylvanas_windrunner::boss_lady_sylvanas_windrunnerAI, creature->AI())->DoPlaySoundToSet(creature, SOUND_CREDIT);
            creature->CastSpell(creature, SPELL_SYLVANAS_CAST, false);

            for (uint8 i = 0; i < 4; ++i)
                creature->SummonCreature(NPC_HIGHBORNE_LAMENTER, HighborneLoc[i][0], HighborneLoc[i][1], HIGHBORNE_LOC_Y, HighborneLoc[i][2], TEMPSUMMON_TIMED_DESPAWN, 160000);
        }
        return true;
    }

    struct boss_lady_sylvanas_windrunnerAI : public QuantumBasicAI
    {
        boss_lady_sylvanas_windrunnerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

		SummonList Summons;

        uint32 FadeTimer;
        uint32 BlackarrowTimer;
        uint32 MultishotTimer;
        uint32 ShootTimer;
        uint32 SummonSkeletonTimer;
		uint32 LamentEventTimer;

		bool LamentEvent;

		uint64 TargetGUID;

        void Reset()
        {
            LamentEventTimer = 5000;
            LamentEvent = false;
            TargetGUID = 0;

            FadeTimer = 10*IN_MILLISECONDS;
            BlackarrowTimer = urand(6, 8)*IN_MILLISECONDS;
            MultishotTimer = urand(4, 6)*IN_MILLISECONDS;
            ShootTimer = urand(2, 4)*IN_MILLISECONDS;
            SummonSkeletonTimer = 0;

            Summons.DespawnAll();

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void EnterToBattle(Unit* /*who*/) 
        {
			me->PlayDirectSound(SYLVANAS_SOUND_AGGRO);
            Talk(SAY_AGGRO);
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_HIGHBORNE_BUNNY)
            {
                Summons.Summon(summoned);

                if (Creature* target = Unit::GetCreature(*summoned, TargetGUID))
                {
                    target->MonsterMoveWithSpeed(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ()+15.0f, 0);
                    // target->SetPosition(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ()+15.0f, 0.0f);
                    summoned->CastSpell(target, SPELL_RIBBON_OF_SOULS, false);
                }

                summoned->SetDisableGravity(true);
                TargetGUID = summoned->GetGUID();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (LamentEvent)
            {
                if (LamentEventTimer <= diff)
                {
                    DoSummon(NPC_HIGHBORNE_BUNNY, me, 10.0f, 3000, TEMPSUMMON_TIMED_DESPAWN);

                    LamentEventTimer = 2000;
                    if (!me->HasAura(SPELL_SYLVANAS_CAST))
                    {
						Talk(SAY_LAMENT_END);
						Talk(EMOTE_LAMENT_END);
                        LamentEvent = false;
                    }
                } 
                else LamentEventTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (FadeTimer <= diff)
            {
                DoCast(SPELL_FADE);
                FadeTimer = 10 *IN_MILLISECONDS;
            }
            else FadeTimer -= diff;

            if (BlackarrowTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, SPELL_BLACK_ARROW);
                    BlackarrowTimer = urand(12, 15) *IN_MILLISECONDS;
                }
            }
            else BlackarrowTimer -= diff;

            if (MultishotTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                {
                    DoCast(target, SPELL_MULTI_SHOT);
                    MultishotTimer = urand(8, 10) *IN_MILLISECONDS;
                }
            }
            else MultishotTimer -= diff;

            if (ShootTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                {
                    DoCast(target, SPELL_SHOOT);
                    ShootTimer = urand(2, 4) *IN_MILLISECONDS;
                }
            }
            else ShootTimer -= diff;

            if (SummonSkeletonTimer <= diff)
            {
                DoCast(SPELL_SUMMON_SKELETON);
                SummonSkeletonTimer = 10 *IN_MILLISECONDS;
            }
            else SummonSkeletonTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void KilledUnit(Unit* victim)
        {
            me->SummonCreature(NPC_SKELETON, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }

        void JustDied (Unit* /*victim*/)
        {
            Summons.DespawnAll();
            Talk(SAY_DEATH);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_sylvanas_windrunnerAI(creature);
    }
};

enum Varimathras
{
	SPELL_SHADOW_BOLT_VOLLEY   = 20741,
	SPELL_DRAIN_LIFE           = 17238,
	SPELL_CARRION_SWARM        = 59434,
	SPELL_SHADOW_NOVA          = 59435,
	SPELL_MIGHT_OF_VARIMATHRAS = 59424,
	SPELL_AURA_OF_VARIMATHRAS  = 60289,
	VARIMATHTAS_SOUND_AGGRO    = 5887,
};

class boss_varimathras : public CreatureScript
{
public:
    boss_varimathras() : CreatureScript("boss_varimathras") {}

    struct boss_varimathrasAI : public QuantumBasicAI
    {
        boss_varimathrasAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShadowBoltVolleyTimer;
		uint32 DrainLifeTimer;
		uint32 CarrionSwarmTimer;
		uint32 ShadowNovaTimer;

        void Reset()
        {
			ShadowBoltVolleyTimer = 500;
			DrainLifeTimer = 4000;
			CarrionSwarmTimer = 6000;
			ShadowNovaTimer = 8000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_AURA_OF_VARIMATHRAS);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(VARIMATHTAS_SOUND_AGGRO);
			me->RemoveAura(SPELL_AURA_OF_VARIMATHRAS);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltVolleyTimer <= diff)
			{
				DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
				ShadowBoltVolleyTimer = urand(4000, 5000);
			}
			else ShadowBoltVolleyTimer -= diff;

			if (DrainLifeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DRAIN_LIFE);
					DrainLifeTimer = urand(6000, 7000);
				}
			}
			else DrainLifeTimer -= diff;

			if (CarrionSwarmTimer <= diff)
			{
				DoCastAOE(SPELL_CARRION_SWARM);
				CarrionSwarmTimer = urand(8000, 9000);
			}
			else CarrionSwarmTimer -= diff;

			if (ShadowNovaTimer <= diff)
			{
				DoCastAOE(SPELL_SHADOW_NOVA);
				ShadowNovaTimer = urand(10000, 11000);
			}
			else ShadowNovaTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_MIGHT_OF_VARIMATHRAS, 0))
				{
					DoCast(me, SPELL_MIGHT_OF_VARIMATHRAS);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_varimathrasAI(creature);
    }
};

class npc_highborne_lamenter : public CreatureScript
{
public:
    npc_highborne_lamenter() : CreatureScript("npc_highborne_lamenter") { }

    struct npc_highborne_lamenterAI : public QuantumBasicAI
    {
        npc_highborne_lamenterAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 EventMoveTimer;
        uint32 EventCastTimer;

        bool EventMove;
        bool EventCast;

        void Reset()
        {
            EventMoveTimer = 10000;
            EventCastTimer = 17500;

            EventMove = true;
            EventCast = true;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(uint32 const diff)
        {
            if (EventMove)
            {
                if (EventMoveTimer <= diff)
                {
                    me->SetDisableGravity(true);
                    me->MonsterMoveWithSpeed(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW, me->GetDistance(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW) / (5000 * 0.001f));
                    me->SetPosition(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW, me->GetOrientation());
                    EventMove = false;
                }
				else EventMoveTimer -= diff;
            }

            if (EventCast)
            {
                if (EventCastTimer <= diff)
                {
                    DoCast(me, SPELL_HIGHBORNE_AURA);
                    EventCast = false;
                }
				else EventCastTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highborne_lamenterAI(creature);
    }
};

enum LorthemarTheron
{
	SPELL_ARCANE_SHOCK = 59715,
	SPELL_LT_CLEAVE    = 15284,
	SPELL_MANA_BURN    = 33384,
	SPELL_MASS_CHARM   = 33385, // Need Implement
	SAY_LT_AGGRO       = -1520786,
	SAY_LT_KILL        = -1520787,
	SAY_LT_DEATH       = -1520788,
};

class boss_lorthemar_theron : public CreatureScript
{
public:
    boss_lorthemar_theron() : CreatureScript("boss_lorthemar_theron") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_lorthemar_theronAI : public QuantumBasicAI
    {
        boss_lorthemar_theronAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ArcaneFlameShockTimer;
		uint32 CleaveTimer;
		uint32 ManaBurnTimer;

        void Reset()
        {
			ArcaneFlameShockTimer = 500;
			CleaveTimer = 2000;
			ManaBurnTimer = 4000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_LT_AGGRO, me);
		}

		void KilledUnit(Unit* /*killed*/)
		{
			DoSendQuantumText(SAY_LT_KILL, me);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoSendQuantumText(SAY_LT_DEATH, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneFlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_SHOCK);
					ArcaneFlameShockTimer = urand(5000, 6000);
				}
			}
			else ArcaneFlameShockTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastAOE(SPELL_LT_CLEAVE);
				CleaveTimer = urand(8000, 9000);
			}
			else CleaveTimer -= diff;
			
			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = urand(11000, 12000);
					}
				}
			}
			else ManaBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lorthemar_theronAI(creature);
    }
};

static Position IronforgeGuard[] =
{
    {-4869.67f, -1037.57f, 502.30f, 0.347319f},
    {-4860.41f, -1051.07f, 502.30f, 1.490071f},
    {-4844.45f, -1048.32f, 502.30f, 2.299032f},
    {-4840.58f, -1033.14f, 502.30f, 3.265075f},
    {-4850.67f, -1018.92f, 502.30f, 4.482435f},
    {-4865.12f, -1024.41f, 505.90f, 5.428836f},
};

enum MagniBronzebeard
{
    SPELL_AVATAR        = 19135,
    SPELL_KNOCK_AWAY    = 20686,
    SPELL_STORM_BOLT    = 20685,
    SPELL_CHARGE        = 20508,	
    SPELL_LAY_ON_HANDS  = 633,
    NPC_IRONFORGE_GUARD = 5595,
    MAGNI_SOUND_AGGRO   = 5896,
};

class boss_king_magni_bronzebeard : public CreatureScript
{
public:
    boss_king_magni_bronzebeard() : CreatureScript("boss_king_magni_bronzebeard") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_king_magni_bronzebeardAI : public QuantumBasicAI
    {
        boss_king_magni_bronzebeardAI(Creature* creature) : QuantumBasicAI(creature) , summons(me) {}

		SummonList summons;

        uint32 AvatarTimer;
        uint32 KnockawayTimer;
        uint32 StormboltTimer;
        uint32 ChargeTimer;
        uint32 SummonTimer;
        uint32 LayOnHandsTimer;

        void Reset()
        {
            summons.DespawnAll();

            AvatarTimer = 15*IN_MILLISECONDS;
            KnockawayTimer = 15*IN_MILLISECONDS;
            StormboltTimer = urand(5, 7)*IN_MILLISECONDS;
            ChargeTimer = urand(13, 17)*IN_MILLISECONDS;
            SummonTimer = urand(20, 40)*IN_MILLISECONDS;
            LayOnHandsTimer = 5*IN_MILLISECONDS*MINUTE;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
        }

        void JustSummoned(Creature* summoned)
        {
            summons.Summon(summoned);
        }

        void JustDied (Unit* /*killer*/)
        {
            summons.DespawnAll();
        }

        void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(MAGNI_SOUND_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
				return;

            if (StormboltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                {
                    DoCast(target, SPELL_STORM_BOLT);
                    StormboltTimer = urand(7, 9) *IN_MILLISECONDS;
                }
            }
            else StormboltTimer -= diff;

            if (AvatarTimer <= diff)
            {
                DoCast(me, SPELL_AVATAR);
                AvatarTimer = 45 *IN_MILLISECONDS;
            }
            else AvatarTimer -= diff;

            if (KnockawayTimer <= diff)
            {
                DoResetThreat();									// Threat Reset
                DoCastVictim(SPELL_KNOCK_AWAY);
                KnockawayTimer = 15 *IN_MILLISECONDS;
            }
            else KnockawayTimer -= diff;

            if (ChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 25.0f))
                {
                    DoCast(target, SPELL_CHARGE);
                    ChargeTimer = 15 *IN_MILLISECONDS;
                }
            }
            else ChargeTimer -= diff;

            if (SummonTimer <= diff)
            {
                for (uint8 i = 0; i < 6; ++i)
				{
                    me->SummonCreature(NPC_IRONFORGE_GUARD, IronforgeGuard[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                    SummonTimer = urand(20, 40) *IN_MILLISECONDS;
				}
            }
            else SummonTimer -= diff;

            if (LayOnHandsTimer <= diff)
            {
                DoCast(me, SPELL_LAY_ON_HANDS);
                LayOnHandsTimer = 5 *MINUTE;
            }
            else LayOnHandsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_king_magni_bronzebeardAI(creature);
    }
};

enum HighTinkerMekkatorque
{
	SPELL_GOBLIN_DRAGON_GUN = 22739,
	SPELL_BOMB              = 9143,
	SPELL_SUPER_SHRINK_RAY  = 22742,
	TINKER_SOUND_AGGRO      = 5897,
};

class boss_high_tinker_mekkatorque : public CreatureScript
{
public:
    boss_high_tinker_mekkatorque() : CreatureScript("boss_high_tinker_mekkatorque") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_high_tinker_mekkatorqueAI : public QuantumBasicAI
    {
        boss_high_tinker_mekkatorqueAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 DragonGunTimer;
		uint32 BombTimer;
		uint32 ShrinkRayTimer;

        void Reset()
        {
			DragonGunTimer = 3000;
			BombTimer = 4000;
			ShrinkRayTimer = 6000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(TINKER_SOUND_AGGRO);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DragonGunTimer <= diff)
			{
				DoCastAOE(SPELL_GOBLIN_DRAGON_GUN);
				DragonGunTimer = urand(4000, 5000);
			}
			else DragonGunTimer -= diff;

			if (BombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BOMB);
					BombTimer = urand(6000, 7000);
				}
			}
			else BombTimer -= diff;

			if (ShrinkRayTimer <= diff)
			{
				DoCastVictim(SPELL_SUPER_SHRINK_RAY);
				ShrinkRayTimer = urand(8000, 9000);
			}
			else ShrinkRayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_tinker_mekkatorqueAI(creature);
    }
};

enum TyrandeWhisperwind
{
	SPELL_SEARING_ARROW = 20688,
	SPELL_TW_CLEAVE     = 20691,
	SPELL_MOONFIRE      = 20690,
	SPELL_STARFALL      = 20687,
	TYRANDE_SOUND_AGGRO = 5885,
};

class boss_tyrande_whisperwind : public CreatureScript
{
public:
    boss_tyrande_whisperwind() : CreatureScript("boss_tyrande_whisperwind") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_tyrande_whisperwindAI : public QuantumBasicAI
    {
        boss_tyrande_whisperwindAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 MoonfireTimer;
		uint32 StarfallTimer;

        void Reset()
        {
			CleaveTimer = 2000;
			MoonfireTimer = 4000;
			StarfallTimer = 6000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(TYRANDE_SOUND_AGGRO);

			DoCastVictim(SPELL_SEARING_ARROW);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastAOE(SPELL_TW_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (MoonfireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MOONFIRE);
					MoonfireTimer = urand(5000, 6000);
				}
			}
			else MoonfireTimer -= diff;

			if (StarfallTimer <= diff)
			{
				DoCastAOE(SPELL_STARFALL);
				StarfallTimer = urand(8000, 9000);
			}
			else StarfallTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_tyrande_whisperwindAI(creature);
    }
};

enum ProphetVelen
{
	SPELL_PRAYER_OF_HEALING = 59698,
	SPELL_STAFF_STRIKE      = 33542,
	SPELL_HOLY_BLAST        = 59700,
	SPELL_HOLY_NOVA         = 59701,
	SPELL_HOLY_SMITE        = 59703,
	VELEN_SOUND_AGGRO       = 10154,
};

class boss_prophet_velen : public CreatureScript
{
public:
    boss_prophet_velen() : CreatureScript("boss_prophet_velen") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_prophet_velenAI : public QuantumBasicAI
    {
        boss_prophet_velenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StaffStrikeTimer;
		uint32 HolySmiteTimer;
		uint32 HolyBlastTimer;
		uint32 HolyNovaTimer;
		uint32 HealingTimer;

        void Reset()
        {
			StaffStrikeTimer = 2000;
			HolySmiteTimer = 3000;
			HolyBlastTimer = 5000;
			HolyNovaTimer = 7000;
			HealingTimer = 9000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(VELEN_SOUND_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StaffStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STAFF_STRIKE);
				StaffStrikeTimer = urand(3000, 4000);
			}
			else StaffStrikeTimer -= diff;

			if (HolySmiteTimer <= diff)
			{
				DoCastVictim(SPELL_HOLY_SMITE);
				HolySmiteTimer = urand(5000, 6000);
			}
			else HolySmiteTimer -= diff;

			if (HolyBlastTimer <= diff)
			{
				DoCastAOE(SPELL_HOLY_BLAST);
				HolyBlastTimer = urand(7000, 8000);
			}
			else HolyBlastTimer -= diff;

			if (HolyNovaTimer <= diff)
			{
				DoCastAOE(SPELL_HOLY_NOVA);
				HolyNovaTimer = urand(9000, 10000);
			}
			else HolyNovaTimer -= diff;

			if (HealingTimer <= diff)
			{
				DoCast(me, SPELL_PRAYER_OF_HEALING, true);
				HealingTimer = urand(11000, 12000);
			}
			else HealingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_prophet_velenAI(creature);
    }
};

enum KingVarianWrynn
{
	SPELL_HEROIC_LEAP = 59688,
	SPELL_WHIRLWIND   = 41056,
};

class boss_king_varian_wrynn : public CreatureScript
{
public:
    boss_king_varian_wrynn() : CreatureScript("boss_king_varian_wrynn") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct boss_king_varian_wrynnAI : public QuantumBasicAI
    {
        boss_king_varian_wrynnAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 WhirlwindTimer;

        void Reset()
        {
			WhirlwindTimer = 2000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_HEROIC_LEAP);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = urand(4000, 5000);
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_king_varian_wrynnAI(creature);
    }
};

enum LadyJainaProudmoore
{
	SPELL_SUMMON_ELEMENTALS = 20681,
	SPELL_FROSTBOLT         = 55807,
	SPELL_FROST_NOVA        = 31250,
	SPELL_FROST_ARMOR       = 31256,
	SPELL_BLIZZARD          = 46195,
	JAINA_SOUND_AGGRO       = 5882,
};

class boss_lady_jaina_proudmoore : public CreatureScript
{
public:
    boss_lady_jaina_proudmoore() : CreatureScript("boss_lady_jaina_proudmoore") {}

    struct boss_lady_jaina_proudmooreAI : public QuantumBasicAI
    {
        boss_lady_jaina_proudmooreAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostArmorTimer;
		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;
		uint32 BlizzardTimer;

        void Reset()
        {
			FrostArmorTimer = 500;
			FrostboltTimer = 1000;
			FrostNovaTimer = 4000;
			BlizzardTimer = 6000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->PlayDirectSound(JAINA_SOUND_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostArmorTimer <= diff)
			{
				DoCast(me, SPELL_FROST_ARMOR, true);
				FrostArmorTimer = urand(5000, 6000);
			}
			else FrostArmorTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = urand(7000, 8000);
				}
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(9000, 10000);
			}
			else FrostNovaTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				DoCastAOE(SPELL_BLIZZARD);
				BlizzardTimer = urand(11000, 12000);
			}
			else BlizzardTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_jaina_proudmooreAI(creature);
    }
};

enum HighlordBolvarFordragon
{
	SPELL_SHIELD_WALL       = 871,
	SPELL_STRONG_CLEAVE     = 8255,
	SPELL_HAMMER_OF_JUSTICE = 10308,
	SPELL_DIVINE_SHIELD     = 13874,
	SPELL_ENRAGE            = 15716,
};

class boss_highlord_bolvar_fordragon : public CreatureScript
{
public:
    boss_highlord_bolvar_fordragon() : CreatureScript("boss_highlord_bolvar_fordragon") {}

    struct boss_highlord_bolvar_fordragonAI : public QuantumBasicAI
    {
        boss_highlord_bolvar_fordragonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShieldWallTimer;
		uint32 StrongCleaveTimer;
		uint32 HammerOfJusticeTimer;
		uint32 EnrageTimer;
		uint32 DivineShieldTimer;

        void Reset()
        {
			ShieldWallTimer = 500;
			StrongCleaveTimer = 1000;
			HammerOfJusticeTimer = 3000;
			EnrageTimer = 5000;
			DivineShieldTimer = 7000;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldWallTimer <= diff)
			{
				DoCast(me, SPELL_SHIELD_WALL);
				ShieldWallTimer = urand(29000, 30000);
			}
			else ShieldWallTimer -= diff;

			if (StrongCleaveTimer <= diff)
			{
				DoCastVictim(SPELL_STRONG_CLEAVE);
				StrongCleaveTimer = urand(4000, 5000);
			}
			else StrongCleaveTimer -= diff;

			if (HammerOfJusticeTimer <= diff)
			{
				DoCastVictim(SPELL_HAMMER_OF_JUSTICE);
				HammerOfJusticeTimer = urand(11000, 12000);
			}
			else HammerOfJusticeTimer -= diff;

			if (EnrageTimer <= diff)
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
					EnrageTimer = urand(45000, 46000);
				}
			}
			else EnrageTimer -= diff;

			if (DivineShieldTimer <= diff)
			{
				DoCast(me, SPELL_DIVINE_SHIELD);
				DivineShieldTimer = urand(48000, 49000);
			}
			else DivineShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_highlord_bolvar_fordragonAI(creature);
    }
};

enum BaineBloodhoof
{
	SPELL_BB_ENRAGE        = 8599,
	SPELL_BB_CLEAVE        = 15284,
	SPELL_BB_UPPERCUT      = 22916,
	SPELL_BB_MORTAL_STRIKE = 32736,
	SPELL_BB_WAR_STOMP     = 59705,
};

class boss_baine_bloodhoof : public CreatureScript
{
public:
    boss_baine_bloodhoof() : CreatureScript("boss_baine_bloodhoof") {}

    struct boss_baine_bloodhoofAI : public QuantumBasicAI
    {
        boss_baine_bloodhoofAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;
		uint32 MortalStrikeTimer;
		uint32 UppercutTimer;
		uint32 WarStompTimer;
		uint32 EnrageTimer;

        void Reset()
        {
			CleaveTimer = 1000;
			MortalStrikeTimer = 2000;
			UppercutTimer = 3000;
			WarStompTimer = 5000;
			EnrageTimer = 7000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_BB_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BB_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5000, 6000);
			}
			else MortalStrikeTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_BB_UPPERCUT);
				UppercutTimer = urand(7000, 8000);
			}
			else UppercutTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_BB_WAR_STOMP);
				WarStompTimer = urand(9000, 10000);
			}
			else WarStompTimer -= diff;

			if (EnrageTimer <= diff)
			{
				if (!me->HasAuraEffect(SPELL_BB_ENRAGE, 0))
				{
					DoCast(me, SPELL_BB_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
					EnrageTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_baine_bloodhoofAI(creature);
    }
};

enum Rexxar
{
	SPELL_THRASH         = 3417,
	SPELL_R_KNOCK_AWAY   = 18813,
	SPELL_PUNCTURE_ARMOR = 35918,
	SPELL_R_FRENZY       = 39249,
	SPELL_R_CLEAVE       = 40504,
	SPELL_SUMMON_HUFFER  = 39251,
	SPELL_SUMMON_SPIRIT  = 39253,
	SPELL_SUMMON_MISHA   = 39269,

	NPC_HUFFER           = 22490,
	NPC_SPIRIT           = 22492,
	NPC_MISHA            = 22498,
};

class npc_boss_rexxar : public CreatureScript
{
public:
    npc_boss_rexxar() : CreatureScript("npc_boss_rexxar") {}

    struct npc_boss_rexxarAI : public QuantumBasicAI
    {
		npc_boss_rexxarAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 CleaveTimer;
		uint32 PunctureArmorTimer;
		uint32 KnockAwayTimer;
		uint32 SummonBeastTimer;
		uint32 FrenzyTimer;

		SummonList Summons;

        void Reset()
        {
			CleaveTimer = 1000;
			PunctureArmorTimer = 2000;
			KnockAwayTimer = 3000;
			SummonBeastTimer = 5000;
			FrenzyTimer = 7000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_HUFFER:
				case NPC_SPIRIT:
				case NPC_MISHA:
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 25.0f))
						summon->SetInCombatWith(target);
					Summons.Summon(summon);
					break;
				default:
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_R_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (PunctureArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PUNCTURE_ARMOR);
				PunctureArmorTimer = urand(5000, 6000);
			}
			else PunctureArmorTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_R_KNOCK_AWAY);
				KnockAwayTimer = urand(7000, 8000);
			}
			else KnockAwayTimer -= diff;

			if (SummonBeastTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_HUFFER, true);
				DoCast(me, SPELL_SUMMON_SPIRIT, true);
				DoCast(me, SPELL_SUMMON_MISHA, true);
				SummonBeastTimer = urand(29000, 30000);
			}
			else SummonBeastTimer -= diff;

			if (FrenzyTimer <= diff)
			{
				if (!me->HasAuraEffect(SPELL_R_FRENZY, 0))
				{
					DoCast(me, SPELL_R_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
					FrenzyTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
			else FrenzyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_boss_rexxarAI(creature);
    }
};

void AddSC_alliance_and_horde_bosses()
{
	new boss_thrall_warchief();
	new boss_voljin();
	new boss_cairne_bloodhoof();
	new boss_lady_sylvanas_windrunner();
	new boss_varimathras();
	new boss_lorthemar_theron();
	new boss_king_magni_bronzebeard();
	new boss_high_tinker_mekkatorque();
	new boss_tyrande_whisperwind();
	new boss_prophet_velen();
	new boss_king_varian_wrynn();
	new boss_lady_jaina_proudmoore();
	new boss_highlord_bolvar_fordragon();
	new boss_baine_bloodhoof();
	new npc_boss_rexxar();
	new npc_highborne_lamenter();
}