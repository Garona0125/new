/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "QuantumEscortAI.h"
#include "PassiveAI.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellAuraEffects.h"
#include "icecrown_citadel.h"

enum Texts
{
    // Highlord Tirion Fordring (at Light's Hammer)
    SAY_TIRION_INTRO_1              = -1659215,
    SAY_TIRION_INTRO_2              = -1659216,
    SAY_TIRION_INTRO_3              = -1659217,
    SAY_TIRION_INTRO_4              = -1659218,
    SAY_TIRION_INTRO_H_5            = -1659219,
    SAY_TIRION_INTRO_A_5            = -1659220,
    SAY_LK_INTRO_1                  = -1659221,
    SAY_LK_INTRO_2                  = -1659222,
    SAY_LK_INTRO_3                  = -1659223,
    SAY_LK_INTRO_4                  = -1659224,
    SAY_LK_INTRO_5                  = -1659225,
    SAY_BOLVAR_INTRO_1              = -1659226,
    SAY_SAURFANG_INTRO_1            = -1659227,
    SAY_SAURFANG_INTRO_2            = -1659228,
    SAY_SAURFANG_INTRO_3            = -1659229,
    SAY_SAURFANG_INTRO_4            = -1659230,
    SAY_MURADIN_INTRO_1             = -1659231,
    SAY_MURADIN_INTRO_2             = -1659232,
    SAY_MURADIN_INTRO_3             = -1659233,
    // Deathbound Ward
    SAY_TRAP_ACTIVATE_1             = -1659234,
	SAY_TRAP_ACTIVATE_2             = -1659235,
	SAY_TRAP_ACTIVATE_3             = -1659236,
    // Rotting Frost Giant
    EMOTE_DEATH_PLAGUE_WARNING      = -1659237,
    // Sister Svalna
    SAY_SVALNA_KILL_CAPTAIN         = 1, // happens when she kills a captain
    SAY_SVALNA_KILL                 = 4,
    SAY_SVALNA_CAPTAIN_DEATH        = 5, // happens when a captain resurrected by her dies
    SAY_SVALNA_DEATH                = 6,
    EMOTE_SVALNA_IMPALE             = 7,
    EMOTE_SVALNA_BROKEN_SHIELD      = 8,
	// Crok intro
    SAY_CROK_INTRO_1                = 0, // Ready your arms, my Argent Brothers. The Vrykul will protect the Frost Queen with their lives.
    SAY_ARNATH_INTRO_2              = 5, // Even dying here beats spending another day collecting reagents for that madman, Finklestein.
    SAY_CROK_INTRO_3                = 1, // Enough idle banter! Our champions have arrived - support them as we push our way through the hall!
    SAY_SVALNA_EVENT_START          = 0, // You may have once fought beside me, Crok, but now you are nothing more than a traitor. Come, your second death approaches!
    SAY_CROK_COMBAT_WP_0            = 2, // Draw them back to us, and we'll assist you.
    SAY_CROK_COMBAT_WP_1            = 3, // Quickly, push on!
    SAY_CROK_FINAL_WP               = 4, // Her reinforcements will arrive shortly, we must bring her down quickly!
    SAY_SVALNA_RESURRECT_CAPTAINS   = 2, // Foolish Crok. You brought my reinforcements with you. Arise, Argent Champions, and serve the Lich King in death!
    SAY_CROK_COMBAT_SVALNA          = 5, // I'll draw her attacks. Return our brothers to their graves, then help me bring her down!
    SAY_SVALNA_AGGRO                = 3, // Come, Scourgebane. I'll show the master which of us is truly worthy of the title of "Champion"!
    SAY_CAPTAIN_DEATH               = 0,
    SAY_CAPTAIN_RESURRECTED         = 1,
    SAY_CAPTAIN_KILL                = 2,
    SAY_CAPTAIN_SECOND_DEATH        = 3,
    SAY_CAPTAIN_SURVIVE_TALK        = 4,
    SAY_CROK_WEAKENING_GAUNTLET     = 6,
    SAY_CROK_WEAKENING_SVALNA       = 7,
    SAY_CROK_DEATH                  = 8,
	SAY_CROK_WARNING_TO_RAID        = 9,   
	SAY_CROK_ICE_TRAP               = 10,
};

enum Spells
{
    // Rotting Frost Giant
    SPELL_DEATH_PLAGUE              = 72879,
    SPELL_DEATH_PLAGUE_AURA         = 72865,
    SPELL_RECENTLY_INFECTED         = 72884,
    SPELL_DEATH_PLAGUE_KILL         = 72867,
    SPELL_STOMP_10                  = 64639,
	SPELL_STOMP_25                  = 64652,
    SPELL_ARCTIC_BREATH             = 72848,
	SPELL_WHITEOUT_10               = 72034,
	SPELL_WHITEOUT_25               = 72096,
	SPELL_BLIZZARD                  = 41482,
	SPELL_FRENZY                    = 55285,
    // Frost Freeze Trap
    SPELL_COLDFLAME_JETS            = 70460,
    // Alchemist Adrianna
    SPELL_HARVEST_BLIGHT_SPECIMEN   = 72155,
    SPELL_HARVEST_BLIGHT_SPECIMEN25 = 72162,
    // Crok Scourgebane
    SPELL_ICEBOUND_ARMOR            = 70714,
    SPELL_SCOURGE_STRIKE            = 71488,
    SPELL_DEATH_STRIKE              = 71489,
	SPELL_DEATH_COIL                = 71490,
    // Sister Svalna
	SPELL_IS_VALKYR                 = 70203,
    SPELL_CARESS_OF_DEATH           = 70078,
    SPELL_IMPALING_SPEAR_KILL       = 70196,
    SPELL_REVIVE_CHAMPION           = 70053,
    SPELL_UNDEATH                   = 70089,
    SPELL_IMPALING_SPEAR            = 71443,
    SPELL_AETHER_SHIELD             = 71463,
	SPELL_DIVINE_SURGE              = 71465,
    SPELL_HURL_SPEAR                = 71466,
	SPELL_SPEAR_VISUAL              = 75498,
    // Captain Arnath
    SPELL_DOMINATE_MIND             = 14515,
    SPELL_FLASH_HEAL_NORMAL         = 71595,
    SPELL_POWER_WORD_SHIELD_NORMAL  = 71548,
    SPELL_SMITE_NORMAL              = 71546,
    SPELL_FLASH_HEAL_UNDEAD         = 71782,
    SPELL_POWER_WORD_SHIELD_UNDEAD  = 71780,
    SPELL_SMITE_UNDEAD              = 71778,
    // Captain Brandon
    SPELL_CRUSADER_STRIKE           = 71549,
    SPELL_DIVINE_SHIELD             = 71550,
    SPELL_JUDGEMENT_OF_COMMAND      = 71551,
    SPELL_HAMMER_OF_BETRAYAL        = 71784,
    // Captain Grondel
    SPELL_CHARGE                    = 71553,
    SPELL_MORTAL_STRIKE             = 71552,
    SPELL_SUNDER_ARMOR              = 71554,
    SPELL_CONFLAGRATION             = 71785,
    // Captain Rupert
    SPELL_FEL_IRON_BOMB_NORMAL      = 71592,
    SPELL_MACHINE_GUN_NORMAL        = 71594,
    SPELL_ROCKET_LAUNCH_NORMAL      = 71590,
    SPELL_FEL_IRON_BOMB_UNDEAD      = 71787,
    SPELL_MACHINE_GUN_UNDEAD        = 71788,
    SPELL_ROCKET_LAUNCH_UNDEAD      = 71786,
	// Invisible Stalker (Float, Uninteractible, LargeAOI)
	SPELL_SOUL_MISSILE              = 72585,
	SPELL_EMPOWERED_BLOOD           = 70227,
	SPELL_DUAL_WIELD_CROK           = 674,
};

// Helper defines
// Captain Arnath
#define SPELL_FLASH_HEAL        (IsUndead ? SPELL_FLASH_HEAL_UNDEAD : SPELL_FLASH_HEAL_NORMAL)
#define SPELL_POWER_WORD_SHIELD (IsUndead ? SPELL_POWER_WORD_SHIELD_UNDEAD : SPELL_POWER_WORD_SHIELD_NORMAL)
#define SPELL_SMITE             (IsUndead ? SPELL_SMITE_UNDEAD : SPELL_SMITE_NORMAL)

// Captain Rupert
#define SPELL_FEL_IRON_BOMB     (IsUndead ? SPELL_FEL_IRON_BOMB_UNDEAD : SPELL_FEL_IRON_BOMB_NORMAL)
#define SPELL_MACHINE_GUN       (IsUndead ? SPELL_MACHINE_GUN_UNDEAD : SPELL_MACHINE_GUN_NORMAL)
#define SPELL_ROCKET_LAUNCH     (IsUndead ? SPELL_ROCKET_LAUNCH_UNDEAD : SPELL_ROCKET_LAUNCH_NORMAL)

enum EventTypes
{
    EVENT_TIRION_INTRO_2                = 1,
    EVENT_TIRION_INTRO_3                = 2,
    EVENT_TIRION_INTRO_4                = 3,
    EVENT_TIRION_INTRO_5                = 4,
    EVENT_LK_INTRO_1                    = 5,
    EVENT_TIRION_INTRO_6                = 6,
    EVENT_LK_INTRO_2                    = 7,
    EVENT_LK_INTRO_3                    = 8,
    EVENT_LK_INTRO_4                    = 9,
    EVENT_BOLVAR_INTRO_1                = 10,
    EVENT_LK_INTRO_5                    = 11,
    EVENT_SAURFANG_INTRO_1              = 12,
    EVENT_TIRION_INTRO_H_7              = 13,
    EVENT_SAURFANG_INTRO_2              = 14,
    EVENT_SAURFANG_INTRO_3              = 15,
    EVENT_SAURFANG_INTRO_4              = 16,
    EVENT_SAURFANG_RUN                  = 17,
    EVENT_MURADIN_INTRO_1               = 18,
    EVENT_MURADIN_INTRO_2               = 19,
    EVENT_MURADIN_INTRO_3               = 20,
    EVENT_TIRION_INTRO_A_7              = 21,
    EVENT_MURADIN_INTRO_4               = 22,
    EVENT_MURADIN_INTRO_5               = 23,
    EVENT_MURADIN_RUN                   = 24,
    // Frost Freeze Trap
    EVENT_ACTIVATE_TRAP                 = 25,
    // Crok Scourgebane
    EVENT_SCOURGE_STRIKE                = 26,
    EVENT_DEATH_STRIKE                  = 27,
	EVENT_DEATH_COIL                    = 28,
    EVENT_HEALTH_CHECK                  = 29,
    EVENT_CROK_INTRO_3                  = 30,
    EVENT_START_PATHING                 = 31,
    // Sister Svalna
    EVENT_ARNATH_INTRO_2                = 32,
    EVENT_SVALNA_START                  = 33,
    EVENT_SVALNA_RESURRECT              = 34,
    EVENT_SVALNA_COMBAT                 = 35,
    EVENT_IMPALING_SPEAR                = 36,
    EVENT_AETHER_SHIELD                 = 37,
    // Captain Arnath
    EVENT_ARNATH_FLASH_HEAL             = 38,
    EVENT_ARNATH_PW_SHIELD              = 39,
    EVENT_ARNATH_SMITE                  = 40,
    EVENT_ARNATH_DOMINATE_MIND          = 41,
    // Captain Brandon
    EVENT_BRANDON_CRUSADER_STRIKE       = 42,
    EVENT_BRANDON_DIVINE_SHIELD         = 43,
    EVENT_BRANDON_JUDGEMENT_OF_COMMAND  = 44,
    EVENT_BRANDON_HAMMER_OF_BETRAYAL    = 45,
    // Captain Grondel
    EVENT_GRONDEL_CHARGE                = 46,
    EVENT_GRONDEL_MORTAL_STRIKE         = 47,
    EVENT_GRONDEL_SUNDER_ARMOR          = 48,
    EVENT_GRONDEL_CONFLAGRATION         = 49,
    // Captain Rupert
    EVENT_RUPERT_FEL_IRON_BOMB          = 50,
    EVENT_RUPERT_MACHINE_GUN            = 51,
    EVENT_RUPERT_ROCKET_LAUNCH          = 52,
	// Invisible Stalker (Float, Uninteractible, LargeAOI)
	EVENT_SOUL_MISSILE                  = 53,
	// Sindragosas Ward
	EVENT_SUB_WAVE_1                    = 54,
	EVENT_SUB_WAVE_2                    = 55,
	EVENT_UPDATE_CHECK                  = 56,
	// Rotting Frost Giant extended
	EVENT_FROST_GIANT_BLIZZARD          = 57,
	EVENT_FROST_GIANT_FRENZY            = 58,
};

enum DataTypesICC
{
    DATA_DAMNED_KILLS       = 1,
};

enum Actions
{
    // Sister Svalna
    ACTION_KILL_CAPTAIN         = 1,
    ACTION_START_GAUNTLET       = 2,
    ACTION_RESURRECT_CAPTAINS   = 3,
    ACTION_CAPTAIN_DIES         = 4,
    ACTION_RESET_EVENT          = 5,
};

enum EventIds
{
    EVENT_AWAKEN_WARD_1 = 22900,
    EVENT_AWAKEN_WARD_2 = 22907,
    EVENT_AWAKEN_WARD_3 = 22908,
    EVENT_AWAKEN_WARD_4 = 22909,
};

enum MovementPoints
{
    POINT_LAND = 1,
};

const Position SvalnaLandPos = { 4356.71f, 2484.33f, 358.5f, 1.571f };

const Position SindragosaGauntletSpawn[12] =
{
    { 4130.71f, 2484.10f, 211.033f, 0 },
    { 4137.93f, 2505.52f, 211.033f, 0 },
    { 4160.64f, 2528.13f, 211.033f, 0 },
    { 4180.81f, 2533.88f, 211.033f, 0 },
    { 4200.92f, 2527.18f, 211.033f, 0 },
    { 4222.23f, 2503.56f, 211.033f, 0 },
    { 4229.40f, 2484.63f, 211.033f, 0 },
    { 4222.01f, 2464.93f, 211.033f, 0 },
    { 4201.55f, 2441.03f, 211.033f, 0 },
    { 4181.29f, 2433.38f, 211.033f, 0 },
    { 4161.86f, 2441.94f, 211.033f, 0 },
    { 4138.78f, 2463.95f, 211.033f, 0 },
};

class FrostwingVrykulSearcher
{
    public:
        FrostwingVrykulSearcher(Creature const* source, float range) : _source(source), _range(range) {}

		Creature const* _source;
        float _range;

        bool operator()(Unit* unit)
        {
            if (!unit->IsAlive())
                return false;

            switch (unit->GetEntry())
            {
                case NPC_YMIRJAR_BATTLE_MAIDEN:
                case NPC_YMIRJAR_DEATHBRINGER:
                case NPC_YMIRJAR_FROSTBINDER:
                case NPC_YMIRJAR_HUNTRESS:
                case NPC_YMIRJAR_WARLORD:
                    break;
                default:
                    return false;
            }

            if (!unit->IsWithinDist(_source, _range, false))
                return false;

            return true;
        }
};

class FrostwingGauntletRespawner
{
    public:
        void operator()(Creature* creature)
        {
            switch (creature->GetOriginalEntry())
            {
                case NPC_YMIRJAR_BATTLE_MAIDEN:
                case NPC_YMIRJAR_DEATHBRINGER:
                case NPC_YMIRJAR_FROSTBINDER:
                case NPC_YMIRJAR_HUNTRESS:
                case NPC_YMIRJAR_WARLORD:
                    break;
                case NPC_CROK_SCOURGEBANE_PART_2:
                case NPC_CAPTAIN_ARNATH:
                case NPC_CAPTAIN_BRANDON:
                case NPC_CAPTAIN_GRONDEL:
                case NPC_CAPTAIN_RUPERT:
                    creature->AI()->DoAction(ACTION_RESET_EVENT);
                    break;
                case NPC_SISTER_SVALNA:
                    creature->AI()->DoAction(ACTION_RESET_EVENT);
                    return;
                default:
                    return;
            }

            uint32 corpseDelay = creature->GetCorpseDelay();
            uint32 respawnDelay = creature->GetRespawnDelay();
            creature->SetCorpseDelay(1);
            creature->SetRespawnDelay(2);

            if (CreatureData const* data = creature->GetCreatureData())
                creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);

            creature->DespawnAfterAction();

            creature->SetCorpseDelay(corpseDelay);
            creature->SetRespawnDelay(respawnDelay);
        }
};

class CaptainSurviveTalk : public BasicEvent
{
public:
	explicit CaptainSurviveTalk(Creature const& owner) : _owner(owner){}

	Creature const& _owner;

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		_owner.AI()->Talk(SAY_CAPTAIN_SURVIVE_TALK);
		return true;
	}
};

class npc_highlord_tirion_fordring_lh : public CreatureScript
{
public:
	npc_highlord_tirion_fordring_lh() : CreatureScript("npc_highlord_tirion_fordring_lh") { }

	struct npc_highlord_tirion_fordringAI : public QuantumBasicAI
	{
		npc_highlord_tirion_fordringAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript()) {}

		EventMap events;
		InstanceScript* const instance;
		uint64 NpcTheLichKing;
		uint64 NpcBolvarFordragon;
		uint64 FactionNpcFollow;
		uint16 DamnedKills;

		void Reset()
		{
			events.Reset();
			NpcTheLichKing = 0;
			NpcBolvarFordragon = 0;
			FactionNpcFollow = 0;
			DamnedKills = 0;
		}

		void SetData(uint32 type, uint32 data)
		{
			if (type == DATA_DAMNED_KILLS && data == 1)
			{
				if (++DamnedKills == 2)
				{
					if (Creature* TheLichKing = me->FindCreatureWithDistance(NPC_THE_LICH_KING_LH, 150.0f))
					{
						if (Creature* BolvarFordragon = me->FindCreatureWithDistance(NPC_HIGHLORD_BOLVAR_FORDRAGON_LH, 150.0f))
						{
							if (Creature* FactionNpc = me->FindCreatureWithDistance(instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE ? NPC_SE_HIGH_OVERLORD_SAURFANG : NPC_SE_MURADIN_BRONZEBEARD, 50.0f))
							{
								me->SetActive(true);
								NpcTheLichKing = TheLichKing->GetGUID();
								TheLichKing->SetActive(true);
								NpcBolvarFordragon = BolvarFordragon->GetGUID();
								BolvarFordragon->SetActive(true);
								FactionNpcFollow = FactionNpc->GetGUID();
								FactionNpc->SetActive(true);
							}
						}
					}

					if (!NpcBolvarFordragon || !NpcTheLichKing || !FactionNpcFollow)
						return;

					DoSendQuantumText(SAY_TIRION_INTRO_1, me);
					events.ScheduleEvent(EVENT_TIRION_INTRO_2, 4000);
					events.ScheduleEvent(EVENT_TIRION_INTRO_3, 14000);
					events.ScheduleEvent(EVENT_TIRION_INTRO_4, 18000);
					events.ScheduleEvent(EVENT_TIRION_INTRO_5, 31000);
					events.ScheduleEvent(EVENT_LK_INTRO_1, 35000);
					events.ScheduleEvent(EVENT_TIRION_INTRO_6, 51000);
					events.ScheduleEvent(EVENT_LK_INTRO_2, 58000);
					events.ScheduleEvent(EVENT_LK_INTRO_3, 74000);
					events.ScheduleEvent(EVENT_LK_INTRO_4, 86000);
					events.ScheduleEvent(EVENT_BOLVAR_INTRO_1, 110000); // 100000
					events.ScheduleEvent(EVENT_LK_INTRO_5, 120000); // 108000 

					if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
					{
						events.ScheduleEvent(EVENT_SAURFANG_INTRO_1, 120000);
						events.ScheduleEvent(EVENT_TIRION_INTRO_H_7, 129000);
						events.ScheduleEvent(EVENT_SAURFANG_INTRO_2, 139000);
						events.ScheduleEvent(EVENT_SAURFANG_INTRO_3, 150000);
						events.ScheduleEvent(EVENT_SAURFANG_INTRO_4, 162000);
						events.ScheduleEvent(EVENT_SAURFANG_RUN, 170000);
					}
					else
					{
						events.ScheduleEvent(EVENT_MURADIN_INTRO_1, 120000);
						events.ScheduleEvent(EVENT_MURADIN_INTRO_2, 124000);
						events.ScheduleEvent(EVENT_MURADIN_INTRO_3, 127000);
						events.ScheduleEvent(EVENT_TIRION_INTRO_A_7, 136000);
						events.ScheduleEvent(EVENT_MURADIN_INTRO_4, 144000);
						events.ScheduleEvent(EVENT_MURADIN_INTRO_5, 151000);
						events.ScheduleEvent(EVENT_MURADIN_RUN, 157000);
					}
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (DamnedKills != 2)
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
                    case EVENT_TIRION_INTRO_2:
						me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
						break;
					case EVENT_TIRION_INTRO_3:
						DoSendQuantumText(SAY_TIRION_INTRO_2, me);
						break;
					case EVENT_TIRION_INTRO_4:
						me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
						break;
					case EVENT_TIRION_INTRO_5:
						DoSendQuantumText(SAY_TIRION_INTRO_3, me);
						break;
                        case EVENT_LK_INTRO_1:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NO_SHEATHE);
                            if (Creature* TheLichKing = ObjectAccessor::GetCreature(*me, NpcTheLichKing))
								DoSendQuantumText(SAY_LK_INTRO_1, TheLichKing);
                            break;
                        case EVENT_TIRION_INTRO_6:
                            DoSendQuantumText(SAY_TIRION_INTRO_4, me);
                            break;
                        case EVENT_LK_INTRO_2:
                            if (Creature* TheLichKing = ObjectAccessor::GetCreature(*me, NpcTheLichKing))
								DoSendQuantumText(SAY_LK_INTRO_2, TheLichKing);
                            break;
                        case EVENT_LK_INTRO_3:
                            if (Creature* TheLichKing = ObjectAccessor::GetCreature(*me, NpcTheLichKing))
								DoSendQuantumText(SAY_LK_INTRO_3, TheLichKing);
                            break;
                        case EVENT_LK_INTRO_4:
                            if (Creature* TheLichKing = ObjectAccessor::GetCreature(*me, NpcTheLichKing))
                                DoSendQuantumText(SAY_LK_INTRO_4, TheLichKing);
                            break;
                        case EVENT_BOLVAR_INTRO_1:
                            if (Creature* BolvarFordragon = ObjectAccessor::GetCreature(*me, NpcBolvarFordragon))
                            {
								DoSendQuantumText(SAY_BOLVAR_INTRO_1, BolvarFordragon);
                                BolvarFordragon->SetActive(false);
                            }
                            break;
                        case EVENT_LK_INTRO_5:
                            if (Creature* TheLichKing = ObjectAccessor::GetCreature(*me, NpcTheLichKing))
                            {
								DoSendQuantumText(SAY_LK_INTRO_5, TheLichKing);
                                TheLichKing->SetActive(false);
                            }
                            break;
                        case EVENT_SAURFANG_INTRO_1:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_SAURFANG_INTRO_1, saurfang);
                            break;
                        case EVENT_TIRION_INTRO_H_7:
                            DoSendQuantumText(SAY_TIRION_INTRO_H_5, me);
                            break;
                        case EVENT_SAURFANG_INTRO_2:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_SAURFANG_INTRO_2, saurfang);
                            break;
                        case EVENT_SAURFANG_INTRO_3:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_SAURFANG_INTRO_3, saurfang);
                            break;
                        case EVENT_SAURFANG_INTRO_4:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_SAURFANG_INTRO_4, saurfang);
                            break;
                        case EVENT_MURADIN_RUN:
                        case EVENT_SAURFANG_RUN:
                            if (Creature* FactionNpc = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
                                FactionNpc->GetMotionMaster()->MovePath(FactionNpc->GetDBTableGUIDLow()*10, false);
                            me->SetActive(false);
                            DamnedKills = 3;
                            break;
                        case EVENT_MURADIN_INTRO_1:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_MURADIN_INTRO_1, muradin);
                            break;
                        case EVENT_MURADIN_INTRO_2:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
                                muradin->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            break;
                        case EVENT_MURADIN_INTRO_3:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
                                muradin->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            break;
                        case EVENT_TIRION_INTRO_A_7:
                            DoSendQuantumText(SAY_TIRION_INTRO_A_5, me);
                            break;
                        case EVENT_MURADIN_INTRO_4:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_MURADIN_INTRO_2, muradin);
                            break;
                        case EVENT_MURADIN_INTRO_5:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, FactionNpcFollow))
								DoSendQuantumText(SAY_MURADIN_INTRO_3, muradin);
                            break;
                        default:
                            break;
                    }
                }

				DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_highlord_tirion_fordringAI>(creature);
        }
};

class npc_rotting_frost_giant : public CreatureScript
{
public:
	npc_rotting_frost_giant() : CreatureScript("npc_rotting_frost_giant") { }

	struct npc_rotting_frost_giantAI : public QuantumBasicAI
	{
		npc_rotting_frost_giantAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript()){}

		InstanceScript* instance;

		uint32 CheckPositionTimer;
		uint32 StompTimer;
		uint32 DeathPlagueTimer;
		uint32 WhiteoutTimer;
		uint32 BlizzardTimer;
		uint32 FrenzyTimer;

		void Reset()
		{
			CheckPositionTimer = 1000;
			StompTimer = 3000;
			DeathPlagueTimer = 5000;
			WhiteoutTimer = 7000;
			BlizzardTimer = 9000;
			FrenzyTimer = 11000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CheckPositionTimer <= diff)
			{
				if (me->GetPositionZ() < 195.0f)
				{
					if (me->GetVictim())
					{
						me->Relocate(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ() + 1.0f);
						me->MonsterMoveWithSpeed(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ() + 1.0f, 0.0f);
					}
				}
				CheckPositionTimer = urand(3000, 4000);
			}
			else CheckPositionTimer -= diff;

			if (StompTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_STOMP_10, SPELL_STOMP_25, SPELL_STOMP_10, SPELL_STOMP_25), true);
				StompTimer = urand(6000, 7000);
			}
			else StompTimer -= diff;

			if (DeathPlagueTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEATH_PLAGUE);
					DoSendQuantumText(EMOTE_DEATH_PLAGUE_WARNING, me, target);
					DeathPlagueTimer = urand(9000, 10000);
				}
			}
			else DeathPlagueTimer -= diff;

			if (WhiteoutTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_WHITEOUT_10, SPELL_WHITEOUT_25, SPELL_WHITEOUT_10, SPELL_WHITEOUT_25), true);
				WhiteoutTimer = urand(12000, 13000);
			}
			else WhiteoutTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLIZZARD, true);
					BlizzardTimer = urand(15000, 16000);
				}
			}
			else BlizzardTimer -= diff;

			if (FrenzyTimer <= diff)
			{
				DoCast(me, SPELL_FRENZY);
				FrenzyTimer = urand(18000, 20000);
			}
			else FrenzyTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetIcecrownCitadelAI<npc_rotting_frost_giantAI>(creature);
	}
};

class npc_frost_freeze_trap : public CreatureScript
{
    public:
        npc_frost_freeze_trap() : CreatureScript("npc_frost_freeze_trap") { }

        struct npc_frost_freeze_trapAI: public QuantumBasicAI
        {
            npc_frost_freeze_trapAI(Creature* creature) : QuantumBasicAI(creature)
			{
				SetCombatMovement(false);
			}

			EventMap events;

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case 1000:
                    case 11000:
                        events.ScheduleEvent(EVENT_ACTIVATE_TRAP, uint32(action));
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_ACTIVATE_TRAP)
                {
                    DoCast(me, SPELL_COLDFLAME_JETS);
                    events.ScheduleEvent(EVENT_ACTIVATE_TRAP, 22000);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_frost_freeze_trapAI>(creature);
        }
};

class npc_alchemist_adrianna : public CreatureScript
{
public:
	npc_alchemist_adrianna() : CreatureScript("npc_alchemist_adrianna") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (!creature->FindCurrentSpellBySpellId(SPELL_HARVEST_BLIGHT_SPECIMEN) && !creature->FindCurrentSpellBySpellId(SPELL_HARVEST_BLIGHT_SPECIMEN25))
		{
			if (player->HasAura(SPELL_ORANGE_BLIGHT_RESIDUE) && player->HasAura(SPELL_GREEN_BLIGHT_RESIDUE))
				creature->CastSpell(creature, SPELL_HARVEST_BLIGHT_SPECIMEN, false);
		}
		return false;
	}
};

class boss_sister_svalna : public CreatureScript
{
    public:
        boss_sister_svalna() : CreatureScript("boss_sister_svalna") { }

        struct boss_sister_svalnaAI : public BossAI
        {
            boss_sister_svalnaAI(Creature* creature) : BossAI(creature, DATA_SISTER_SVALNA), IsEventInProgress(false) {}

			bool IsEventInProgress;

            void InitializeAI()
            {
                if (!me->IsDead())
                    Reset();

                me->SetReactState(REACT_PASSIVE);
            }

            void Reset()
            {
                _Reset();
                me->SetReactState(REACT_DEFENSIVE);
                IsEventInProgress = false;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
				DoCast(me, SPELL_IS_VALKYR, true);
				DoCast(me, SPELL_DIVINE_SURGE, true);

				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                if (Creature* crok = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CROK_SCOURGEBANE_PART_2)))
				{
                    crok->AI()->Talk(SAY_CROK_COMBAT_SVALNA);
					crok->AI()->AttackStart(me);
				}

                events.ScheduleEvent(EVENT_SVALNA_COMBAT, 9000);
                events.ScheduleEvent(EVENT_IMPALING_SPEAR, urand(40000, 50000));
                events.ScheduleEvent(EVENT_AETHER_SHIELD, urand(100000, 110000));
            }

            void KilledUnit(Unit* victim)
            {
                switch (victim->GetTypeId())
                {
                    case TYPE_ID_PLAYER:
                        Talk(SAY_SVALNA_KILL);
                        break;
                    case TYPE_ID_UNIT:
                        switch (victim->GetEntry())
                        {
                            case NPC_CAPTAIN_ARNATH:
                            case NPC_CAPTAIN_BRANDON:
                            case NPC_CAPTAIN_GRONDEL:
                            case NPC_CAPTAIN_RUPERT:
                                Talk(SAY_SVALNA_KILL_CAPTAIN);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                me->SetReactState(REACT_PASSIVE);
                me->SetDisableGravity(false);
                me->SetHover(false);
            }

			void JustDied(Unit* killer)
            {
                _JustDied();

                Talk(SAY_SVALNA_DEATH);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}

				if (Creature* crok = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CROK_SCOURGEBANE_PART_2)))
					 crok->AI()->Talk(SAY_CROK_WARNING_TO_RAID);

                uint64 delay = 1;
                for (uint32 i = 0; i < 4; ++i)
                {
                    if (Creature* crusader = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CAPTAIN_ARNATH + i)))
                    {
                        if (crusader->IsAlive() && crusader->GetEntry() == crusader->GetCreatureData()->id)
                        {
                            crusader->m_Events.AddEvent(new CaptainSurviveTalk(*crusader), crusader->m_Events.CalculateTime(delay));
                            delay += 6000;
                        }
                    }
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_KILL_CAPTAIN:
                        me->CastCustomSpell(SPELL_CARESS_OF_DEATH, SPELLVALUE_MAX_TARGETS, 1, me, true);
                        break;
                    case ACTION_START_GAUNTLET:
                        me->SetActive(true);
                        IsEventInProgress = true;
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        events.ScheduleEvent(EVENT_SVALNA_START, 25000);
                        break;
                    case ACTION_RESURRECT_CAPTAINS:
                        events.ScheduleEvent(EVENT_SVALNA_RESURRECT, 7000);
                        break;
                    case ACTION_CAPTAIN_DIES:
                        Talk(SAY_SVALNA_CAPTAIN_DEATH);
                        break;
                    case ACTION_RESET_EVENT:
                        me->SetActive(false);
                        Reset();
                        break;
                    default:
                        break;
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_HURL_SPEAR && me->HasAura(SPELL_AETHER_SHIELD))
                {
                    me->RemoveAurasDueToSpell(SPELL_AETHER_SHIELD);
                    Talk(EMOTE_SVALNA_BROKEN_SHIELD, caster->GetGUID());
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != EFFECT_MOTION_TYPE || id != POINT_LAND)
                    return;

                IsEventInProgress = false;
                me->SetActive(false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                me->SetDisableGravity(false);
                me->SetHover(false);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_IMPALING_SPEAR_KILL:
                        me->Kill(target);
                        break;
                    case SPELL_IMPALING_SPEAR:
                        if (TempSummon* summon = target->SummonCreature(NPC_IMPALING_SPEAR, *target))
                        {
                            Talk(EMOTE_SVALNA_IMPALE, target->GetGUID());
                            summon->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, target, false);
                            summon->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1 | UNIT_FLAG2_ALLOW_ENEMY_INTERACT);
							summon->CastSpell(summon, SPELL_SPEAR_VISUAL, true);
                        }
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() && !IsEventInProgress)
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SVALNA_START:
                            Talk(SAY_SVALNA_EVENT_START);
                            break;
                        case EVENT_SVALNA_RESURRECT:
                            Talk(SAY_SVALNA_RESURRECT_CAPTAINS);
                            me->CastSpell(me, SPELL_REVIVE_CHAMPION, false);
                            break;
                        case EVENT_SVALNA_COMBAT:
                            me->SetReactState(REACT_DEFENSIVE);
                            Talk(SAY_SVALNA_AGGRO);
                            break;
                        case EVENT_IMPALING_SPEAR:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, -SPELL_IMPALING_SPEAR))
                            {
                                DoCast(me, SPELL_AETHER_SHIELD, true);
                                DoCast(target, SPELL_IMPALING_SPEAR, true);
                            }
                            events.ScheduleEvent(EVENT_IMPALING_SPEAR, urand(20000, 25000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_sister_svalnaAI>(creature);
        }
};

class npc_crok_scourgebane : public CreatureScript
{
    public:
        npc_crok_scourgebane() : CreatureScript("npc_crok_scourgebane") { }

        struct npc_crok_scourgebaneAI : public npc_escortAI
        {
			npc_crok_scourgebaneAI(Creature* creature) : npc_escortAI(creature), instance(creature->GetInstanceScript()), RespawnTime(creature->GetRespawnDelay()), CorpseDelay(creature->GetCorpseDelay())
            {
                SetDespawnAtEnd(false);
                SetDespawnAtFar(false);
                IsEventActive = false;

                IsEventDone = instance->GetBossState(DATA_SISTER_SVALNA) == DONE;
                DidUnderTenPercentText = false;
            }

			EventMap events;
            InstanceScript* instance;

            uint32 CurrentWPid;
            uint32 WipeCheckTimer;
            uint32 const RespawnTime;
            uint32 const CorpseDelay;

			std::set<uint64> AliveTrash;

            bool IsEventActive;
            bool IsEventDone;
            bool DidUnderTenPercentText;

			bool CanAIAttack(Unit const* target) const
            {
                // do not see targets inside Frostwing Halls when we are not there
                return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
            }

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_SCOURGE_STRIKE, 2*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DEATH_STRIKE, 3*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_DEATH_COIL, 5*IN_MILLISECONDS);

                me->SetReactState(REACT_DEFENSIVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);

				IsEventActive = false;
				DidUnderTenPercentText = false;

				if (instance)
					IsEventDone = instance->GetBossState(DATA_SISTER_SVALNA) == DONE;

                WipeCheckTimer = 1000;

				AliveTrash.clear();

				CurrentWPid = 0;
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_GAUNTLET)
                {
                    if (IsEventDone || !me->IsAlive())
                        return;

                    IsEventActive = true;
                    IsEventDone = true;
                    // Load Grid with Sister Svalna
                    me->GetMap()->LoadGrid(4356.71f, 2484.33f);
                    if (Creature* svalna = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SISTER_SVALNA)))
                        svalna->AI()->DoAction(ACTION_START_GAUNTLET);

                    Talk(SAY_CROK_INTRO_1);
                    events.ScheduleEvent(EVENT_ARNATH_INTRO_2, 7000);
                    events.ScheduleEvent(EVENT_CROK_INTRO_3, 14000);
                    events.ScheduleEvent(EVENT_START_PATHING, 37000);
                    me->SetActive(true);
                    for (uint32 i = 0; i < 4; ++i)
					{
                        if (Creature* crusader = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CAPTAIN_ARNATH + i)))
                            crusader->AI()->DoAction(ACTION_START_GAUNTLET);
					}
                }
                else if (action == ACTION_RESET_EVENT)
                {
                    IsEventActive = false;
                    IsEventDone = instance->GetBossState(DATA_SISTER_SVALNA) == DONE;
                    me->SetActive(false);
                    AliveTrash.clear();
                    CurrentWPid = 0;
                }
            }

            void SetGUID(uint64 guid, int32 type/* = 0*/)
            {
                if (type == ACTION_VRYKUL_DEATH)
                {
                    AliveTrash.erase(guid);
                    if (AliveTrash.empty())
                    {
                        SetEscortPaused(false);
                        if (CurrentWPid == 4 && IsEventActive)
                        {
                            IsEventActive = false;
							me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetActive(false);
                            Talk(SAY_CROK_FINAL_WP);
                            if (Creature* svalna = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SISTER_SVALNA)))
                                svalna->AI()->DoAction(ACTION_RESURRECT_CAPTAINS);
                        }
                    }
                }
            }

            void WaypointReached(uint32 waypointId)
            {
                switch (waypointId)
                {
                    // pause pathing until trash pack is cleared
                    case 0:
                        Talk(SAY_CROK_COMBAT_WP_0);
                        if (!AliveTrash.empty())
                            SetEscortPaused(true);
                        break;
                    case 1:
                        Talk(SAY_CROK_COMBAT_WP_1);
                        if (!AliveTrash.empty())
                            SetEscortPaused(true);
                        break;
                    case 4:
                        if (AliveTrash.empty() && IsEventActive)
                        {
                            IsEventActive = false;
							me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetActive(false);
                            Talk(SAY_CROK_FINAL_WP);
                            if (Creature* svalna = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SISTER_SVALNA)))
                                svalna->AI()->DoAction(ACTION_RESURRECT_CAPTAINS);
                        }
                        break;
                    default:
                        break;
                }
            }

            void WaypointStart(uint32 waypointId)
            {
                CurrentWPid = waypointId;
                switch (waypointId)
                {
                    case 0:
                    case 1:
                    case 4:
                    {
                        // get spawns by home position
                        float minY = 2600.0f;
                        float maxY = 2650.0f;
                        if (waypointId == 1)
                        {
                            minY -= 50.0f;
                            maxY -= 50.0f;
                            // at waypoints 1 and 2 she kills one captain
                            if (Creature* svalna = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SISTER_SVALNA)))
                                svalna->AI()->DoAction(ACTION_KILL_CAPTAIN);
                        }
                        else if (waypointId == 4)
                        {
                            minY -= 100.0f;
                            maxY -= 100.0f;
                        }

                        // get all nearby vrykul
                        std::list<Creature*> temp;
                        FrostwingVrykulSearcher check(me, 80.0f);
                        Trinity::CreatureListSearcher<FrostwingVrykulSearcher> searcher(me, temp, check);
                        me->VisitNearbyGridObject(80.0f, searcher);

                        AliveTrash.clear();
                        for (std::list<Creature*>::iterator itr = temp.begin(); itr != temp.end(); ++itr)
						{
                            if ((*itr)->GetHomePosition().GetPositionY() < maxY && (*itr)->GetHomePosition().GetPositionY() > minY)
								AliveTrash.insert((*itr)->GetGUID());
						}
                        break;
                    }
                    // at waypoints 1 and 2 she kills one captain
                    case 2:
                        if (Creature* svalna = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SISTER_SVALNA)))
                            svalna->AI()->DoAction(ACTION_KILL_CAPTAIN);
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                // check wipe
                if (!WipeCheckTimer)
                {
                    WipeCheckTimer = 1000;
                    Player* player = NULL;
                    Trinity::AnyPlayerInObjectRangeCheck check(me, 60.0f);
                    Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, player, check);
                    me->VisitNearbyWorldObject(60.0f, searcher);
                    // wipe
                    if (!player)
                    {
                        damage *= 100;
                        if (damage >= me->GetHealth())
                        {
                            FrostwingGauntletRespawner respawner;
                            Trinity::CreatureWorker<FrostwingGauntletRespawner> worker(me, respawner);
                            me->VisitNearbyGridObject(333.0f, worker);
                            Talk(SAY_CROK_DEATH);
                        }
                        return;
                    }
                }

                if (HealthBelowPct(10) || damage >= me->GetHealth())
                {
                    if (!DidUnderTenPercentText)
                    {
                        DidUnderTenPercentText = true;

                        if (IsEventActive)
                            Talk(SAY_CROK_WEAKENING_GAUNTLET);
                        else
                            Talk(SAY_CROK_WEAKENING_SVALNA);
                    }

                    damage = 0;
                    DoCast(me, SPELL_ICEBOUND_ARMOR);
                    events.ScheduleEvent(EVENT_HEALTH_CHECK, 1000);
                }
            }

            void UpdateEscortAI(uint32 const diff)
            {
                if (WipeCheckTimer <= diff)
				{
                    WipeCheckTimer = 0;
				}
                else WipeCheckTimer -= diff;

                if (!UpdateVictim() && !IsEventActive)
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARNATH_INTRO_2:
                            if (Creature* arnath = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CAPTAIN_ARNATH)))
                                arnath->AI()->Talk(SAY_ARNATH_INTRO_2);
                            break;
                        case EVENT_CROK_INTRO_3:
                            Talk(SAY_CROK_INTRO_3);
                            break;
                        case EVENT_START_PATHING:
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            Start(true, true);
                            break;
                        case EVENT_SCOURGE_STRIKE:
                            DoCastVictim(SPELL_SCOURGE_STRIKE);
                            events.ScheduleEvent(EVENT_SCOURGE_STRIKE, urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            break;
                        case EVENT_DEATH_STRIKE:
                            if (HealthBelowPct(25))
                                DoCastVictim(SPELL_DEATH_STRIKE);
                            events.ScheduleEvent(EVENT_DEATH_STRIKE, urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            break;
						case EVENT_DEATH_COIL:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, SPELL_DEATH_COIL);
							events.ScheduleEvent(EVENT_DEATH_COIL, urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS));
							break;
                        case EVENT_HEALTH_CHECK:
                            if (HealthAbovePct(15))
							{
                                me->RemoveAurasDueToSpell(SPELL_ICEBOUND_ARMOR);
                                DidUnderTenPercentText = false;
							}
                            else
                            {
                                me->DealHeal(me, me->CountPctFromMaxHealth(HEALTH_PERCENT_5));
                                events.ScheduleEvent(EVENT_HEALTH_CHECK, 1*IN_MILLISECONDS);
                            }
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_crok_scourgebaneAI>(creature);
        }
};

struct npc_argent_captainAI : public QuantumBasicAI
{
    public:
        npc_argent_captainAI(Creature* creature) : QuantumBasicAI(creature), Instance(creature->GetInstanceScript()), FirstDeath(true)
        {
            FollowAngle = PET_FOLLOW_ANGLE;
            FollowDist = PET_FOLLOW_DIST;
            IsUndead = false;
        }

		EventMap Events;
        InstanceScript* Instance;

        float FollowAngle;
        float FollowDist;
        bool IsUndead;
		bool FirstDeath;

        void JustDied(Unit* killer)
        {
            if (FirstDeath)
            {
                FirstDeath = false;
                Talk(SAY_CAPTAIN_DEATH);
            }
            else Talk(SAY_CAPTAIN_SECOND_DEATH);

			// Quest credit
			if (Player* player = killer->ToPlayer())
			{
				player->CastSpell(player, SPELL_SOUL_FEAST, true);
				me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
			}
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPE_ID_PLAYER)
                Talk(SAY_CAPTAIN_KILL);
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_START_GAUNTLET)
            {
                if (Creature* crok = ObjectAccessor::GetCreature(*me, Instance->GetData64(DATA_CROK_SCOURGEBANE_PART_2)))
                {
                    me->SetReactState(REACT_DEFENSIVE);
                    FollowAngle = me->GetAngle(crok) + me->GetOrientation();
                    FollowDist = me->GetDistance2d(crok);
                    me->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
                }

                me->SetActive(true);
            }
            else if (action == ACTION_RESET_EVENT)
            {
                FirstDeath = true;
            }
        }

        void EnterToBattle(Unit* /*target*/)
        {
            me->SetHomePosition(*me);

            if (IsUndead)
                DoZoneInCombat();
        }

        bool CanAIAttack(Unit const* target) const
        {
            // do not see targets inside Frostwing Halls when we are not there
            return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
        }

        void EnterEvadeMode()
        {
			if (IsUndead)
				me->DespawnAfterAction();

            // not yet following
			if (me->GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_IDLE) != CHASE_MOTION_TYPE)
            {
                QuantumBasicAI::EnterEvadeMode();
                return;
            }

            if (!_EnterEvadeMode())
                return;

            if (!me->GetVehicle())
            {
                me->GetMotionMaster()->Clear(false);
                if (Creature* crok = ObjectAccessor::GetCreature(*me, Instance->GetData64(DATA_CROK_SCOURGEBANE_PART_2)))
                    me->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
            }

            Reset();
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_REVIVE_CHAMPION && !IsUndead)
            {
                IsUndead = true;
                me->setDeathState(JUST_RESPAWNED);
                uint32 newEntry = 0;
                switch (me->GetEntry())
                {
                    case NPC_CAPTAIN_ARNATH:
                        newEntry = NPC_CAPTAIN_ARNATH_UNDEAD;
                        break;
                    case NPC_CAPTAIN_BRANDON:
                        newEntry = NPC_CAPTAIN_BRANDON_UNDEAD;
                        break;
                    case NPC_CAPTAIN_GRONDEL:
                        newEntry = NPC_CAPTAIN_GRONDEL_UNDEAD;
                        break;
                    case NPC_CAPTAIN_RUPERT:
                        newEntry = NPC_CAPTAIN_RUPERT_UNDEAD;
                        break;
                    default:
                        return;
                }

                Talk(SAY_CAPTAIN_RESURRECTED);
                me->UpdateEntry(newEntry, Instance->GetData(DATA_TEAM_IN_INSTANCE), me->GetCreatureData());
                DoCast(me, SPELL_UNDEATH, true);
				me->SetReactState(REACT_AGGRESSIVE);
				DoZoneInCombat(me, 150.0f);
            }
        }
};

class npc_captain_arnath : public CreatureScript
{
    public:
        npc_captain_arnath() : CreatureScript("npc_captain_arnath") { }

        struct npc_captain_arnathAI : public npc_argent_captainAI
        {
            npc_captain_arnathAI(Creature* creature) : npc_argent_captainAI(creature){}

			Creature* FindFriendlyCreature() const
            {
                Creature* target = NULL;
                Trinity::MostHPMissingInRange u_check(me, 60.0f, 0);
                Trinity::CreatureLastSearcher<Trinity::MostHPMissingInRange> searcher(me, target, u_check);
                me->VisitNearbyGridObject(60.0f, searcher);
                return target;
            }

            void Reset()
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_ARNATH_FLASH_HEAL, 2*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_ARNATH_PW_SHIELD, 4*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_ARNATH_SMITE, 0.5*IN_MILLISECONDS);
                if (Is25ManRaid() && IsUndead)
                    Events.ScheduleEvent(EVENT_ARNATH_DOMINATE_MIND, urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS));

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARNATH_FLASH_HEAL:
                            if (Creature* target = FindFriendlyCreature())
                                DoCast(target, SPELL_FLASH_HEAL, true);
                            Events.ScheduleEvent(EVENT_ARNATH_FLASH_HEAL, urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            break;
                        case EVENT_ARNATH_PW_SHIELD:
                        {
                            std::list<Creature*> targets = DoFindFriendlyMissingBuff(40.0f, SPELL_POWER_WORD_SHIELD);
                            DoCast(Quantum::DataPackets::SelectRandomContainerElement(targets), SPELL_POWER_WORD_SHIELD);
                            Events.ScheduleEvent(EVENT_ARNATH_PW_SHIELD, urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                            break;
                        }
                        case EVENT_ARNATH_SMITE:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, SPELL_SMITE);
                            Events.ScheduleEvent(EVENT_ARNATH_SMITE, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS));
                            break;
                        case EVENT_ARNATH_DOMINATE_MIND:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_DOMINATE_MIND);
                            Events.ScheduleEvent(EVENT_ARNATH_DOMINATE_MIND, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_captain_arnathAI>(creature);
        }
};

class npc_captain_brandon : public CreatureScript
{
    public:
        npc_captain_brandon() : CreatureScript("npc_captain_brandon") { }

        struct npc_captain_brandonAI : public npc_argent_captainAI
        {
            npc_captain_brandonAI(Creature* creature) : npc_argent_captainAI(creature){}

            void Reset()
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_BRANDON_CRUSADER_STRIKE, 2*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 0.5*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_BRANDON_JUDGEMENT_OF_COMMAND, 3*IN_MILLISECONDS);
                if (IsUndead)
                    Events.ScheduleEvent(EVENT_BRANDON_HAMMER_OF_BETRAYAL, 9*IN_MILLISECONDS);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BRANDON_CRUSADER_STRIKE:
                            DoCastVictim(SPELL_CRUSADER_STRIKE);
                            Events.ScheduleEvent(EVENT_BRANDON_CRUSADER_STRIKE, urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            break;
                        case EVENT_BRANDON_DIVINE_SHIELD:
                            if (HealthBelowPct(20))
                                DoCast(me, SPELL_DIVINE_SHIELD);
                            else
								Events.ScheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 0.5*IN_MILLISECONDS);
                            break;
                        case EVENT_BRANDON_JUDGEMENT_OF_COMMAND:
                            DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
                            Events.ScheduleEvent(EVENT_BRANDON_JUDGEMENT_OF_COMMAND, urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            break;
                        case EVENT_BRANDON_HAMMER_OF_BETRAYAL:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_HAMMER_OF_BETRAYAL);
                            Events.ScheduleEvent(EVENT_BRANDON_HAMMER_OF_BETRAYAL, urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_captain_brandonAI>(creature);
        }
};

class npc_captain_grondel : public CreatureScript
{
    public:
        npc_captain_grondel() : CreatureScript("npc_captain_grondel") { }

        struct npc_captain_grondelAI : public npc_argent_captainAI
        {
            npc_captain_grondelAI(Creature* creature) : npc_argent_captainAI(creature){}

            void Reset()
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_GRONDEL_CHARGE, 0.2*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_GRONDEL_MORTAL_STRIKE, 2*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_GRONDEL_SUNDER_ARMOR, 4*IN_MILLISECONDS);
                if (IsUndead)
                    Events.ScheduleEvent(EVENT_GRONDEL_CONFLAGRATION, 10*IN_MILLISECONDS);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
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

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GRONDEL_CHARGE:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, SPELL_CHARGE);
                            Events.ScheduleEvent(EVENT_GRONDEL_CHARGE, 4*IN_MILLISECONDS);
                            break;
                        case EVENT_GRONDEL_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            Events.ScheduleEvent(EVENT_GRONDEL_MORTAL_STRIKE, urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS));
                            break;
                        case EVENT_GRONDEL_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            Events.ScheduleEvent(EVENT_GRONDEL_SUNDER_ARMOR, urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            break;
                        case EVENT_GRONDEL_CONFLAGRATION:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_CONFLAGRATION);
                            Events.ScheduleEvent(EVENT_GRONDEL_CONFLAGRATION, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_captain_grondelAI>(creature);
        }
};

class npc_captain_rupert : public CreatureScript
{
    public:
        npc_captain_rupert() : CreatureScript("npc_captain_rupert") { }

        struct npc_captain_rupertAI : public npc_argent_captainAI
        {
            npc_captain_rupertAI(Creature* creature) : npc_argent_captainAI(creature){}

            void Reset()
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_RUPERT_FEL_IRON_BOMB, 3*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_RUPERT_MACHINE_GUN, 5*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_RUPERT_ROCKET_LAUNCH, 7*IN_MILLISECONDS);

				me->SetPowerType(POWER_ENERGY);
				me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);

				DoCast(me, SPELL_DUAL_WIELD_CROK);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RUPERT_FEL_IRON_BOMB:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FEL_IRON_BOMB);
                            Events.ScheduleEvent(EVENT_RUPERT_FEL_IRON_BOMB, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            break;
                        case EVENT_RUPERT_MACHINE_GUN:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                                DoCast(target, SPELL_MACHINE_GUN);
                            Events.ScheduleEvent(EVENT_RUPERT_MACHINE_GUN, urand(25*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            break;
                        case EVENT_RUPERT_ROCKET_LAUNCH:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                                DoCast(target, SPELL_ROCKET_LAUNCH);
                            Events.ScheduleEvent(EVENT_RUPERT_ROCKET_LAUNCH, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_captain_rupertAI>(creature);
        }
};

class npc_impaling_spear : public CreatureScript
{
    public:
        npc_impaling_spear() : CreatureScript("npc_impaling_spear") { }

        struct npc_impaling_spearAI : public CreatureAI
        {
            npc_impaling_spearAI(Creature* creature) : CreatureAI(creature){}

			uint32 VehicleCheckTimer;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                VehicleCheckTimer = 0.5*IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const diff)
            {
                if (VehicleCheckTimer <= diff)
                {
                    VehicleCheckTimer = 0.5*IN_MILLISECONDS;
                    if (!me->GetVehicle())
                        me->DespawnAfterAction(0.1*IN_MILLISECONDS);
                }
                else VehicleCheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_impaling_spearAI(creature);
        }
};

class npc_arthas_teleport_visual : public CreatureScript
{
    public:
        npc_arthas_teleport_visual() : CreatureScript("npc_arthas_teleport_visual") { }

        struct npc_arthas_teleport_visualAI : public NullCreatureAI
        {
            npc_arthas_teleport_visualAI(Creature* creature) : NullCreatureAI(creature), instance(creature->GetInstanceScript()){}

			InstanceScript* instance;
            EventMap events;

            void Reset()
            {
                events.Reset();

                if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE &&instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && instance->GetBossState(DATA_SINDRAGOSA) == DONE)
					events.ScheduleEvent(EVENT_SOUL_MISSILE, urand(1000, 6000));
            }

            void UpdateAI(uint32 const diff)
            {
                if (events.Empty())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SOUL_MISSILE)
                {
                    DoCastAOE(SPELL_SOUL_MISSILE);
                    events.ScheduleEvent(EVENT_SOUL_MISSILE, urand(5000, 7000));
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            // Distance from the center of the spire
            if (creature->GetExactDist2d(4357.052f, 2769.421f) < 100.0f && creature->GetHomePosition().GetPositionZ() < 315.0f)
                return GetIcecrownCitadelAI<npc_arthas_teleport_visualAI>(creature);

            // Default to no script
            return NULL;
        }
};

class spell_icc_stoneform : public SpellScriptLoader
{
    public:
        spell_icc_stoneform() : SpellScriptLoader("spell_icc_stoneform") { }

        class spell_icc_stoneform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_icc_stoneform_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* target = GetTarget()->ToCreature())
                {
                    target->SetReactState(REACT_PASSIVE);
                    target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    target->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CUSTOM_SPELL_02);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* target = GetTarget()->ToCreature())
                {
                    target->SetReactState(REACT_AGGRESSIVE);
                    target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    target->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_icc_stoneform_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_icc_stoneform_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_icc_stoneform_AuraScript();
        }
};

class spell_icc_sprit_alarm : public SpellScriptLoader
{
    public:
        spell_icc_sprit_alarm() : SpellScriptLoader("spell_icc_sprit_alarm") { }

        class spell_icc_sprit_alarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_sprit_alarm_SpellScript);

            void HandleEvent(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 trapId = 0;
                switch (GetSpellInfo()->Effects[effIndex].MiscValue)
                {
                    case EVENT_AWAKEN_WARD_1:
                        trapId = GO_SPIRIT_ALARM_1;
                        break;
                    case EVENT_AWAKEN_WARD_2:
                        trapId = GO_SPIRIT_ALARM_2;
                        break;
                    case EVENT_AWAKEN_WARD_3:
                        trapId = GO_SPIRIT_ALARM_3;
                        break;
                    case EVENT_AWAKEN_WARD_4:
                        trapId = GO_SPIRIT_ALARM_4;
                        break;
                    default:
                        return;
                }

                if (GameObject* trap = GetCaster()->FindGameobjectWithDistance(trapId, 5.0f))
					trap->SetRespawnTime(trap->GetGOInfo()->GetAutoCloseTime());

                std::list<Creature*> wards;
                GetCaster()->GetCreatureListWithEntryInGrid(wards, NPC_DEATHBOUND_WARD, 150.0f);
                wards.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
                for (std::list<Creature*>::iterator itr = wards.begin(); itr != wards.end(); ++itr)
                {
                    if ((*itr)->IsAlive() && (*itr)->HasAura(SPELL_STONEFORM))
                    {
						DoSendQuantumText(RAND(SAY_TRAP_ACTIVATE_1, SAY_TRAP_ACTIVATE_2, SAY_TRAP_ACTIVATE_3), *itr);
                        (*itr)->RemoveAurasDueToSpell(SPELL_STONEFORM);
                        if (Unit* target = (*itr)->SelectNearestTarget(150.0f))
                            (*itr)->AI()->AttackStart(target);
                        break;
                    }
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_icc_sprit_alarm_SpellScript::HandleEvent, EFFECT_2, SPELL_EFFECT_SEND_EVENT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_sprit_alarm_SpellScript();
        }
};

class spell_icc_soul_missile : public SpellScriptLoader
{
    public:
        spell_icc_soul_missile() : SpellScriptLoader("spell_icc_soul_missile") { }

        class spell_icc_soul_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_soul_missile_SpellScript);

            void RelocateDest()
            {
                static Position const offset = {0.0f, 0.0f, 200.0f, 0.0f};
                const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
            }

            void Register()
            {
                OnCast += SpellCastFn(spell_icc_soul_missile_SpellScript::RelocateDest);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_soul_missile_SpellScript();
        }
};

enum Wrapping
{
	SPELL_WEB_WRAP_CASTER = 70980,
	SPELL_WEB_WRAP_EEFECT = 71009,
	SPELL_WEB_WRAP_STUN   = 71010,
};

class spell_icc_web_wrap : public SpellScriptLoader
{
    public:
        spell_icc_web_wrap() : SpellScriptLoader("spell_icc_web_wrap") {}

        class spell_icc_web_wrap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_web_wrap_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WEB_WRAP_CASTER))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster();

				if (Player* caster = GetHitPlayer())
					caster->CastSpell(caster, SPELL_WEB_WRAP_STUN, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_icc_web_wrap_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_web_wrap_SpellScript();
        }
};

class DeathPlagueTargetSelector
{
    public:
        explicit DeathPlagueTargetSelector(Unit* caster) : _caster(caster) {}

        bool operator()(Unit* unit)
        {
            if (unit == _caster)
                return true;

            if (unit->GetTypeId() != TYPE_ID_PLAYER)
                return true;

            if (unit->HasAura(SPELL_RECENTLY_INFECTED) || unit->HasAura(SPELL_DEATH_PLAGUE_AURA))
                return true;

            return false;
        }

    private:
        Unit* _caster;
};

class spell_frost_giant_death_plague : public SpellScriptLoader
{
    public:
        spell_frost_giant_death_plague() : SpellScriptLoader("spell_frost_giant_death_plague") { }

        class spell_frost_giant_death_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frost_giant_death_plague_SpellScript);

			bool Failed;

            bool Load()
            {
                Failed = false;
                return true;
            }

            // First effect
            void CountTargets(std::list<Unit*>& unitList)
            {
                unitList.remove(GetCaster());
                Failed = unitList.empty();
            }

            // Second effect
            void FilterTargets(std::list<Unit*>& unitList)
            {
                // Select valid targets for jump
                unitList.remove_if (DeathPlagueTargetSelector(GetCaster()));
                if (!unitList.empty())
                {
                    Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                    unitList.clear();
                    unitList.push_back(target);
                }

                unitList.push_back(GetCaster());
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (GetHitUnit() != GetCaster())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_DEATH_PLAGUE_AURA, true);
                else if (Failed)
                    GetCaster()->CastSpell(GetCaster(), SPELL_DEATH_PLAGUE_KILL, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_frost_giant_death_plague_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_frost_giant_death_plague_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_frost_giant_death_plague_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_frost_giant_death_plague_SpellScript();
        }
};

class spell_icc_harvest_blight_specimen : public SpellScriptLoader
{
    public:
        spell_icc_harvest_blight_specimen() : SpellScriptLoader("spell_icc_harvest_blight_specimen") { }

        class spell_icc_harvest_blight_specimen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_harvest_blight_specimen_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void HandleQuestComplete(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_icc_harvest_blight_specimen_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnEffectHitTarget += SpellEffectFn(spell_icc_harvest_blight_specimen_SpellScript::HandleQuestComplete, EFFECT_1, SPELL_EFFECT_QUEST_COMPLETE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_harvest_blight_specimen_SpellScript();
        }
};

class AliveCheck
{
public:
	bool operator()(Unit* unit)
	{
		return unit->IsAlive();
	}
};

class spell_svalna_revive_champion : public SpellScriptLoader
{
    public:
        spell_svalna_revive_champion() : SpellScriptLoader("spell_svalna_revive_champion") { }

        class spell_svalna_revive_champion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_svalna_revive_champion_SpellScript);

            void RemoveAliveTarget(std::list<Unit*>& unitList)
            {
                unitList.remove_if(AliveCheck());
                Quantum::DataPackets::RandomResizeList(unitList, 2);
            }

            void Land(SpellEffIndex /*effIndex*/)
            {
                Creature* caster = GetCaster()->ToCreature();
                if (!caster)
                    return;

                Position pos;
                caster->GetPosition(&pos);
                caster->GetNearPosition(pos, 5.0f, 0.0f);
                caster->SetHomePosition(pos);
                caster->GetMotionMaster()->MoveLand(POINT_LAND, pos);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_svalna_revive_champion_SpellScript::RemoveAliveTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                OnEffectHit += SpellEffectFn(spell_svalna_revive_champion_SpellScript::Land, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_svalna_revive_champion_SpellScript();
        }
};

class spell_svalna_remove_spear : public SpellScriptLoader
{
    public:
        spell_svalna_remove_spear() : SpellScriptLoader("spell_svalna_remove_spear") { }

        class spell_svalna_remove_spear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_svalna_remove_spear_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Creature* target = GetHitCreature())
                {
                    if (Unit* vehicle = target->GetVehicleBase())
                        vehicle->RemoveAurasDueToSpell(SPELL_IMPALING_SPEAR);

                    target->DespawnAfterAction(1);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_svalna_remove_spear_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_svalna_remove_spear_SpellScript();
        }
};

class at_icc_saurfang_portal : public AreaTriggerScript
{
public:
	at_icc_saurfang_portal() : AreaTriggerScript("at_icc_saurfang_portal") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
	{
		InstanceScript* instance = player->GetInstanceScript();
		if (!instance || instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
			return true;

		player->TeleportTo(631, 4126.35f, 2769.23f, 350.963f, 0.0f);

		if (instance->GetData(DATA_COLDFLAME_JETS) == NOT_STARTED)
		{
			player->GetMap()->PlayerRelocation(player, 4126.35f, 2769.23f, 350.963f, 0.0f);

			instance->SetData(DATA_COLDFLAME_JETS, IN_PROGRESS);
			std::list<Creature*> traps;
			GetCreatureListWithEntryInGrid(traps, player, NPC_FROST_FREEZE_TRAP, 120.0f);
			traps.sort(Trinity::ObjectDistanceOrderPred(player));
			bool instant = false;
			for (std::list<Creature*>::iterator itr = traps.begin(); itr != traps.end(); ++itr)
			{
				(*itr)->AI()->DoAction(instant ? 1000 : 11000);
				instant = !instant;
			}
		}
		return true;
	}
};

class at_icc_shutdown_traps : public AreaTriggerScript
{
public:
	at_icc_shutdown_traps() : AreaTriggerScript("at_icc_shutdown_traps") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
	{
		if (InstanceScript* instance = player->GetInstanceScript())
			instance->SetData(DATA_COLDFLAME_JETS, DONE);
		return true;
	}
};

class at_icc_start_blood_quickening : public AreaTriggerScript
{
public:
	at_icc_start_blood_quickening() : AreaTriggerScript("at_icc_start_blood_quickening") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
	{
		if (InstanceScript* instance = player->GetInstanceScript())
		{
			if (instance->GetData(DATA_BLOOD_QUICKENING_STATE) == NOT_STARTED)
				instance->SetData(DATA_BLOOD_QUICKENING_STATE, IN_PROGRESS);
		}
		return true;
	}
};

class at_icc_start_frostwing_gauntlet : public AreaTriggerScript
{
public:
	at_icc_start_frostwing_gauntlet() : AreaTriggerScript("at_icc_start_frostwing_gauntlet") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
	{
		if (InstanceScript* instance = player->GetInstanceScript())
		{
			if (Creature* crok = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_CROK_SCOURGEBANE_PART_2)))
				crok->AI()->DoAction(ACTION_START_GAUNTLET);
		}
		return true;
	}
};

class npc_sindragosas_ward : public CreatureScript
{
    public:
        npc_sindragosas_ward() : CreatureScript("npc_sindragosas_ward") { }

        struct npc_sindragosas_wardAI : public BossAI
        {
            npc_sindragosas_wardAI(Creature* creature) : BossAI(creature, DATA_SINDRAGOSA_GAUNTLET){}

			bool IsEventInProgressOrDone;

            uint32 SpawnCountToBeSummonedInWave;
            uint8 WaveNumber;

            void Reset()
            {
                _Reset();
                IsEventInProgressOrDone = false;
                SpawnCountToBeSummonedInWave = 0;
                WaveNumber = 0;
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_GAUNTLET)
				{
                    if (!IsEventInProgressOrDone)
					{
                        if (!IsAnyPlayerOutOfRange())
						{
							me->PlayDirectSound(17167);
                            DoZoneInCombat(me, 150.0f);
						}
					}
				}
            }

            void EnterToBattle(Unit* /*attacker*/)
            {
                _EnterToBattle();

                IsEventInProgressOrDone = true;

                SpawnCountToBeSummonedInWave = 32;

                WaveNumber = 1;

                DoSummonWave(WaveNumber);

                events.ScheduleEvent(EVENT_SUB_WAVE_1, 10000);
                events.ScheduleEvent(EVENT_SUB_WAVE_2, 25000);
                events.ScheduleEvent(EVENT_UPDATE_CHECK, 5000);
            }

            void DoSummonWave(uint8 number)
            {
                switch (number)
                {
                    case 1:
                    case 3:
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[4], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[7], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[10], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[2], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[5], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[8], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[11], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        break;
                    case 2:
                        me->SummonCreature(NPC_FROSTWARDEN_SORCERESS, SindragosaGauntletSpawn[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_FROSTWARDEN_SORCERESS, SindragosaGauntletSpawn[9], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[9], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[9], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        break;
                    case EVENT_SUB_WAVE_1:
                    case EVENT_SUB_WAVE_2:
                        for (uint8 i = 0; i < 12; i++)
                            me->SummonCreature(NPC_NERUBAR_BROODLING, SindragosaGauntletSpawn[i], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                        break;
                    default:
                        break;
                }
            }

            bool IsAnyPlayerOutOfRange()
            {
                if (!me->GetMap())
                    return true;

                Map::PlayerList const& playerList = me->GetMap()->GetPlayers();

                if (playerList.isEmpty())
                    return true;

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if (Player* player = itr->getSource())
                    {
                        if (player->IsGameMaster())
                            continue;

                        if (player->IsAlive() && me->GetDistance(player) > 125.0f)
                            return true;
                    }
                }

                return false;
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                DoZoneInCombat(summon, 150.0f);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*who*/)
            {
                SpawnCountToBeSummonedInWave--;
                summon->DespawnAfterAction(30*IN_MILLISECONDS);
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                if (summon->IsAlive())
                {
                    EnterEvadeMode();
                    return;
                }

                summons.Despawn(summon);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !IsEventInProgressOrDone)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUB_WAVE_1:
                            DoSummonWave(EVENT_SUB_WAVE_1);
                            break;
                        case EVENT_SUB_WAVE_2:
                            DoSummonWave(EVENT_SUB_WAVE_2);
                            break;
                        case EVENT_UPDATE_CHECK:
                        {
                            if (SpawnCountToBeSummonedInWave <= 5)
                            {
                                if (summons.size() < SpawnCountToBeSummonedInWave)
                                    SpawnCountToBeSummonedInWave = summons.size();

                                if (!SpawnCountToBeSummonedInWave)
                                {
                                    switch (WaveNumber)
                                    {
                                        case 1:
                                            SpawnCountToBeSummonedInWave += 30;
                                            break;
                                        case 2:
                                            SpawnCountToBeSummonedInWave += 32;
                                            break;
                                        case 3:
                                            me->Kill(me);
                                            return;
                                    }

                                    WaveNumber++;
                                    DoSummonWave(WaveNumber);
                                    events.ScheduleEvent(EVENT_SUB_WAVE_1, 10000);
                                    events.ScheduleEvent(EVENT_SUB_WAVE_2, 25000);
                                }
                            }

                            if (IsAnyPlayerOutOfRange())
                            {
                                EnterEvadeMode();
                                return;
                            }

                            events.ScheduleEvent(EVENT_UPDATE_CHECK, 5000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_sindragosas_wardAI>(creature);
        }
};

enum PutricidesTrap
{
	EVENT_PREPARE_TO_START      = 1,
	EVENT_START_INSECTING       = 2,
	EVENT_SUMMON_SPIDERS        = 3,
	EVENT_END_INSECTING         = 4,

	SOUND_INSECTING_SWARM       = 17125,

	SPELL_GIANT_INSECT_SWARM    = 70475,
	SPELL_SUMMON_PLAGUE_INSECTS = 70484,
};

class npc_putricides_trap : public CreatureScript
{
public:
    npc_putricides_trap() : CreatureScript("npc_putricides_trap") { }

	struct npc_putricides_trapAI : public QuantumBasicAI
    {
		npc_putricides_trapAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			instance = creature->GetInstanceScript();

			SetCombatMovement(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}

		EventMap events;
		InstanceScript* instance;
		SummonList Summons;

		bool Insected;

        void Reset()
        {
			events.Reset();

			Summons.DespawnAll();

			Insected = false;
        }

		void JustDied(Unit* /*killer*/){}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_FLESH_EATING_INSECT)
				Summons.Summon(summon);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!instance || Insected || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 20.0f))
				return;

			if (instance && Insected == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 20.0f))
			{
				me->AI()->DoZoneInCombat();
				me->PlayDirectSound(SOUND_INSECTING_SWARM);

				events.ScheduleEvent(EVENT_PREPARE_TO_START, 2*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_END_INSECTING, 2*MINUTE*IN_MILLISECONDS);
				Insected = true;
			}
		}

		void CloseAllDoors()
		{
			if (GameObject* ProfDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_ENTRANCE, 250.0f))
				ProfDoor->SetGoState(GO_STATE_READY);

			if (GameObject* CollisionDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_AIRLOCK_DOOR_COLLISION, 250.0f))
				CollisionDoor->SetGoState(GO_STATE_READY);

			if (GameObject* OrangeDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_AIRLOCK_DOOR_ORANGE, 250.0f))
				OrangeDoor->SetGoState(GO_STATE_READY);

			if (GameObject* GreenDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_AIRLOCK_DOOR_GREEN, 250.0f))
				GreenDoor->SetGoState(GO_STATE_READY);
		}

		void OpenAllDoors()
		{
			if (GameObject* ProfDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_ENTRANCE, 250.0f))
				ProfDoor->SetGoState(GO_STATE_ACTIVE);

			if (GameObject* CollisionDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_AIRLOCK_DOOR_COLLISION, 250.0f))
				CollisionDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

			if (GameObject* OrangeDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_AIRLOCK_DOOR_ORANGE, 250.0f))
				OrangeDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

			if (GameObject* GreenDoor = me->FindGameobjectWithDistance(GO_SCIENTIST_AIRLOCK_DOOR_GREEN, 250.0f))
				GreenDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
		}

		void JustReachedHome()
		{
			events.CancelEvent(EVENT_PREPARE_TO_START);
			events.CancelEvent(EVENT_END_INSECTING);

			OpenAllDoors();

			me->DisappearAndDie();

			me->Respawn();
		}

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_PREPARE_TO_START:
						CloseAllDoors();
						events.ScheduleEvent(EVENT_START_INSECTING, 2*IN_MILLISECONDS);
						break;
			        case EVENT_START_INSECTING:
						DoCastAOE(SPELL_GIANT_INSECT_SWARM);
						events.ScheduleEvent(EVENT_SUMMON_SPIDERS, 2*IN_MILLISECONDS);
						break;
					case EVENT_SUMMON_SPIDERS:
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						me->SummonCreature(NPC_FLESH_EATING_INSECT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_START_INSECTING, 10*IN_MILLISECONDS);
						break;
					case EVENT_END_INSECTING:
						OpenAllDoors();
						Summons.DespawnAll();
						me->DisappearAndDie();
						break;
					default:
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_putricides_trapAI(creature);
	}
};

enum TeleportEvent
{
	EVENT_TELEPORT_SUMMON = 1,
	EVENT_SPEECH_BRANDON  = 2,
	EVENT_SPEECH_GRONDEL  = 3,
	EVENT_SPEECH_RUPERT   = 4,
	EVENT_SPEECH_CROK     = 5,
	EVENT_MOVE_TO_BATTLE  = 6,

	SPELL_VISUAL_TELEPORT = 60427,

	SAY_BRANDON_TELEPORT  = -1609587,
	SAY_GRONDEL_TELEPORT  = -1609588,
	SAY_RUPERT_TELEPORT   = -1609589,
	SAY_CROK_TELEPORT     = -1609590,
};

Position const CrokToBattlePos[1] =
{
	{4265.26f, 2769.05f, 349.385f, 6.25611f},
};

class npc_icecrown_citadel_teleport_trigger : public CreatureScript
{
public:
    npc_icecrown_citadel_teleport_trigger() : CreatureScript("npc_icecrown_citadel_teleport_trigger") { }

	struct npc_icecrown_citadel_teleport_triggerAI : public QuantumBasicAI
    {
		npc_icecrown_citadel_teleport_triggerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			FollowAngle = PET_FOLLOW_ANGLE;
			FollowDist = PET_FOLLOW_DIST;
		}

		EventMap events;
		InstanceScript* instance;

		float FollowAngle;
        float FollowDist;

		bool Started;

        void Reset()
        {
			events.Reset();

			Started = false;
        }

		void JustDied(Unit* /*killer*/){}

		void MoveInLineOfSight(Unit* who)
		{
			if (!instance || Started || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 36.0f))
				return;

			if (instance && Started == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 36.0f))
			{
				me->PlayDirectSound(17329);

				events.ScheduleEvent(EVENT_TELEPORT_SUMMON, 3*IN_MILLISECONDS);
				Started = true;
			}
		}

        void UpdateAI(const uint32 diff)
        {
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_TELEPORT_SUMMON:

						if (Creature* brandon = me->SummonCreature(NPC_CAPTAIN_BRANDON, 4203.14f, 2772.34f, 350.963f, 0.0112128f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS))
						{
							brandon->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
							brandon->CastSpell(brandon, SPELL_VISUAL_TELEPORT, true);
						}

						if (Creature* arnath = me->SummonCreature(NPC_CAPTAIN_ARNATH, 4199.5f, 2772.33f, 351.257f, 0.0277061f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS))
						{
							arnath->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
							arnath->CastSpell(arnath, SPELL_VISUAL_TELEPORT, true);
						}

						if (Creature* grondel = me->SummonCreature(NPC_CAPTAIN_GRONDEL, 4203.16f, 2766.71f, 350.964f, 0.00414419f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS))
						{
							grondel->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
							grondel->CastSpell(grondel, SPELL_VISUAL_TELEPORT, true);
						}

						if (Creature* rupert = me->SummonCreature(NPC_CAPTAIN_RUPERT, 4199.4f, 2766.48f, 351.215f, 0.015924f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS))
						{
							rupert->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);
							rupert->CastSpell(rupert, SPELL_VISUAL_TELEPORT, true);
						}

						if (Creature* crok = me->SummonCreature(NPC_CROK_SCOURGEBANE_PART_1, 4221.87f, 2769.16f, 350.963f, 0.00432539f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS))
							crok->CastSpell(crok, SPELL_VISUAL_TELEPORT, true);

						events.ScheduleEvent(EVENT_SPEECH_BRANDON, 4*IN_MILLISECONDS);
						break;
			        case EVENT_SPEECH_BRANDON:

						if (Creature* crok1 = me->FindCreatureWithDistance(NPC_CROK_SCOURGEBANE_PART_1, 50.0f, true))
							crok1->SetFacingTo(3.09487f);

						if (Creature* brandon = me->FindCreatureWithDistance(NPC_CAPTAIN_BRANDON, 50.0f, true))
							DoSendQuantumText(SAY_BRANDON_TELEPORT, brandon);

						events.ScheduleEvent(EVENT_SPEECH_GRONDEL, 4*IN_MILLISECONDS);
						break;
					case EVENT_SPEECH_GRONDEL:
						if (Creature* grondel = me->FindCreatureWithDistance(NPC_CAPTAIN_GRONDEL, 50.0f, true))
							DoSendQuantumText(SAY_GRONDEL_TELEPORT, grondel);

						events.ScheduleEvent(EVENT_SPEECH_RUPERT, 5*IN_MILLISECONDS);
						break;
					case EVENT_SPEECH_RUPERT:
						if (Creature* rupert = me->FindCreatureWithDistance(NPC_CAPTAIN_RUPERT, 50.0f, true))
							DoSendQuantumText(SAY_RUPERT_TELEPORT, rupert);

						events.ScheduleEvent(EVENT_SPEECH_CROK, 4*IN_MILLISECONDS);
						break;
					case EVENT_SPEECH_CROK:
						if (Creature* crock = me->FindCreatureWithDistance(NPC_CROK_SCOURGEBANE_PART_1, 50.0f, true))
							DoSendQuantumText(SAY_CROK_TELEPORT, crock);

						events.ScheduleEvent(EVENT_MOVE_TO_BATTLE, 4*IN_MILLISECONDS);
						break;
					case EVENT_MOVE_TO_BATTLE:
						me->PlayDirectSound(16997);
						if (Creature* crok = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CROK_SCOURGEBANE_PART_1)))
						{
							if (Creature* crok1 = me->FindCreatureWithDistance(NPC_CROK_SCOURGEBANE_PART_1, 50.0f, true))
							{
								crok1->GetMotionMaster()->MovePoint(0, CrokToBattlePos[0]);
								crok1->DespawnAfterAction(6*IN_MILLISECONDS);
							}

							if (Creature* brandon = me->FindCreatureWithDistance(NPC_CAPTAIN_BRANDON, 50.0f, true))
							{
								FollowAngle = brandon->GetAngle(crok) + me->GetOrientation();
								FollowDist = brandon->GetDistance2d(crok);
								brandon->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
								brandon->DespawnAfterAction(6*IN_MILLISECONDS);
							}

							if (Creature* arnath = me->FindCreatureWithDistance(NPC_CAPTAIN_ARNATH, 50.0f, true))
							{
								FollowAngle = arnath->GetAngle(crok) + me->GetOrientation();
								FollowDist = arnath->GetDistance2d(crok);
								arnath->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
								arnath->DespawnAfterAction(6*IN_MILLISECONDS);
							}

							if (Creature* grondel = me->FindCreatureWithDistance(NPC_CAPTAIN_GRONDEL, 50.0f, true))
							{
								FollowAngle = grondel->GetAngle(crok) + me->GetOrientation();
								FollowDist = grondel->GetDistance2d(crok);
								grondel->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
								grondel->DespawnAfterAction(6*IN_MILLISECONDS);
							}

							if (Creature* rupert = me->FindCreatureWithDistance(NPC_CAPTAIN_RUPERT, 50.0f, true))
							{
								FollowAngle = rupert->GetAngle(crok) + me->GetOrientation();
								FollowDist = rupert->GetDistance2d(crok);
								rupert->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
								rupert->DespawnAfterAction(6*IN_MILLISECONDS);
							}
						}

						me->DespawnAfterAction(7*IN_MILLISECONDS);
						break;
					default:
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_icecrown_citadel_teleport_triggerAI(creature);
	}
};

enum MograineQuests
{
	SPELL_SHADOWS_EDGE_AURA     = 70504, // CASTED FOR BUNNY
	SPELL_CREATE_SHADOWS_EDGE   = 72995, // CREATE ITEM
	SPELL_MOGRAINE_FORGE_BEAM_1 = 70471, // CASTED TO FORGE BUNNY
	SPELL_MOGRAINE_FORGE_BEAM_2 = 72209, // CASTED TO FORGE BUNNY
	SPELL_FORGED_FLAME_BLUE     = 70367, // AFTER MOVE TRIGGER
	SPELL_PULL_SHADOW_EDGE      = 71310, // USE FROM MOGRAINE HANDS
	SPELL_BLUE_EXPLOSION        = 70509, // do use flame blue
	//
	SPELL_SUMMON_ALEXANDROS     = 72468,
	SPELL_ALEXANDROS_SPAWN      = 72469,
	// Texts
	SAY_MOGRAINE_REUNION_1      = -1609591,
	SAY_MOGRAINE_REUNION_2      = -1609592,
	SAY_MOGRAINE_REUNION_3      = -1609593,
	SAY_ALEXANDROS_MOGRAINE_1   = -1609594,
	SAY_ALEXANDROS_MOGRAINE_2   = -1609595,
	SAY_ALEXANDROS_MOGRAINE_3   = -1609596,
	SAY_ALEXANDROS_MOGRAINE_4   = -1609597,
	SAY_ALEXANDROS_MOGRAINE_5   = -1609598,
	SAY_ALEXANDROS_MOGRAINE_6   = -1609599,
	SAY_ALEXANDROS_MOGRAINE_7   = -1609600,

	PHASE_NO_QUEST                 = 0,
	PHASE_REUNION                  = 1,
	PHASE_SHADOWS_EDGE             = 2,

	ACTION_MOGRAINE_REUNION        = 1,

	EVENT_MOGRAINE_REUNION_SAY_1   = 1,
	EVENT_MOGRAINE_REUNION_SAY_2   = 2,
	EVENT_ALEXANDROS_REUNION_SAY_1 = 3,
	EVENT_ALEXANDROS_REUNION_SAY_2 = 4,
	EVENT_ALEXANDROS_REUNION_SAY_3 = 5,
	EVENT_ALEXANDROS_REUNION_SAY_4 = 6,
	EVENT_ALEXANDROS_REUNION_SAY_5 = 7,
	EVENT_ALEXANDROS_REUNION_SAY_6 = 8,
	EVENT_ALEXANDROS_REUNION_SAY_7 = 9,
	EVENT_MOGRAINE_REUNION_SAY_3   = 10,
	EVENT_RESET_STATES             = 11,
};

class npc_darion_mograine_icc : public CreatureScript
{
public:
    npc_darion_mograine_icc() : CreatureScript("npc_darion_mograine_icc") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

	bool OnQuestReward(Player* /*player*/, Creature* creature, Quest const* quest, uint32 /*item*/)
	{
		if (quest->GetQuestId() == QUEST_MOGRAINES_REUNION)
		{
			creature->CastSpell(creature, SPELL_SUMMON_ALEXANDROS, true);

			if (Creature* alexandros = creature->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
				alexandros->CastSpell(alexandros, SPELL_ALEXANDROS_SPAWN, true);

			creature->AI()->DoAction(ACTION_MOGRAINE_REUNION);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

	struct npc_darion_mograine_iccAI : public QuantumBasicAI
    {
        npc_darion_mograine_iccAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.SetPhase(PHASE_NO_QUEST);

			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
        }

		void DoAction(int32 const action)
		{
			switch (action)
			{
			    case ACTION_MOGRAINE_REUNION:
					events.ScheduleEvent(EVENT_MOGRAINE_REUNION_SAY_1, 2*IN_MILLISECONDS);
					break;
			}
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_MOGRAINE_REUNION_SAY_1:
						events.SetPhase(PHASE_REUNION);
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							me->SetFacingToObject(alexandros);

						DoSendQuantumText(SAY_MOGRAINE_REUNION_1, me);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_1, 3*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_1:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_1, alexandros);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_2, 3*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_2:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_2, alexandros);
						events.ScheduleEvent(EVENT_MOGRAINE_REUNION_SAY_2, 6*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_MOGRAINE_REUNION_SAY_2:
						DoSendQuantumText(SAY_MOGRAINE_REUNION_2, me);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_3, 6*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_3:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_3, alexandros);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_4, 7*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_4:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_4, alexandros);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_5, 10*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_5:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_5, alexandros);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_6, 7*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_6:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_6, alexandros);
						events.ScheduleEvent(EVENT_ALEXANDROS_REUNION_SAY_7, 9*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_ALEXANDROS_REUNION_SAY_7:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
							DoSendQuantumText(SAY_ALEXANDROS_MOGRAINE_7, alexandros);
						events.ScheduleEvent(EVENT_MOGRAINE_REUNION_SAY_3, 8*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_MOGRAINE_REUNION_SAY_3:
						DoSendQuantumText(SAY_MOGRAINE_REUNION_3, me);
						events.ScheduleEvent(EVENT_RESET_STATES, 6*IN_MILLISECONDS, PHASE_REUNION);
						break;
					case EVENT_RESET_STATES:
						if (Creature* alexandros = me->FindCreatureWithDistance(NPC_ALEXANDROS_MOGRAINE, 25.0f, true))
						{
							alexandros->CastSpell(alexandros, SPELL_ALEXANDROS_SPAWN, true);
							alexandros->DespawnAfterAction(0.5*IN_MILLISECONDS);
							me->Kill(me);
							me->Respawn();
						}
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darion_mograine_iccAI(creature);
    }
};

enum Jaina
{
	SAY_JAINA_LOCKET_1   = -1609601,
	SAY_JAINA_LOCKET_2   = -1609602,
	SAY_JAINA_LOCKET_3   = -1609603,
	SAY_JAINA_LOCKET_4   = -1609604,
	SAY_JAINA_LOCKET_5   = -1609605,
	SAY_JAINA_LOCKET_6   = -1609606,

	EVENT_JAINA_LOCKET_1 = 1,
	EVENT_JAINA_LOCKET_2 = 2,
	EVENT_JAINA_LOCKET_3 = 3,
	EVENT_JAINA_LOCKET_4 = 4,
	EVENT_JAINA_LOCKET_5 = 5,
	EVENT_JAINA_LOCKET_6 = 6,
	EVENT_RESET_JAINA    = 7,

	ACTION_JAINAS_LOCKET = 1,
};

class npc_jaina_quest_icc : public CreatureScript
{
public:
    npc_jaina_quest_icc() : CreatureScript("npc_jaina_quest_icc") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

	bool OnQuestReward(Player* /*player*/, Creature* creature, Quest const* quest, uint32 /*item*/)
	{
		if (quest->GetQuestId() == QUEST_JAINAS_LOCKET)
		{
			creature->AI()->DoAction(ACTION_JAINAS_LOCKET);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

	struct npc_jaina_quest_iccAI : public QuantumBasicAI
    {
        npc_jaina_quest_iccAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
        }

		void DoAction(int32 const action)
		{
			switch (action)
			{
			    case ACTION_JAINAS_LOCKET:
					events.ScheduleEvent(EVENT_JAINA_LOCKET_1, 2*IN_MILLISECONDS);
					break;
			}
		}

		void UpdateAI(uint32 const diff)
		{
			// Creature not visible for players
			if (instance->GetBossState(DATA_THE_LICH_KING) == IN_PROGRESS)
			{
				me->SetVisible(false);
				return;
			}

			// Creature visible for players after complete Lich King Encounter
			me->SetVisible(instance->GetBossState(DATA_THE_LICH_KING) == DONE);

			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_JAINA_LOCKET_1:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_JAINA_LOCKET_1, me);
						events.ScheduleEvent(EVENT_JAINA_LOCKET_2, 2*IN_MILLISECONDS);
						break;
					case EVENT_JAINA_LOCKET_2:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_JAINA_LOCKET_2, me);
						events.ScheduleEvent(EVENT_JAINA_LOCKET_3, 6*IN_MILLISECONDS);
						break;
					case EVENT_JAINA_LOCKET_3:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_JAINA_LOCKET_3, me);
						events.ScheduleEvent(EVENT_JAINA_LOCKET_4, 2*IN_MILLISECONDS);
						break;
					case EVENT_JAINA_LOCKET_4:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_JAINA_LOCKET_4, me);
						events.ScheduleEvent(EVENT_JAINA_LOCKET_5, 8*IN_MILLISECONDS);
						break;
					case EVENT_JAINA_LOCKET_5:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_JAINA_LOCKET_5, me);
						events.ScheduleEvent(EVENT_JAINA_LOCKET_6, 6*IN_MILLISECONDS);
						break;
					case EVENT_JAINA_LOCKET_6:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_JAINA_LOCKET_6, me);
						events.ScheduleEvent(EVENT_RESET_JAINA, 8*IN_MILLISECONDS);
						break;
					case EVENT_RESET_JAINA:
						me->AI()->Reset();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jaina_quest_iccAI(creature);
    }
};

enum SylvanasWindrunner
{
	SAY_SYLVANAS_VENGEANCE_1   = -1609607,
	SAY_SYLVANAS_VENGEANCE_2   = -1609608,
	SAY_SYLVANAS_VENGEANCE_3   = -1609609,
	SAY_SYLVANAS_VENGEANCE_4   = -1609610,
	SAY_SYLVANAS_VENGEANCE_5   = -1609611,
	SAY_SYLVANAS_VENGEANCE_6   = -1609612,
	SAY_SYLVANAS_VENGEANCE_7   = -1609613,

	EVENT_SYLVANAS_VENGEANCE_1 = 1,
	EVENT_SYLVANAS_VENGEANCE_2 = 2,
	EVENT_SYLVANAS_VENGEANCE_3 = 3,
	EVENT_SYLVANAS_VENGEANCE_4 = 4,
	EVENT_SYLVANAS_VENGEANCE_5 = 5,
	EVENT_SYLVANAS_VENGEANCE_6 = 6,
	EVENT_SYLVANAS_VENGEANCE_7 = 7,
	EVENT_RESET_SYLVANAS       = 8,

	ACTION_SYLVANAS_VENGEANCE  = 1,
};

class npc_sylvanas_windrunner_quest_icc : public CreatureScript
{
public:
    npc_sylvanas_windrunner_quest_icc() : CreatureScript("npc_sylvanas_windrunner_quest_icc") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

	bool OnQuestReward(Player* /*player*/, Creature* creature, Quest const* quest, uint32 /*item*/)
	{
		if (quest->GetQuestId() == QUEST_SYLVANAS_VENGEANCE)
		{
			creature->AI()->DoAction(ACTION_SYLVANAS_VENGEANCE);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

	struct npc_sylvanas_windrunner_quest_iccAI : public QuantumBasicAI
    {
        npc_sylvanas_windrunner_quest_iccAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

        void Reset()
		{
			if (instance->GetBossState(DATA_THE_LICH_KING) == DONE)
				DoCast(me, SPELL_SONG_OF_SYLVANAS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
        }

		void DoAction(int32 const action)
		{
			if (action == ACTION_SYLVANAS_VENGEANCE)
				events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_1, 2*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			// Creature not visible for players
			if (instance->GetBossState(DATA_THE_LICH_KING) == IN_PROGRESS)
			{
				me->SetVisible(false);
				return;
			}

			// Creature visible for players after complete Lich King Encounter
			me->SetVisible(instance->GetBossState(DATA_THE_LICH_KING) == DONE);

			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_SYLVANAS_VENGEANCE_1:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_1, me);
						events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_2, 2*IN_MILLISECONDS);
						break;
					case EVENT_SYLVANAS_VENGEANCE_2:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_2, me);
						events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_3, 7*IN_MILLISECONDS);
						break;
					case EVENT_SYLVANAS_VENGEANCE_3:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_3, me);
						events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_4, 7*IN_MILLISECONDS);
						break;
					case EVENT_SYLVANAS_VENGEANCE_4:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_4, me);
						events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_5, 7*IN_MILLISECONDS);
						break;
					case EVENT_SYLVANAS_VENGEANCE_5:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_5, me);
						events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_6, 7*IN_MILLISECONDS);
						break;
					case EVENT_SYLVANAS_VENGEANCE_6:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_6, me);
						events.ScheduleEvent(EVENT_SYLVANAS_VENGEANCE_7, 2*IN_MILLISECONDS);
						break;
					case EVENT_SYLVANAS_VENGEANCE_7:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_SYLVANAS_VENGEANCE_7, me);
						events.ScheduleEvent(EVENT_RESET_SYLVANAS, 8*IN_MILLISECONDS);
						break;
					case EVENT_RESET_SYLVANAS:
						me->AI()->Reset();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sylvanas_windrunner_quest_iccAI(creature);
    }
};

enum UtherTheLightbringer
{
	SAY_UTHER_REDEMPTION_1   = -1609614,
	SAY_UTHER_REDEMPTION_2   = -1609615,
	SAY_UTHER_REDEMPTION_3   = -1609616,
	SAY_UTHER_REDEMPTION_4   = -1609617,
	SAY_UTHER_REDEMPTION_5   = -1609618,
	SAY_UTHER_REDEMPTION_6   = -1609619,
	SAY_UTHER_REDEMPTION_7   = -1609620,
	SAY_UTHER_REDEMPTION_8   = -1609621,
	SAY_UTHER_REDEMPTION_9   = -1609622,

	EVENT_UTHER_REDEMPTION_1 = 1,
	EVENT_UTHER_REDEMPTION_2 = 2,
	EVENT_UTHER_REDEMPTION_3 = 3,
	EVENT_UTHER_REDEMPTION_4 = 4,
	EVENT_UTHER_REDEMPTION_5 = 5,
	EVENT_UTHER_REDEMPTION_6 = 6,
	EVENT_UTHER_REDEMPTION_7 = 7,
	EVENT_UTHER_REDEMPTION_8 = 8,
	EVENT_UTHER_REDEMPTION_9 = 9,
	EVENT_RESET_UTHER        = 10,

	ACTION_UTHER_REDEMPTION  = 1,
};

class npc_uther_the_lightbringer_quest_icc : public CreatureScript
{
public:
    npc_uther_the_lightbringer_quest_icc() : CreatureScript("npc_uther_the_lightbringer_quest_icc") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

	bool OnQuestReward(Player* /*player*/, Creature* creature, Quest const* quest, uint32 /*item*/)
	{
		if (quest->GetQuestId() == QUEST_THE_LIGHTBRINGERS_REDEMPTION)
		{
			creature->AI()->DoAction(ACTION_SYLVANAS_VENGEANCE);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

	struct npc_uther_the_lightbringer_quest_iccAI : public QuantumBasicAI
    {
        npc_uther_the_lightbringer_quest_iccAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
        }

		void DoAction(int32 const action)
		{
			if (action == ACTION_UTHER_REDEMPTION)
				events.ScheduleEvent(EVENT_UTHER_REDEMPTION_1, 2*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			// Creature not visible for players
			if (instance->GetBossState(DATA_THE_LICH_KING) == IN_PROGRESS)
			{
				me->SetVisible(false);
				return;
			}

			// Creature visible for players after complete Lich King Encounter
			me->SetVisible(instance->GetBossState(DATA_THE_LICH_KING) == DONE);

			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_UTHER_REDEMPTION_1:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_1, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_2, 2*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_2:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_2, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_3, 6*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_3:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_3, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_4, 8*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_4:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_4, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_5, 7*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_5:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_5, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_6, 10*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_6:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_6, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_7, 5*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_7:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_7, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_8, 5*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_8:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_8, me);
						events.ScheduleEvent(EVENT_UTHER_REDEMPTION_9, 5*IN_MILLISECONDS);
						break;
					case EVENT_UTHER_REDEMPTION_9:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_UTHER_REDEMPTION_9, me);
						events.ScheduleEvent(EVENT_RESET_UTHER, 2*IN_MILLISECONDS);
						break;
					case EVENT_RESET_UTHER:
						me->AI()->Reset();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_uther_the_lightbringer_quest_iccAI(creature);
    }
};

enum MuradinBronzebeard
{
	SAY_MURADIN_LAMENT_1   = -1609623,
	SAY_MURADIN_LAMENT_2   = -1609624,
	SAY_MURADIN_LAMENT_3   = -1609625,
	SAY_MURADIN_LAMENT_4   = -1609626,
	SAY_MURADIN_LAMENT_5   = -1609627,
	SAY_MURADIN_LAMENT_6   = -1609628,
	SAY_MURADIN_LAMENT_7   = -1609629,
	SAY_MURADIN_LAMENT_8   = -1609630,

	EVENT_MURADIN_LAMENT_1 = 1,
	EVENT_MURADIN_LAMENT_2 = 2,
	EVENT_MURADIN_LAMENT_3 = 3,
	EVENT_MURADIN_LAMENT_4 = 4,
	EVENT_MURADIN_LAMENT_5 = 5,
	EVENT_MURADIN_LAMENT_6 = 6,
	EVENT_MURADIN_LAMENT_7 = 7,
	EVENT_MURADIN_LAMENT_8 = 8,
	EVENT_RESET_MURADIN    = 9,

	ACTION_MURADINS_LAMENT = 1,
};

class npc_muradin_bronzebeard_quest_icc : public CreatureScript
{
public:
    npc_muradin_bronzebeard_quest_icc() : CreatureScript("npc_muradin_bronzebeard_quest_icc") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

	bool OnQuestReward(Player* /*player*/, Creature* creature, Quest const* quest, uint32 /*item*/)
	{
		if (quest->GetQuestId() == QUEST_MURADINS_LAMENT)
		{
			creature->AI()->DoAction(ACTION_MURADINS_LAMENT);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

	struct npc_muradin_bronzebeard_quest_iccAI : public QuantumBasicAI
    {
        npc_muradin_bronzebeard_quest_iccAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
        }

		void DoAction(int32 const action)
		{
			if (action == ACTION_MURADINS_LAMENT)
				events.ScheduleEvent(EVENT_MURADIN_LAMENT_1, 2*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			// Creature not visible for players
			if (instance->GetBossState(DATA_THE_LICH_KING) == IN_PROGRESS)
			{
				me->SetVisible(false);
				return;
			}

			// Creature visible for players after complete Lich King Encounter
			me->SetVisible(instance->GetBossState(DATA_THE_LICH_KING) == DONE);

			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_MURADIN_LAMENT_1:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_1, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_2, 2*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_2:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_2, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_3, 5*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_3:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_3, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_4, 5*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_4:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_4, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_5, 5*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_5:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_5, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_6, 7*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_6:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_6, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_7, 6*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_7:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_7, me);
						events.ScheduleEvent(EVENT_MURADIN_LAMENT_8, 6*IN_MILLISECONDS);
						break;
					case EVENT_MURADIN_LAMENT_8:
						me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						DoSendQuantumText(SAY_MURADIN_LAMENT_8, me);
						events.ScheduleEvent(EVENT_RESET_MURADIN, 4*IN_MILLISECONDS);
						break;
						break;
					case EVENT_RESET_MURADIN:
						me->AI()->Reset();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_muradin_bronzebeard_quest_iccAI(creature);
    }
};

class npc_valithria_dreamwalker_quest_icc : public CreatureScript
{
public:
    npc_valithria_dreamwalker_quest_icc() : CreatureScript("npc_valithria_dreamwalker_quest_icc") { }

	struct npc_valithria_dreamwalker_quest_iccAI : public QuantumBasicAI
    {
        npc_valithria_dreamwalker_quest_iccAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

        void Reset()
		{
			DoCast(me, SPELL_NATURE_SPOTLIGHT);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void UpdateAI(uint32 const diff)
		{
			if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) == IN_PROGRESS)
			{
				me->SetVisible(false);
				return;
			}

			me->SetVisible(instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) == DONE);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valithria_dreamwalker_quest_iccAI(creature);
    }
};

enum GeistEvent
{
	SAY_FLESHREAPER_INTRO_1 = -1609631,
	SAY_FLESHREAPER_INTRO_2 = -1609632,
	SAY_FLESHREAPER_INTRO_3 = -1609633,

	EVENT_GEIST_INTRO_1     = 1,
	EVENT_GEIST_INTRO_2     = 2,
	EVENT_GEIST_INTRO_3     = 3,
	EVENT_GEIST_INTRO_4     = 4,
};

Position const GeistHomePos[1] =
{
	{4388.87f, 2926.78f, 351.154f, 1.07322f},
};

class npc_intro_event_plague_icc : public CreatureScript
{
public:
    npc_intro_event_plague_icc() : CreatureScript("npc_intro_event_plague_icc") { }

	struct npc_intro_event_plague_iccAI : public QuantumBasicAI
    {
        npc_intro_event_plague_iccAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

		bool Intro;

        void Reset()
		{
			Intro = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void MoveInLineOfSight(Unit* who)
        {
            if (Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 25.0f))
				return;

			if (Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 25.0f))
			{
				events.ScheduleEvent(EVENT_GEIST_INTRO_1, 1*IN_MILLISECONDS);
				Intro = true;
			}
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_GEIST_INTRO_1:
						DoSendQuantumText(SAY_FLESHREAPER_INTRO_1, me);
						events.ScheduleEvent(EVENT_GEIST_INTRO_2, 3*IN_MILLISECONDS);
						break;
					case EVENT_GEIST_INTRO_2:
						DoSendQuantumText(SAY_FLESHREAPER_INTRO_2, me);
						events.ScheduleEvent(EVENT_GEIST_INTRO_3, 4*IN_MILLISECONDS);
						break;
					case EVENT_GEIST_INTRO_3:
						DoSendQuantumText(SAY_FLESHREAPER_INTRO_3, me);
						events.ScheduleEvent(EVENT_GEIST_INTRO_4, 3*IN_MILLISECONDS);
						break;
					case EVENT_GEIST_INTRO_4:
						me->GetMotionMaster()->MovePoint(0, GeistHomePos[0]);
						me->DespawnAfterAction(5*IN_MILLISECONDS);
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_intro_event_plague_iccAI(creature);
    }
};

class at_icc_start_sindragosa_gauntlet : public AreaTriggerScript
{
public:
	at_icc_start_sindragosa_gauntlet() : AreaTriggerScript("at_icc_start_sindragosa_gauntlet") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*triggerId*/)
	{
		if (InstanceScript* instance = player->GetInstanceScript())
		{
			if (Creature* ward = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_SINDRAGOSA_GAUNTLET)))
				ward->AI()->DoAction(ACTION_START_GAUNTLET);
		}
		return true;
	}
};

class go_empowering_blood_orb : public GameObjectScript
{
public:
	go_empowering_blood_orb() : GameObjectScript("go_empowering_blood_orb") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		player->CastSpell(player, SPELL_EMPOWERED_BLOOD, true);

		go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
		go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
		go->GetRespawnDelay();
		
		if (Creature* stalker = go->FindCreatureWithDistance(NPC_ORB_VISUAL_STALKER, 7.0f))
			stalker->DespawnAfterAction();

		return true;
	}
};

void AddSC_icecrown_citadel()
{
    new npc_highlord_tirion_fordring_lh();
    new npc_rotting_frost_giant();
    new npc_frost_freeze_trap();
    new npc_alchemist_adrianna();
    new boss_sister_svalna();
    new npc_crok_scourgebane();
    new npc_captain_arnath();
    new npc_captain_brandon();
    new npc_captain_grondel();
    new npc_captain_rupert();
    new npc_impaling_spear();
	new npc_arthas_teleport_visual();
    new spell_icc_stoneform();
    new spell_icc_sprit_alarm();
	new spell_icc_soul_missile();
	new spell_icc_web_wrap();
    new spell_frost_giant_death_plague();
    new spell_icc_harvest_blight_specimen();
    new spell_trigger_spell_from_caster("spell_svalna_caress_of_death", SPELL_IMPALING_SPEAR_KILL);
    new spell_svalna_revive_champion();
    new spell_svalna_remove_spear();
    new at_icc_saurfang_portal();
    new at_icc_shutdown_traps();
    new at_icc_start_blood_quickening();
    new at_icc_start_frostwing_gauntlet();
	new npc_sindragosas_ward();
	new npc_putricides_trap();
	new npc_icecrown_citadel_teleport_trigger();
	new npc_darion_mograine_icc();
	new npc_jaina_quest_icc();
	new npc_sylvanas_windrunner_quest_icc();
	new npc_uther_the_lightbringer_quest_icc();
	new npc_muradin_bronzebeard_quest_icc();
	new npc_valithria_dreamwalker_quest_icc();
	new npc_intro_event_plague_icc();
	new at_icc_start_sindragosa_gauntlet();
	new go_empowering_blood_orb();
}