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
#include "WorldSession.h"

#define GOSSIP_HL  "Talk to me"
#define GOSSIP_SL1 "What do you do here?"
#define GOSSIP_SL2 "I can help you"
#define GOSSIP_SL3 "What deal?"
#define GOSSIP_SL4 "Then what happened?"
#define GOSSIP_SL5 "He is not safe, i'll make sure of that."

class npc_lorax : public CreatureScript
{
public:
    npc_lorax() : CreatureScript("npc_lorax") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(5126) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(3759, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(3760, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(3761, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(3762, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(3763, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(5126);
                break;
        }
        return true;
    }
};

enum RivernFrostWind
{
	FACTION_WINTERSABER_TRAINERS = 589,
};

class npc_rivern_frostwind : public CreatureScript
{
public:
    npc_rivern_frostwind() : CreatureScript("npc_rivern_frostwind") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor() && player->GetReputationRank(FACTION_WINTERSABER_TRAINERS) == REP_EXALTED)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
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

#define GOSSIP_HWDM "I'd like you to make me a new Cache of Mau'ari please."

enum DoctorMauari
{
	SPELL_CREATE_CACHE_OF_MAUARI = 16351,

	QUEST_CACHE_OF_MAUARI        = 975,
};

class npc_witch_doctor_mauari : public CreatureScript
{
public:
    npc_witch_doctor_mauari() : CreatureScript("npc_witch_doctor_mauari") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestRewardStatus(QUEST_CACHE_OF_MAUARI))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HWDM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(3377, creature->GetGUID());
        }
		else
			player->SEND_GOSSIP_MENU(3375, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_CREATE_CACHE_OF_MAUARI, false);
        }
        return true;
    }
};

enum Says
{
    // Escort texts
    SAY_QUEST_START         = 0,
    SAY_ENTER_OWL_THICKET   = 1,
    SAY_REACH_TORCH         = 2,
    SAY_AFTER_TORCH         = 3,
    SAY_REACH_ALTAR_1       = 4,
    SAY_REACH_ALTAR_2       = 5,

    // After lighting the altar cinematic
    SAY_RANSHALLA_ALTAR_1   = 6,
    SAY_RANSHALLA_ALTAR_2   = 7,
    SAY_PRIESTESS_ALTAR_3   = 8,
    SAY_PRIESTESS_ALTAR_4   = 9,
    SAY_RANSHALLA_ALTAR_5   = 10,
    SAY_RANSHALLA_ALTAR_6   = 11,
    SAY_PRIESTESS_ALTAR_7   = 12,
    SAY_PRIESTESS_ALTAR_8   = 13,
    SAY_PRIESTESS_ALTAR_9   = 14,
    SAY_PRIESTESS_ALTAR_10  = 15,
    SAY_PRIESTESS_ALTAR_11  = 16,
    SAY_PRIESTESS_ALTAR_12  = 17,
    SAY_PRIESTESS_ALTAR_13  = 18,
    SAY_PRIESTESS_ALTAR_14  = 19,
    SAY_VOICE_ALTAR_15      = 20,
    SAY_PRIESTESS_ALTAR_16  = 21,
    SAY_PRIESTESS_ALTAR_17  = 22,
    SAY_PRIESTESS_ALTAR_18  = 23,
    SAY_PRIESTESS_ALTAR_19  = 24,
    SAY_PRIESTESS_ALTAR_20  = 25,
    SAY_PRIESTESS_ALTAR_21  = 26,
    SAY_RANSHALLA_END_1     = 27,
    SAY_RANSHALLA_END_2     = 28,

    EMOTE_CHANT_SPELL       = 29,
};

enum Spells
{
    SPELL_LIGHT_TORCH       = 18953,  // channeled spell by Ranshalla while waiting for the torches / altar
};

enum NPCs
{
    NPC_RANSHALLA           = 10300,
    NPC_PRIESTESS_ELUNE     = 12116,
    NPC_VOICE_ELUNE         = 12152,
    NPC_GUARDIAN_ELUNE      = 12140,
};

enum GOs
{
    GO_ELUNE_ALTAR          = 177404,
    GO_ELUNE_FIRE           = 177417,
    GO_ELUNE_GEM            = 177414, // is respawned in script
    GO_ELUNE_LIGHT          = 177415, // are respawned in script
	GO_ELUNE_AURA           = 177416,
};

enum Quests
{
    QUEST_GUARDIANS_ALTAR   = 4901,
};

enum Dummies
{
    NPC_PRIESTESS_DATA_1    = -1, // dummy member for the first priestess (right)
    NPC_PRIESTESS_DATA_2    = -2, // dummy member for the second priestess (left)
    DATA_MOVE_PRIESTESS     = -3, // dummy member to check the priestess movement
    DATA_EVENT_END          = -4, // dummy member to indicate the event end

    EVENT_RESUME            = 1,  // trigger rest of event
};

// DialogueHelper (imported from SD)

struct DialogueEntry
{
    int32 TextEntry;    ///< To be said text entry
    int32 SayerEntry;   ///< Entry of the mob who should say
    uint32 SayTimer;    ///< Time delay until next text of array is said (0 stops)
};

class DialogueHelper
{
public:
    // The array MUST be terminated by {0, 0, 0}
    DialogueHelper(DialogueEntry const* dialogueArray) : _dialogueArray(dialogueArray), _currentEntry(NULL), _actionTimer(0) { }

	void StartNextDialogueText(int32 textEntry)
	{
          // Find textEntry
          bool found = false;

          for (DialogueEntry const* entry = _dialogueArray; entry->TextEntry; ++entry)
          {
              if (entry->TextEntry == textEntry)
              {
                  _currentEntry = entry;
                  found = true;
                  break;
              }
          }

          if (!found)
              return;

          DoNextDialogueStep();
      }

      void DialogueUpdate(uint32 diff)
      {
          if (_actionTimer)
          {
              if (_actionTimer <= diff)
                  DoNextDialogueStep();
              else
                  _actionTimer -= diff;
          }
      }

protected:
    /// Will be called when a dialogue step was done
    virtual void JustDidDialogueStep(int32 /*entry*/) { }
    /// Will be called to get a speaker, MUST be implemented if not used in instances
    virtual Creature* GetSpeakerByEntry(int32 /*entry*/) { return NULL; }

private:
    void DoNextDialogueStep()
    {
        // Last Dialogue Entry done?
        if (!_currentEntry || !_currentEntry->TextEntry)
        {
            _actionTimer = 0;
            return;
        }

        // Get Text, SpeakerEntry and Timer
        int32 textEntry = _currentEntry->TextEntry;
        uint32 sayerEntry = _currentEntry->SayerEntry;
        _actionTimer = _currentEntry->SayTimer;

        // Simulate Case
        if (sayerEntry && textEntry >= 0)
        {
            // Use Speaker if directly provided
            if (Creature* speaker = GetSpeakerByEntry(sayerEntry))
                speaker->AI()->Talk(textEntry);
        }

        JustDidDialogueStep(_currentEntry->TextEntry);

        // Increment position
        ++_currentEntry;
    }

    DialogueEntry const* _dialogueArray;
    DialogueEntry const* _currentEntry;

    uint32 _actionTimer;
};

const DialogueEntry introDialogue[] =
{
    {SAY_REACH_ALTAR_1,         NPC_RANSHALLA,          2000},
    {SAY_REACH_ALTAR_2,         NPC_RANSHALLA,          3000},
    {NPC_RANSHALLA,             0,                         0}, // start the altar channeling
    {SAY_PRIESTESS_ALTAR_3,     NPC_PRIESTESS_DATA_2,   1000},
    {SAY_PRIESTESS_ALTAR_4,     NPC_PRIESTESS_DATA_1,   4000},
    {SAY_RANSHALLA_ALTAR_5,     NPC_RANSHALLA,          4000},
    {SAY_RANSHALLA_ALTAR_6,     NPC_RANSHALLA,          4000}, // start the escort here
    {SAY_PRIESTESS_ALTAR_7,     NPC_PRIESTESS_DATA_2,   4000},
    {SAY_PRIESTESS_ALTAR_8,     NPC_PRIESTESS_DATA_2,   5000}, // show the gem
    {GO_ELUNE_GEM,              0,                      5000},
    {SAY_PRIESTESS_ALTAR_9,     NPC_PRIESTESS_DATA_1,   4000}, // move priestess 1 near me
    {NPC_PRIESTESS_DATA_1,      0,                      3000},
    {SAY_PRIESTESS_ALTAR_10,    NPC_PRIESTESS_DATA_1,   5000},
    {SAY_PRIESTESS_ALTAR_11,    NPC_PRIESTESS_DATA_1,   4000},
    {SAY_PRIESTESS_ALTAR_12,    NPC_PRIESTESS_DATA_1,   5000},
    {SAY_PRIESTESS_ALTAR_13,    NPC_PRIESTESS_DATA_1,   8000}, // summon voice and guard of elune
    {NPC_VOICE_ELUNE,           0,                     12000},
    {SAY_VOICE_ALTAR_15,        NPC_VOICE_ELUNE,        5000}, // move priestess 2 near me
    {NPC_PRIESTESS_DATA_2,      0,                      3000},
    {SAY_PRIESTESS_ALTAR_16,    NPC_PRIESTESS_DATA_2,   4000},
    {SAY_PRIESTESS_ALTAR_17,    NPC_PRIESTESS_DATA_2,   6000},
    {SAY_PRIESTESS_ALTAR_18,    NPC_PRIESTESS_DATA_1,   5000},
    {SAY_PRIESTESS_ALTAR_19,    NPC_PRIESTESS_DATA_1,   3000}, // move the owlbeast
    {NPC_GUARDIAN_ELUNE,        0,                      2000},
    {SAY_PRIESTESS_ALTAR_20,    NPC_PRIESTESS_DATA_1,   4000}, // move the first priestess up
    {SAY_PRIESTESS_ALTAR_21,    NPC_PRIESTESS_DATA_2,  10000}, // move second priestess up
    {DATA_MOVE_PRIESTESS,       0,                      6000}, // despawn the gem
    {DATA_EVENT_END,            0,                      2000}, // turn towards the player
    {SAY_RANSHALLA_END_2,       NPC_RANSHALLA, 0},
    {0,                         0,                         0},
};

static Position wingThicketLocations[] =
{
    {5515.98f, -4903.43f, 846.30f, 4.58f},  // 0 right priestess summon loc
    {5501.94f, -4920.20f, 848.69f, 6.15f},  // 1 left priestess summon loc
    {5497.35f, -4906.49f, 850.83f, 2.76f},  // 2 guard of elune summon loc
    {5518.38f, -4913.47f, 845.57f, 0.00f},  // 3 right priestess move loc
    {5510.36f, -4921.17f, 846.33f, 0.00f},  // 4 left priestess move loc
    {5511.31f, -4913.82f, 847.17f, 0.00f},  // 5 guard of elune move loc
    {5518.51f, -4917.56f, 845.23f, 0.00f},  // 6 right priestess second move loc
    {5514.40f, -4921.16f, 845.49f, 0.00f},   // 7 left priestess second move loc
};

class npc_ranshalla : public CreatureScript
{
public:
    npc_ranshalla() : CreatureScript("npc_ranshalla") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_GUARDIANS_ALTAR)
        {
            creature->AI()->Talk(SAY_QUEST_START);
            creature->SetCurrentFaction(FACTION_ESCORT_A_NEUTRAL_PASSIVE);

            if (npc_ranshallaAI* escortAI = dynamic_cast<npc_ranshallaAI*>(creature->AI()))
                escortAI->Start(false, false, player->GetGUID(), quest);

            return true;
        }

        return false;
    }

	struct npc_ranshallaAI : public npc_escortAI, private DialogueHelper
    {
		npc_ranshallaAI(Creature* creature) : npc_escortAI(creature), DialogueHelper(introDialogue)
        {
            Initialize();
        }

		EventMap events;

        uint32 DelayTimer;

        uint64 FirstPriestessGUID;
        uint64 SecondPriestessGUID;
        uint64 GuardEluneGUID;
        uint64 VoiceEluneGUID;
        uint64 AltarGUID;

		void Initialize()
        {
            DelayTimer = 0;
        }

        void Reset()
        {
            Initialize();
        }

        // Called when the player activates the torch / altar
        void DoContinueEscort(bool isAltarWaypoint = false)
        {
            me->InterruptNonMeleeSpells(false);

            if (isAltarWaypoint)
                Talk(SAY_RANSHALLA_ALTAR_1);
            else
                Talk(SAY_AFTER_TORCH);

            DelayTimer = 2000;
        }

        // Called when Ranshalla starts to channel on a torch / altar
        void DoChannelTorchSpell(bool isAltarWaypoint = false)
        {
            // Check if we are using the fire or the altar and remove the no_interact flag
            if (isAltarWaypoint)
            {
                if (GameObject* go = GetClosestGameObjectWithEntry(me, GO_ELUNE_ALTAR, 10.0f))
                {
                    go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    me->SetFacingToObject(go);
                    AltarGUID = go->GetGUID();
                }
            }
            else if (GameObject* go = GetClosestGameObjectWithEntry(me, GO_ELUNE_FIRE, 10.0f))
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

            // Yell and set escort to pause
            Talk(SAY_REACH_TORCH);
            Talk(EMOTE_CHANT_SPELL);
            SetEscortPaused(true);
            DoCast(me, SPELL_LIGHT_TORCH);
        }

        void DoSummonPriestess()
        {
            // Summon 2 Elune priestess and make each of them move to a different spot
            if (Creature* priestess = me->SummonCreature(NPC_PRIESTESS_ELUNE, wingThicketLocations[0].m_positionX, wingThicketLocations[0].m_positionY, wingThicketLocations[0].m_positionZ, wingThicketLocations[0].GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 5*IN_MILLISECONDS))
            {
                priestess->GetMotionMaster()->MovePoint(0, wingThicketLocations[3].m_positionX, wingThicketLocations[3].m_positionY, wingThicketLocations[3].m_positionZ);
                FirstPriestessGUID = priestess->GetGUID();
            }
            if (Creature* priestess = me->SummonCreature(NPC_PRIESTESS_ELUNE, wingThicketLocations[1].m_positionX, wingThicketLocations[1].m_positionY, wingThicketLocations[1].m_positionZ, wingThicketLocations[1].GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 5*IN_MILLISECONDS))
            {
                // Left priestess should have a distinct move point because she is the one who starts the dialogue at point reach
                priestess->GetMotionMaster()->MovePoint(1, wingThicketLocations[4].m_positionX, wingThicketLocations[4].m_positionY, wingThicketLocations[4].m_positionZ);
                SecondPriestessGUID = priestess->GetGUID();
            }
        }

        void SummonedMovementInform(Creature* summoned, uint32 type, uint32 pointId)
        {
            if (type != POINT_MOTION_TYPE || summoned->GetEntry() != NPC_PRIESTESS_ELUNE || pointId != 1)
                return;

            // Start the dialogue when the priestess reach the altar (they should both reach the point in the same time)
            StartNextDialogueText(SAY_PRIESTESS_ALTAR_3);
        }

        void WaypointReached(uint32 pointId)
        {
            switch (pointId)
            {
                case 3:
                    Talk(SAY_ENTER_OWL_THICKET);
                    break;
                case 10: // Cavern 1
                case 15: // Cavern 2
                case 20: // Cavern 3
                case 25: // Cavern 4
                case 36: // Cavern 5
                    DoChannelTorchSpell();
                    break;
                case 39:
                    StartNextDialogueText(SAY_REACH_ALTAR_1);
                    SetEscortPaused(true);
                    break;
                case 41:
                {
                    // Search for all nearest lights and respawn them
                    std::list<GameObject*> eluneLights;
                    GetGameObjectListWithEntryInGrid(eluneLights, me, GO_ELUNE_LIGHT, 20.0f);
                    for (std::list<GameObject*>::const_iterator itr = eluneLights.begin(); itr != eluneLights.end(); ++itr)
                    {
                        if ((*itr)->isSpawned())
                            continue;

                        (*itr)->SetRespawnTime(115);
                        (*itr)->Refresh();
                    }

                    if (GameObject* altar = me->GetMap()->GetGameObject(AltarGUID))
                        me->SetFacingToObject(altar);
                    break;
                }
                case 42:
                    // Summon the 2 priestess
                    SetEscortPaused(true);
                    DoSummonPriestess();
                    Talk(SAY_RANSHALLA_ALTAR_2);
                    events.ScheduleEvent(EVENT_RESUME, 2000);
                    break;
                case 44:
                    // Stop the escort and turn towards the altar
                    SetEscortPaused(true);
                    if (GameObject* altar = me->GetMap()->GetGameObject(AltarGUID))
                        me->SetFacingToObject(altar);
                    break;
            }
        }

        void JustDidDialogueStep(int32 entry)
        {
            switch (entry)
            {
                case NPC_RANSHALLA:
                    // Start the altar channeling
                    DoChannelTorchSpell(true);
                    break;
                case SAY_RANSHALLA_ALTAR_6:
                    SetEscortPaused(false);
                    break;
                case SAY_PRIESTESS_ALTAR_8:
                    // make the gem respawn
                    if (GameObject* gem = GetClosestGameObjectWithEntry(me, GO_ELUNE_GEM, 10.0f))
                    {
                        if (gem->isSpawned())
                            break;

                        gem->SetRespawnTime(90);
                        gem->Refresh();
                    }
					if (GameObject* aura = GetClosestGameObjectWithEntry(me, GO_ELUNE_AURA, 10.0f))
					{
						if (aura->isSpawned())
							break;

						aura->SetRespawnTime(90);
						aura->Refresh();
					}
                    break;
                case SAY_PRIESTESS_ALTAR_9:
                    // move near the escort npc
                    if (Creature* priestess = me->GetMap()->GetCreature(FirstPriestessGUID))
                        priestess->GetMotionMaster()->MovePoint(0, wingThicketLocations[6].m_positionX, wingThicketLocations[6].m_positionY, wingThicketLocations[6].m_positionZ);
                    break;
                case SAY_PRIESTESS_ALTAR_13:
                    // summon the Guardian of Elune
                    if (Creature* guard = me->SummonCreature(NPC_GUARDIAN_ELUNE, wingThicketLocations[2].m_positionX, wingThicketLocations[2].m_positionY, wingThicketLocations[2].m_positionZ, wingThicketLocations[2].GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 5*IN_MILLISECONDS))
                    {
                        guard->GetMotionMaster()->MovePoint(0, wingThicketLocations[5].m_positionX, wingThicketLocations[5].m_positionY, wingThicketLocations[5].m_positionZ);
                        GuardEluneGUID = guard->GetGUID();
                    }
                    // summon the Voice of Elune
                    if (GameObject* altar = me->GetMap()->GetGameObject(AltarGUID))
                    {
                        if (Creature* voice = me->SummonCreature(NPC_VOICE_ELUNE, altar->GetPositionX(), altar->GetPositionY(), altar->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30*IN_MILLISECONDS))
                            VoiceEluneGUID = voice->GetGUID();
                    }
                    break;
                case SAY_VOICE_ALTAR_15:
                    // move near the escort npc and continue dialogue
                    if (Creature* priestess = me->GetMap()->GetCreature(SecondPriestessGUID))
                    {
                        priestess->AI()->Talk(SAY_PRIESTESS_ALTAR_14);
                        priestess->GetMotionMaster()->MovePoint(0, wingThicketLocations[7].m_positionX, wingThicketLocations[7].m_positionY, wingThicketLocations[7].m_positionZ);
                    }
                    break;
                case SAY_PRIESTESS_ALTAR_19:
                    // make the voice of elune leave
                    if (Creature* guard = me->GetMap()->GetCreature(GuardEluneGUID))
                    {
                        guard->GetMotionMaster()->MovePoint(0, wingThicketLocations[2].m_positionX, wingThicketLocations[2].m_positionY, wingThicketLocations[2].m_positionZ);
                        guard->DespawnAfterAction(4*IN_MILLISECONDS);
                    }
                    break;
                case SAY_PRIESTESS_ALTAR_20:
                    // make the first priestess leave
                    if (Creature* priestess = me->GetMap()->GetCreature(FirstPriestessGUID))
                    {
                        priestess->GetMotionMaster()->MovePoint(0, wingThicketLocations[0].m_positionX, wingThicketLocations[0].m_positionY, wingThicketLocations[0].m_positionZ);
                        priestess->DespawnAfterAction(4*IN_MILLISECONDS);
                    }
                    break;
                case SAY_PRIESTESS_ALTAR_21:
                    // make the second priestess leave
                    if (Creature* priestess = me->GetMap()->GetCreature(SecondPriestessGUID))
                    {
                        priestess->GetMotionMaster()->MovePoint(0, wingThicketLocations[1].m_positionX, wingThicketLocations[1].m_positionY, wingThicketLocations[1].m_positionZ);
                        priestess->DespawnAfterAction(4*IN_MILLISECONDS);
                    }
                    break;
                case DATA_EVENT_END:
                    // Turn towards the player
                    if (Player* player = GetPlayerForEscort())
                    {
                        me->SetFacingToObject(player);
                        Talk(SAY_RANSHALLA_END_1, player->GetGUID());
                    }
                    break;
                case SAY_RANSHALLA_END_2:
                    // Turn towards the altar and kneel - quest complete
                    if (GameObject* altar = me->GetMap()->GetGameObject(AltarGUID))
                    {
                        me->SetFacingToObject(altar);
                        altar->ResetDoorOrButton();
                    }
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    if (Player* player = GetPlayerForEscort())
                    {
                        player->GroupEventHappens(QUEST_GUARDIANS_ALTAR, me);
                        Talk(SAY_RANSHALLA_END_2, player->GetGUID());
                    }
                    me->DespawnAfterAction(4*IN_MILLISECONDS);
                    break;
            }
        }

        Creature* GetSpeakerByEntry(int32 entry)
        {
            switch (entry)
            {
                case NPC_RANSHALLA:
                    return me;
                case NPC_VOICE_ELUNE:
                    return me->GetMap()->GetCreature(VoiceEluneGUID);
                case NPC_PRIESTESS_DATA_1:
                    return me->GetMap()->GetCreature(FirstPriestessGUID);
                case NPC_PRIESTESS_DATA_2:
                    return me->GetMap()->GetCreature(SecondPriestessGUID);
                default:
                    return NULL;
            }

        }

        void UpdateEscortAI(uint32 const diff)
        {
            DialogueUpdate(diff);

            if (DelayTimer)
            {
                if (DelayTimer <= diff)
                {
                    SetEscortPaused(false);
                    DelayTimer = 0;
                }
                else DelayTimer -= diff;
            }
            events.Update(diff);

            if (events.ExecuteEvent() == EVENT_RESUME)
                StartNextDialogueText(SAY_PRIESTESS_ALTAR_3);

            npc_escortAI::UpdateEscortAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ranshallaAI(creature);
    }
};

class go_elune_fire : public GameObjectScript
{
public:
    go_elune_fire() : GameObjectScript("go_elune_fire") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        bool isAltar = false;

        if (go->GetEntry() == GO_ELUNE_ALTAR)
			isAltar = true;

        if (Creature* ranshalla = GetClosestCreatureWithEntry(go, NPC_RANSHALLA, 10.0f))
        {
            if (npc_ranshalla::npc_ranshallaAI* escortAI = dynamic_cast<npc_ranshalla::npc_ranshallaAI*>(ranshalla->AI()))
                escortAI->DoContinueEscort(isAltar);
        }

        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        return false;
    }
};

void AddSC_winterspring()
{
    new npc_lorax();
    new npc_rivern_frostwind();
    new npc_witch_doctor_mauari();
	new npc_ranshalla();
	new go_elune_fire();
}