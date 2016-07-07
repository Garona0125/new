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
#include "QuantumEscortAI.h"

enum SecondTrial
{
    QUEST_SECOND_TRIAL            = 9686,

    SPELL_FLASH_OF_LIGHT          = 19939,
    SPELL_SEAL_OF_JUSTICE         = 20164,
    SPELL_JUDGEMENT_OF_LIGHT      = 20271,
    SPELL_SEAL_OF_COMMAND         = 20375,

    NPC_MASTER_KELERUN_BLOODMOURN = 17807,
    NPC_CHAMPION_BLOODWRATH       = 17809,
    NPC_CHAMPION_LIGHTREND        = 17810,
    NPC_CHAMPION_SWIFTBLADE       = 17811,
    NPC_CHAMPION_SUNSTRIKER       = 17812,

	GO_SECOND_TRIAL               = 182052,

    FACTION_HOSTILE               = 16,
    FACTION_FRIENDLY              = 35,

    TEXT_SECOND_TRIAL_1           = -1000637,
    TEXT_SECOND_TRIAL_2           = -1000638,
    TEXT_SECOND_TRIAL_3           = -1000639,
    TEXT_SECOND_TRIAL_4           = -1000640,

	ACTION_START_QUEST            = 1,

	EVENT_SEND_BATTLE_1           = 1,
	EVENT_SEND_BATTLE_2           = 2,
	EVENT_SEND_BATTLE_3           = 3,
	EVENT_SEND_BATTLE_4           = 4,
};

class npc_master_kelerun_bloodmourn : public CreatureScript
{
public:
    npc_master_kelerun_bloodmourn() : CreatureScript("npc_master_kelerun_bloodmourn") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
		{
			player->PrepareQuestMenu(creature->GetGUID());
			player->SendPreparedQuest(creature->GetGUID());
		}

        player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
        return true;
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_SECOND_TRIAL)
			creature->SummonGameObject(GO_SECOND_TRIAL, 10099.3f, -6622.68f, 4.10258f, 0.881337f, 0, 0, 0.426544f, 0.904467f, 5*MINUTE*IN_MILLISECONDS);

        return true;
    }

	struct npc_master_kelerun_bloodmournAI : public QuantumBasicAI
    {
        npc_master_kelerun_bloodmournAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_START_QUEST:
					SummonChampions();
					events.ScheduleEvent(EVENT_SEND_BATTLE_1, 2*IN_MILLISECONDS);
					break;
			}
		}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SummonChampions()
		{
			me->SummonCreature(NPC_CHAMPION_BLOODWRATH, 10103.1f, -6614.8f, 4.10197f, 4.29065f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
			me->SummonCreature(NPC_CHAMPION_LIGHTREND, 10095.7f, -6630.3f, 4.10082f, 4.10082f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
			me->SummonCreature(NPC_CHAMPION_SWIFTBLADE, 10091.4f, -6618.93f, 4.10204f, 5.80616f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
			me->SummonCreature(NPC_CHAMPION_SUNSTRIKER, 10107.3f, -6626.59f, 4.10229f, 2.71985f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*MINUTE*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_SEND_BATTLE_1:
						if (Creature* wrath = me->FindCreatureWithDistance(NPC_CHAMPION_BLOODWRATH, 150.f))
						{
							DoSendQuantumText(TEXT_SECOND_TRIAL_1, me);

							wrath->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);
							wrath->SetCurrentFaction(FACTION_HOSTILE);

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
								wrath->SetInCombatWith(target);
						}
						events.ScheduleEvent(EVENT_SEND_BATTLE_2, 40*IN_MILLISECONDS);
						break;
					case EVENT_SEND_BATTLE_2:
						if (Creature* rend = me->FindCreatureWithDistance(NPC_CHAMPION_LIGHTREND, 150.f))
						{
							DoSendQuantumText(TEXT_SECOND_TRIAL_2, me);

							rend->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);
							rend->SetCurrentFaction(FACTION_HOSTILE);

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
								rend->SetInCombatWith(target);
						}
						events.ScheduleEvent(EVENT_SEND_BATTLE_3, 40*IN_MILLISECONDS);
						break;
					case EVENT_SEND_BATTLE_3:
						if (Creature* blade = me->FindCreatureWithDistance(NPC_CHAMPION_SWIFTBLADE, 150.f))
						{
							DoSendQuantumText(TEXT_SECOND_TRIAL_3, me);

							blade->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);
							blade->SetCurrentFaction(FACTION_HOSTILE);

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
								blade->SetInCombatWith(target);
						}
						events.ScheduleEvent(EVENT_SEND_BATTLE_4, 40*IN_MILLISECONDS);
						break;
					case EVENT_SEND_BATTLE_4:
						if (Creature* striker = me->FindCreatureWithDistance(NPC_CHAMPION_SUNSTRIKER, 150.f))
						{
							DoSendQuantumText(TEXT_SECOND_TRIAL_4, me);

							striker->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);
							striker->SetCurrentFaction(FACTION_HOSTILE);

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
								striker->SetInCombatWith(target);
						}
						break;
				}
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_kelerun_bloodmournAI(creature);
    }
};

class npc_second_trial_paladin : public CreatureScript
{
public:
    npc_second_trial_paladin() : CreatureScript("npc_second_trial_paladin") { }

    struct npc_secondTrialAI : public QuantumBasicAI
    {
        npc_secondTrialAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 TimerFlashLight;
        uint32 TimerJustice;
        uint32 TimerJudLight;
        uint32 TimerCommand;

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_KNEEL);
			me->SetCurrentFaction(FACTION_FRIENDLY);

			switch (me->GetEntry())
			{
                case NPC_CHAMPION_BLOODWRATH:
					TimerFlashLight = 3225;
					break;
				case NPC_CHAMPION_LIGHTREND:
					TimerJustice = 500;
					break;
				case NPC_CHAMPION_SWIFTBLADE:
					TimerJudLight = 500;
					break;
				case NPC_CHAMPION_SUNSTRIKER:
					TimerFlashLight = 3225;
					TimerJudLight = 500;
					TimerCommand = 1500;
					break;
			}
		}

		void EnterToBattle(Unit* /*who*/){}

		void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPE_ID_PLAYER)
			{
                if (CAST_PLR(victim)->GetQuestStatus(QUEST_SECOND_TRIAL) == QUEST_STATUS_INCOMPLETE)
                    CAST_PLR(victim)->FailQuest(QUEST_SECOND_TRIAL);
			}
        }

		void JustDied(Unit* killer)
		{
			if (me->GetEntry() == NPC_CHAMPION_SUNSTRIKER)
			{
				if (killer->GetTypeId() == TYPE_ID_PLAYER)
					killer->ToPlayer()->GroupEventHappens(QUEST_SECOND_TRIAL, killer);
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
				return;

			if (TimerFlashLight <= diff && HealthBelowPct(70))
			{
				DoCast(me, SPELL_FLASH_OF_LIGHT);
				TimerFlashLight = 3225 + rand()%3225;
			}
			else TimerFlashLight -= diff;

			if (TimerJustice <= diff)
			{
				DoCast(me, SPELL_SEAL_OF_JUSTICE);
				TimerJustice = urand(10000, 20000);
			}
			else TimerJustice -= diff;

			if (TimerJudLight <= diff)
			{
				DoCast(me, SPELL_JUDGEMENT_OF_LIGHT);
				TimerJudLight = urand(10000, 20000);
			}
			else TimerJudLight -= diff;

			if (TimerCommand <= diff)
			{
				DoCast(me, SPELL_SEAL_OF_COMMAND);
				TimerCommand = urand(20000, 40000);
			}
			else TimerCommand -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_secondTrialAI(creature);
    }
};

class go_second_trial : public GameObjectScript
{
public:
    go_second_trial() : GameObjectScript("go_second_trial") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
		if (Creature* mourn = go->FindCreatureWithDistance(NPC_MASTER_KELERUN_BLOODMOURN, 150.f))
			mourn->AI()->DoAction(ACTION_START_QUEST);

		return true;
    }
};

enum Mirveda
{
	QUEST_UNEXPECTED_RESULT = 8488,
	NPC_GHARZUL             = 15958,
	NPC_ANGERSHADE          = 15656,
};

class npc_apprentice_mirveda : public CreatureScript
{
public:
    npc_apprentice_mirveda() : CreatureScript("npc_apprentice_mirveda") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_UNEXPECTED_RESULT)
        {
            CAST_AI(npc_apprentice_mirveda::npc_apprentice_mirvedaAI, creature->AI())->Summon = true;
            CAST_AI(npc_apprentice_mirveda::npc_apprentice_mirvedaAI, creature->AI())->PlayerGUID = player->GetGUID();
        }
        return true;
    }

    struct npc_apprentice_mirvedaAI : public QuantumBasicAI
    {
        npc_apprentice_mirvedaAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

        uint32 KillCount;
        uint64 PlayerGUID;
        bool Summon;
        SummonList Summons;

        void Reset()
        {
            KillCount = 0;
            PlayerGUID = 0;
            Summons.DespawnAll();
            Summon = false;
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
            Summons.Summon(summoned);
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            Summons.Despawn(summoned);
            ++KillCount;
        }

        void JustDied(Unit* /*killer*/)
        {
            if (PlayerGUID)
			{
                if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    player->FailQuest(QUEST_UNEXPECTED_RESULT);
			}
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (KillCount >= 3 && PlayerGUID)
			{
                if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    player->CompleteQuest(QUEST_UNEXPECTED_RESULT);
			}

            if (Summon)
            {
                me->SummonCreature(NPC_GHARZUL, 8745, -7134.32f, 35.22f, 0, TEMPSUMMON_CORPSE_DESPAWN, 4000);
                me->SummonCreature(NPC_ANGERSHADE, 8745, -7134.32f, 35.22f, 0, TEMPSUMMON_CORPSE_DESPAWN, 4000);
                me->SummonCreature(NPC_ANGERSHADE, 8745, -7134.32f, 35.22f, 0, TEMPSUMMON_CORPSE_DESPAWN, 4000);
                Summon = false;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_apprentice_mirvedaAI(creature);
    }
};

enum InfusedCrystal
{
	NPC_ENRAGED_WRAITH           = 17086,
	NPC_CREDIT_INFUSED_CRYSTAL   = 16364,
	EMOTE                        = 0,
	QUEST_POWERING_OUR_DEFENSES  = 8490,
};

struct Location
{
    float x, y, z;
};

static Location SpawnLocations[] =
{
    {8270.68f, -7188.53f, 139.619f},
    {8284.27f, -7187.78f, 139.603f},
    {8297.43f, -7193.53f, 139.603f},
    {8303.5f, -7201.96f, 139.577f},
    {8273.22f, -7241.82f, 139.382f},
    {8254.89f, -7222.12f, 139.603f},
    {8278.51f, -7242.13f, 139.162f},
    {8267.97f, -7239.17f, 139.517f}
};

class npc_infused_crystal : public CreatureScript
{
public:
    npc_infused_crystal() : CreatureScript("npc_infused_crystal") { }

    struct npc_infused_crystalAI : public QuantumBasicAI
    {
        npc_infused_crystalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint32 EndTimer;
        uint32 WaveTimer;
        bool Completed;
        bool Progress;
        uint64 PlayerGUID;

        void Reset()
        {
            EndTimer = 0;
            Completed = false;
            Progress = false;
            PlayerGUID = 0;
            WaveTimer = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!Progress && who->GetTypeId() == TYPE_ID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
            {
                if (CAST_PLR(who)->GetQuestStatus(QUEST_POWERING_OUR_DEFENSES) == QUEST_STATUS_INCOMPLETE)
                {
                    PlayerGUID = who->GetGUID();
                    WaveTimer = 1000;
                    EndTimer = 60000;
                    Progress = true;
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (PlayerGUID && !Completed)
			{
                if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    CAST_PLR(player)->FailQuest(QUEST_POWERING_OUR_DEFENSES);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (EndTimer < diff && Progress)
            {
				Talk(EMOTE);
                Completed = true;

                if (PlayerGUID)
				{
					if (Player* player = me->GetPlayer(*me, PlayerGUID))
						player->KilledMonsterCredit(NPC_CREDIT_INFUSED_CRYSTAL, PlayerGUID);
				}

				me->DisappearAndDie();
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            }
			else EndTimer -= diff;

            if (WaveTimer < diff && !Completed && Progress)
            {
                uint32 ran1 = rand()%8;
                uint32 ran2 = rand()%8;
                uint32 ran3 = rand()%8;
                me->SummonCreature(NPC_ENRAGED_WRAITH, SpawnLocations[ran1].x, SpawnLocations[ran1].y, SpawnLocations[ran1].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                me->SummonCreature(NPC_ENRAGED_WRAITH, SpawnLocations[ran2].x, SpawnLocations[ran2].y, SpawnLocations[ran2].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                me->SummonCreature(NPC_ENRAGED_WRAITH, SpawnLocations[ran3].x, SpawnLocations[ran3].y, SpawnLocations[ran3].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                WaveTimer = 30000;
            }
			else WaveTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infused_crystalAI(creature);
    }
};

enum QuestSwiftDiscipline
{
	SPELL_DISCIPLINARY_ROD  = 27907,
	SPELL_FIREBALL          = 20811,

	NPC_KILL_CREDIT_RALEN   = 15941,
	NPC_KILL_CREDIT_MELEDOR = 15945,

	SAY_RALEN_SPELLHIT      = -1156300,
	SAY_MELEDOR_SPELLHIT    = -1156301,
};

class npc_apprentice_ralen : public CreatureScript
{
public:
    npc_apprentice_ralen() : CreatureScript("npc_apprentice_ralen") {}

    struct npc_apprentice_ralenAI : public QuantumBasicAI
    {
        npc_apprentice_ralenAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireballTimer;

        void Reset()
        {
			FireballTimer = 1000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DISCIPLINARY_ROD)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_RALEN, 0);
					DoSendQuantumText(SAY_RALEN_SPELLHIT, me);
				}
            }
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				DoCastVictim(SPELL_FIREBALL);
				FireballTimer = urand(3000, 4000);
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_apprentice_ralenAI(creature);
    }
};

class npc_apprentice_meledor : public CreatureScript
{
public:
    npc_apprentice_meledor() : CreatureScript("npc_apprentice_meledor") {}

    struct npc_apprentice_meledorAI : public QuantumBasicAI
    {
        npc_apprentice_meledorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FireballTimer;

        void Reset()
        {
			FireballTimer = 500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DISCIPLINARY_ROD)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_MELEDOR, 0);
					DoSendQuantumText(SAY_MELEDOR_SPELLHIT, me);
				}
            }
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = urand(3000, 4000);
				}
			}
			else FireballTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_apprentice_meledorAI(creature);
    }
};

enum CleansingTheScar
{
	SPELL_POWER_WORD_FORTITUDE = 1243,
	SPELL_SHOOT                = 6660,

	NPC_KILL_CREDIT_RANGER     = 15938,
	QUEST_CLEANSING_THE_SCAR   = 9489,
};

class npc_eversong_ranger : public CreatureScript
{
public:
    npc_eversong_ranger() : CreatureScript("npc_eversong_ranger") {}

    struct npc_eversong_rangerAI : public QuantumBasicAI
    {
        npc_eversong_rangerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHOOT);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
			if (caster->ToPlayer()->GetQuestStatus(QUEST_CLEANSING_THE_SCAR) == QUEST_STATUS_COMPLETE)
				return;

            if (spell->Id == SPELL_POWER_WORD_FORTITUDE)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_RANGER, 0);
					me->DisappearAndDie();
					me->SetRespawnTime(60);
				}
            }
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eversong_rangerAI(creature);
    }
};

void AddSC_eversong_woods()
{
	new npc_master_kelerun_bloodmourn();
    new npc_second_trial_paladin();
    new go_second_trial();
    new npc_apprentice_mirveda();
    new npc_infused_crystal();
	new npc_apprentice_ralen();
	new npc_apprentice_meledor();
	new npc_eversong_ranger();
}