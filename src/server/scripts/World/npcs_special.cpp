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
#include "QuantumEscortAI.h"
#include "QuantumSystemText.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "PetAI.h"
#include <cstring>

enum ForceBots
{
    SPELL_GUARDS_MARK          = 38067,
    AURA_DURATION_TIME_LEFT    = 5000,
};

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon Creature at smaller range
    SPAWNTYPE_ALARMBOT,                                     // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 ThisCreatureEntry;
    uint32 SpawnedCreatureEntry;
    SpawnType SpawnData;
};

float const RANGE_TRIPWIRE     = 15.0f;
float const RANGE_GUARDS_MARK  = 50.0f;

SpawnAssociation spawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

class npc_air_force_bots : public CreatureScript
{
public:
    npc_air_force_bots() : CreatureScript("npc_air_force_bots") {}

    struct npc_air_force_botsAI : public QuantumBasicAI
    {
        npc_air_force_botsAI(Creature* creature) : QuantumBasicAI(creature)
        {
            SpawnAssoc = NULL;
            SpawnedGUID = 0;

            // find the correct spawnhandling
            static uint32 entryCount = sizeof(spawnAssociations) / sizeof(SpawnAssociation);

            for (uint8 i = 0; i < entryCount; ++i)
            {
                if (spawnAssociations[i].ThisCreatureEntry == creature->GetEntry())
                {
                    SpawnAssoc = &spawnAssociations[i];
                    break;
                }
            }

            if (!SpawnAssoc)
                sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", creature->GetEntry());
            else
            {
                CreatureTemplate const* spawnedTemplate = sObjectMgr->GetCreatureTemplate(SpawnAssoc->SpawnedCreatureEntry);

                if (!spawnedTemplate)
                {
                    sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", SpawnAssoc->SpawnedCreatureEntry);
                    SpawnAssoc = NULL;
                    return;
                }
            }
        }

		SpawnAssociation* SpawnAssoc;

        uint64 SpawnedGUID;

        void Reset() {}

        Creature* SummonGuard()
        {
            Creature* summoned = me->SummonCreature(SpawnAssoc->SpawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

            if (summoned)
                SpawnedGUID = summoned->GetGUID();
            else
            {
                sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: npc_air_force_bots: wasn't able to spawn Creature %u", SpawnAssoc->SpawnedCreatureEntry);
                SpawnAssoc = NULL;
            }

            return summoned;
        }

        Creature* GetSummonedGuard()
        {
            Creature* creature = Unit::GetCreature(*me, SpawnedGUID);

            if (creature && creature->IsAlive())
                return creature;

            return NULL;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!SpawnAssoc)
                return;

            if (me->IsValidAttackTarget(who))
            {
                Player* playerTarget = who->ToPlayer();

                // airforce guards only spawn for players
                if (!playerTarget)
                    return;

                Creature* lastSpawnedGuard = SpawnedGUID == 0 ? NULL : GetSummonedGuard();

                // prevent calling Unit::GetUnit at next MoveInLineOfSight call - speedup
                if (!lastSpawnedGuard)
                    SpawnedGUID = 0;

                switch (SpawnAssoc->SpawnData)
                {
                    case SPAWNTYPE_ALARMBOT:
                    {
                        if (!who->IsWithinDistInMap(me, RANGE_GUARDS_MARK))
                            return;

                        Aura* markAura = who->GetAura(SPELL_GUARDS_MARK);
                        if (markAura)
                        {
                            // the target wasn't able to move out of our range within 25 seconds
                            if (!lastSpawnedGuard)
                            {
                                lastSpawnedGuard = SummonGuard();

                                if (!lastSpawnedGuard)
                                    return;
                            }

                            if (markAura->GetDuration() < AURA_DURATION_TIME_LEFT)
                                if (!lastSpawnedGuard->GetVictim())
                                    lastSpawnedGuard->AI()->AttackStart(who);
                        }
                        else
                        {
                            if (!lastSpawnedGuard)
                                lastSpawnedGuard = SummonGuard();

                            if (!lastSpawnedGuard)
                                return;

                            lastSpawnedGuard->CastSpell(who, SPELL_GUARDS_MARK, true);
                        }
                        break;
                    }
                    case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                    {
                        if (!who->IsWithinDistInMap(me, RANGE_TRIPWIRE))
                            return;

                        if (!lastSpawnedGuard)
                            lastSpawnedGuard = SummonGuard();

                        if (!lastSpawnedGuard)
                            return;

                        // ROOFTOP only triggers if the player is on the ground
                        if (!playerTarget->IsFlying() && !lastSpawnedGuard->GetVictim())
                            lastSpawnedGuard->AI()->AttackStart(who);

                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_air_force_botsAI(creature);
    }
};

enum LunaclawSpirit
{
    QUEST_BODY_HEART_A  = 6001,
    QUEST_BODY_HEART_H  = 6002,

    TEXT_ID_DEFAULT     = 4714,
    TEXT_ID_PROGRESS    = 4715,
};

#define GOSSIP_ITEM_GRANT "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

class npc_lunaclaw_spirit : public CreatureScript
{
public:
    npc_lunaclaw_spirit() : CreatureScript("npc_lunaclaw_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, creature->GetGUID());
            player->AreaExploredOrEventHappens(player->GetTeam() == ALLIANCE ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
        }
        return true;
    }
};

enum Cluck
{
	EMOTE_HELLO       = -1070004,
	EMOTE_CLUCK_TEXT  = -1070006,

	QUEST_CLUCK       = 3861,
	FACTION_FRIENDLY  = 35,
	FACTION_CHICKEN   = 31,
};

class npc_chicken_cluck : public CreatureScript
{
public:
    npc_chicken_cluck() : CreatureScript("npc_chicken_cluck") { }

    struct npc_chicken_cluckAI : public QuantumBasicAI
    {
        npc_chicken_cluckAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ResetFlagTimer;

        void Reset()
        {
            ResetFlagTimer = 120000;
            me->SetCurrentFaction(FACTION_CHICKEN);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(uint32 const diff)
        {
            if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER))
            {
                if (ResetFlagTimer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else ResetFlagTimer -= diff;
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote)
        {
            switch (emote)
            {
                case TEXT_EMOTE_CHICKEN:
                    if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE && rand() % 30 == 1)
                    {
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
                        me->SetCurrentFaction(FACTION_FRIENDLY);
                        DoSendQuantumText(EMOTE_HELLO, me);
                    }
                    break;
                case TEXT_EMOTE_CHEER:
                    if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
                    {
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
                        me->SetCurrentFaction(FACTION_FRIENDLY);
                        DoSendQuantumText(EMOTE_CLUCK_TEXT, me);
                    }
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chicken_cluckAI(creature);
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }

    bool OnQuestComplete(Player* /*player*/, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }
};

enum DancingFlames
{
	SPELL_BRAZIER     = 45423,
	SPELL_SEDUCTION   = 47057,
	SPELL_FIERY_AURA  = 45427,
};

class npc_dancing_flames : public CreatureScript
{
public:
    npc_dancing_flames() : CreatureScript("npc_dancing_flames") { }

    struct npc_dancing_flamesAI : public QuantumBasicAI
    {
        npc_dancing_flamesAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Active;
        uint32 CanIteract;

        void Reset()
        {
            Active = true;
            CanIteract = 3500;
            DoCast(me, SPELL_BRAZIER, true);
            DoCast(me, SPELL_FIERY_AURA, false);
            float x, y, z;
            me->GetPosition(x, y, z);
            me->Relocate(x, y, z + 0.94f);
            me->SetDisableGravity(true);
            me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
            WorldPacket data;                       //send update position to client
            me->BuildHeartBeatMsg(&data);
            me->SendMessageToSet(&data, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!Active)
            {
                if (CanIteract <= diff)
                {
                    Active = true;
                    CanIteract = 3500;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                }
                else
                    CanIteract -= diff;
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void ReceiveEmote(Player* player, uint32 emote)
        {
            if (me->IsWithinLOS(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()) && me->IsWithinDistInMap(player, 30.0f))
            {
                me->SetInFront(player);
                Active = false;

                WorldPacket data;
                me->BuildHeartBeatMsg(&data);
                me->SendMessageToSet(&data, true);
                switch (emote)
                {
                    case TEXT_EMOTE_KISS:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);
                        break;
                    case TEXT_EMOTE_WAVE:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        break;
                    case TEXT_EMOTE_BOW:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                        break;
                    case TEXT_EMOTE_JOKE:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                        break;
                    case TEXT_EMOTE_DANCE:
                        if (!player->HasAura(SPELL_SEDUCTION))
                            DoCast(player, SPELL_SEDUCTION, true);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dancing_flamesAI(creature);
    }
};

enum Triage
{
	SAY_DOC1        = -1000201,
	SAY_DOC2        = -1000202,
	SAY_DOC3        = -1000203,

	DOCTOR_ALLIANCE = 12939,
	DOCTOR_HORDE    = 12920,

	ALLIANCE_COORDS = 7,
	HORDE_COORDS    = 6,
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[] =
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                      // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                      // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                      // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                      // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                      // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                      // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[] =
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                      // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                      // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                      // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                      // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                      // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f}                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

uint32 const AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

uint32 const HordeSoldierId[3] =
{
    12923,                                                  // 12923 Injured Soldier
    12924,                                                  // 12924 Badly injured Soldier
    12925                                                   // 12925 Critically injured Soldier
};

class npc_doctor : public CreatureScript
{
public:
    npc_doctor() : CreatureScript("npc_doctor") {}

    struct npc_doctorAI : public QuantumBasicAI
    {
        npc_doctorAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;

        uint32 SummonPatientTimer;
        uint32 SummonPatientCount;
        uint32 PatientDiedCount;
        uint32 PatientSavedCount;

        bool Event;

        std::list<uint64> Patients;
        std::vector<Location*> Coordinates;

        void Reset()
        {
            PlayerGUID = 0;

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            Patients.clear();
            Coordinates.clear();

            Event = false;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void BeginEvent(Player* player)
        {
            PlayerGUID = player->GetGUID();

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            switch (me->GetEntry())
            {
                case DOCTOR_ALLIANCE:
                    for (uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                        Coordinates.push_back(&AllianceCoords[i]);
                    break;
                case DOCTOR_HORDE:
                    for (uint8 i = 0; i < HORDE_COORDS; ++i)
                        Coordinates.push_back(&HordeCoords[i]);
                    break;
            }

            Event = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void PatientDied(Location* point)
        {
            Player* player = Unit::GetPlayer(*me, PlayerGUID);
            if (player && ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
            {
                ++PatientDiedCount;

                if (PatientDiedCount > 5 && Event)
                {
                    if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6624);
                    else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6622);

                    Reset();
                    return;
                }

                Coordinates.push_back(point);
            }
            else
                // If no player or player abandon quest in progress
                Reset();
        }

        void PatientSaved(Creature* /*soldier*/, Player* player, Location* point)
        {
            if (player && PlayerGUID == player->GetGUID())
            {
                if ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
                {
                    ++PatientSavedCount;

                    if (PatientSavedCount == 15)
                    {
                        if (!Patients.empty())
                        {
                            std::list<uint64>::const_iterator itr;
                            for (itr = Patients.begin(); itr != Patients.end(); ++itr)
                            {
                                if (Creature* patient = Unit::GetCreature(*me, *itr))
                                    patient->setDeathState(JUST_DIED);
                            }
                        }

                        if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6624);
                        else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6622);

                        Reset();
                        return;
                    }

                    Coordinates.push_back(point);
                }
            }
        }

        void UpdateAI(uint32 const diff);

        void EnterToBattle(Unit* /*who*/){}
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if ((quest->GetQuestId() == 6624) || (quest->GetQuestId() == 6622))
            CAST_AI(npc_doctor::npc_doctorAI, creature->AI())->BeginEvent(player);

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_doctorAI(creature);
    }
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

class npc_injured_patient : public CreatureScript
{
public:
    npc_injured_patient() : CreatureScript("npc_injured_patient") { }

    struct npc_injured_patientAI : public QuantumBasicAI
    {
        npc_injured_patientAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 DoctorGUID;
        Location* Coord;

        void Reset()
        {
            DoctorGUID = 0;
            Coord = NULL;

            //no select
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //no regen health
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            //to make them lay with face down
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
            uint32 mobId = me->GetEntry();

            switch (mobId)
            {
                case 12923:
                case 12938: 
                    me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_75));
                    break;
                case 12924:
                case 12936:
                    me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_50));
                    break;
                case 12925:
                case 12937:
                    me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_25));
                    break;
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (caster->GetTypeId() == TYPE_ID_PLAYER && me->IsAlive() && spell->Id == 20804)
            {
                if ((CAST_PLR(caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (CAST_PLR(caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
                    if (DoctorGUID)
                        if (Creature* doctor = Unit::GetCreature(*me, DoctorGUID))
                            CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientSaved(me, CAST_PLR(caster), Coord);

                //make not selectable
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                //regen health
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                //stand up
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);
                DoSendQuantumText(RAND(SAY_DOC1, SAY_DOC2, SAY_DOC3), me);
                uint32 mobId = me->GetEntry();
                me->SetWalk(false);

                switch (mobId)
                {
                    case 12923:
                    case 12924:
                    case 12925:
                        me->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                        break;
                    case 12936:
                    case 12937:
                    case 12938:
                        me->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                        break;
                }
            }
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            //lower HP on every world tick makes it a useful counter, not officlone though
            if (me->IsAlive() && me->GetHealth() > 6)
                me->ModifyHealth(-5);

            if (me->IsAlive() && me->GetHealth() <= 6)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setDeathState(JUST_DIED);
                me->SetFlag(UNIT_DYNAMIC_FLAGS, 32);

                if (DoctorGUID)
                    if (Creature* doctor = Unit::GetCreature(*me, DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientDied(Coord);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_patientAI(creature);
    }
};

void npc_doctor::npc_doctorAI::UpdateAI(uint32 const diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatientTimer <= diff)
        {
            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin() + rand() % Coordinates.size();
            uint32 patientEntry = 0;

            switch (me->GetEntry())
            {
                case DOCTOR_ALLIANCE:
                    patientEntry = AllianceSoldierId[rand() % 3];
                    break;
                case DOCTOR_HORDE:
                    patientEntry = HordeSoldierId[rand() % 3];
                    break;
                default:
                    sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            if (Location* point = *itr)
            {
                if (Creature* Patient = me->SummonCreature(patientEntry, point->x, point->y, point->z, point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                {
                    //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                    Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                    Patients.push_back(Patient->GetGUID());
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->DoctorGUID = me->GetGUID();
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->Coord = point;

                    Coordinates.erase(itr);
                }
            }
            SummonPatientTimer = 10000;
            ++SummonPatientCount;
        }
        else SummonPatientTimer -= diff;
    }
}

enum Garments
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    NPC_GUARD_ROBERTS       = 12423,
    NPC_MOUNTAINEER_DOLF    = 12427,
	NPC_DEATHGUARD_KEL      = 12428,
	NPC_SENTINEL_SHAYA      = 12429,
    NPC_GRUNT_KORJA         = 12430,
    
	SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263, // Signed for 21469
};

class npc_garments_of_quests : public CreatureScript
{
public:
    npc_garments_of_quests() : CreatureScript("npc_garments_of_quests") { }

    struct npc_garments_of_questsAI : public npc_escortAI
    {
        npc_garments_of_questsAI(Creature* creature) : npc_escortAI(creature)
		{
			Reset();
		}

        uint64 CasterGUID;

        bool IsHealed;
        bool CanRun;

        uint32 RunAwayTimer;

        void Reset()
        {
            CasterGUID = 0;

            IsHealed = false;
            CanRun = false;

            RunAwayTimer = 5000;

            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_70));
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_LESSER_HEAL_R2 || spell->Id == SPELL_FORTITUDE_R1)
            {
                if (me->IsInCombatActive())
                    return;

                if (IsHealed && CanRun)
                    return;

                if (Player* player = caster->ToPlayer())
                {
                    switch (me->GetEntry())
                    {
                        case NPC_SENTINEL_SHAYA:
                            if (player->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    DoSendQuantumText(SAY_SHAYA_THANKS, me, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    DoSendQuantumText(SAY_COMMON_HEALED, me, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case NPC_GUARD_ROBERTS:
                            if (player->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    DoSendQuantumText(SAY_ROBERTS_THANKS, me, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    DoSendQuantumText(SAY_COMMON_HEALED, me, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case NPC_MOUNTAINEER_DOLF:
                            if (player->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    DoSendQuantumText(SAY_DOLF_THANKS, me, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    DoSendQuantumText(SAY_COMMON_HEALED, me, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case NPC_GRUNT_KORJA:
                            if (player->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    DoSendQuantumText(SAY_KORJA_THANKS, me, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    DoSendQuantumText(SAY_COMMON_HEALED, me, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case NPC_DEATHGUARD_KEL:
                            if (player->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    DoSendQuantumText(SAY_DG_KEL_THANKS, me, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    DoSendQuantumText(SAY_COMMON_HEALED, me, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                    }

                    if (CanRun)
                        player->TalkedToCreature(me->GetEntry(), me->GetGUID());
                }
            }
        }

        void WaypointReached(uint32 /*point*/){}

        void UpdateAI(uint32 const diff)
        {
            if (CanRun && !me->IsInCombatActive())
            {
                if (RunAwayTimer <= diff)
                {
                    if (Unit* unit = Unit::GetUnit(*me, CasterGUID))
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_SENTINEL_SHAYA:
                                DoSendQuantumText(SAY_SHAYA_GOODBYE, me, unit);
                                break;
                            case NPC_GUARD_ROBERTS:
                                DoSendQuantumText(SAY_ROBERTS_GOODBYE, me, unit);
                                break;
                            case NPC_MOUNTAINEER_DOLF:
                                DoSendQuantumText(SAY_DOLF_GOODBYE, me, unit);
                                break;
                            case NPC_GRUNT_KORJA:
                                DoSendQuantumText(SAY_KORJA_GOODBYE, me, unit);
                                break;
                            case NPC_DEATHGUARD_KEL:
                                DoSendQuantumText(SAY_DG_KEL_GOODBYE, me, unit);
                                break;
                        }

                        Start(false, true, true);
                    }
                    else EnterEvadeMode();

                    RunAwayTimer = 30000;
                }
                else RunAwayTimer -= diff;

				DoMeleeAttackIfReady();
            }

            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_garments_of_questsAI(creature);
    }
};

/*######
## npc_guardian
######*/

#define SPELL_DEATH_TOUCH  5

class npc_guardian : public CreatureScript
{
public:
    npc_guardian() : CreatureScript("npc_guardian") { }

    struct npc_guardianAI : public QuantumBasicAI
    {
        npc_guardianAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

            if (me->IsAttackReady())
            {
                DoCastVictim(SPELL_DEATH_TOUCH, true);
                me->ResetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guardianAI(creature);
    }
};

enum KingdomDalaran
{
    SPELL_TELEPORT_DALARAN  = 53360,
    ITEM_KT_SIGNET          = 39740,
    QUEST_MAGICAL_KINGDOM_A = 12794,
    QUEST_MAGICAL_KINGDOM_H = 12791,
    QUEST_MAGICAL_KINGDOM_N = 12796,
};

#define GOSSIP_ITEM_TELEPORT_TO "I am ready to be teleported to Dalaran."

class npc_kingdom_of_dalaran_quests : public CreatureScript
{
public:
    npc_kingdom_of_dalaran_quests() : CreatureScript("npc_kingdom_of_dalaran_quests") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->HasItemCount(ITEM_KT_SIGNET, 1) && (!player->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_A) ||
            !player->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_H) || !player->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_N)))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT_TO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_DALARAN, false);
        }
        return true;
    }
};

class npc_mount_vendor : public CreatureScript
{
public:
    npc_mount_vendor() : CreatureScript("npc_mount_vendor") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        bool CanBuy = false;
        uint32 vendor = creature->GetEntry();
        uint8 race = player->GetCurrentRace();

        switch (vendor)
        {
            case 384: // Katie Hunter
            case 1460: // Unger Statforth
            case 2357: // Merideth Carlson
            case 4885: // Gregor MacVince
                if (player->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                    player->SEND_GOSSIP_MENU(5855, creature->GetGUID());
                else CanBuy = true;
                break;
            case 1261: // Veron Amberstill
                if (player->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                    player->SEND_GOSSIP_MENU(5856, creature->GetGUID());
                else CanBuy = true;
                break;
            case 3362: // Ogunaro Wolfrunner
                if (player->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                    player->SEND_GOSSIP_MENU(5841, creature->GetGUID());
                else CanBuy = true;
                break;
            case 3685: // Harb Clawhoof
                if (player->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                    player->SEND_GOSSIP_MENU(5843, creature->GetGUID());
                else CanBuy = true;
                break;
            case 4730: // Lelanai
                if (player->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHT_ELF)
                    player->SEND_GOSSIP_MENU(5844, creature->GetGUID());
                else CanBuy = true;
                break;
            case 4731: // Zachariah Post
                if (player->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
                    player->SEND_GOSSIP_MENU(5840, creature->GetGUID());
                else CanBuy = true;
                break;
            case 7952: // Zjolnir
                if (player->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                    player->SEND_GOSSIP_MENU(5842, creature->GetGUID());
                else CanBuy = true;
                break;
            case 7955: // Milli Featherwhistle
                if (player->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                    player->SEND_GOSSIP_MENU(5857, creature->GetGUID());
                else CanBuy = true;
                break;
            case 16264: // Winaestra
                if (player->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOOD_ELF)
                    player->SEND_GOSSIP_MENU(10305, creature->GetGUID());
                else CanBuy = true;
                break;
            case 17584: // Torallius the Pack Handler
                if (player->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                    player->SEND_GOSSIP_MENU(10239, creature->GetGUID());
                else CanBuy = true;
                break;
        }

        if (CanBuy)
        {
            if (creature->IsVendor())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_TRADE)
			player->GetSession()->SendListInventory(creature->GetGUID());

		return true;
    }
};

enum SpecSpells
{
	SPELL_TALENT_SPECIALISATION         = 63680,
	SPELL_SECOND_TALENT_SPECIALISATION  = 63624,
};

#define GOSSIP_HELLO_ROGUE1 "I wish to unlearn my talents"
#define GOSSIP_HELLO_ROGUE2 "<Take the letter>"
#define GOSSIP_HELLO_ROGUE3 "Purchase a Dual Talent Specialization."

class npc_rogue_trainer : public CreatureScript
{
public:
    npc_rogue_trainer() : CreatureScript("npc_rogue_trainer") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsTrainer())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TRAINER_TRAIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        if (creature->IsCanTrainingAndResetTalentsOf(player))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE1, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

        if (player->GetSpecsCount() == 1 && creature->IsCanTrainingAndResetTalentsOf(player) && !(player->GetCurrentLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE3, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_LEARNDUALSPEC);

        if (player->GetCurrentClass() == CLASS_ROGUE && player->GetCurrentLevel() >= 24 && !player->HasItemCount(17126, 1) && !player->GetQuestRewardStatus(6681))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_ROGUE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(5996, creature->GetGUID());
        }
		else
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 21100, false);
                break;
            case GOSSIP_ACTION_TRAIN:
                player->GetSession()->SendTrainerList(creature->GetGUID());
                break;
            case GOSSIP_OPTION_UNLEARNTALENTS:
                player->CLOSE_GOSSIP_MENU();
                player->SendTalentWipeConfirm(creature->GetGUID());
                break;
            case GOSSIP_OPTION_LEARNDUALSPEC:
                if (player->GetSpecsCount() == 1 && !(player->GetCurrentLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)))
                {
                    if (!player->HasEnoughMoney(10000000))
                    {
                        player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
                        player->PlayerTalkClass->SendCloseGossip();
                        break;
                    }
                    else
                    {
                        player->ModifyMoney(-10000000);
                        player->CastSpell(player, SPELL_TALENT_SPECIALISATION, true, NULL, NULL, player->GetGUID());
                        player->CastSpell(player, SPELL_SECOND_TALENT_SPECIALISATION, true, NULL, NULL, player->GetGUID());
                        player->PlayerTalkClass->SendCloseGossip();
                    }
                }
                break;
        }
        return true;
    }
};

/*######
## npc_sayge
######*/

#define GOSSIP_HELLO_SAYGE "Yes"
#define GOSSIP_SENDACTION_SAYGE1 "Slay the Man"
#define GOSSIP_SENDACTION_SAYGE2 "Turn him over to liege"
#define GOSSIP_SENDACTION_SAYGE3 "Confiscate the corn"
#define GOSSIP_SENDACTION_SAYGE4 "Let him go and have the corn"
#define GOSSIP_SENDACTION_SAYGE5 "Execute your friend painfully"
#define GOSSIP_SENDACTION_SAYGE6 "Execute your friend painlessly"
#define GOSSIP_SENDACTION_SAYGE7 "Let your friend go"
#define GOSSIP_SENDACTION_SAYGE8 "Confront the diplomat"
#define GOSSIP_SENDACTION_SAYGE9 "Show not so quiet defiance"
#define GOSSIP_SENDACTION_SAYGE10 "Remain quiet"
#define GOSSIP_SENDACTION_SAYGE11 "Speak against your brother openly"
#define GOSSIP_SENDACTION_SAYGE12 "Help your brother in"
#define GOSSIP_SENDACTION_SAYGE13 "Keep your brother out without letting him know"
#define GOSSIP_SENDACTION_SAYGE14 "Take credit, keep gold"
#define GOSSIP_SENDACTION_SAYGE15 "Take credit, share the gold"
#define GOSSIP_SENDACTION_SAYGE16 "Let the knight take credit"
#define GOSSIP_SENDACTION_SAYGE17 "Thanks"

enum Sayge
{
	SPELL_DARK_FORTUNE_DAMAGE        = 23768,
	SPELL_DARK_FORTUNE_RESISTANCE    = 23769,
	SPELL_DARK_FORTUNE_ARMOR         = 23767,
	SPELL_DARK_FORTUNE_SPIRIT        = 23738,
	SPELL_DARK_FORTUNE_INTELLIGENCE  = 23766,
	SPELL_DARK_FORTUNE_STAMINA       = 23737,
	SPELL_DARK_FORTUNE_STRENGHT      = 23735,
	SPELL_DARK_FORTUNE_AGILITY       = 23736,
	SPELL_DARKMOON_FAIRE_FORTUNE     = 23765,
};

class npc_sayge : public CreatureScript
{
public:
    npc_sayge() : CreatureScript("npc_sayge") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->HasSpellCooldown(SPELL_DARK_FORTUNE_INTELLIGENCE) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_ARMOR) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_DAMAGE) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_RESISTANCE) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_STRENGHT) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_AGILITY) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_STAMINA) ||
            player->HasSpellCooldown(SPELL_DARK_FORTUNE_SPIRIT))
            player->SEND_GOSSIP_MENU(7393, creature->GetGUID());
        else
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SAYGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(7339, creature->GetGUID());
        }

        return true;
    }

    void SendAction(Player* player, Creature* creature, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(7340, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE5, GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE6, GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE7, GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7341, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE8, GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE9, GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE10, GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7361, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE11, GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE12, GOSSIP_SENDER_MAIN + 7, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE13, GOSSIP_SENDER_MAIN + 8, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7362, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE14, GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE15, GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE16, GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7363, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE17, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(7364, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                creature->CastSpell(player, SPELL_DARKMOON_FAIRE_FORTUNE, false);
                player->SEND_GOSSIP_MENU(7365, creature->GetGUID());
                break;
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (sender)
        {
            case GOSSIP_SENDER_MAIN:
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 1:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_DAMAGE, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_DAMAGE, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 2:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_RESISTANCE, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_RESISTANCE, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 3:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_ARMOR, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_ARMOR, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 4:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_SPIRIT, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_SPIRIT, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 5:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_INTELLIGENCE, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_INTELLIGENCE, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 6:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_STAMINA, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_STAMINA, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 7:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_STRENGHT, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_STRENGHT, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 8:
                creature->CastSpell(player, SPELL_DARK_FORTUNE_AGILITY, false);
                player->AddSpellCooldown(SPELL_DARK_FORTUNE_AGILITY, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
        }
        return true;
    }
};

class npc_steam_tonk : public CreatureScript
{
public:
    npc_steam_tonk() : CreatureScript("npc_steam_tonk") { }

    struct npc_steam_tonkAI : public QuantumBasicAI
    {
        npc_steam_tonkAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void OnPossess(bool apply)
        {
            if (apply)
            {
                // Initialize the action bar without the melee attack command
                me->InitCharmInfo();
                me->GetCharmInfo()->InitEmptyActionBar(false);

                me->SetReactState(REACT_PASSIVE);
            }
            else
                me->SetReactState(REACT_AGGRESSIVE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steam_tonkAI(creature);
    }
};

#define SPELL_TONK_MINE_DETONATE 25099

class npc_tonk_mine : public CreatureScript
{
public:
    npc_tonk_mine() : CreatureScript("npc_tonk_mine") { }

    struct npc_tonk_mineAI : public QuantumBasicAI
    {
        npc_tonk_mineAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 ExplosionTimer;

        void Reset()
        {
            ExplosionTimer = 3000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(uint32 const diff)
        {
            if (ExplosionTimer <= diff)
            {
                DoCast(me, SPELL_TONK_MINE_DETONATE, true);
                me->setDeathState(DEAD); // unsummon it
            }
            else ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tonk_mineAI(creature);
    }
};

class npc_brewfest_reveler : public CreatureScript
{
public:
    npc_brewfest_reveler() : CreatureScript("npc_brewfest_reveler") { }

    struct npc_brewfest_revelerAI : public QuantumBasicAI
    {
        npc_brewfest_revelerAI(Creature* creature) : QuantumBasicAI(creature) {}
        void ReceiveEmote(Player* player, uint32 emote)
        {
            if (!IsHolidayActive(HOLIDAY_BREWFEST))
                return;

            if (emote == TEXT_EMOTE_DANCE)
                me->CastSpell(player, 41586, false);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brewfest_revelerAI(creature);
    }
};

/*####
## npc_snake_trap_serpents
####*/

enum SnakeTrap
{
	SPELL_MIND_NUMBING_POISON  = 25810, // Viper
	SPELL_DEADLY_POISON        = 34655, // Venomous Snake
	SPELL_CRIPPLING_POISON     = 30981, // Viper

	NPC_VIPER                  = 19921,
};

enum Timers
{
	VENOMOUS_SNAKE_TIMER = 1500,
	VIPER_TIMER          = 3000,
};

class npc_snake_trap : public CreatureScript
{
public:
    npc_snake_trap() : CreatureScript("npc_snake_trap_serpents") { }

    struct npc_snake_trap_serpentsAI : public QuantumBasicAI
    {
        npc_snake_trap_serpentsAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SpellTimer;

        bool IsViper;

        void EnterToBattle(Unit* /*who*/){}

        void Reset()
        {
            SpellTimer = 0;

            CreatureTemplate const* Info = me->GetCreatureTemplate();

            IsViper = Info->Entry == NPC_VIPER ? true : false;

            me->SetMaxHealth(uint32(107 * (me->GetCurrentLevel() - 40) * 0.025f));
            // Add delta to make them not all hit the same time
            uint32 delta = (rand() % 7) * 100;
            me->SetStatFloatValue(UNIT_FIELD_BASE_ATTACK_TIME, float(Info->baseattacktime + delta));
            me->SetStatFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER, float(Info->attackpower));

            // Start attacking attacker of owner on first ai update after spawn - move in line of sight may choose better target
            if (!me->GetVictim() && me->IsSummon())
			{
                if (Unit* owner = me->ToTempSummon()->GetSummoner())
				{
                    if (owner->getAttackerForHelper())
                        AttackStart(owner->getAttackerForHelper());
				}
			}
        }

        // Redefined for random target selection:
        void MoveInLineOfSight(Unit* who)
        {
            if (!me->GetVictim() && me->CanCreatureAttack(who))
            {
                if (me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                    return;

                float AttackRadius = me->GetAttackDistance(who);
                if (me->IsWithinDistInMap(who, AttackRadius) && me->IsWithinLOSInMap(who))
                {
                    if (!(rand() % 5))
                    {
                        me->SetAttackTimer(BASE_ATTACK, (rand() % 10) * 100);
                        SpellTimer = (rand() % 10) * 100;
                        AttackStart(who);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->GetVictim()->HasBreakableByDamageCrowdControlAura(me))
			{
				me->InterruptNonMeleeSpells(false);
				return;
			}

            if (SpellTimer <= diff)
            {
                if (IsViper) // Viper
                {
                    if (urand(0, 2) == 0) // 33% chance to cast
                    {
                        uint32 spell;
                        if (urand(0, 1) == 0)
                            spell = SPELL_MIND_NUMBING_POISON;
                        else
                            spell = SPELL_CRIPPLING_POISON;

                        DoCastVictim(spell);
                    }

                    SpellTimer = VIPER_TIMER;
                }
                else // Venomous Snake
                {
                    if (urand(0, 2) == 0) //33% chance to cast
                        DoCastVictim(SPELL_DEADLY_POISON);
                    SpellTimer = VENOMOUS_SNAKE_TIMER + (rand() % 5) * 100;
                }
            }
            else
                SpellTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snake_trap_serpentsAI(creature);
    }
};

#define SAY_RANDOM_MOJO_0 "Now that's what I call froggy-style!"
#define SAY_RANDOM_MOJO_1 "Your lily pad or mine?"
#define SAY_RANDOM_MOJO_2 "This won't take long, did it?"
#define SAY_RANDOM_MOJO_3 "I thought you'd never ask!"
#define SAY_RANDOM_MOJO_4 "I promise not to give you warts..."
#define SAY_RANDOM_MOJO_5 "Feelin' a little froggy, are ya?"
#define SAY_RANDOM_MOJO_6A "Listen, "
#define SAY_RANDOM_MOJO_6B ",I know of a little swamp not too far from here..."
#define SAY_RANDOM_MOJO_7 "There's just never enough Mojo to go around..."

enum Mojo
{
	SPELL_TAG_HEARTS     = 20372,
	SPELL_FEELING_FROGGY = 43906,
};

class npc_mojo : public CreatureScript
{
public:
    npc_mojo() : CreatureScript("npc_mojo") { }

    struct npc_mojoAI : public QuantumBasicAI
    {
        npc_mojoAI(Creature* creature) : QuantumBasicAI(creature) {Reset();}

        uint32 Hearts;
        uint64 PlayerGuid;

        void Reset()
        {
            PlayerGuid = 0;

            Hearts = 15000;

            if (Unit* who = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(who, 0, 0);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (me->HasAura(SPELL_TAG_HEARTS))
            {
                if (Hearts <= diff)
                {
                    me->RemoveAurasDueToSpell(SPELL_TAG_HEARTS);
                    Hearts = 15000;
                }
                Hearts -= diff;
            }
        }

        void ReceiveEmote(Player* player, uint32 emote)
        {
            me->HandleEmoteCommand(emote);

            Unit* who = me->GetOwner();

            if (!who || who->GetTypeId() != TYPE_ID_PLAYER || CAST_PLR(who)->GetTeam() != player->GetTeam())
                return;

            if (emote == TEXT_EMOTE_KISS)
            {
                std::string whisp = "";

                switch (rand() % 8)
                {
                    case 0:
                        whisp.append(SAY_RANDOM_MOJO_0);
                        break;
                    case 1:
                        whisp.append(SAY_RANDOM_MOJO_1);
                        break;
                    case 2:
                        whisp.append(SAY_RANDOM_MOJO_2);
                        break;
                    case 3:
                        whisp.append(SAY_RANDOM_MOJO_3);
                        break;
                    case 4:
                        whisp.append(SAY_RANDOM_MOJO_4);
                        break;
                    case 5:
                        whisp.append(SAY_RANDOM_MOJO_5);
                        break;
                    case 6:
                        whisp.append(SAY_RANDOM_MOJO_6A);
                        whisp.append(player->GetName());
                        whisp.append(SAY_RANDOM_MOJO_6B);
                        break;
                    case 7:
                        whisp.append(SAY_RANDOM_MOJO_7);
                        break;
                }

                me->MonsterWhisper(whisp.c_str(), player->GetGUID());

                if (PlayerGuid)
				{
                    if (Player* victim = Unit::GetPlayer(*me, PlayerGuid))
                        victim->RemoveAura(SPELL_FEELING_FROGGY);
				}

                me->AddAura(SPELL_FEELING_FROGGY, player);
                PlayerGuid = player->GetGUID();
                DoCast(me, SPELL_TAG_HEARTS, true);
                me->GetMotionMaster()->MoveFollow(player, 0, 0);
                Hearts = 15000;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mojoAI(creature);
    }
};

class npc_mirror_image : public CreatureScript
{
public:
    npc_mirror_image() : CreatureScript("npc_mirror_image") { }

    struct npc_mirror_imageAI : CasterAI
    {
        npc_mirror_imageAI(Creature* creature) : CasterAI(creature) {}

        void InitializeAI()
        {
            CasterAI::InitializeAI();
            Unit* owner = me->GetOwner();
            if (!owner)
                return;

            owner->CastSpell((Unit*)NULL, 58838, true);
            owner->CastSpell(me, 45204, false);

			if (owner->ToPlayer() && owner->ToPlayer()->GetSelectedUnit())
				me->AI()->AttackStart(owner->ToPlayer()->GetSelectedUnit());
		}

        void EnterToBattle(Unit* who)
        {
            if (spells.empty())
                return;

            for (SpellVct::iterator itr = spells.begin(); itr != spells.end(); ++itr)
            {
                if (AISpellInfo[*itr].condition == AICOND_AGGRO)
                    me->CastSpell(who, *itr, false);

                else if (AISpellInfo[*itr].condition == AICOND_COMBAT)
                {
                    uint32 cooldown = GetAISpellInfo(*itr)->realCooldown;
                    events.ScheduleEvent(*itr, cooldown);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            bool hasCC = false;
            if (me->GetCharmerOrOwnerGUID() && me->GetVictim())
                hasCC = me->GetVictim()->HasAuraType(SPELL_AURA_MOD_CONFUSE);

            if (hasCC)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    me->CastStop();
                me->AI()->EnterEvadeMode();  
                return;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 spellId = events.ExecuteEvent())
            {
                if (hasCC)
                {
                    events.ScheduleEvent(spellId, 500);
                    return;
                }
                DoCast(spellId);
                uint32 casttime = me->GetCurrentSpellCastTime(spellId);
                events.ScheduleEvent(spellId, (casttime ? casttime : 500) + GetAISpellInfo(spellId)->realCooldown);
            }
        }

        void EnterEvadeMode()
        {
            if (me->IsInEvadeMode() || !me->IsAlive())
                return;

            Unit* owner = me->GetCharmerOrOwner();

            me->CombatStop(true);
            if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
            {
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mirror_imageAI(creature);
    }
};

enum EboneGargoyle
{
	SPELL_SUMMON_GARGOYLE   = 49206,
	SPELL_SUMMON_GARGOYLE_1 = 50514,
	SPELL_DISMISS_GARGOYLE  = 50515,
	SPELL_SANCTUARY         = 54661,
};

class npc_ebon_gargoyle : public CreatureScript
{
public:
    npc_ebon_gargoyle() : CreatureScript("npc_ebon_gargoyle") { }

    struct npc_ebon_gargoyleAI : CasterAI
    {
        npc_ebon_gargoyleAI(Creature* creature) : CasterAI(creature) {}

        uint32 DespawnTimer;

        void InitializeAI()
        {
            CasterAI::InitializeAI();
            uint64 ownerGuid = me->GetOwnerGUID();
            if (!ownerGuid)
                return;
            // Not needed to be despawned now
            DespawnTimer = 0;
            // Find victim of Summon Gargoyle spell
            std::list<Unit*> targets;
            Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 30);
            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
            me->VisitNearbyObject(30, searcher);
            for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
			{
                if ((*iter)->GetAura(SPELL_SUMMON_GARGOYLE, ownerGuid))
                {
                    me->Attack((*iter), false);
                    break;
                }
			}
        }

        void JustDied(Unit* /*killer*/)
        {
            // Stop Feeding Gargoyle when it dies
            if (Unit* owner = me->GetOwner())
                owner->RemoveAurasDueToSpell(SPELL_SUMMON_GARGOYLE_1);
        }

        // Fly away when dismissed
        void SpellHit(Unit* source, SpellInfo const* spell)
        {
            if (spell->Id != SPELL_DISMISS_GARGOYLE || !me->IsAlive())
                return;

            Unit* owner = me->GetOwner();

            if (!owner || owner != source)
                return;

            // Stop Fighting
            me->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE, true);
            // Sanctuary
            me->CastSpell(me, SPELL_SANCTUARY, true);
            me->SetReactState(REACT_PASSIVE);
            // Fly Away
            me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY|MOVEMENTFLAG_ASCENDING|MOVEMENTFLAG_FLYING);
            me->SetSpeed(MOVE_FLIGHT, 0.75f, true);
            me->SetSpeed(MOVE_RUN, 0.75f, true);
            float x = me->GetPositionX() + 20 * cos(me->GetOrientation());
            float y = me->GetPositionY() + 20 * sin(me->GetOrientation());
            float z = me->GetPositionZ() + 40;
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            // Despawn as soon as possible
            DespawnTimer = 4 * IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer > 0)
            {
                if (DespawnTimer > diff)
                    DespawnTimer -= diff;
                else
                    me->DespawnAfterAction();
                return;
            }
            CasterAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ebon_gargoyleAI(creature);
    }
};

enum Lightwell
{
	SPELL_LIGHTWELL_CHARGES = 59907,
};

class npc_lightwell : public CreatureScript
{
public:
	npc_lightwell() : CreatureScript("npc_lightwell") { }

	struct npc_lightwellAI : public PassiveAI
	{
		npc_lightwellAI(Creature* creature) : PassiveAI(creature)
		{
			DoCast(me, SPELL_LIGHTWELL_CHARGES, false);
		}

		void EnterEvadeMode()
		{
			if (!me->IsAlive())
				return;

			me->DeleteThreatList();
			me->CombatStop(true);
			me->ResetPlayerDamageReq();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_lightwellAI(creature);
	}
};

enum TrainingDummy
{
	NPC_TARGET_DUMMY           = 2673,
    NPC_ADVANCED_TARGET_DUMMY  = 2674,
};

class npc_training_dummy : public CreatureScript
{
public:
    npc_training_dummy() : CreatureScript("npc_training_dummy") { }

    struct npc_training_dummyAI : QuantumBasicAI
    {
        npc_training_dummyAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            Entry = creature->GetEntry();
        }

        uint32 Entry;
        uint32 ResetTimer;
        uint32 DespawnTimer;

        void Reset()
        {
            me->SetControlled(true, UNIT_STATE_STUNNED);

            ResetTimer = 5000;
            DespawnTimer = 15000;
        }

        void EnterEvadeMode()
        {
            if (!_EnterEvadeMode())
                return;

            Reset();
        }

        void DamageTaken(Unit* /*doneBy*/, uint32& damage)
        {
            ResetTimer = 5000;
            damage = 0;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (Entry != NPC_ADVANCED_TARGET_DUMMY && Entry != NPC_TARGET_DUMMY)
                return;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (!me->HasUnitState(UNIT_STATE_STUNNED))
                me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate

            if (Entry != NPC_ADVANCED_TARGET_DUMMY && Entry != NPC_TARGET_DUMMY)
            {
                if (ResetTimer <= diff)
                {
                    EnterEvadeMode();
                    ResetTimer = 5000;
                }
                else
                    ResetTimer -= diff;
                return;
            }
            else
            {
                if (DespawnTimer <= diff)
                    me->DespawnAfterAction();
                else
                    DespawnTimer -= diff;
            }
        }

		void MoveInLineOfSight(Unit* /*who*/)
		{
			return;
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_training_dummyAI(creature);
    }
};

enum Shadowfiend
{
	GLYPH_OF_SHADOWFIEND_MANA = 58227,
	GLYPH_OF_SHADOWFIEND      = 58228,
};

class npc_shadowfiend : public CreatureScript
{
public:
	npc_shadowfiend() : CreatureScript("npc_shadowfiend") { }

	struct npc_shadowfiendAI : public PetAI
	{
		npc_shadowfiendAI(Creature* creature) : PetAI(creature) {}

		void JustDied(Unit* /*killer*/)
		{
			if (me->IsSummon())
			{
				if (Unit* owner = me->ToTempSummon()->GetSummoner())
				{
					if (owner->HasAura(GLYPH_OF_SHADOWFIEND))
						owner->CastSpell(owner, GLYPH_OF_SHADOWFIEND_MANA, true);
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadowfiendAI(creature);
	}
};

#define GOSSIP_ENGINEERING1 "Borean Tundra."
#define GOSSIP_ENGINEERING2 "Howling Fjord."
#define GOSSIP_ENGINEERING3 "Sholazar Basin."
#define GOSSIP_ENGINEERING4 "Icecrown."
#define GOSSIP_ENGINEERING5 "Storm Peaks."
#define GOSSIP_ENGINEERING6 "The Underground."

enum Wormhole
{
    SPELL_HOWLING_FJORD       = 67838,
    SPELL_SHOLAZAR_BASIN      = 67835,
    SPELL_ICECROWN            = 67836,
    SPELL_STORM_PEAKS         = 67837,
    SPELL_UNDERGROUND         = 68081,

    TEXT_WORMHOLE             = 907,
    DATA_UNDERGROUND          = 1,
};

class npc_wormhole : public CreatureScript
{
    public:
        npc_wormhole() : CreatureScript("npc_wormhole") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player == creature->ToTempSummon()->GetSummoner())
            {
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

                if (creature->AI()->GetData(DATA_UNDERGROUND) == 1)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            }
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            bool roll = urand(0, 1);

            switch(action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1: // Borean Tundra
                    player->CLOSE_GOSSIP_MENU();
                    if (roll) // At the moment we don't have chance on spell_target_position table so we hack this
                        player->TeleportTo(571, 4305.505859f, 5450.839844f, 63.005806f, 0.627286f);
                    else
                        player->TeleportTo(571, 3201.936279f, 5630.123535f, 133.658798f, 3.855272f);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2: // Howling Fjord
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_HOWLING_FJORD, true);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3: // Sholazar Basin
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_SHOLAZAR_BASIN, true);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4: // Icecrown
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_ICECROWN, true);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5: // Storm peaks
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_STORM_PEAKS, true);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 6: // Underground
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_UNDERGROUND, true);
                    break;
            }
            return true;
        }

        struct npc_wormholeAI : PassiveAI
        {
            npc_wormholeAI(Creature* creature) : PassiveAI(creature)
            {
                random = urand(0, 9);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_UNDERGROUND)
                    return (random > 0) ? 0 : 1;
                return 0;
			}
		private:
			uint8 random;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_wormholeAI(creature);
        }
};

enum PetTrainer
{
    TEXT_ISHUNTER  = 5838,
    TEXT_NOTHUNTER = 5839,
    TEXT_PETINFO   = 13474,
    TEXT_CONFIRM   = 7722,
};

#define GOSSIP_PET1        "How do I train my pet?"
#define GOSSIP_PET2        "I wish to untrain my pet."
#define GOSSIP_PET_CONFIRM "Yes, please do."

class npc_pet_trainer : public CreatureScript
{
public:
    npc_pet_trainer() : CreatureScript("npc_pet_trainer") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetCurrentClass() == CLASS_HUNTER)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            player->PlayerTalkClass->SendGossipMenu(TEXT_ISHUNTER, creature->GetGUID());
            return true;
        }
        player->PlayerTalkClass->SendGossipMenu(TEXT_NOTHUNTER, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
				player->PlayerTalkClass->SendGossipMenu(TEXT_PETINFO, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET_CONFIRM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
				player->PlayerTalkClass->SendGossipMenu(TEXT_CONFIRM, creature->GetGUID());
			}
			break;
			case GOSSIP_ACTION_INFO_DEF + 3:
			{
				player->ResetPetTalents();
				player->CLOSE_GOSSIP_MENU();
			}
			break;
		}
        return true;
    }
};

enum LockSmith
{
    QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ = 10704,
    QUEST_DARK_IRON_LEGACY                = 3802,
    QUEST_THE_KEY_TO_SCHOLOMANCE_A        = 5505,
    QUEST_THE_KEY_TO_SCHOLOMANCE_H        = 5511,
    QUEST_HOTTER_THAN_HELL_A              = 10758,
    QUEST_HOTTER_THAN_HELL_H              = 10764,
    QUEST_RETURN_TO_KHAGDAR               = 9837,
    QUEST_CONTAINMENT                     = 13159,
    QUEST_ETERNAL_VIGILANCE               = 11011,
    QUEST_KEY_TO_THE_FOCUSING_IRIS        = 13372,
    QUEST_HC_KEY_TO_THE_FOCUSING_IRIS     = 13375,

    ITEM_ARCATRAZ_KEY                     = 31084,
    ITEM_SHADOWFORGE_KEY                  = 11000,
    ITEM_SKELETON_KEY                     = 13704,
    ITEM_SHATTERED_HALLS_KEY              = 28395,
    ITEM_THE_MASTERS_KEY                  = 24490,
    ITEM_VIOLET_HOLD_KEY                  = 42482,
    ITEM_ESSENCE_INFUSED_MOONSTONE        = 32449,
    ITEM_KEY_TO_THE_FOCUSING_IRIS         = 44582,
    ITEM_HC_KEY_TO_THE_FOCUSING_IRIS      = 44581,

    SPELL_ARCATRAZ_KEY                    = 54881,
    SPELL_SHADOWFORGE_KEY                 = 54882,
    SPELL_SKELETON_KEY                    = 54883,
    SPELL_SHATTERED_HALLS_KEY             = 54884,
    SPELL_THE_MASTERS_KEY                 = 54885,
    SPELL_VIOLET_HOLD_KEY                 = 67253,
    SPELL_ESSENCE_INFUSED_MOONSTONE       = 40173,
};

#define GOSSIP_LOST_ARCATRAZ_KEY                "I've lost my key to the Arcatraz."
#define GOSSIP_LOST_SHADOWFORGE_KEY             "I've lost my key to the Blackrock Depths."
#define GOSSIP_LOST_SKELETON_KEY                "I've lost my key to the Scholomance."
#define GOSSIP_LOST_SHATTERED_HALLS_KEY         "I've lost my key to the Shattered Halls."
#define GOSSIP_LOST_THE_MASTERS_KEY             "I've lost my key to the Karazhan."
#define GOSSIP_LOST_VIOLET_HOLD_KEY             "I've lost my key to the Violet Hold."
#define GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE   "I've lost my Essence-Infused Moonstone."
#define GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS    "I've lost my Key to the Focusing Iris."
#define GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS "I've lost my Heroic Key to the Focusing Iris."

class npc_locksmith : public CreatureScript
{
public:
    npc_locksmith() : CreatureScript("npc_locksmith") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        // Arcatraz Key
        if (player->GetQuestRewardStatus(QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ) && !player->HasItemCount(ITEM_ARCATRAZ_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ARCATRAZ_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        // Shadowforge Key
        if (player->GetQuestRewardStatus(QUEST_DARK_IRON_LEGACY) && !player->HasItemCount(ITEM_SHADOWFORGE_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHADOWFORGE_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        // Skeleton Key
        if ((player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_A) || player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_H)) &&
            !player->HasItemCount(ITEM_SKELETON_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SKELETON_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        // Shatered Halls Key
        if ((player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_A) || player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_H)) &&
            !player->HasItemCount(ITEM_SHATTERED_HALLS_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHATTERED_HALLS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

        // Master's Key
        if (player->GetQuestRewardStatus(QUEST_RETURN_TO_KHAGDAR) && !player->HasItemCount(ITEM_THE_MASTERS_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_THE_MASTERS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

        // Violet Hold Key
        if (player->GetQuestRewardStatus(QUEST_CONTAINMENT) && !player->HasItemCount(ITEM_VIOLET_HOLD_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_VIOLET_HOLD_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

        // Essence-Infused Moonstone
        if (player->GetQuestRewardStatus(QUEST_ETERNAL_VIGILANCE) && !player->HasItemCount(ITEM_ESSENCE_INFUSED_MOONSTONE, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

        // Key to the Focusing Iris
        if (player->GetQuestRewardStatus(QUEST_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_KEY_TO_THE_FOCUSING_IRIS, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

        // Heroic Key to the Focusing Iris
        if (player->GetQuestRewardStatus(QUEST_HC_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_ARCATRAZ_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SHADOWFORGE_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SKELETON_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SHATTERED_HALLS_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_THE_MASTERS_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_VIOLET_HOLD_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_ESSENCE_INFUSED_MOONSTONE, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 8:
                player->CLOSE_GOSSIP_MENU();
                player->AddItem(ITEM_KEY_TO_THE_FOCUSING_IRIS,1);
                break;
            case GOSSIP_ACTION_INFO_DEF + 9:
                player->CLOSE_GOSSIP_MENU();
                player->AddItem(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS,1);
                break;
        }
        return true;
    }
};

enum TabardVendorQuests
{
	QUEST_TRUE_MASTERS_OF_LIGHT                = 9737,
    QUEST_THE_UNWRITTEN_PROPHECY               = 9762,
    QUEST_INTO_THE_BREACH                      = 10259,
    QUEST_BATTLE_OF_THE_CRIMSON_WATCH          = 10781,
    QUEST_SHARDS_OF_AHUNE                      = 11972,

    ACHIEVEMENT_EXPLORE_NORTHREND              = 45,
    ACHIEVEMENT_TWENTYFIVE_TABARDS             = 1021,
    ACHIEVEMENT_THE_LOREMASTER_A               = 1681,
    ACHIEVEMENT_THE_LOREMASTER_H               = 1682,

    ITEM_TABARD_OF_THE_HAND                    = 24344,
    ITEM_TABARD_OF_THE_BLOOD_KNIGHT            = 25549,
    ITEM_TABARD_OF_THE_PROTECTOR               = 28788,
    ITEM_OFFERING_OF_THE_SHATAR                = 31408,
    ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI   = 31404,
    ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI  = 31405,
    ITEM_TABARD_OF_THE_SUMMER_SKIES            = 35279,
    ITEM_TABARD_OF_THE_SUMMER_FLAMES           = 35280,
    ITEM_TABARD_OF_THE_ACHIEVER                = 40643,
    ITEM_LOREMASTERS_COLORS                    = 43300,
    ITEM_TABARD_OF_THE_EXPLORER                = 43348,

    SPELL_TABARD_OF_THE_BLOOD_KNIGHT           = 54974,
    SPELL_TABARD_OF_THE_HAND                   = 54976,
    SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI  = 54977,
    SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI = 54982,
    SPELL_TABARD_OF_THE_ACHIEVER               = 55006,
    SPELL_TABARD_OF_THE_PROTECTOR              = 55008,
    SPELL_LOREMASTERS_COLORS                   = 58194,
    SPELL_TABARD_OF_THE_EXPLORER               = 58224,
    SPELL_TABARD_OF_SUMMER_SKIES               = 62768,
    SPELL_TABARD_OF_SUMMER_FLAMES              = 62769,
};

#define GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT "I've lost my Tabard of Blood Knight."
#define GOSSIP_LOST_TABARD_OF_THE_HAND "I've lost my Tabard of the Hand."
#define GOSSIP_LOST_TABARD_OF_THE_PROTECTOR "I've lost my Tabard of the Protector."
#define GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI "I've lost my Green Trophy Tabard of the Illidari."
#define GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI "I've lost my Purple Trophy Tabard of the Illidari."
#define GOSSIP_LOST_TABARD_OF_SUMMER_SKIES "I've lost my Tabard of Summer Skies."
#define GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES "I've lost my Tabard of Summer Flames."
#define GOSSIP_LOST_LOREMASTERS_COLORS "I've lost my Loremaster's Colors."
#define GOSSIP_LOST_TABARD_OF_THE_EXPLORER "I've lost my Tabard of the Explorer."
#define GOSSIP_LOST_TABARD_OF_THE_ACHIEVER "I've lost my Tabard of the Achiever."

class npc_tabard_vendor : public CreatureScript
{
public:
    npc_tabard_vendor() : CreatureScript("npc_tabard_vendor") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        bool LostBloodKnight = false;
        bool LostHand = false;
        bool LostProtector = false;
        bool LostIllidari = false;
        bool LostSummer = false;
        bool LostExplorer = false;

        // Tabard of the Blood Knight
        if (player->GetQuestRewardStatus(QUEST_TRUE_MASTERS_OF_LIGHT) && !player->HasItemCount(ITEM_TABARD_OF_THE_BLOOD_KNIGHT, 1, true))
            LostBloodKnight = true;
        // Tabard of the Hand
        if (player->GetQuestRewardStatus(QUEST_THE_UNWRITTEN_PROPHECY) && !player->HasItemCount(ITEM_TABARD_OF_THE_HAND, 1, true))
            LostHand = true;
        // Tabard of the Protector
        if (player->GetQuestRewardStatus(QUEST_INTO_THE_BREACH) && !player->HasItemCount(ITEM_TABARD_OF_THE_PROTECTOR, 1, true))
            LostProtector = true;
        // Green Trophy Tabard of the Illidari
        // Purple Trophy Tabard of the Illidari
        if (player->GetQuestRewardStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) &&
            (!player->HasItemCount(ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
            !player->HasItemCount(ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
            !player->HasItemCount(ITEM_OFFERING_OF_THE_SHATAR, 1, true)))
            LostIllidari = true;
        // Tabard of Summer Skies
        // Tabard of Summer Flames
        if (player->GetQuestRewardStatus(QUEST_SHARDS_OF_AHUNE) &&
            !player->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_SKIES, 1, true) &&
            !player->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_FLAMES, 1, true))
            LostSummer = true;
        if (player->HasAchieved(ACHIEVEMENT_EXPLORE_NORTHREND) &&
            !player->HasItemCount(ITEM_TABARD_OF_THE_EXPLORER, 1, true))
            LostExplorer = true;

        if (LostBloodKnight || LostHand || LostProtector || LostIllidari || LostSummer || LostExplorer)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

            if (LostBloodKnight)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (LostHand)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_HAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            if (LostProtector)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_PROTECTOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

            if (LostIllidari)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            }

            if (LostSummer)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_SKIES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            }

            if (LostExplorer)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_EXPLORER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        else
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_TABARD_OF_THE_BLOOD_KNIGHT, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_TABARD_OF_THE_HAND, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_TABARD_OF_THE_PROTECTOR, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_TABARD_OF_SUMMER_SKIES, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_TABARD_OF_SUMMER_FLAMES, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 8:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_TABARD_OF_THE_EXPLORER, false);
                break;
        }
        return true;
    }
};

#define GOSSIP_XP_OFF "I no longer wish to gain experience."
#define GOSSIP_XP_ON  "I wish to start gaining experience again."

enum Experience
{
	EXP_COST        = 100000,
	GOSSIP_TEXT_EXP = 14736,
};

class npc_experience : public CreatureScript
{
public:
    npc_experience() : CreatureScript("npc_experience") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_OFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_EXP, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        bool noXPGain = player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        bool doSwitch = false;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
			{
				if (!noXPGain)
					doSwitch = true;
			}
			break;
			case GOSSIP_ACTION_INFO_DEF + 2:
			{
				if (noXPGain)
					doSwitch = true;
			}
			break;
        }
        if (doSwitch)
        {
            if (!player->HasEnoughMoney(EXP_COST))
                player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
            else if (noXPGain)
            {
                player->ModifyMoney(-EXP_COST);
                player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
            else if (!noXPGain)
            {
                player->ModifyMoney(-EXP_COST);
                player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
        }
        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }
};

enum DuthorianRall
{
	QUEST_TOME_OF_THE_DIVINITY_1 = 2998,
	QUEST_TOME_OF_THE_DIVINITY_2 = 1642,
	QUEST_TOME_OF_THE_DIVINITY_3 = 2997,
	QUEST_TOME_OF_THE_DIVINITY_4 = 1646,
	ITEM_TOME_OF_DIVINITY        = 6775,
};

class npc_duthorian_rall : public CreatureScript
{
public:
    npc_duthorian_rall() : CreatureScript("npc_duthorian_rall") { }

    bool OnGossipHello (Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

        if ((player->GetQuestStatus(QUEST_TOME_OF_THE_DIVINITY_1) == QUEST_STATUS_COMPLETE && player->GetQuestStatus(QUEST_TOME_OF_THE_DIVINITY_2) == QUEST_STATUS_INCOMPLETE)
			|| (player->GetQuestStatus(QUEST_TOME_OF_THE_DIVINITY_3) == QUEST_STATUS_COMPLETE && player->GetQuestStatus(QUEST_TOME_OF_THE_DIVINITY_4) == QUEST_STATUS_INCOMPLETE))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I need a new tome!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect (Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		switch (action)
		{
        case GOSSIP_ACTION_INFO_DEF + 1:
            ItemPosCountVec dest;
            uint8 canStoreNewItem = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_TOME_OF_DIVINITY, 1);
            if (canStoreNewItem == EQUIP_ERR_OK)
			{
				Item *newItem = NULL;
                newItem = player->StoreNewItem(dest, ITEM_TOME_OF_DIVINITY, 1, true);
                player->SendNewItem(newItem, 1, true, false);
            }
			else
                creature->MonsterSay("Sorry, you do not fit in your inventory!", LANG_UNIVERSAL, player->GetGUID());
            break;
        }
        return true;
    }
};

enum Brewfest
{
    SPELL_APPLE_TRAP             = 43450,
    SPELL_RACING_RAM             = 42146,
    SPELL_RAM_FATIGUE            = 43052,
    SPELL_CREATE_KEG             = 42414,
    SPELL_HAS_KEG                = 44066,
    SPELL_THROW_KEG              = 43660,
    SPELL_THROW_KEG_PLAYER       = 43662,
    SPELL_WORKING_FOR_THE_MAN    = 43534,
    SPELL_RELAY_RACE_DEBUFF      = 44689,
    SPELL_RENTAL_RACING_RAM      = 43883,
    SPELL_CREATE_TICKETS         = 44501,

    QUEST_THERE_AND_BACK_AGAIN_A = 11122,
    QUEST_THERE_AND_BACK_AGAIN_H = 11412,
    QUEST_BARK_FOR_THE_1         = 11293,
    QUEST_BARK_FOR_THE_2         = 11294,
    QUEST_BARK_FOR_THE_3         = 11407,
    QUEST_BARK_FOR_THE_4         = 11408,

    ITEM_PORTABLE_BREWFEST_KEG   = 33797,

    NPC_DELIVERY_CREDIT          = 24337, // TODO: use spell
    NPC_FLYNN_FIREBREW           = 24364,
    NPC_BOK_DROPCERTAIN          = 24527,
    NPC_RAM_MASTER_RAY           = 24497,
    NPC_NEILL_RAMSTEIN           = 23558,
	
	ACHIEVEMENT_BREW_OF_THE_MONTH  = 2796,
};

class npc_apple_trap_bunny : public CreatureScript
{
    public:
        npc_apple_trap_bunny() : CreatureScript("npc_apple_trap_bunny") { }

        struct npc_apple_trap_bunnyAI : public QuantumBasicAI
        {
            npc_apple_trap_bunnyAI(Creature* creature) : QuantumBasicAI(creature) { }

            void MoveInLineOfSight(Unit* who)
            {
                if (who && who->ToPlayer() && who->HasAura(SPELL_RACING_RAM) && !who->HasAura(SPELL_APPLE_TRAP) && me->GetDistance(who) < 9.0f)
                {
                    who->RemoveAurasDueToSpell(SPELL_RAM_FATIGUE);
                    who->CastSpell(who, SPELL_APPLE_TRAP, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_apple_trap_bunnyAI(creature);
        }
};

class npc_keg_delivery : public CreatureScript
{
    public:
        npc_keg_delivery() : CreatureScript("npc_keg_delivery") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if ((player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_A) ||
                player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_H)) && !player->HasAura(SPELL_RELAY_RACE_DEBUFF))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gibt es noch mehr zu tun?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->CastSpell(player, SPELL_RENTAL_RACING_RAM, true);
                player->CastSpell(player, SPELL_WORKING_FOR_THE_MAN, true);
                creature->AddAura(SPELL_RELAY_RACE_DEBUFF, player);
                player->CLOSE_GOSSIP_MENU();
            }
            return true;
        }

        struct npc_keg_deliveryAI : public QuantumBasicAI
        {
            npc_keg_deliveryAI(Creature* creature) : QuantumBasicAI(creature) { }

            void MoveInLineOfSight(Unit* who)
            {
				if (who && who->ToPlayer() && who->HasAura(SPELL_RACING_RAM) && me->GetDistance(who) < 15.0f &&
					(who->ToPlayer()->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_A) == QUEST_STATUS_INCOMPLETE ||
					who->ToPlayer()->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_H) == QUEST_STATUS_INCOMPLETE ||
					who->HasAura(SPELL_WORKING_FOR_THE_MAN)))
				{
					switch (me->GetEntry())
					{
                        case NPC_FLYNN_FIREBREW:
                        case NPC_BOK_DROPCERTAIN:
                            if (!who->HasAura(SPELL_HAS_KEG))
                            {
                                me->CastSpell(who, SPELL_CREATE_KEG, true);
                                me->CastSpell(who, SPELL_THROW_KEG, true); // visual
                            }
                            break;
                        case NPC_RAM_MASTER_RAY:
                        case NPC_NEILL_RAMSTEIN:
                            if (who->HasAura(SPELL_HAS_KEG))
                            {
                                who->CastSpell(me, SPELL_THROW_KEG_PLAYER, true);
                                who->ToPlayer()->DestroyItemCount(ITEM_PORTABLE_BREWFEST_KEG, 1, true);

                                // rewards
                                if (!who->HasAura(SPELL_WORKING_FOR_THE_MAN))
                                    who->ToPlayer()->KilledMonsterCredit(NPC_DELIVERY_CREDIT, 0);
                                else
                                {
                                    // give 2 tickets
                                    who->CastSpell(who, SPELL_CREATE_TICKETS, true);

                                    // plus 30s ram duration
                                    if (Aura* aura = who->GetAura(SPELL_RENTAL_RACING_RAM))
                                        aura->SetDuration(aura->GetDuration() + 30*IN_MILLISECONDS);
                                }
                            }
							break;
					}
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_keg_deliveryAI(creature);
        }
};

class npc_bark_bunny : public CreatureScript
{
    public:
        npc_bark_bunny() : CreatureScript("npc_bark_bunny") { }

        struct npc_bark_bunnyAI : public QuantumBasicAI
        {
            npc_bark_bunnyAI(Creature* creature) : QuantumBasicAI(creature) { }

            void MoveInLineOfSight(Unit* who)
            {
                if (who && who->ToPlayer() && who->HasAura(SPELL_RACING_RAM) && me->GetDistance(who) < 20.0f &&
                   (who->ToPlayer()->GetQuestStatus(QUEST_BARK_FOR_THE_1) == QUEST_STATUS_INCOMPLETE ||
                    who->ToPlayer()->GetQuestStatus(QUEST_BARK_FOR_THE_2) == QUEST_STATUS_INCOMPLETE ||
                    who->ToPlayer()->GetQuestStatus(QUEST_BARK_FOR_THE_3) == QUEST_STATUS_INCOMPLETE ||
					who->ToPlayer()->GetQuestStatus(QUEST_BARK_FOR_THE_4) == QUEST_STATUS_INCOMPLETE))
				{
					who->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_bark_bunnyAI(creature);
        }
};

class npc_brew_vendor : public CreatureScript
{
public:
	npc_brew_vendor() : CreatureScript("npc_brew_vendor") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->HasAchieved(ACHIEVEMENT_BREW_OF_THE_MONTH))
			player->GetSession()->SendListInventory(creature->GetGUID());
		else
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

		return true;
	}
};

enum DarkIronAttack
{
    GO_FESTIVE_KEG             = 186183, // .. 186187
    GO_MOLE_MACHINE_WRECKAGE_A = 189989,
    GO_MOLE_MACHINE_WRECKAGE_H = 189990,

    NPC_DARK_IRON_GUZZLER      = 23709,
    NPC_DARK_IRON_HERALD       = 24536,

    SPELL_BREWFEST_STUN        = 42436,
    SPELL_MOLE_MACHINE_SPAWN   = 43563
};

class npc_dark_iron_herald : public CreatureScript
{
    public:
        npc_dark_iron_herald() : CreatureScript("npc_dark_iron_herald") { }

        struct npc_dark_iron_heraldAI : public QuantumBasicAI
        {
            npc_dark_iron_heraldAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
            {
                me->SetActive(true);

                if (me->IsDead())
                    me->Respawn();
            }

			SummonList summons;
            uint32 EventTimer;
            uint32 SpawnTimer;

            void Reset()
            {
                EventTimer = 5*MINUTE*IN_MILLISECONDS;
                SpawnTimer = 15*IN_MILLISECONDS;
            }

            void ResetKegs()
            {
                for (uint32 i = GO_FESTIVE_KEG; i < GO_FESTIVE_KEG+5; ++i)
                {
                    GameObject* keg = me->FindGameobjectWithDistance(i, 100.0f);
                    if (keg && keg->GetGoState() == GO_STATE_ACTIVE)
                        keg->SetGoState(GO_STATE_READY);
                }
            }

            GameObject* GetKeg() const
            {
                std::list<GameObject*> tempList;

                // get all valid near kegs
                for (uint32 i = GO_FESTIVE_KEG; i < GO_FESTIVE_KEG+5; ++i)
                {
                    GameObject* keg = me->FindGameobjectWithDistance(i, 100.0f);
                    if (keg && keg->GetGoState() != GO_STATE_ACTIVE)
                        tempList.push_back(keg);
                }

                // select a random one
                if (!tempList.empty())
                {
                    std::list<GameObject*>::iterator itr = tempList.begin();
                    std::advance(itr, urand(0, tempList.size() - 1));
                    if (GameObject* keg = *itr)
                        return keg;
                }
                return NULL;
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            void UpdateAI(uint32 const diff)
            {
                if (EventTimer <= diff)
                {
                    float x, y, z;
                    me->GetPosition(x, y, z);
                    uint32 area = me->GetAreaId();
                    me->SummonGameObject((area == 1) ? GO_MOLE_MACHINE_WRECKAGE_A : GO_MOLE_MACHINE_WRECKAGE_H, x, y, z, 0, 0, 0, 0, 0, 90);

                    summons.DespawnAll();
                    ResetKegs();
                    me->DisappearAndDie();
                    return;
                }
                else EventTimer -= diff;

                if (SpawnTimer <= diff)
                {
                    Position spawn;
                    me->GetRandomNearPosition(spawn, 20.0f);

                    if (Creature* guzzler = me->SummonCreature(NPC_DARK_IRON_GUZZLER, spawn))
                    {
                        guzzler->SetReactState(REACT_PASSIVE);
                        guzzler->SetWalk(true);
                        guzzler->CastSpell(guzzler, SPELL_MOLE_MACHINE_SPAWN, true);
                        guzzler->SetVisible(false);

                        if (GameObject* keg = GetKeg())
                        {
                            Position pos;
                            keg->GetNearPosition(pos, 3.0f, keg->GetAngle(me->GetPositionX(), me->GetPositionZ()) - float(M_PI*rand_norm()));
                            guzzler->GetMotionMaster()->MovePoint(1, pos);
                            guzzler->AI()->SetGUID(keg->GetGUID());
                        }
                        else
                        {
                            summons.DespawnAll();
                            ResetKegs();
                            me->DisappearAndDie();
                        }
                    }
                    SpawnTimer = urand(1, 4)*IN_MILLISECONDS;
                }
                else SpawnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_dark_iron_heraldAI(creature);
        }
};

class npc_dark_iron_guzzler : public CreatureScript
{
    public:
        npc_dark_iron_guzzler() : CreatureScript("npc_dark_iron_guzzler") { }

        struct npc_dark_iron_guzzlerAI : public QuantumBasicAI
        {
            npc_dark_iron_guzzlerAI(Creature* creature) : QuantumBasicAI(creature) { }

			uint64 KegGUID;
            uint32 DestroyTimer;
            uint32 WaitTimer;
            bool KegReached;
            bool Waiting;

            void Reset()
            {
                KegGUID = 0;
                WaitTimer = 2*IN_MILLISECONDS;
                DestroyTimer = 20*IN_MILLISECONDS;
                KegReached = false;
                Waiting = true;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_BREWFEST_STUN)
                {
                    me->GetMotionMaster()->Clear();
                    me->Kill(me);
                    me->DespawnAfterAction(10*IN_MILLISECONDS);
                    KegReached = false;
                }
            }

            void SetGUID(uint64 guid, int32 /*id*/ = 0)
            {
                KegGUID = guid;
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (GameObject* keg = ObjectAccessor::GetGameObject(*me, KegGUID))
                {
                    KegReached = true;
                    me->SetFacingToObject(keg);
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_ATTACK_2H_LOOSE);
                }
                else me->DespawnAfterAction();
            }

            void UpdateAI(uint32 const diff)
            {
                if (Waiting)
                {
                    if (WaitTimer <= diff)
                    {
                        Waiting = false;
                        me->SetVisible(true);
                    }
                    else  WaitTimer -= diff;
                }
                if (KegReached)
                {
                    GameObject* keg = ObjectAccessor::GetGameObject(*me, KegGUID);
                    if (!keg || (keg && keg->GetGoState() == GO_STATE_ACTIVE))
                    {
                        me->DespawnAfterAction();
                        KegReached = false;
                        return;
                    }

                    if (DestroyTimer <= diff)
                    {
                        keg->SetGoState(GO_STATE_ACTIVE);
                        me->DespawnAfterAction();
                        KegReached = false;
                    }
                    else DestroyTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_dark_iron_guzzlerAI(creature);
        }
};

enum WildTurkey
{
    SPELL_TURKEY_TRACKER = 62014,
};

class npc_wild_turkey : public CreatureScript
{
public:
	npc_wild_turkey() : CreatureScript("npc_wild_turkey") { }

	struct npc_wild_turkeyAI : public CritterAI
	{
		npc_wild_turkeyAI(Creature* creature) : CritterAI(creature) { }

		void JustDied(Unit* killer)
		{
			if (killer && killer->GetTypeId() == TYPE_ID_PLAYER)
				killer->CastSpell(killer, SPELL_TURKEY_TRACKER, true);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_wild_turkeyAI(creature);
	}
};

enum FireElementalSpells
{
	SPELL_FIRENOVA    = 12470,
	SPELL_FIRE_SHIELD = 13376,
	SPELL_FIREBLAST   = 57984,
};

class npc_fire_elemental : public CreatureScript
{
public:
    npc_fire_elemental() : CreatureScript("npc_fire_elemental") { }

    struct npc_fire_elementalAI : public QuantumBasicAI
    {
        npc_fire_elementalAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FireNovaTimer;
        uint32 FireShieldTimer;
        uint32 FireBlastTimer;

        void Reset()
        {
            FireNovaTimer = 5000 + rand() % 15000; // 5-20 sec cd
            FireBlastTimer = 5000 + rand() % 15000; // 5-20 sec cd
            FireShieldTimer = 0;
            me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (FireShieldTimer <= diff)
            {
                DoCastVictim(SPELL_FIRE_SHIELD);
                FireShieldTimer = 2 * IN_MILLISECONDS;
            }
            else FireShieldTimer -= diff;

            if (FireBlastTimer <= diff)
            {
                DoCastVictim(SPELL_FIREBLAST);
                FireBlastTimer = 5000 + rand() % 15000; // 5-20 sec cd
            }
            else FireBlastTimer -= diff;

            if (FireNovaTimer <= diff)
            {
                DoCastVictim(SPELL_FIRENOVA);
                FireNovaTimer = 5000 + rand() % 15000; // 5-20 sec cd
            }
            else FireNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fire_elementalAI(creature);
    }
};

enum EarthElementalSpells
{
	SPELL_ANGERED_EARTH = 36213,
};

class npc_earth_elemental : public CreatureScript
{
public:
    npc_earth_elemental() : CreatureScript("npc_earth_elemental") { }

    struct npc_earth_elementalAI : public QuantumBasicAI
    {
        npc_earth_elementalAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 AngeredEarthTimer;

        void Reset()
        {
            AngeredEarthTimer = 0;
            me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (AngeredEarthTimer <= diff)
            {
                DoCastVictim(SPELL_ANGERED_EARTH);
                AngeredEarthTimer = 5000 + rand() % 15000; // 5-20 sec cd
            }
            else AngeredEarthTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_earth_elementalAI(creature);
    }
};

enum FireworkSpells
{
    NPC_OMEN                = 15467,
    NPC_MINION_OF_OMEN      = 15466,
    NPC_FIREWORK_BLUE       = 15879,
    NPC_FIREWORK_GREEN      = 15880,
    NPC_FIREWORK_PURPLE     = 15881,
    NPC_FIREWORK_RED        = 15882,
    NPC_FIREWORK_YELLOW     = 15883,
    NPC_FIREWORK_WHITE      = 15884,
    NPC_FIREWORK_BIG_BLUE   = 15885,
    NPC_FIREWORK_BIG_GREEN  = 15886,
    NPC_FIREWORK_BIG_PURPLE = 15887,
    NPC_FIREWORK_BIG_RED    = 15888,
    NPC_FIREWORK_BIG_YELLOW = 15889,
    NPC_FIREWORK_BIG_WHITE  = 15890,

    NPC_CLUSTER_BLUE        = 15872,
    NPC_CLUSTER_RED         = 15873,
    NPC_CLUSTER_GREEN       = 15874,
    NPC_CLUSTER_PURPLE      = 15875,
    NPC_CLUSTER_WHITE       = 15876,
    NPC_CLUSTER_YELLOW      = 15877,
    NPC_CLUSTER_BIG_BLUE    = 15911,
    NPC_CLUSTER_BIG_GREEN   = 15912,
    NPC_CLUSTER_BIG_PURPLE  = 15913,
    NPC_CLUSTER_BIG_RED     = 15914,
    NPC_CLUSTER_BIG_WHITE   = 15915,
    NPC_CLUSTER_BIG_YELLOW  = 15916,
    NPC_CLUSTER_ELUNE       = 15918,

    GO_FIREWORK_LAUNCHER_1  = 180771,
    GO_FIREWORK_LAUNCHER_2  = 180868,
    GO_FIREWORK_LAUNCHER_3  = 180850,
    GO_CLUSTER_LAUNCHER_1   = 180772,
    GO_CLUSTER_LAUNCHER_2   = 180859,
    GO_CLUSTER_LAUNCHER_3   = 180869,
    GO_CLUSTER_LAUNCHER_4   = 180874,

    SPELL_ROCKET_BLUE       = 26344,
    SPELL_ROCKET_GREEN      = 26345,
    SPELL_ROCKET_PURPLE     = 26346,
    SPELL_ROCKET_RED        = 26347,
    SPELL_ROCKET_WHITE      = 26348,
    SPELL_ROCKET_YELLOW     = 26349,
    SPELL_ROCKET_BIG_BLUE   = 26351,
    SPELL_ROCKET_BIG_GREEN  = 26352,
    SPELL_ROCKET_BIG_PURPLE = 26353,
    SPELL_ROCKET_BIG_RED    = 26354,
    SPELL_ROCKET_BIG_WHITE  = 26355,
    SPELL_ROCKET_BIG_YELLOW = 26356,
    SPELL_LUNAR_FORTUNE     = 26522,

    ANIM_GO_LAUNCH_FIREWORK = 3,
    ZONE_MOONGLADE          = 493,
};

Position omenSummonPos = {7558.993f, -2839.999f, 450.0214f, 4.46f};

class npc_firework : public CreatureScript
{
public:
    npc_firework() : CreatureScript("npc_firework") { }

    struct npc_fireworkAI : public QuantumBasicAI
    {
        npc_fireworkAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool isCluster()
        {
            switch (me->GetEntry())
            {
                case NPC_FIREWORK_BLUE:
                case NPC_FIREWORK_GREEN:
                case NPC_FIREWORK_PURPLE:
                case NPC_FIREWORK_RED:
                case NPC_FIREWORK_YELLOW:
                case NPC_FIREWORK_WHITE:
                case NPC_FIREWORK_BIG_BLUE:
                case NPC_FIREWORK_BIG_GREEN:
                case NPC_FIREWORK_BIG_PURPLE:
                case NPC_FIREWORK_BIG_RED:
                case NPC_FIREWORK_BIG_YELLOW:
                case NPC_FIREWORK_BIG_WHITE:
					return false;
                case NPC_CLUSTER_BLUE:
                case NPC_CLUSTER_GREEN:
                case NPC_CLUSTER_PURPLE:
                case NPC_CLUSTER_RED:
                case NPC_CLUSTER_YELLOW:
                case NPC_CLUSTER_WHITE:
                case NPC_CLUSTER_BIG_BLUE:
                case NPC_CLUSTER_BIG_GREEN:
                case NPC_CLUSTER_BIG_PURPLE:
                case NPC_CLUSTER_BIG_RED:
                case NPC_CLUSTER_BIG_YELLOW:
                case NPC_CLUSTER_BIG_WHITE:
                case NPC_CLUSTER_ELUNE:
                default:
                    return true;
            }
        }

        GameObject* FindNearestLauncher()
        {
            GameObject* launcher = NULL;

            if (isCluster())
            {
                GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_1, 0.5f);
                GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_2, 0.5f);
                GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_3, 0.5f);
                GameObject* launcher4 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_4, 0.5f);

                if (launcher1)
                    launcher = launcher1;
                else if (launcher2)
                    launcher = launcher2;
                else if (launcher3)
                    launcher = launcher3;
                else if (launcher4)
                    launcher = launcher4;
            }
            else
            {
                GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_1, 0.5f);
                GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_2, 0.5f);
                GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_3, 0.5f);

                if (launcher1)
                    launcher = launcher1;
                else if (launcher2)
                    launcher = launcher2;
                else if (launcher3)
                    launcher = launcher3;
            }

            return launcher;
        }

        uint32 GetFireworkSpell(uint32 entry)
        {
            switch (entry)
            {
                case NPC_FIREWORK_BLUE:
                    return SPELL_ROCKET_BLUE;
                case NPC_FIREWORK_GREEN:
                    return SPELL_ROCKET_GREEN;
                case NPC_FIREWORK_PURPLE:
                    return SPELL_ROCKET_PURPLE;
                case NPC_FIREWORK_RED:
                    return SPELL_ROCKET_RED;
                case NPC_FIREWORK_YELLOW:
                    return SPELL_ROCKET_YELLOW;
                case NPC_FIREWORK_WHITE:
                    return SPELL_ROCKET_WHITE;
                case NPC_FIREWORK_BIG_BLUE:
                    return SPELL_ROCKET_BIG_BLUE;
                case NPC_FIREWORK_BIG_GREEN:
                    return SPELL_ROCKET_BIG_GREEN;
                case NPC_FIREWORK_BIG_PURPLE:
                    return SPELL_ROCKET_BIG_PURPLE;
                case NPC_FIREWORK_BIG_RED:
                    return SPELL_ROCKET_BIG_RED;
                case NPC_FIREWORK_BIG_YELLOW:
                    return SPELL_ROCKET_BIG_YELLOW;
                case NPC_FIREWORK_BIG_WHITE:
                    return SPELL_ROCKET_BIG_WHITE;
                default:
                    return 0;
            }
        }

        uint32 GetFireworkGameObjectId()
        {
            uint32 spellId = 0;

            switch (me->GetEntry())
            {
                case NPC_CLUSTER_BLUE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BLUE);
                    break;
                case NPC_CLUSTER_GREEN:
                    spellId = GetFireworkSpell(NPC_FIREWORK_GREEN);
                    break;
                case NPC_CLUSTER_PURPLE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_PURPLE);
                    break;
                case NPC_CLUSTER_RED:
                    spellId = GetFireworkSpell(NPC_FIREWORK_RED);
                    break;
                case NPC_CLUSTER_YELLOW:
                    spellId = GetFireworkSpell(NPC_FIREWORK_YELLOW);
                    break;
                case NPC_CLUSTER_WHITE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_WHITE);
                    break;
                case NPC_CLUSTER_BIG_BLUE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_BLUE);
                    break;
                case NPC_CLUSTER_BIG_GREEN:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_GREEN);
                    break;
                case NPC_CLUSTER_BIG_PURPLE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_PURPLE);
                    break;
                case NPC_CLUSTER_BIG_RED:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_RED);
                    break;
                case NPC_CLUSTER_BIG_YELLOW:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_YELLOW);
                    break;
                case NPC_CLUSTER_BIG_WHITE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_WHITE);
                    break;
                case NPC_CLUSTER_ELUNE:
                    spellId = GetFireworkSpell(urand(NPC_FIREWORK_BLUE, NPC_FIREWORK_WHITE));
                    break;
            }

            const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);

            if (spellInfo && spellInfo->Effects[0].Effect == SPELL_EFFECT_SUMMON_OBJECT_WILD)
                return spellInfo->Effects[0].MiscValue;

            return 0;
        }

        void Reset()
        {
            if (GameObject* launcher = FindNearestLauncher())
            {
                launcher->SendCustomAnim(ANIM_GO_LAUNCH_FIREWORK);
                me->SetOrientation(launcher->GetOrientation() + M_PI/2);
            }
            else
                return;

            if (isCluster())
            {
                // Check if we are near Elune'ara lake south, if so try to summon Omen or a minion
                if (me->GetZoneId() == ZONE_MOONGLADE)
                {
                    if (!me->FindCreatureWithDistance(NPC_OMEN, 100.0f, false) && me->GetDistance2d(omenSummonPos.GetPositionX(), omenSummonPos.GetPositionY()) <= 100.0f)
                    {
                        switch (urand(0,9))
                        {
                            case 0:
                            case 1:
                            case 2:
                            case 3:
                                if (Creature* minion = me->SummonCreature(NPC_MINION_OF_OMEN, me->GetPositionX()+frand(-5.0f, 5.0f), me->GetPositionY()+frand(-5.0f, 5.0f), me->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
								{
                                    minion->AI()->AttackStart(me->SelectNearestPlayer(20.0f));
								}
                                break;
                            case 9:
                                me->SummonCreature(NPC_OMEN, omenSummonPos);
                                break;
                        }
                    }
                }
                if (me->GetEntry() == NPC_CLUSTER_ELUNE)
                    DoCast(SPELL_LUNAR_FORTUNE);

                float displacement = 0.7f;
                for (uint8 i = 0; i < 4; i++)
                    me->SummonGameObject(GetFireworkGameObjectId(), me->GetPositionX() + (i%2 == 0 ? displacement : -displacement), me->GetPositionY() + (i > 1 ? displacement : -displacement), me->GetPositionZ() + 4.0f, me->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 1);
            }
            else me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), GetFireworkSpell(me->GetEntry()), true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fireworkAI(creature);
    }
};

enum RabbitSpells
{
    SPELL_SPRING_FLING          = 61875,
    SPELL_SPRING_RABBIT_JUMP    = 61724,
    SPELL_SPRING_RABBIT_WANDER  = 61726,
    SPELL_SUMMON_BABY_BUNNY     = 61727,
    SPELL_SPRING_RABBIT_IN_LOVE = 61728,
    NPC_SPRING_RABBIT           = 32791,
};

class npc_spring_rabbit : public CreatureScript
{
public:
    npc_spring_rabbit() : CreatureScript("npc_spring_rabbit") { }

    struct npc_spring_rabbitAI : public QuantumBasicAI
    {
        npc_spring_rabbitAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool InLove;
        uint32 JumpTimer;
        uint32 BunnyTimer;
        uint32 SearchTimer;
        uint64 RabbitGUID;

        void Reset()
        {
            InLove = false;
            RabbitGUID = 0;
            JumpTimer = urand(5000, 10000);
            BunnyTimer = urand(10000, 20000);
            SearchTimer = urand(5000, 10000);
            if (Unit* owner = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        void EnterToBattle(Unit* /*who*/){}

        void DoAction(const int32 /*id*/)
        {
            InLove = true;
            if (Unit* owner = me->GetOwner())
                owner->CastSpell(owner, SPELL_SPRING_FLING, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (InLove)
            {
                if (JumpTimer <= diff)
                {
                    if (Unit* rabbit = Unit::GetUnit(*me, RabbitGUID))
                        DoCast(rabbit, SPELL_SPRING_RABBIT_JUMP);
                    JumpTimer = urand(5000, 10000);
                }
				else JumpTimer -= diff;

                if (BunnyTimer <= diff)
                {
                    DoCast(SPELL_SUMMON_BABY_BUNNY);
                    BunnyTimer = urand(20000, 40000);
                }
				else BunnyTimer -= diff;
            }
            else
            {
                if (SearchTimer <= diff)
                {
                    if (Creature* rabbit = me->FindCreatureWithDistance(NPC_SPRING_RABBIT, 10.0f))
                    {
                        if (rabbit == me || rabbit->HasAura(SPELL_SPRING_RABBIT_IN_LOVE))
                            return;

                        me->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, me);
                        DoAction(1);
                        rabbit->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, rabbit);
                        rabbit->AI()->DoAction(1);
                        rabbit->CastSpell(rabbit, SPELL_SPRING_RABBIT_JUMP, true);
                        RabbitGUID = rabbit->GetGUID();
                    }
                    SearchTimer = urand(5000, 10000);
                }
				else SearchTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spring_rabbitAI(creature);
    }
};

enum Rustrocket
{
	ITEM_SNIVELS_LEDGER     = 49915,
	QUEST_A_FRIENDLY_CHAT_H = 24576,
	QUEST_A_FRIENDLY_CHAT_A = 24657,
};

class npc_snivel_rustrocket : public CreatureScript
{
    public:
        npc_snivel_rustrocket() : CreatureScript("npc_snivel_rustrocket") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_A_FRIENDLY_CHAT_H) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_A_FRIENDLY_CHAT_A) == QUEST_STATUS_INCOMPLETE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I listen to you, Flenni", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
			return true;
		}

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->AddItem(ITEM_SNIVELS_LEDGER, 1);

            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

enum TrainWrecker
{
    GO_TOY_TRAIN_SET    = 193963,
    SPELL_TRAIN_WRECKER = 62943,
    POINT_JUMP          = 1,
    EVENT_SEARCH        = 1,
    EVENT_JUMP_TRAIN    = 2,
    EVENT_WRECK         = 3,
    EVENT_DANCE         = 4,
};

class npc_train_wrecker : public CreatureScript
{
    public:
        npc_train_wrecker() : CreatureScript("npc_train_wrecker") { }

        struct npc_train_wreckerAI : public QuantumBasicAI
        {
            npc_train_wreckerAI(Creature* creature) : QuantumBasicAI(creature) { }

			EventMap events;

            void Reset()
            {
                events.ScheduleEvent(EVENT_SEARCH, 3000);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id == POINT_JUMP)
                    events.ScheduleEvent(EVENT_JUMP, 500);
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SEARCH:
                            if (GameObject* train = me->FindGameobjectWithDistance(GO_TOY_TRAIN_SET, 20.0f))
                            {
                                if (me->GetDistance(train) > 1.5f)
                                {
                                    float x, y, z;
                                    me->GetNearPoint(me, x, y, z, 0.0f, me->GetDistance(train) - 1.5f, me->GetAngle(train));
                                    me->SetWalk(true);
                                    me->GetMotionMaster()->MovePoint(POINT_JUMP, x, y, z);
                                }
                                else events.ScheduleEvent(EVENT_JUMP, 500);
                            }
                            else events.ScheduleEvent(EVENT_SEARCH, 3000);
                            break;
                        case EVENT_JUMP_TRAIN:
                            if (GameObject* train = me->FindGameobjectWithDistance(GO_TOY_TRAIN_SET, 5.0f))
                                me->GetMotionMaster()->MoveJump(train->GetPositionX(), train->GetPositionY(), train->GetPositionZ(), 4.0f, 6.0f);
                            events.ScheduleEvent(EVENT_WRECK, 2500);
                            break;
                        case EVENT_WRECK:
                            if (GameObject* train = me->FindGameobjectWithDistance(GO_TOY_TRAIN_SET, 5.0f))
                            {
                                DoCast(SPELL_TRAIN_WRECKER);
                                train->SetLootState(GO_JUST_DEACTIVATED); // TODO: fix SPELL_TRAIN_WRECKER's effect
                                events.ScheduleEvent(EVENT_DANCE, 2500);
                            }
                            else
                                me->DespawnAfterAction(3000);
                            break;
                        case EVENT_DANCE:
                            me->HandleEmoteCommand(EMOTE_STATE_DANCE);
                            me->DespawnAfterAction(10000);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_train_wreckerAI(creature);
        }
};

class npc_generic_harpoon_cannon : public CreatureScript
{
public:
    npc_generic_harpoon_cannon() : CreatureScript("npc_generic_harpoon_cannon") { }

    struct npc_generic_harpoon_cannonAI : public QuantumBasicAI
    {
        npc_generic_harpoon_cannonAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->SetUnitMovementFlags(MOVEMENTFLAG_ROOT);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_generic_harpoon_cannonAI(creature);
    }
};

enum LonelyTurkey
{
    SPELL_STINKER_BROKEN_HEART = 62004,
};

class npc_lonely_turkey : public CreatureScript
{
public:
	npc_lonely_turkey() : CreatureScript("npc_lonely_turkey") { }

	struct npc_lonely_turkeyAI : public QuantumBasicAI
	{
		npc_lonely_turkeyAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 StinkerBrokenHeartTimer;

		void Reset()
		{
			if (me->IsSummon())
			{
				if (Unit* owner = me->ToTempSummon()->GetSummoner())
					me->GetMotionMaster()->MovePoint(0, owner->GetPositionX() + 25 * cos(owner->GetOrientation()), owner->GetPositionY() + 25 * cos(owner->GetOrientation()), owner->GetPositionZ());
			}

			StinkerBrokenHeartTimer = 3.5 * IN_MILLISECONDS;
		}

		void UpdateAI(uint32 const diff)
		{
			if (StinkerBrokenHeartTimer <= diff)
			{
				DoCast(SPELL_STINKER_BROKEN_HEART);
				me->SetCurrentFaction(31);
			}
			StinkerBrokenHeartTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_lonely_turkeyAI(creature);
	}
};

enum Mohawk
{
	SPELL_CREATE_MOHAWK_GRENADE  = 69243,

	GOSSIP_MENU_NIGHT_ELF_MOHAWK = 31111,

	ITEM_ID_MOHAWK_GRENADE       = 43489,
};

class npc_night_elf_mohawk : public CreatureScript
{
public:
    npc_night_elf_mohawk() : CreatureScript("npc_night_elf_mohawk") {}

    struct npc_night_elf_mohawkAI : public QuantumBasicAI
    {
        npc_night_elf_mohawkAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_NIGHT_ELF_MOHAWK && gossipListId == 0)
			{
				if (!player->HasItemCount(ITEM_ID_MOHAWK_GRENADE, 1, true))
					player->CastSpell(player, SPELL_CREATE_MOHAWK_GRENADE, true);

				player->PlayerTalkClass->SendCloseGossip();
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
        return new npc_night_elf_mohawkAI(creature);
    }
};

enum HellfireSmith
{
	SPELL_CREATE_SHATTERED_HALLS_KEY = 54884,
	ITEM_ID_SHATTERED_HALLS_KEY      = 28395,
};

#define HELLFIRE_SMITH_KEY "I have lost the key to the shattered halls. If you can help me?"

class npc_hellfire_smith : public CreatureScript
{
public:
    npc_hellfire_smith() : CreatureScript("npc_hellfire_smith") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(1, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (creature->IsTrainer())
            player->ADD_GOSSIP_ITEM(3, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TRAINER_TRAIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        if (((player->GetQuestStatus(10758) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(10758)) || (player->GetQuestStatus(10764) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(10764))) && !player->HasItemCount(ITEM_ID_SHATTERED_HALLS_KEY, 1))
            player->ADD_GOSSIP_ITEM(0,HELLFIRE_SMITH_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature),creature->GetGUID());
        return true;
	}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
            {
                player->CLOSE_GOSSIP_MENU();
                ItemPosCountVec dest;
                uint8 canStoreNewItem = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_ID_SHATTERED_HALLS_KEY, 1);
                if (canStoreNewItem == EQUIP_ERR_OK)
                {
                    Item *newItem = NULL;
                    newItem = player->StoreNewItem(dest, ITEM_ID_SHATTERED_HALLS_KEY, 1, true);
                    player->SendNewItem(newItem, 1, true, false);
                }
            }
            break;
        case GOSSIP_ACTION_TRAIN:
            player->GetSession()->SendTrainerList(creature->GetGUID());
            break;
        case GOSSIP_ACTION_TRADE:
            player->GetSession()->SendListInventory(creature->GetGUID());
            break;
        }
        return true;
    }
};

enum WhelpingSpells
{
	SPELL_ONYXIAN_WHELPLING_BREATH = 69004,
	SPELL_ONYXIAN_WHELPLING_TRGGER = 69005,
	SPELL_ONYXIAN_WHELPLING_STUN   = 69006,
};

class npc_onyxian_whelpling : public CreatureScript
{
public:
    npc_onyxian_whelpling() : CreatureScript("npc_onyxian_whelpling") { }

    struct npc_onyxian_whelplingAI : public QuantumBasicAI
    {
        npc_onyxian_whelplingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			Reset();
		}

        bool breath;

        void Reset()
        {
            breath = false;

            me->CastSpell(me, SPELL_ONYXIAN_WHELPLING_TRGGER, true);

            if (Unit* own = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(own, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

		void Aggro(Unit* /*who*/) {}

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (caster->GetGUID() != me->GetGUID())
                return;

            if (spell->Id == SPELL_ONYXIAN_WHELPLING_STUN)
            {
                me->MonsterTextEmote("%s takes in a deep breath...", me->GetGUID());
                breath = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (breath)
            {
                if (!me->HasUnitState(UNIT_STATE_STUNNED))
                {
                    me->CastSpell(me, SPELL_ONYXIAN_WHELPLING_BREATH, false);
                    breath = false;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onyxian_whelplingAI(creature);
    }
};

enum CompanionPet
{
	SPELL_KIRINTOR_FAMILIAR = 61478,
	NPC_KIRIN_TOR_FAMILIAR  = 32643,
};

class npc_kirintor_familiar : public CreatureScript
{
public:
    npc_kirintor_familiar() : CreatureScript("npc_kirintor_familiar") { }

    struct npc_kirintor_familiarAI : public QuantumBasicAI
    {
        npc_kirintor_familiarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			Reset();
		}

        void Reset()
        {
			switch (me->GetEntry())
            {
                case NPC_KIRIN_TOR_FAMILIAR:
					me->CastSpell(me, SPELL_KIRINTOR_FAMILIAR, true);
					break;
			}

            if (Unit* own = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(own, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        void Aggro(Unit* /*who*/) {}

        void UpdateAI(const uint32 /*diff*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kirintor_familiarAI (creature);
    }
};

enum VehicleDefault
{
	NPC_ARGENT_SKYTALON              = 30228,
	NPC_WYRMREST_VANQUISHER          = 27996,
    SPELL_ARGENT_SKYTALON_FLIGHT     = 56682,
    SPELL_WYRMREST_VANQUISHER_FLIGHT = 50345,
};

class npc_default_vehicle : public CreatureScript
{
public:
    npc_default_vehicle() : CreatureScript("npc_default_vehicle") { }

    struct npc_default_vehicleAI : public VehicleAI
    {
        npc_default_vehicleAI(Creature* creature) : VehicleAI(creature) { }

        void Reset()
        {
            VehicleAI::Reset();

            switch (me->GetEntry())
            {
                case NPC_ARGENT_SKYTALON:
					me->AddAura(SPELL_ARGENT_SKYTALON_FLIGHT, me);
					break;
				case NPC_WYRMREST_VANQUISHER:
					me->AddAura(SPELL_WYRMREST_VANQUISHER_FLIGHT, me);
					break;
            }
        }

        void JustRespawned()
        {
            VehicleAI::JustRespawned();
        }

        void OnCharmed(bool apply)
        {
            VehicleAI::OnCharmed(apply);
        }

        void UpdateAI(const uint32 diff)
        {
            VehicleAI::UpdateAI(diff);
        }

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_default_vehicleAI(creature);
		}
    };
};

enum FlymasterTyp
{
	Bathander,
    Dragonhawkhander,
    Wyvernhander,
    Gryphonhander,
    Hypogryphhander,
    Neutralhander
};

enum Entrys
{
	NPC_BAT         = 9521,
	NPC_GRYPHON     = 9526,
	NPC_HYPOGRYPHON = 9527,
	NPC_WYVERN      = 9297,
	NPC_DRAGONHAWK  = 27946,
};

class npc_flight_master : public CreatureScript
{
public:
    npc_flight_master() : CreatureScript("npc_flight_master") { }

    struct npc_flight_masterAI : public QuantumBasicAI
    {
        npc_flight_masterAI(Creature* creature) : QuantumBasicAI(creature) { }

        FlymasterTyp HanderTyp;
        uint32 SpawnTimer;

        void Reset()
        {
            HanderTyp = GetType();
            SpawnTimer = 30000;
        }

        void MoveInLineOfSight(Unit*) {}

        void EnterToBattle(Unit* who)
        {
            SpawnGuardian(HanderTyp,who);
        }

        FlymasterTyp GetType()
        {
            switch (me->GetEntry())
            {
            case 2389:
            case 16189:
            case 12636:
            case 16192:
            case 4551:
            case 2226:
				return Bathander;
            case 26560:
                return Dragonhawkhander;
            case 22931:
            case 10583:
            case 24851:
            case 23612:
            case 16227:
            case 22216:
            case 18938:
            case 18940:
            case 19581:
            case 22455:
            case 21766:
            case 20515:
            case 19583:
                return Neutralhander;
            case 20234:
            case 18809:
            case 8018:
            case 8609:
            case 18939:
            case 7823:
            case 12596:
            case 12617:
            case 21107:
            case 18931:
            case 16822:
            case 24366:
            case 1571:
            case 1572:
            case 2835:
            case 2409:
            case 523:
            case 2941:
            case 1573:
            case 2299:
            case 352:
            case 4321:
            case 931:
            case 2859:
            case 2432:
                return Gryphonhander;
            case 3838:
            case 3841:
            case 15177:
            case 18937:
            case 17555:
            case 18785:
            case 18788:
            case 18789:
            case 17554:
            case 4267:
            case 4319:
            case 1233:
            case 6706:
            case 22935:
            case 22485:
            case 10897:
            case 11138:
            case 4407:
            case 12577:
            case 12578:
            case 8019:
                return Hypogryphhander;
            case 2861:
            case 19558:
            case 2858:
            case 18942:
            case 1387:
            case 19317:
            case 2851:
            case 20762:
            case 8020:
            case 18953:
            case 18930:
            case 2995:
            case 11901:
            case 4312:
            case 11899:
            case 11139:
            case 4314:
            case 10378:
            case 8610:
            case 4317:
            case 7824:
            case 6726:
            case 12616:
            case 12740:
            case 13177:
            case 18808:
            case 18807:
            case 18791:
            case 14242:
            case 3305:
            case 16587:
            case 3310:
            case 3615:
            case 11900:
            case 15178:
            case 6026:
				return Wyvernhander;
            default:
				return Neutralhander;
            }
        }

        void SpawnGuardian(FlymasterTyp type, Unit *target)
        {
            uint32 SpawnEntry = 0;
            switch (type)
            {
            case Bathander:
                SpawnEntry = NPC_BAT;
                break;
            case Dragonhawkhander:
                SpawnEntry = NPC_DRAGONHAWK;
                break;
            case Wyvernhander:
                SpawnEntry = NPC_WYVERN;
                break;
            case Gryphonhander:
                SpawnEntry = NPC_GRYPHON;
                break;
            case Hypogryphhander:
                SpawnEntry = NPC_HYPOGRYPHON;
                break;
            case Neutralhander:
                bool isHorde = (target->GetFaction() == 1610 ||target->GetFaction() == 2 ||target->GetFaction() == 6 ||target->GetFaction() == 116 ||target->GetFaction() == 5) ;
                if (isHorde)
                    SpawnEntry = NPC_GRYPHON;
                else
                    SpawnEntry = NPC_WYVERN;
                break;
            }

            for (int i = 0; i < 2; i++)
            {
                Creature* spawn = DoSpawnCreature(SpawnEntry, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                spawn->AI()->AttackStart(target);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SpawnTimer <= diff)
            {
                SpawnGuardian(HanderTyp, me->GetVictim());
                SpawnTimer = 30000;
            }
			else SpawnTimer -= diff;

            DoMeleeAttackIfReady();
        }

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_flight_masterAI(creature);
		}
    };
};

enum EtherealSoulTrader
{
	SPELL_ETHEREAL_PET_AURA_ON_KILL          = 50050,
	SPELL_ETHEREAL_PET_AURA_RESET            = 50051,
	SPELL_ETHEREAL_PET_ON_UNSUMMON           = 50052,
	SPELL_ETHEREAL_PET_AURA_REMOVE           = 50055,
	SPELL_ETHEREAL_PET_GIVE_TOKEN            = 50063,
	SPELL_ETHEREAL_PET_ON_KILL_STEAL_ESSENCE = 50101,
	// Text
	SAY_ETHEREAL_TRADER_RANDOM_1             = -1166300,
	SAY_ETHEREAL_TRADER_RANDOM_2             = -1166301,
	SAY_ETHEREAL_TRADER_RANDOM_3             = -1166302,
};

class npc_ethereal_soul_trader : public CreatureScript
{
public:
    npc_ethereal_soul_trader() : CreatureScript("npc_ethereal_soul_trader") {}

    struct npc_ethereal_soul_traderAI : public QuantumBasicAI
    {
        npc_ethereal_soul_traderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RandomSayTimer;
		uint32 StealEssenceTimer;

        void Reset()
        {
			RandomSayTimer = 1*IN_MILLISECONDS;
			StealEssenceTimer = 2*IN_MILLISECONDS;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PLAYER_VS_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (RandomSayTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(RAND(SAY_ETHEREAL_TRADER_RANDOM_1, SAY_ETHEREAL_TRADER_RANDOM_2, SAY_ETHEREAL_TRADER_RANDOM_3), me);
				RandomSayTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else RandomSayTimer -= diff;

			if (StealEssenceTimer <= diff && !me->IsInCombatActive())
			{
				if (Player* player = me->FindPlayerWithDistance(65.0f, true))
				{
					me->SetFacingToObject(player);
					DoCast(player, SPELL_ETHEREAL_PET_ON_KILL_STEAL_ESSENCE);
					StealEssenceTimer = 1.5*MINUTE*IN_MILLISECONDS;
				}
			}
			else StealEssenceTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_soul_traderAI(creature);
    }
};

class npc_risen_ally : public CreatureScript
{
public:
	npc_risen_ally() : CreatureScript("npc_risen_ally") { }

	struct npc_risen_allyAI : QuantumBasicAI
	{
		npc_risen_allyAI(Creature* creature) : QuantumBasicAI(creature){}

		void InitializeAI()
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
			me->SetSheath(SHEATH_STATE_MELEE);
			me->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_ABANDONED);
			me->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
			me->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
			me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
			me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 1.5f);
		}

		void Reset()
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
		}

		void IsSummonedBy(Unit* owner)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* /*killer*/)
		{
			if (me->GetOwner())
			{
				me->GetOwner()->RemoveAurasDueToSpell(62218);
				me->GetOwner()->RemoveAurasDueToSpell(46619);
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!me->IsCharmed())
				me->DespawnAfterAction();

			if (me->IsInCombatActive())
				DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_risen_allyAI(creature);
	}
};

enum GmIslandAccess
{
	SOUND_ALARM_KILLER = 12867,
};

class npc_gm_island_access : public CreatureScript
{
public:
    npc_gm_island_access() : CreatureScript("npc_gm_island_access") {}

    struct npc_gm_island_accessAI : public  QuantumBasicAI
    {
        npc_gm_island_accessAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 GmIslandAccessTimer;

        void Reset()
        {
			GmIslandAccessTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void CheckSanctuary()
		{
			Map::PlayerList const& Players = me->GetMap()->GetPlayers();

			for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
			{
				if (Player* player = itr->getSource())
				{
					if (player->IsWithinDist(me, 955.0f) && player->GetSession()->GetSecurity() == SEC_PLAYER)
					{
						if (player->IsGameMaster())
							return;

						me->PlayDirectSound(SOUND_ALARM_KILLER);
						player->TeleportTo(1, -7426.87f, 1005.31f, 1.13359f, 2.96086f);
						sWorld->BanCharacter(player->GetName(), secsToTimeString(259200, true).c_str(), "Gm Island Defender", "Cheat Access to GM Island");
						sWorld->SendWorldText(LANG_GM_ISLAND_HACK_DETECTION, player->GetName());
					}
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (GmIslandAccessTimer <= diff && !me->IsInCombatActive())
			{
				CheckSanctuary();
				GmIslandAccessTimer = 1*IN_MILLISECONDS;
			}
			else GmIslandAccessTimer -= diff;

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gm_island_accessAI(creature);
    }
};

enum PlayerPetSpells
{
	SPELL_FULL_HEAL                    = 17683,
	SPELL_FULL_MANA                    = 60628,
	SPELL_GHOUL_TAUNT                  = 43264,
	SPELL_HEALTH_LEECH_PASSIVE         = 50453,
	SPELL_DEATH_KNIGHT_PET_SCALLING_1  = 54566,
	SPELL_DEATH_KNIGHT_PET_SCALLING_2  = 51996,
	SPELL_DEATH_KNIGHT_PET_SCALLING_3  = 61697,
	SPELL_DEATH_KNIGHT_AVOIDANCE       = 62137,
	SPELL_DEATH_KNIGHT_WEAPON_SCALLING = 51906,
	SPELL_ARMY_OF_DEAD_VISUAL          = 63107,
	SPELL_GHOUL_BIRTH                  = 7398,
};

class npc_dk_bloodworm : public CreatureScript
{
public:
    npc_dk_bloodworm() : CreatureScript("npc_dk_bloodworm") {}

    struct npc_dk_bloodwormAI : public QuantumBasicAI
    {
        npc_dk_bloodwormAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_HEALTH_LEECH_PASSIVE, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dk_bloodwormAI(creature);
    }
};

class npc_dk_rune_weapon : public CreatureScript
{
public:
    npc_dk_rune_weapon() : CreatureScript("npc_dk_rune_weapon") {}

    struct npc_dk_rune_weaponAI : public QuantumBasicAI
    {
        npc_dk_rune_weaponAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_DEATH_KNIGHT_WEAPON_SCALLING, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dk_rune_weaponAI(creature);
    }
};

enum FreezingCircle
{
	SPELL_FREEZING_CIRCLE = 34779,
	SPELL_FROST_RING_1    = 34740,
	SPELL_FROST_RING_2    = 34746,
};

class npc_freezing_circle : public CreatureScript
{
public:
    npc_freezing_circle() : CreatureScript("npc_freezing_circle") {}

    struct npc_freezing_circleAI : public QuantumBasicAI
    {
        npc_freezing_circleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
		{
			DoCastAOE(SPELL_FREEZING_CIRCLE, true);
			DoCastAOE(SPELL_FROST_RING_1, true);
			DoCastAOE(SPELL_FROST_RING_2, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freezing_circleAI(creature);
    }
};

enum LilXt
{
	SPELL_LIL_XT_GAME_1 = 76092,
	SPELL_LIL_XT_GAME_2 = 76096,
	SPELL_LIL_XT_GAME_3 = 76114,
	SPELL_LIL_XT_GAME_4 = 76116,
};

class npc_lil_xt : public CreatureScript
{
public:
    npc_lil_xt() : CreatureScript("npc_lil_xt") {}

    struct npc_lil_xtAI : public QuantumBasicAI
    {
        npc_lil_xtAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 XtGameTimer;

        void Reset()
        {
			XtGameTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (XtGameTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(RAND(SPELL_LIL_XT_GAME_1, SPELL_LIL_XT_GAME_2, SPELL_LIL_XT_GAME_3, SPELL_LIL_XT_GAME_4));
				XtGameTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else XtGameTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lil_xtAI(creature);
    }
};

void AddSC_npcs_special()
{
    new npc_air_force_bots();
    new npc_lunaclaw_spirit();
    new npc_chicken_cluck();
    new npc_dancing_flames();
    new npc_doctor();
    new npc_injured_patient();
    new npc_garments_of_quests();
    new npc_guardian();
    new npc_mount_vendor();
    new npc_rogue_trainer();
    new npc_sayge();
    new npc_steam_tonk();
    new npc_tonk_mine();
    new npc_brewfest_reveler();
    new npc_snake_trap();
    new npc_mirror_image();
    new npc_ebon_gargoyle();
    new npc_lightwell();
    new npc_mojo();
    new npc_training_dummy();
    new npc_shadowfiend();
    new npc_wormhole();
    new npc_pet_trainer();
    new npc_locksmith();
    new npc_tabard_vendor();
    new npc_experience();
    new npc_duthorian_rall();
    new npc_apple_trap_bunny();
    new npc_keg_delivery();
    new npc_bark_bunny();
    new npc_brew_vendor();
    new npc_dark_iron_herald();
    new npc_dark_iron_guzzler();
	new npc_wild_turkey();
	new npc_fire_elemental();
	new npc_earth_elemental();
	new npc_firework();
	new npc_spring_rabbit();
	new npc_snivel_rustrocket();
	new npc_train_wrecker();
	new npc_generic_harpoon_cannon();
	new npc_lonely_turkey();
	new npc_night_elf_mohawk();
	new npc_hellfire_smith();
	new npc_onyxian_whelpling();
	new npc_kirintor_familiar();
	new npc_default_vehicle();
	new npc_flight_master();
	new npc_ethereal_soul_trader();
	new npc_risen_ally();
	new npc_gm_island_access();
	new npc_dk_bloodworm();
	new npc_dk_rune_weapon();
	new npc_freezing_circle();
	new npc_lil_xt();
}