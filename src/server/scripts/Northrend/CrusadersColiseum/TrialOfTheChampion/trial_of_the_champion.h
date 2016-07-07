/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_TRIAL_OT_THE_CHAMPION_H
#define DEF_TRIAL_OT_THE_CHAMPION_H

enum Data
{
    BOSS_GRAND_CHAMPIONS,
    BOSS_ARGENT_CHALLENGE_E,
    BOSS_ARGENT_CHALLENGE_P,
    BOSS_BLACK_KNIGHT,
    MINION_ENCOUNTER,

    DATA_TEAM,
    DATA_CHAMPION_DEFEATED,
    DATA_GRAND_CHAMPION_ENTRY,
    DATA_GRAND_CHAMPION_GUID,
    DATA_MEMORY_ENTRY,
};

enum Data64
{
    DATA_ANNOUNCER,
    DATA_TIRION,
    DATA_THRALL,
    DATA_GARROSH,
    DATA_VARIAN,
    DATA_JAINA,
    DATA_MAIN_GATE,
    DATA_PORTCULLIS,
    DATA_CHEST,
    DATA_BLACK_KNIGHT,
    DATA_BLACK_KNIGHT_GRYPHON,
};

enum AnnouncerPhases
{
    EVENT_INTRO = 0, // Presentation of champions
    EVENT_WAVES,
    EVENT_CHAMPIONS,
    EVENT_INTRO_ARGENT,
    EVENT_WAVES_ARGENT,
    EVENT_BLACK_KNIGHT_INTRO,
    EVENTS_MAX,
};

enum Creatures
{
    NPC_MOKRA	               = 35572,
    NPC_ERESSEA                = 35569,
    NPC_RUNOK	               = 35571,
    NPC_ZULTORE                = 35570,
    NPC_VISCERI                = 35617,
    NPC_JACOB	               = 34705,
    NPC_AMBROSE                = 34702,
    NPC_COLOSOS                = 34701,
    NPC_JAELYNE                = 34657,
    NPC_LANA	               = 34703,
    NPC_STORMWIND_CHAMPION     = 35328,
    NPC_IRONFORGE_CHAMPION     = 35329,
	NPC_EXODAR_CHAMPION        = 35330,
    NPC_GNOMEREGAN_CHAMPION    = 35331,
    NPC_DARNASSUS_CHAMPION     = 35332,
    NPC_ORGRIMMAR_CHAMPION     = 35314,
    NPC_THUNDER_BLUFF_CHAMPION = 35325,
    NPC_UNDERCITY_CHAMPION     = 35327,
    NPC_SENJIN_CHAMPION        = 35323,
    NPC_SILVERMOON_CHAMPION    = 35326,
    NPC_EADRIC_THE_PURE        = 35119,
    NPC_ARGENT_PALETRESS       = 34928,
    NPC_MEMORY_HOGGER          = 34942,
    NPC_MEMORY_VAN_CLEEF       = 35028,
    NPC_MEMORY_MUTANUS         = 35029,
    NPC_MEMORY_HEROD           = 35030,
    NPC_MEMORY_LUCIFIRON       = 35031,
    NPC_MEMORY_THUNDERAAN      = 35032,
    NPC_MEMORY_CHROMAGGUS      = 35033,
    NPC_MEMORY_HAKKAR          = 35034,
    NPC_MEMORY_VEKNILASH       = 35036,
    NPC_MEMORY_KALITHRESH      = 35037,
    NPC_MEMORY_MALCHEZAAR      = 35038,
    NPC_MEMORY_GRUUL           = 35039,
    NPC_MEMORY_LADY_VASHJ       = 35040,
    NPC_MEMORY_ARCHIMONDE      = 35041,
    NPC_MEMORY_ILLIDAN         = 35042,
    NPC_MEMORY_DELRISSA        = 35043,
    NPC_MEMORY_ENTROPIUS       = 35044,
    NPC_MEMORY_INGVAR          = 35045,
    NPC_MEMORY_CYANIGOSA       = 35046,
    NPC_MEMORY_ECK             = 35047,
    NPC_MEMORY_ONYXIA          = 35048,
    NPC_MEMORY_HEIGAN          = 35049,
    NPC_MEMORY_IGNIS           = 35050,
    NPC_MEMORY_VEZAX           = 35051,
    NPC_MEMORY_ALGALON         = 35052,
    NPC_ARGENT_LIGHWIELDER     = 35309,
    NPC_ARGENT_MONK            = 35305,
    NPC_ARGENT_PRIESTESS       = 35307,
    NPC_BLACK_KNIGHT           = 35451,
    NPC_RISEN_JAEREN           = 35545,
    NPC_RISEN_ARELAS           = 35564,
    NPC_RISEN_CHAMPION         = 35590,
    NPC_JAEREN_SUNSWORN        = 35004,
    NPC_ARELAS_BRIGHTSTAR      = 35005,
    NPC_TIRION	               = 41400,
    NPC_THRALL	               = 41401,
    NPC_GARROSH	               = 41402,
    NPC_VARIAN	               = 41403,
    NPC_JAINA                  = 34992,
	NPC_ARGENT_WARHORSE        = 35644,
    NPC_ARGENT_BATTLEWORG      = 36558,
	NPC_BLACK_KNIGHT_GRYPHON   = 35491,
	//
	NPC_MARSHAL_JACOB_ALERIUS_MOUNT = 35637,
    NPC_AMBROSE_BOLTSPARK_MOUNT     = 35633,
    NPC_COLOSOS_MOUNT				= 35768,
    NPC_EVENSONG_MOUNT				= 34658,
    NPC_LANA_STOUTHAMMER_MOUNT		= 35636,
    NPC_DARNASSIAN_NIGHTSABER		= 33319,
    NPC_EXODAR_ELEKK				= 33318,
    NPC_STORMWIND_STEED				= 33217,
    NPC_GNOMEREGAN_MECHANOSTRIDER	= 33317,
    NPC_IRONFORGE_RAM               = 33316,
    NPC_MOKRA_SKILLCRUSHER_MOUNT    = 35638,
    NPC_ERESSEA_DAWNSINGER_MOUNT	= 35635,
    NPC_RUNOK_WILDMANE_MOUNT	    = 35640,
    NPC_ZUL_TORE_MOUNT              = 35641,
    NPC_DEATHSTALKER_VESCERI_MOUNT  = 35634,
    NPC_FORSAKEN_WARHORSE           = 33324,
    NPC_THUNDER_BLUFF_KODO          = 33322,
    NPC_ORGRIMMAR_WOLF              = 33320,
    NPC_SILVERMOON_HAWKSTRIDER      = 33323,
    NPC_DARKSPEAR_RAPTOR            = 33321,
};

enum GameObjects
{
    GO_MAIN_GATE        = 195647,
    GO_PORTCULLIS       = 195650,
    GO_CHAMPIONS_LOOT	= 195709,
    GO_CHAMPIONS_LOOT_H	= 195710,
    GO_EADRIC_LOOT      = 195374,
    GO_EADRIC_LOOT_H    = 195375,
    GO_PALETRESS_LOOT   = 195323,
    GO_PALETRESS_LOOT_H	= 195324,
};

enum AchievementCriteria
{
    CRITERIA_CHAMPION_JACOB     = 11420,
    CRITERIA_CHAMPION_LANA      = 12298,
    CRITERIA_CHAMPION_COLOSOS   = 12299,
    CRITERIA_CHAMPION_AMBROSE   = 12300,
    CRITERIA_CHAMPION_JAELYNE   = 12301,
    CRITERIA_CHAMPION_MOKRA     = 12302,
    CRITERIA_CHAMPION_VISCERI   = 12303,
    CRITERIA_CHAMPION_RUNOK     = 12304,
    CRITERIA_CHAMPION_ERESSEA   = 12305,
    CRITERIA_CHAMPION_ZULTORE   = 12306,
    CRITERIA_CHAMPION_JACOB_H   = 12310,
    CRITERIA_CHAMPION_LANA_H    = 12311,
    CRITERIA_CHAMPION_COLOSOS_H = 12312,
    CRITERIA_CHAMPION_AMBROSE_H = 12313,
    CRITERIA_CHAMPION_JAELYNE_H = 12314,
    CRITERIA_CHAMPION_MOKRA_H   = 12318,
    CRITERIA_CHAMPION_VISCERI_H = 12319,
    CRITERIA_CHAMPION_RUNOK_H   = 12320,
    CRITERIA_CHAMPION_ERESSEA_H = 12321,
    CRITERIA_CHAMPION_ZULTORE_H = 12322,
    CRITERIA_MEMORY_HOGGER      = 11863,
    CRITERIA_MEMORY_VAN_CLEEF   = 11904,
    CRITERIA_MEMORY_MUTANUS     = 11905,
    CRITERIA_MEMORY_HEROD       = 11906,
    CRITERIA_MEMORY_LUCIFIRON   = 11907,
    CRITERIA_MEMORY_THUNDERAAN  = 11908,
    CRITERIA_MEMORY_CHROMAGGUS  = 11909,
    CRITERIA_MEMORY_HAKKAR      = 11910,
    CRITERIA_MEMORY_VEKNILASH   = 11911,
    CRITERIA_MEMORY_KALITHRESH  = 11912,
    CRITERIA_MEMORY_MALCHEZAAR  = 11913,
    CRITERIA_MEMORY_GRUUL       = 11914,
    CRITERIA_MEMORY_LADY_VASHJ  = 11915,
    CRITERIA_MEMORY_ARCHIMONDE  = 11916,
    CRITERIA_MEMORY_ILLIDAN     = 11917,
    CRITERIA_MEMORY_DELRISSA    = 11918,
    CRITERIA_MEMORY_ENTROPIUS   = 11919,
    CRITERIA_MEMORY_INGVAR      = 11920,
    CRITERIA_MEMORY_CYANIGOSA   = 11921,
    CRITERIA_MEMORY_ECK         = 11922,
    CRITERIA_MEMORY_ONYXIA      = 11923,
    CRITERIA_MEMORY_HEIGAN      = 11924,
    CRITERIA_MEMORY_IGNIS       = 11925,
    CRITERIA_MEMORY_VEZAX       = 11926,
    CRITERIA_MEMORY_ALGALON     = 11927,
    CRITERIA_I_VE_HAD_WORSE     = 11789,
};

enum InstanceSpells
{
	SPELL_FEIGN_DEATH              = 57626,
    SPELL_MOUNT_LANCE_STAND	       = 64723,
    SPELL_DEATH_RESPITE_INTRO      = 66798,
    SPELL_DEATH_PUSH_INTRO         = 66797,
	SPELL_ACHIEVEMENT_CHAMPIONS    = 68572,
    SPELL_ACHIEVEMENT_PALETRESS    = 68574,
    SPELL_ACHIEVEMENT_MEMORIES     = 68206,
    SPELL_ACHIEVEMENT_EADRIC       = 68575,
    SPELL_ACHIEVEMENT_BLACK_KNIGHT = 68663,
};

enum Encounters
{
	MAX_ENCOUNTER = 4,
};

static void BindPlayersToInstance(Creature* creature)
{
	Map* map = creature->GetMap();

    if (map && map->IsDungeon() && map->IsRaidOrHeroicDungeon())
    {
        Map::PlayerList const &PlList = map->GetPlayers();

        if (!PlList.isEmpty() && PlList.begin()->getSource())
            ((InstanceMap*)map)->PermBindAllPlayers(PlList.begin()->getSource());
    }
};

static void AggroAllPlayers(Creature* creature)
{
    Map::PlayerList const &PlList = creature->GetMap()->GetPlayers();

    if (PlList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator itr = PlList.begin(); itr != PlList.end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
            if (player->IsGameMaster())
                continue;

            if (player->IsAlive())
            {
                if (Unit* mount = player->GetVehicleBase())
                {
                    creature->SetInCombatWith(mount);
                    mount->SetInCombatWith(creature);
                    creature->AddThreat(mount, 0.0f);
                }
                else
                {
                    creature->SetInCombatWith(player);
                    player->SetInCombatWith(creature);
                    creature->AddThreat(player, 0.0f);
                }
            }
        }
    }
};

#endif