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
#include "the_black_morass.h"

enum Texts
{
	SAY_SAAT_WELCOME        = -1269019,
	MEDIVH_SAY_INTRO        = -1269020,
	MEDIVH_SAY_START        = -1269021,
	MEDIVH_SAY_SHIELD_75    = -1269022,
	MEDIVH_SAY_SHIELD_50    = -1269023,
	MEDIVH_SAY_SHIELD_25    = -1269024,
	MEDIVH_SAY_DEATH        = -1269025,
	MEDIVH_SAY_WIN          = -1269026,
	MEDIVH_SAY_ORCS_ENTER   = -1269027,
	ACOLYTE_SAY_ORCS_ANSWER = -1269028,
};

class npc_medivh_bm : public CreatureScript
{
public:
    npc_medivh_bm() : CreatureScript("npc_medivh_bm") {}

    struct mob_medivhAI : public QuantumBasicAI
    {
        mob_medivhAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            RunesGUID = 0;
        }

        InstanceScript *instance;

        uint32 WaveTimer;
        uint32 Phase;
        uint64 Portal_GUID;
        uint64 RunesGUID;
        uint32 Control;
		bool Intro;
        bool Has75hp;
        bool Has50hp;
        bool Has25hp;
        bool Finished;

        void Reset()
        {
            WaveTimer = 0;
            Phase = 0;
            Control = rand()%4;
			Intro = true;
            Has75hp = false;
            Has50hp = false;
            Has25hp = false;
            Finished = false;
            me->SetActive(true);

            if (instance)
            {
                instance->SetData(DATA_WAVE, 0);
                instance->SetData(DATA_MEDIVH_DEATH, 0);
                instance->SetData(DATA_SHIELD_PERCENT, 100);

                if (instance->GetData(DATA_AEONUS_DEATH) == 1)
                {
                    instance->SetData(DATA_WAVE, 18);
                    DoSendQuantumText(MEDIVH_SAY_WIN, me);
                    me->RemoveAurasDueToSpell(SPELL_MEDIVH_CHANNEL);
                    me->CombatStop();
                    me->SetUInt32Value(UNIT_NPC_FLAGS, 3);
                    Finished = true;
                }
            }

            DoCast(me, SPELL_MEDIVH_SHIELD, true);
            DoCast(me, SPELL_MEDIVH_CHANNEL);
            me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
			SpawnRunes(); // Summoning Rune
			SummonCrystals(); // Summon Black Crystals
        }

        void UpdateWorldState(uint32 field, uint32 value)
        {
            Map* map = me->GetMap();
            if (!map->IsDungeon())
				return;

            WorldPacket data(SMSG_UPDATE_WORLD_STATE, 8);

            data << field;
            data << value;

            ((InstanceMap*)map)->SendToPlayers(&data);
        }

        void EnterToBattle(Unit* /*who*/){}

        void StartEvent()
        {
			DoSendQuantumText(MEDIVH_SAY_START, me);
            Phase = 1;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || me->GetVictim())
                return;

			if (who->IsAlive() && (who->GetTypeId() == TYPE_ID_PLAYER) && me->IsWithinDistInMap(who, 75.0f))
			{
				if (instance->GetData(DATA_AEONUS_DEATH) == 1)
					return;

				if (Intro)
				{
					DoSendQuantumText(MEDIVH_SAY_INTRO, me);
					Intro = false;
				}
			}

			if (who->IsAlive() && (who->GetTypeId() == TYPE_ID_PLAYER) && me->IsWithinDistInMap(who, 5.0f))
            {
                if (Phase == 0)
                {
                    UpdateWorldState(WORLD_STATE_BLACK_MORASS, 1);
                    UpdateWorldState(WORLD_STATE_MEDIVH_SHIELD, 100);
                    UpdateWorldState(WORLD_STATE_RIFT_OPENED, 0);
					StartEvent();
                }
            }
        }

        void SummonPortalRandomPosition(uint32 cas)
        {
            Creature* Portal;
            Portal = me->SummonCreature(NPC_TIME_RIFT,RiftLocations[cas][0],RiftLocations[cas][1],RiftLocations[cas][2], 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);

            do
            {
                Control = rand()%4;
            }
			while (Control == cas);
            Portal->SetActive(true);
            Portal_GUID = Portal->GetGUID();
        }

        void JustDied(Unit* Killer)
        {
            if (instance)
            {
                instance->SetData(DATA_MEDIVH_DEATH, 1);
                instance->SetData(DATA_SHIELD_PERCENT, 0);
            }

            UpdateWorldState(WORLD_STATE_BLACK_MORASS, 0);
            DoSendQuantumText(MEDIVH_SAY_DEATH, me);
        }

		void SpawnRunes()
		{
			for (uint8 i = 0; i < 1; i++)
				me->SummonCreature(NPC_PURPLE_GROUND_RUNE, RuneSpawnPos[i], TEMPSUMMON_DEAD_DESPAWN, 10000);
		}

		void SummonCrystals()
		{
			for (uint8 i = 0; i < 6; i++)
				me->SummonCreature(NPC_DARK_PORTAL_CRYSTAL, DarkCrystalSpawnPos[i], TEMPSUMMON_DEAD_DESPAWN, 10000);
		}

		void SummonGuards()
		{
			for (uint8 i = 0; i < 36; i++)
				me->SummonCreature(NPC_SHADOW_ENFORCER, ShadowEnforcerSpawnPos[i], TEMPSUMMON_DEAD_DESPAWN, 10000);

			for (uint8 i = 0; i < 1; i++)
				me->SummonCreature(NPC_SHADOW_ACOLYTE, ShadowAcolyteSpawnPos[i], TEMPSUMMON_DEAD_DESPAWN, 10000);
		}

        void UpdateAI(const uint32 diff)
        {
            if (Phase == 0 || instance->GetData(DATA_MEDIVH_DEATH) == 1)
                return;

            if (Phase == 1)
            {
                Unit* Portal_control;
                Portal_control =  Unit::GetUnit(*me,Portal_GUID);
                switch(instance->GetData(DATA_WAVE))
                {
                    case 0: // 1
                        if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
                    case 1: // 2
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 2: // 3
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 3: // 4
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 4: // 5
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
                    // CHRONO LORD DEJA
                    case 5: // 6
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 125000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
                    case 6: // 7
						if (WaveTimer <= diff)
						{
							instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
							SummonPortalRandomPosition(Control);
							WaveTimer = 60000;
							UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
						}
						else WaveTimer -= diff;
                        break;
                    case 7: // 8
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 8: // 9
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 9: // 10
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 10: // 11
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 11: // 12
						// TEMPORUS
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 125000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
                    case 12: // 13
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
                    case 13: // 14
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 14: // 15
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 15: // 16
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
					case 16: // 17
						if (WaveTimer <= diff)
                        {
                            instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
                            SummonPortalRandomPosition(Control);
                            WaveTimer = 60000;
                            UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
                        }
						else WaveTimer -= diff;
                        break;
                    // AEONUS
                    case 17: // 18
						if (WaveTimer <= diff)
						{
							instance->SetData(DATA_WAVE, instance->GetData(DATA_WAVE)+1);
							SummonPortalRandomPosition(Control);
							UpdateWorldState(WORLD_STATE_RIFT_OPENED, instance->GetData(DATA_WAVE));
							WaveTimer = 125000;
							Phase = 2;
						}
						else WaveTimer -= diff;
                        break;
					}
				}
                else
				{
                if (instance->GetData(DATA_AEONUS_DEATH) == 1)
                {
                    if (!Finished)
                    {
                        DoSendQuantumText(MEDIVH_SAY_WIN, me);
                        me->RemoveAurasDueToSpell(SPELL_MEDIVH_CHANNEL);
                        me->InterruptNonMeleeSpells(false);
                        me->CombatStop();
                        me->SetUInt32Value(UNIT_NPC_FLAGS, 3);
						SummonGuards(); // Summon Orcs
                        Finished = true;

                        Map* map = me->GetMap();
                        if (map->IsDungeon())
                        {
                            InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
                            for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            {
                                if (Player* player = i->getSource())
                                {
                                    if (player->GetQuestStatus(QUEST_THE_OPENING_DARK_PORTAL) == QUEST_STATUS_INCOMPLETE)
										player->CastSpell(player, SPELL_QUEST_10297, true);
                                    if (player->GetQuestStatus(QUEST_THE_MASTERS_TOUCH) == QUEST_STATUS_INCOMPLETE)
                                        player->CompleteQuest(QUEST_THE_MASTERS_TOUCH);
                                }
                            }
                        }
					}
				}
            }

            if (!Has75hp)
			{
                if (instance->GetData(DATA_SHIELD_PERCENT) <= 75)
                {
					DoSendQuantumText(MEDIVH_SAY_SHIELD_75, me);
                    Has75hp = true;
                }
            }
			else if (!Has50hp)
			{
                if (instance->GetData(DATA_SHIELD_PERCENT) <= 50)
                {
					DoSendQuantumText(MEDIVH_SAY_SHIELD_50, me);
                    Has50hp = true;
                }
            }
			else if (!Has25hp)
			{
                if (instance->GetData(DATA_SHIELD_PERCENT) <= 25)
                {
					DoSendQuantumText(MEDIVH_SAY_SHIELD_25, me);
                    Has25hp = true;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_medivhAI(creature);
    }
};

class npc_time_rift : public CreatureScript
{
public:
    npc_time_rift() : CreatureScript("npc_time_rift") {}

    struct npc_time_riftAI : public QuantumBasicAI
    {
        npc_time_riftAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            RiftGuardianSpawned = false;
            HeroicMode = me->GetMap()->IsHeroic();
        }

		InstanceScript* instance;

        bool HeroicMode;
		bool RiftGuardianSpawned;

        uint32 Add_SpawnTimer;
        uint32 RiftGuardSpawnTimer;
        uint32 Control;

        uint64 Guardian_GUID;

        void Reset()
        {
            me->SetActive(true);

            Add_SpawnTimer = rand()%5000;
            RiftGuardSpawnTimer = 15000;

            Control = 0;
        }

        void EnterToBattle(Unit* /*who*/){}

        void SummonInfiniteSpawnAdd(uint32 entry)
        {
            Unit* pMedivh = Unit::GetUnit(*me, instance->GetData64(DATA_MEDIVH));
            Creature* Infinite_Spawn;

            switch(entry)
            {
            case NPC_INFINITE_ASSASSIN:
                Infinite_Spawn = DoSpawnCreature(NPC_INFINITE_ASSASSIN, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            case NPC_INFINITE_WHELP:
                for(int i = 0 ; i < 3 ; i++)
                {
                    Infinite_Spawn = DoSpawnCreature(NPC_INFINITE_WHELP, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                }
                break;
            case NPC_INFINITE_CHRONOMANCER:
                Infinite_Spawn = DoSpawnCreature(NPC_INFINITE_CHRONOMANCER, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            case NPC_INFINITE_EXECUTIONER:
                Infinite_Spawn = DoSpawnCreature(NPC_INFINITE_EXECUTIONER, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            case NPC_INFINITE_VANQUISHER:
                Infinite_Spawn = DoSpawnCreature(NPC_INFINITE_VANQUISHER, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            }
        }

        uint32 SummonInfiniteSpawn(uint32 cas)
        {
            if (instance->GetData(DATA_WAVE) <= 6)
            {
                switch(cas)
                {
                case 0:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_ASSASSIN);
                    cas++;
                    return cas;
                case 1:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_WHELP);
                    cas++;
                    return cas;
                case 2:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_CHRONOMANCER);
                    cas = 0;
                    return cas;
                }
            }
            if ((instance->GetData(DATA_WAVE) >= 7) && (instance->GetData(DATA_WAVE) <= 12 ))
            {
                switch(cas)
                {
                case 0:
                    //Infinite Executioner
                    SummonInfiniteSpawnAdd(NPC_INFINITE_EXECUTIONER);
                    cas++;
                    return cas;
                case 1:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_CHRONOMANCER);
                    cas++;
                    return cas;
                case 2:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_WHELP);
                    cas++;
                    return cas;
                case 3:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_ASSASSIN);
                    cas = 0;
                    return cas;
                }
            }
            if ((instance->GetData(DATA_WAVE) >= 13) && (instance->GetData(DATA_WAVE) <= 17 ))
            {
                switch(cas)
                {
                case 0:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_EXECUTIONER);
                    cas++;
                    return cas;
                case 1:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_VANQUISHER);
                    cas++;
                    return cas;
                case 2:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_CHRONOMANCER);
                    cas++;
                    return cas;
                case 3:
                    SummonInfiniteSpawnAdd(NPC_INFINITE_ASSASSIN);
                    cas = 0;
                    return cas;
                }
            }
            return 0;
        }

        void SummonRandomGuardian()
        {
            Creature* Guardian;
            Unit* pMedivh = Unit::GetUnit(*me, instance->GetData64(DATA_MEDIVH));
            uint32 wave = instance->GetData(DATA_WAVE); 
        
            switch (wave)
            {   
            case 6:
                if (HeroicMode)
                {    
                    if (instance->GetData(DATA_CHRONO_LORD_DEJA_DEATH) == 1)
						Guardian = DoSpawnCreature(NPC_INFINITE_CHRONO_LORD, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    else Guardian = DoSpawnCreature(NPC_CHRONO_LORD_DEJA, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 600000);
                }
                else 
                {
                    Guardian = DoSpawnCreature(NPC_CHRONO_LORD_DEJA, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 600000);
                }
                break;
            case 12:
                if (HeroicMode)
                {
                    if (instance->GetData(DATA_TEMPORUS_DEATH) == 1)
						Guardian = DoSpawnCreature(NPC_INFINITE_TIMEREAVER, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    else Guardian = DoSpawnCreature(NPC_TEMPORUS, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 600000);
                }
                else
                {
                    Guardian = DoSpawnCreature(NPC_TEMPORUS, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 600000);
                }
                break;
            case 18:
                Guardian = DoSpawnCreature(NPC_AEONUS, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 600000);
                break;
            default:
                switch(rand()%2)
                {
                case 0:
                    //Rift Keeper
                    Guardian = DoSpawnCreature(NPC_RIFT_KEEPER, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    break;
                case 1:
                    //Rift Lord
                    Guardian = DoSpawnCreature(NPC_RIFT_LORD, float(rand()%5), float(rand()%5), 0.0f, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    break;
                }
                break;
            }
            Guardian_GUID = Guardian->GetGUID();
            DoCast(Guardian, SPELL_TIME_RIFT_CHANNEL);
        }

        void UpdateAI(const uint32 diff)
        {   
            Creature* Guardian = (Creature*)Unit::GetUnit(*me, Guardian_GUID);
            /** Despawn if Medivh dies **/
            if (instance->GetData(DATA_MEDIVH_DEATH) == 1)
            {
                if (Guardian)
                {
                    if (Guardian->IsAlive())
                        Guardian->DealDamage(Guardian, Guardian->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    Guardian->RemoveCorpse();
                }
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
                return;
            }

            if (!RiftGuardianSpawned)
            {
                if (RiftGuardSpawnTimer <= diff)
                {
                    SummonRandomGuardian();
                    RiftGuardianSpawned = true;
                }
				else RiftGuardSpawnTimer -= diff;
            }
			else
            {
                if (Add_SpawnTimer <= diff)
                {
                    Control = SummonInfiniteSpawn(Control);
                    Add_SpawnTimer = 15000;
                }
				else Add_SpawnTimer -= diff;
            }        

            if (Guardian)
            {
                if (!Guardian->IsAlive())
                {
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    me->RemoveCorpse();
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_riftAI(creature);
    }
};

#define BACKSTAB                    7159
#define KIDNEY_SHOT                 30832
#define RUPTURE                     15583 // 14874
#define SINISTER_STRIKE             15667 // 14873
#define ARCANE_BOLT                 15230 // 15124
#define ARCANE_EXPLOSION            33860 // 33623
#define FROST_NOVA                  15531 // 15063
#define FROST_BOLT_CHRONO           15497 // 12675 
#define CLEAVE                      15496
#define HAMSTRING                   9080
#define OVERPOWER                   37321 // 17198
#define STRIKE                      34920 // 15580
#define BLAST_WAVE                  38536 // 36278 
#define CURSE_OF_VULNERABILITY      36276
#define ENRAGE                      8269
#define FEAR                        12542
#define FROST_BOLT_KEEPER           38534 // 36279
#define POLYMORPH                   13323
#define PYROBLAST                   38535 // 36277
#define SHADOW_BOLT_VALLEY          36275
#define KNOCKDOWN                   11428
#define MORTAL_STRIKE               35054 // 15708
#define SUNDER_ARMOR                16145
#define THUNDERCLAP                 38537 // 36214

class npc_infinite_guards : public CreatureScript
{
public:
    npc_infinite_guards() : CreatureScript("npc_infinite_guards") {}

    struct npc_infinite_guardsAI : public QuantumBasicAI
    {
        npc_infinite_guardsAI(Creature* creature) : QuantumBasicAI(creature) 
        {
            instance = creature->GetInstanceScript();
        }

        bool IsChanneling;

        // "Corrupt Medivh" Timer
        uint32 CorruptTimer;

        // Infinite Assasin Timers
        uint32 Assasin_BackstabTimer;
        uint32 Assasin_KidneyShotTimer;
        uint32 Assasin_RuptureTimer;
        uint32 Assasin_SinisterStrikeTimer;

        // Infinite Chronomancer Timers
        uint32 ChronoArcaneBoltTimer;
        uint32 ChronoArcaneExplosionTimer;
        uint32 ChronoFrostNovaTimer;
        uint32 ChronoFrostBoltTimer;

        // Infinite Executioner Timers
        uint32 ExecutionerCleaveTimer;
        uint32 ExecutionerHamstringTimer;
        uint32 ExecutionerOverpowerTimer;
        uint32 ExecutionerStrikeTimer;

        // Rift Keeper Type + Timers
        uint32 KeeperType;
        uint32 KeeperBlastWaveTimer;
        uint32 KeeperCurseTimer;
        uint32 KeeperEnrageTimer;
        uint32 KeeperFearTimer;
        uint32 KeeperFrostBoltTimer;
        uint32 KeeperPolymorphTimer;
        uint32 KeeperPyroblastTimer;
        uint32 KeeperShadowBoltValleyTimer;

        // Rift Lord Timers
        uint32 LordEnrageTimer;
        uint32 LordHamstringTimer;
        uint32 LordKnockdownTimer;
        uint32 LordMortalStrikeTimer;
        uint32 LordSunderArmorTimer;
        uint32 LordThunderclapTimer;

        InstanceScript* instance;

        void Reset()
        {
            //me->SetActive(true); // cause Crash ... isnt neccesary
            IsChanneling = false;
            // Corrupt Medivh
            CorruptTimer = 3000;

            // Infinite Assasin Timers
            Assasin_BackstabTimer = 9999999; //dunno know
            Assasin_KidneyShotTimer = 5000;
            Assasin_RuptureTimer = 3000;
            Assasin_SinisterStrikeTimer = 1000;

            // Infinite Chronomancer Timers
            ChronoArcaneBoltTimer = 5000;
            ChronoArcaneExplosionTimer = 6000+rand()%2000;
            ChronoFrostNovaTimer = 1000;
            ChronoFrostBoltTimer = 2000;

            // Infinite Executioner Timers
            ExecutionerCleaveTimer = 9000;
            ExecutionerHamstringTimer = 5000;
            ExecutionerOverpowerTimer = 7000;
            ExecutionerStrikeTimer = 10000;

            // Rift Keeper Type + Timers
            KeeperType = rand()%2;
            KeeperBlastWaveTimer = 9000+rand()%6000;
            KeeperCurseTimer = 20000; //dunno know
            KeeperEnrageTimer = 120000;
            KeeperFearTimer = 10000;
            KeeperFrostBoltTimer = 1000;
            KeeperPolymorphTimer = 10000;
            KeeperPyroblastTimer = 4000+rand()%6000;
            KeeperShadowBoltValleyTimer = 5000+rand()%2000;

            // Rift Lord Timers
            LordEnrageTimer = 120000;
            LordHamstringTimer = 30000;
            LordKnockdownTimer = 35000;
            LordMortalStrikeTimer = 15000;
            LordSunderArmorTimer = 1000;
            LordThunderclapTimer = 10000;

            uint32 CreatureID = me->GetEntry();
            if ((CreatureID == NPC_RIFT_LORD || CreatureID == NPC_RIFT_KEEPER))
                return;

            Creature* pMedivh = (Creature*)(Unit::GetUnit(*me, instance->GetData64(DATA_MEDIVH)));
            if (pMedivh)
            {
                float x, y;
                pMedivh->GetNearPoint2D(x, y, 10, pMedivh->GetAngle(me));
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(1, x, y, pMedivh->GetPositionZ());
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
				return;

            uint32 CreatureID = me->GetEntry();
            if ((CreatureID == NPC_RIFT_LORD || CreatureID == NPC_RIFT_KEEPER))
                return;

            if (id == 1)
            {
                Creature* pMedivh = (Creature*)(Unit::GetUnit(*me, instance->GetData64(DATA_MEDIVH)));
                if (pMedivh)
                {
                    DoCast(pMedivh, SPELL_CORRUPT_MEDIVH_2);
                }
                IsChanneling = true;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->InterruptNonMeleeSpells(false);

            IsChanneling = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            uint32 CreatureID = me->GetEntry();
            if (!(CreatureID == NPC_RIFT_LORD || CreatureID == NPC_RIFT_KEEPER))
                return;

            QuantumBasicAI::MoveInLineOfSight(who);
        }
        void UpdateAI(const uint32 diff)
        {
            Creature* pMedivh = (Creature*)(Unit::GetUnit(*me, instance->GetData64(DATA_MEDIVH)));

            if (IsChanneling)
            {
                if (CorruptTimer <= diff)
                {
                    if (instance)
                    {
                        uint32 ShieldPercent = instance->GetData(DATA_SHIELD_PERCENT);
                        instance->SetData(DATA_SHIELD_PERCENT, ShieldPercent-1);
                        if (pMedivh)
                        {
                            ((npc_medivh_bm::mob_medivhAI *)pMedivh->AI())->UpdateWorldState(WORLD_STATE_MEDIVH_SHIELD, ShieldPercent-1);
                            if (!pMedivh->IsDead() && ShieldPercent <= 0)
                            {
                                me->DealDamage(pMedivh, pMedivh->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                            
                            }
                        }
                    }
                    CorruptTimer = 3000;
                }
				else CorruptTimer -= diff;
            }

            if (instance->GetData(DATA_MEDIVH_DEATH) == 1)
            {
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
                return;
            }

            if (!UpdateVictim())
                return;

            uint32 CreatureID = me->GetEntry();

            if (me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER)
            {
                switch (CreatureID)
                {
                case NPC_RIFT_KEEPER:
                    if (me->GetHealth()*100 / me->GetMaxHealth() < 20)
                    {
                        DoCast(me, ENRAGE);
                        KeeperEnrageTimer = 120000;
                    }
					else KeeperEnrageTimer -= diff;

                    if (KeeperType == 0)
                    {
                        if (KeeperBlastWaveTimer <= diff)
                        {
                            DoCastVictim(BLAST_WAVE);
                            KeeperBlastWaveTimer = 9999999;
                        }
						else KeeperBlastWaveTimer -= diff;

                        if (KeeperFrostBoltTimer <= diff)
                        {
                            Unit* target = SelectTarget(TARGET_RANDOM, 0);
                            DoCast(target, FROST_BOLT_KEEPER);
                            KeeperFrostBoltTimer = 2000+rand()%2000;
                        }
						else KeeperFrostBoltTimer -= diff;

                        if (KeeperPolymorphTimer <= diff)
                        {
                            Unit* target = SelectTarget(TARGET_TOP_AGGRO, 1);
                            DoCast(target, POLYMORPH);
                            KeeperPolymorphTimer = 9999999;
                        }
						else KeeperPolymorphTimer -= diff;

                        if (KeeperPyroblastTimer <= diff)
                        {
                            Unit* target = SelectTarget(TARGET_RANDOM, 0);
                            DoCast(target, PYROBLAST);
                            KeeperPyroblastTimer = 15000;
                        }
						else KeeperPyroblastTimer -= diff;
                    }
                    else
                    {
                        if (KeeperShadowBoltValleyTimer <= diff)
                        {
                            DoCastVictim(SHADOW_BOLT_VALLEY);
                            KeeperShadowBoltValleyTimer = 20000;
                        }
						else KeeperShadowBoltValleyTimer -= diff;

                        if (KeeperCurseTimer <= diff)
                        {
                            Unit* target = SelectTarget(TARGET_RANDOM, 0);
                            DoCast(target, CURSE_OF_VULNERABILITY);
                            KeeperCurseTimer = 9999999;
                        }
						else KeeperCurseTimer -= diff;

                        if (KeeperFearTimer <= diff)
                        {
                            Unit* target = SelectTarget(TARGET_TOP_AGGRO, 1);
                            DoCast(target, FEAR);
                            KeeperFearTimer = 9999999;
                        }
						else KeeperFearTimer -= diff;
                    }
                    break;
                case NPC_RIFT_LORD:
                    if (me->GetHealth()*100 / me->GetMaxHealth() < 20)
                    {
                        DoCast(me, ENRAGE);
                        LordEnrageTimer = 120000;
                    }
					else LordEnrageTimer -= diff;

                    if (LordHamstringTimer <= diff)
                    {
                        DoCastVictim(HAMSTRING);
                        LordHamstringTimer = 9999999;
                    }
					else LordHamstringTimer -= diff;

                    if (LordKnockdownTimer <= diff)
                    {
                        DoCastVictim(KNOCKDOWN);
                        LordKnockdownTimer = 9999999;
                    }
					else LordKnockdownTimer -= diff;

                    if (LordMortalStrikeTimer <= diff)
                    {
                        DoCastVictim(MORTAL_STRIKE);
                        LordMortalStrikeTimer = 9999999;
                    }
					else LordMortalStrikeTimer -= diff;

                    if (LordSunderArmorTimer <= diff)
                    {
                        DoCastVictim(SUNDER_ARMOR);
                        LordSunderArmorTimer = 45000;
                    }
					else LordSunderArmorTimer -= diff;

                    if (LordThunderclapTimer <= diff)
                    {
                        DoCastVictim(THUNDERCLAP);
                        LordThunderclapTimer = 9999999;
                    }
					else LordThunderclapTimer -= diff;
                    break;
                case NPC_INFINITE_ASSASSIN:
                    if (Assasin_BackstabTimer <= diff)
                    {
                        DoCastVictim(BACKSTAB);
                        Assasin_BackstabTimer = 9999999;
                    }
					else Assasin_BackstabTimer -= diff;

                    if (Assasin_KidneyShotTimer <= diff)
                    {
                        DoCastVictim(KIDNEY_SHOT);
                        Assasin_KidneyShotTimer = 9999999;
                    }
					else Assasin_KidneyShotTimer -= diff;

                    if (Assasin_RuptureTimer <= diff)
                    {
                        DoCastVictim(RUPTURE);
                        Assasin_RuptureTimer = 9999999;
                    }
					else Assasin_RuptureTimer -= diff;

                    if (Assasin_SinisterStrikeTimer <= diff)
                    {
                        DoCastVictim(SINISTER_STRIKE);
                        Assasin_SinisterStrikeTimer = 10000;
                    }
					else Assasin_SinisterStrikeTimer -= diff;
                    break;
                case NPC_INFINITE_CHRONOMANCER:
                    if (ChronoArcaneBoltTimer <= diff)
                    {
                        DoCastVictim(ARCANE_BOLT);
                        ChronoArcaneBoltTimer = 9999999;
                    }
					else ChronoArcaneBoltTimer -= diff;

                    if (ChronoArcaneExplosionTimer <= diff)
                    {
                        DoCastVictim(ARCANE_EXPLOSION);
                        ChronoArcaneExplosionTimer = 9999999;
                    }
					else ChronoArcaneExplosionTimer -= diff;

                    if (ChronoFrostNovaTimer <= diff)
                    {
                        DoCastVictim(FROST_NOVA);
                        ChronoFrostNovaTimer = 9999999;
                    }
					else ChronoFrostNovaTimer -= diff;

                    if (ChronoFrostBoltTimer <= diff)
                    {
                        DoCastVictim(FROST_BOLT_CHRONO);
                        ChronoFrostBoltTimer = 9999999;
                    }
					else ChronoFrostBoltTimer -= diff;
                    break;
                case NPC_INFINITE_EXECUTIONER:
                    if (ExecutionerCleaveTimer <= diff)
                    {
                        DoCastVictim(CLEAVE);
                        ExecutionerCleaveTimer = 7000;
                    }
					else ExecutionerCleaveTimer -= diff;

                    if (ExecutionerHamstringTimer <= diff)
                    {
                        DoCastVictim(HAMSTRING);
                        ExecutionerHamstringTimer = 9999999;
                    }
					else ExecutionerHamstringTimer -= diff;

                    if (ExecutionerOverpowerTimer <= diff)
                    {
                        DoCastVictim(OVERPOWER);
                        ExecutionerOverpowerTimer = 9999999;
                    }
					else ExecutionerOverpowerTimer -= diff;

                    if (ExecutionerStrikeTimer <= diff)
                    {
                        DoCastVictim(STRIKE);
                        ExecutionerStrikeTimer = 15000;
                    }
					else ExecutionerStrikeTimer -= diff;
                    break;
                case NPC_INFINITE_VANQUISHER:
                    break;
                case NPC_INFINITE_WHELP:
                    break;
				case NPC_INFINITE_CHRONO_LORD:
					break;
				case NPC_INFINITE_TIMEREAVER:
					break;
                }
            }

            if (!IsChanneling)
                DoMeleeAttackIfReady();
        }

        void KilledUnit(Unit *who)
        {
            Creature* pMedivh = (Creature*)(Unit::GetUnit(*me, instance->GetData64(DATA_MEDIVH)));
            uint32 CreatureID = me->GetEntry();
        
            if (!me->GetVictim())
            {
                if (!(CreatureID == NPC_RIFT_LORD || CreatureID == NPC_RIFT_KEEPER))
                {
                    IsChanneling = false;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_guardsAI(creature);
    }
};

class npc_saat : public CreatureScript
{
public:
    npc_saat() : CreatureScript("npc_saat") {}

    bool GossipHello_npc_saat(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
        {
            player->PrepareQuestMenu(creature->GetGUID());
            player->SendPreparedQuest(creature->GetGUID());
        }

        InstanceScript* instance = creature->GetInstanceScript();
        if (!instance->IsEncounterInProgress() && player->GetQuestStatus(QUEST_THE_BLACK_MORASS) == QUEST_STATUS_COMPLETE)
        {
            player->ADD_GOSSIP_ITEM(0, "Give me a Chrono Beacon",GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true; 
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_CREATE_BEACON, false);
        }
        return true;
    }

    struct npc_saatAI : public QuantumBasicAI
    {
        npc_saatAI(Creature* creature) : QuantumBasicAI(creature) 
        {
            once = false;
        }

        bool once;

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->IsHostileTo(who) && !once && me->IsWithinDistInMap(who, 20))
            {
                DoSendQuantumText(SAY_SAAT_WELCOME, me);
                once = true;
            }
        }
        void UpdateAI(const uint32 /*diff*/){}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_saatAI (creature);
    }
};

class npc_time_keeper : public CreatureScript
{
public:
    npc_time_keeper() : CreatureScript("npc_time_keeper") {}

    struct npc_time_keeperAI : public QuantumBasicAI
    {
        npc_time_keeperAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 DeathTimer;
        uint32 BreathTimer;

        void Reset()
        {
            DeathTimer = 60000;
            BreathTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (DeathTimer <= diff)
            {
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
                DeathTimer = 99999999;
            }
			else DeathTimer -= diff;

            if (!UpdateVictim())
                return;

            if (BreathTimer <= diff)
            {
                DoCastVictim(SPELL_SANDBREATH_TIMEKEEPER);
                BreathTimer = 10000;
            }
			else BreathTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_keeperAI(creature);
    }
};

enum Acolyte
{
	EVENT_SPAWN  = 1,
	EVENT_STEP_1 = 2,
	EVENT_STEP_2 = 3,
	EVENT_STEP_3 = 4,
	EVENT_STEP_4 = 5,
};

class npc_shadow_council_acolyte : public CreatureScript
{
public:
    npc_shadow_council_acolyte() : CreatureScript("npc_shadow_council_acolyte") { }

    struct npc_shadow_council_acolyteAI  : public QuantumBasicAI
    {
        npc_shadow_council_acolyteAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			events.ScheduleEvent(EVENT_SPAWN, 17000);
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_SPAWN:
						if (Creature* medivh = me->FindCreatureWithDistance(NPC_MEDIVH, 150.0f))
						{
							DoSendQuantumText(MEDIVH_SAY_ORCS_ENTER, medivh);
						}
						events.ScheduleEvent(EVENT_STEP_1, 5000);
						break;
					case EVENT_STEP_1:
						DoSendQuantumText(ACOLYTE_SAY_ORCS_ANSWER, me);
						events.ScheduleEvent(EVENT_STEP_2, 5000);
						break;
					case EVENT_STEP_2:
						DoCast(me, SPELL_BANISH_GREEN, true);
						events.ScheduleEvent(EVENT_STEP_3, 3000);
						break;
					case EVENT_STEP_3:
					{
						std::list<Creature*> Enforcer;
						me->GetCreatureListWithEntryInGrid(Enforcer, NPC_SHADOW_ENFORCER, 1000.0f);
						if (!Enforcer.empty())
						{
							for (std::list<Creature*>::const_iterator itr = Enforcer.begin(); itr != Enforcer.end(); ++itr)
							{
								if (Creature* Enforcer = *itr)
								{
									Enforcer->GetMotionMaster()->MovePoint(0, EnforcerPortalMovePos[0]);
									Enforcer->DespawnAfterAction(5*IN_MILLISECONDS);
								}
							}
						}
						events.ScheduleEvent(EVENT_STEP_4, 5000);
					}
					break;
					case EVENT_STEP_4:
						DoCast(me, SPELL_TRANSLOCATE);
						me->DespawnAfterAction(3.2*IN_MILLISECONDS);
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadow_council_acolyteAI(creature);
    }
};

void AddSC_the_black_morass()
{
    new npc_medivh_bm();
    new npc_time_rift();
    new npc_infinite_guards();
    new npc_time_keeper();
    new npc_saat();
	new npc_shadow_council_acolyte();
}