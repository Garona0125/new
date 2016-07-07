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
#include "Config.h"

enum Custom
{
	FACTION_ID_KIRIN_TOR_GENERIC = 2006,
	MODEL_ID_SAKYRA              = 28989,
	NPC_MIRROR_IMAGE             = 47244,
	SAY_PANDA_STREET_MAGIC       = -1413010,
};

enum Spells
{
	// Auras For Panda
	SPELL_SAKYRA_PEACE           = 62579,
	SPELL_WATER_SHIELD           = 57960,
	SPELL_WILD_GROWTH            = 52948,
	// Focus Spells
	SPELL_MASS_FIREBOLT          = 62669,
	SPELL_SUN_BEAM_VISUAL        = 62216,
	SPELL_BLUE_FIREBALL          = 42604,
	SPELL_SPOTLIGHT              = 50236,
	SPELL_BLUE_SHIELD_VISUAL     = 53143,
	SPELL_FLAME_TSUNAMI          = 57494,
	SPELL_HOLY_ZONE_VISUAL       = 70571,
	SPELL_HOLY_ORB_VISUAL        = 70785,
	SPELL_SHADOW_PORTAL_VISUAL   = 40280,
	SPELL_ARCANE_PORTAL_VISUAL   = 42047,
	SPELL_FROST_PORTAL_VISUAL    = 42049,
	SPELL_NATURE_PORTAL_VISUAL   = 42050,
	SPELL_HOLY_PORTAL_VISUAL     = 42051,
	SPELL_FEL_PORTAL_VISUAL      = 46907,
	SPELL_RED_PORTAL_VISUAL      = 55810,
	SPELL_PURPLE_PORTAL_VISUAL   = 62170,
	SPELL_YELLOW_PORTAL_VISUAL   = 33341,
	SPELL_KINETIC_ORB_VISUAL     = 72054,
	SPELL_LIGHT_SHIELD_VISUAL    = 74621,
	SPELL_ARRIVAL                = 64997,
	SPELL_RIDE_THE_LIGHTNING     = 64986,
	SPELL_FROST_SPOTLIGHT        = 63769,
	SPELL_NATURE_SPOTLIGHT       = 63771,
	SPELL_FIRE_SPOTLIGHT         = 63772,
	SPELL_LIGHTNING_SPOTLIGHT    = 63773,
	SPELL_RANDOM_LIGHTNING       = 56914,
	SPELL_OMINOUS_CLOUD_VISUAL   = 63084,
	SPELL_LIFEBINDER_GIFT_VISUAL = 62579,
	SPELL_PURPLE_ORB_VISUAL      = 57887,
	SPELL_FIRE_ORB_VISUAL        = 55928,
	SPELL_FLAME_SPHERE_SPAWN     = 55891,
	SPELL_SUNWELL_LIGHT_2        = 71183, 
	SPELL_POWER_BALL_VISUAL      = 54141,
	SPELL_OPEN_PORTAL_VISUAL     = 57687,
	SPELL_FROSTWIND_VISUAL       = 70022,
	SPELL_WHIRLWIND_VISUAL       = 70300,
	SPELL_SARONITE_FOG_VISUAL    = 63096,
	SPELL_HOLY_AURA              = 70788, // need implement!
	// Spells For Players
	SPELL_PICCOLO_DANCER         = 18400,
	SPELL_CASCADE_OF_ROSES       = 27571,
	SPELL_CASCADE_OF_EBON_PETALS = 61415,
	SPELL_SNOWFLAKES             = 44755,
	SPELL_SUMMER_FLOWERS         = 45417,
	SPELL_HOLIDAY_CHEER          = 26074,
	SPELL_PARTY_GRENADE          = 51510,
	SPELL_LANTERNED_PUMPKIN      = 44212,
	SPELL_THROW_GROUND_FLOWER    = 45729,
	SPELL_CANDY_SIZE             = 24924,
	SPELL_CANDY_SKELETON         = 24925,
	SPELL_CANDY_PIRATE           = 24926,
	SPELL_CANDY_GHOST_FLY        = 24927,
	SPELL_CORSAIR_COSTUME        = 51926,
};

enum Phases
{
	PHASE_ONE        = 1,
    PHASE_TWO        = 2,
    PHASE_THREE      = 3,
};

enum Events
{
	EVENT_FOCUS_SUN_BEAM         = 1,
	EVENT_FOCUS_BLUE_FIREBALL    = 2,
	EVENT_FOCUS_SPOTLIGHT        = 3,
	EVENT_CASCADE_OF_ROSES       = 4,
	EVENT_FOCUS_BLUE_SHIELD      = 5,
	EVENT_FOCUS_HOLY_PORTAL      = 6,
	EVENT_FOCUS_FROST_PORTAL     = 7,
	EVENT_FOCUS_FLAME_TSUNAMI    = 8,
	EVENT_FOCUS_HOLY_ZONE        = 9,
	EVENT_FOCUS_FEL_PORTAL       = 10,
	EVENT_FOCUS_HOLY_ORB         = 11,
	EVENT_FOCUS_RED_PORTAL       = 12,
	EVENT_FOCUS_ARCANE_PORTAL    = 13,
	EVENT_FOCUS_SHADOW_PORTAL    = 14,
	EVENT_FOCUS_NATURE_PORTAL    = 15,
	EVENT_FOCUS_PURPLE_PORTAL    = 16,
	EVENT_FOCUS_KINETIC_ORB      = 17,
	EVENT_FOCUS_LIGHT_SHIELD     = 18,
	EVENT_FOCUS_RIDE_THE_LIGHT   = 19,
	EVENT_FOCUS_BLUE_SPOTLIGHT   = 20,
	EVENT_FOCUS_GREEN_SPOTLIGHT  = 21,
	EVENT_FOCUS_RED_SPOTLIGHT    = 22,
	EVENT_FOCUS_LIGHT_SPOTLIGHT  = 23,
	EVENT_FOCUS_YELLOW_PORTAL    = 24,
	EVENT_FOCUS_RANDOM_LIGHTNING = 25,
	EVENT_FOCUS_LIFEBINDER_LIGHT = 26,
	EVENT_FOCUS_OMINOUS_CLOUD    = 27,
	EVENT_FOCUS_PURPLE_ORB       = 28,
	EVENT_FOCUS_FIRE_ORB         = 29,
	EVENT_FOCUS_SUNWELL_LIGHT    = 30,
	EVENT_FOCUS_POWER_BALL       = 31,
	EVENT_FOCUS_OPEN_PORTAL      = 32,
	EVENT_FOCUS_FROSTWIND        = 33,
	EVENT_FOCUS_WHIRLWIND        = 34,
	EVENT_FOCUS_SARONITE_FOG     = 35,
};

enum Sounds
{
	SOUND_VODOO                = 8452,
	SOUND_PIRATE_TAVERN        = 11805,
	SOUND_TAVERN_NIGHT_ELF     = 12136,
	SOUND_ESCAPE_FROM_PANDARIA = 12325,
	SOUND_DEFEAT_KILJAEDEN     = 12538,
	SOUND_RETURN_FROM_PANDARIA = 17485,
	SOUND_LEAVE_FORM_PANDARIA  = 17486,
	// Special
	SOUND_DWARF_ANECDOTE_1     = 11812,
	SOUND_DWARF_ANECDOTE_2     = 11813,
	SOUND_GOBLIN_ANECDOTE_1    = 11814,
	SOUND_GOBLIN_ANECDOTE_2    = 11815,
};

enum WeeklyQuests
{
	QUEST_ILLIDAN_STORMRAGE_MUST_DIE      = 24600,
	QUEST_ARCHIMONDE_THE_DEFILER_MUST_DIE = 24601,
	QUEST_KILJAEDEN_MUST_DIE              = 24602,
	QUEST_KAELTHAS_SUNSTRIDER_MUST_DIE    = 24603,
	QUEST_LADY_VASHJ_MUST_DIE             = 24604,
	QUEST_ZULJIN_MUST_DIE                 = 24605,
	QUEST_PRINCE_MALCHEZAAR_MUST_DIE      = 24606,
	QUEST_NEFARIAN_MUST_DIE               = 24607,
	QUEST_RAGNAROS_MUST_DIE               = 24608,
	QUEST_MAGTHERIDON_MUST_DIE            = 24616,
	QUEST_ONYXIA_MUST_DIE                 = 24617,
	QUEST_CTHUN_MUST_DIE                  = 24618,
	QUEST_KEL_THUZAD_MUST_DIE             = 24619,
	QUEST_DEATHBRINGER_SAURFANG_MUST_DIE  = 24620,
	QUEST_AURIAYA_MUST_DIE                = 24621,
	QUEST_THE_FALLING_VAULT_OF_ARCHAVON   = 24622,
};

enum DailyQuests
{
	QUEST_PROOF_OF_DEMISE_HARBINGER_SRYRISS    = 24623,
	QUEST_PROOF_OF_DEMISE_WARP_SPLINTER        = 24624,
	QUEST_PROOF_OF_DEMISE_PATHALEON            = 24625,
	QUEST_PROOF_OF_DEMISE_THE_MURMUR           = 24626,
	QUEST_PROOF_OF_DEMISE_TALON_KING_IKISS     = 24627,
	QUEST_PROOF_OF_DEMISE_EXARCH_MALADAAR      = 24628,
	QUEST_PROOF_OF_DEMISE_NEXUS_PRINCE_SHAFFAR = 24630,
	QUEST_PROOF_OF_DEMISE_KAELTHAS_SUNSTRIDER  = 24631,
	QUEST_PROOF_OF_DEMISE_KARGATH_BLADEFIST    = 24632,
	QUEST_PROOF_OF_DEMISE_KELIDAN_THE_BREAKER  = 24633,
	QUEST_PROOF_OF_DEMISE_VAZRUDEN_THE_HERALD  = 24634,
	QUEST_PROOF_OF_DEMISE_WARLORD_KALITHRESH   = 24637,
	QUEST_PROOF_OF_DEMISE_THE_BLACK_STALKER    = 24639,
	QUEST_PROOF_OF_DEMISE_QUAGMIRRAN           = 24640,
	QUEST_PROOF_OF_DEMISE_EPOCH_HUNTER         = 24641,
	QUEST_PROOF_OF_DEMISE_AEONUS               = 24642,
};

class npc_funny_panda_of_shaolin : public CreatureScript
{
public:
	npc_funny_panda_of_shaolin() : CreatureScript("npc_funny_panda_of_shaolin") {}

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	struct npc_funny_panda_of_shaolinAI : public QuantumBasicAI
	{
		npc_funny_panda_of_shaolinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetCurrentFaction(FACTION_ID_KIRIN_TOR_GENERIC);
			me->SetObjectScale(3.0f);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
		}

		EventMap events;

		void Reset()
		{
			events.Reset();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);

			if (GetConfigSettings::GetBoolState("Funny.Panda.Of.Shaolin.Enable", true))
			{
				DoCast(me, SPELL_WILD_GROWTH, true);

				events.ScheduleEvent(EVENT_FOCUS_SUN_BEAM, 3*IN_MILLISECONDS);
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (!me->HasAura(SPELL_WATER_SHIELD))
				DoCast(me, SPELL_WATER_SHIELD, true);

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_FOCUS_SUN_BEAM: // Id name for event
						events.SetPhase(PHASE_ONE); // Change event mask
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_SARONITE_FOG_VISUAL); // Remove old aura from last event
						DoSendQuantumText(SAY_PANDA_STREET_MAGIC, me); // Use text
						DoCast(me, SPELL_SUN_BEAM_VISUAL, true); // Cast spell
						DoCast(me, SPELL_MASS_FIREBOLT, true);
						// Cast Lanterned Pumkin for random player in spell range
						if (Player* player = me->FindPlayerWithDistance(30.0f, true))
							me->CastSpell(player, SPELL_LANTERNED_PUMPKIN, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Light and Dark] [1]"); // For console logging
						events.ScheduleEvent(EVENT_FOCUS_BLUE_FIREBALL, 30*IN_MILLISECONDS, PHASE_ONE); // Switch to next event
						break;
					case EVENT_FOCUS_BLUE_FIREBALL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_SUN_BEAM_VISUAL);
						DoCast(me, SPELL_BLUE_FIREBALL);
						// Cast Corsair Costume
						if (Player* player = me->FindPlayerWithDistance(5.0f, true))
							me->CastSpell(player, SPELL_CORSAIR_COSTUME, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Blue Fireball] [2]");
						events.ScheduleEvent(EVENT_FOCUS_SPOTLIGHT, 5*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_SPOTLIGHT:
						me->SetFacingTo(me->GetOrientation());
						DoCast(me, SPELL_SPOTLIGHT);
						// Cast Picolo Dancer
						DoCast(me, SPELL_PICCOLO_DANCER, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Spotlight] [3]");
						events.ScheduleEvent(EVENT_CASCADE_OF_ROSES, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_CASCADE_OF_ROSES:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_SPOTLIGHT);
						me->RemoveAurasDueToSpell(SPELL_PICCOLO_DANCER);
						DoCast(me, SPELL_CASCADE_OF_ROSES, true);
						// Cast Holiday Cheer
						if (Player* player = me->FindPlayerWithDistance(20.0f, true))
							me->CastSpell(player, SPELL_HOLIDAY_CHEER, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Cascade Of Roses] [4]");
						events.ScheduleEvent(EVENT_FOCUS_BLUE_SHIELD, 5*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_BLUE_SHIELD:
						me->SetFacingTo(me->GetOrientation());
						DoCast(me, SPELL_BLUE_SHIELD_VISUAL);
						// Cast Throw Ground Flower
						if (Player* player = me->FindPlayerWithDistance(20.0f, true))
							me->CastSpell(player, SPELL_THROW_GROUND_FLOWER, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Blue Shield] [5]");
						events.ScheduleEvent(EVENT_FOCUS_HOLY_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_HOLY_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_BLUE_SHIELD_VISUAL);
						// Cast Snowflakes for random player in spell range
						if (Player* player = me->FindPlayerWithDistance(30.0f, true))
							me->CastSpell(player, SPELL_SNOWFLAKES, true);
						DoCast(me, SPELL_HOLY_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Holy Portal] [6]");
						events.ScheduleEvent(EVENT_FOCUS_FROST_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_FROST_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_HOLY_PORTAL_VISUAL);
						// Cast Cascade of Roses for random player in spell range
						if (Player* player = me->FindPlayerWithDistance(30.0f, true))
							me->CastSpell(player, SPELL_CASCADE_OF_ROSES, true);
						DoCast(me, SPELL_FROST_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Frost Portal] [7]");
						events.ScheduleEvent(EVENT_FOCUS_FLAME_TSUNAMI, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_FLAME_TSUNAMI:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FROST_PORTAL_VISUAL);
						// Cast Summer Flowers for random player in spell range
						if (Player* player = me->FindPlayerWithDistance(30.0f, true))
							me->CastSpell(player, SPELL_SUMMER_FLOWERS, true);
						DoCast(me, SPELL_FLAME_TSUNAMI, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Flame Tsunami] [8]");
						events.ScheduleEvent(EVENT_FOCUS_FEL_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_FEL_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FLAME_TSUNAMI);
						// Cast Cascade of Ebon Petals for random player in spell range
						if (Player* player = me->FindPlayerWithDistance(30.0f, true))
							me->CastSpell(player, SPELL_CASCADE_OF_EBON_PETALS, true);
						DoCast(me, SPELL_FEL_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Fel Portal] [9]");
						events.ScheduleEvent(EVENT_FOCUS_HOLY_ZONE, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_HOLY_ZONE:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FEL_PORTAL_VISUAL);
						DoCast(me, SPELL_HOLY_ZONE_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Holy Zone] [10]");
						events.ScheduleEvent(EVENT_FOCUS_HOLY_ORB, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_HOLY_ORB:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_HOLY_ZONE_VISUAL);
						DoCast(me, SPELL_HOLY_ORB_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Holy Orb] [11]");
						events.ScheduleEvent(EVENT_FOCUS_RED_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_RED_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_HOLY_ORB_VISUAL);
						DoCast(me, SPELL_RED_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Red Portal] [12]");
						events.ScheduleEvent(EVENT_FOCUS_ARCANE_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_ARCANE_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_RED_PORTAL_VISUAL);
						DoCast(me, SPELL_ARCANE_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Arcane Portal] [13]");
						events.ScheduleEvent(EVENT_FOCUS_SHADOW_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_SHADOW_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_ARCANE_PORTAL_VISUAL);
						DoCast(me, SPELL_SHADOW_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Shadow Portal] [14]");
						events.ScheduleEvent(EVENT_FOCUS_NATURE_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_NATURE_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_SHADOW_PORTAL_VISUAL);
						DoCast(me, SPELL_NATURE_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Nature Portal] [15]");
						events.ScheduleEvent(EVENT_FOCUS_PURPLE_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_FOCUS_PURPLE_PORTAL:
						events.SetPhase(PHASE_TWO);
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_NATURE_PORTAL_VISUAL);
						DoCast(me, SPELL_PURPLE_PORTAL_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Purple Portal] [16]");
						events.ScheduleEvent(EVENT_FOCUS_KINETIC_ORB, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_KINETIC_ORB:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_PURPLE_PORTAL_VISUAL);
						DoCast(me, SPELL_KINETIC_ORB_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Kinetic Bomb] [17]");
						events.ScheduleEvent(EVENT_FOCUS_LIGHT_SHIELD, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_LIGHT_SHIELD:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_KINETIC_ORB_VISUAL);
						DoCast(me, SPELL_LIGHT_SHIELD_VISUAL);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Light Shield] [18]");
						events.ScheduleEvent(EVENT_FOCUS_RIDE_THE_LIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_RIDE_THE_LIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_LIGHT_SHIELD_VISUAL);
						DoCast(me, SPELL_ARRIVAL, true);
						DoCast(me, SPELL_RIDE_THE_LIGHTNING , true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Ride The Lightning] [19]");
						events.ScheduleEvent(EVENT_FOCUS_BLUE_SPOTLIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_BLUE_SPOTLIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_ARRIVAL);
						me->RemoveAurasDueToSpell(SPELL_RIDE_THE_LIGHTNING);
						DoCast(me, SPELL_FROST_SPOTLIGHT, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Blue Spotlight] [20]");
						events.ScheduleEvent(EVENT_FOCUS_GREEN_SPOTLIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_GREEN_SPOTLIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FROST_SPOTLIGHT);
						DoCast(me, SPELL_NATURE_SPOTLIGHT, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Green Spotlight] [21]");
						events.ScheduleEvent(EVENT_FOCUS_RED_SPOTLIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_RED_SPOTLIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_NATURE_SPOTLIGHT);
						DoCast(me, SPELL_FIRE_SPOTLIGHT, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Red Spotlight] [22]");
						events.ScheduleEvent(EVENT_FOCUS_LIGHT_SPOTLIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_LIGHT_SPOTLIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FIRE_SPOTLIGHT);
						DoCast(me, SPELL_LIGHTNING_SPOTLIGHT, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Lightning Spotlight] [23]");
						events.ScheduleEvent(EVENT_FOCUS_YELLOW_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_YELLOW_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_LIGHTNING_SPOTLIGHT);
						DoCast(me, SPELL_YELLOW_PORTAL_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Yellow Portal] [24]");
						events.ScheduleEvent(EVENT_FOCUS_RANDOM_LIGHTNING, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_RANDOM_LIGHTNING:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_YELLOW_PORTAL_VISUAL);
						DoCast(me, SPELL_RANDOM_LIGHTNING, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Random Lightning] [25]");
						events.ScheduleEvent(EVENT_FOCUS_LIFEBINDER_LIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_LIFEBINDER_LIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_RANDOM_LIGHTNING);
						DoCast(me, SPELL_LIFEBINDER_GIFT_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Lifebinder Light] [26]");
						events.ScheduleEvent(EVENT_FOCUS_OMINOUS_CLOUD, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_OMINOUS_CLOUD:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_LIFEBINDER_GIFT_VISUAL);
						DoCast(me, SPELL_OMINOUS_CLOUD_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Ominous Cloud] [27]");
						events.ScheduleEvent(EVENT_FOCUS_PURPLE_ORB, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_PURPLE_ORB:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_OMINOUS_CLOUD_VISUAL);
						DoCast(me, SPELL_PURPLE_ORB_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Purple Orb] [28]");
						events.ScheduleEvent(EVENT_FOCUS_FIRE_ORB, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_FIRE_ORB:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_PURPLE_ORB_VISUAL);
						DoCast(me, SPELL_FLAME_SPHERE_SPAWN, true);
						DoCast(me, SPELL_FIRE_ORB_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Fire Orb] [29]");
						events.ScheduleEvent(EVENT_FOCUS_SUNWELL_LIGHT, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_SUNWELL_LIGHT:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FIRE_ORB_VISUAL);
						DoCast(me, SPELL_SUNWELL_LIGHT_2, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Sunwell Light] [30]");
						events.ScheduleEvent(EVENT_FOCUS_POWER_BALL, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_POWER_BALL:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_SUNWELL_LIGHT_2);
						DoCast(me, SPELL_POWER_BALL_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Power Ball] [31]");
						events.ScheduleEvent(EVENT_FOCUS_OPEN_PORTAL, 1*MINUTE*IN_MILLISECONDS, PHASE_TWO);
						break;
					case EVENT_FOCUS_OPEN_PORTAL:
						me->SetFacingTo(me->GetOrientation());
						events.SetPhase(PHASE_THREE);
						me->RemoveAurasDueToSpell(SPELL_POWER_BALL_VISUAL);
						DoCast(me, SPELL_OPEN_PORTAL_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Open Portal] [32]");
						events.ScheduleEvent(EVENT_FOCUS_FROSTWIND, 1*MINUTE*IN_MILLISECONDS, PHASE_THREE);
						break;
					case EVENT_FOCUS_FROSTWIND:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_OPEN_PORTAL_VISUAL);
						DoCast(me, SPELL_FROSTWIND_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Frostwind] [33]");
						events.ScheduleEvent(EVENT_FOCUS_WHIRLWIND, 1*MINUTE*IN_MILLISECONDS, PHASE_THREE);
						break;
					case EVENT_FOCUS_WHIRLWIND:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_FROSTWIND_VISUAL);
						DoCast(me, SPELL_WHIRLWIND_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Whirlwind] [34]");
						events.ScheduleEvent(EVENT_FOCUS_SARONITE_FOG, 1*MINUTE*IN_MILLISECONDS, PHASE_THREE);
						break;
					case EVENT_FOCUS_SARONITE_FOG:
						me->SetFacingTo(me->GetOrientation());
						me->RemoveAurasDueToSpell(SPELL_WHIRLWIND_VISUAL);
						DoCast(me, SPELL_SARONITE_FOG_VISUAL, true);
						sLog->OutConsole("EVENT MANAGER: Funny Panda of Shaolin [Started Event: Saronite Fog] [35]");
						events.ScheduleEvent(EVENT_FOCUS_SUN_BEAM, 1*MINUTE*IN_MILLISECONDS, PHASE_THREE);
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
		return new npc_funny_panda_of_shaolinAI(creature);
	}
};

class npc_sakyra : public CreatureScript
{
public:
    npc_sakyra() : CreatureScript("npc_sakyra") {}

    struct npc_sakyraAI : public QuantumBasicAI
    {
        npc_sakyraAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
		{
			DoCast(me, SPELL_SAKYRA_PEACE, true);

			me->SetObjectScale(1.5f);
			me->SetDisplayId(MODEL_ID_SAKYRA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sakyraAI(creature);
    }
};

void AddSC_funny_panda_of_shaolin()
{
	new npc_funny_panda_of_shaolin();
	new npc_sakyra();
}