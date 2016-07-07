# Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/ahnkahet_the_old_kingdom_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/azjol_nerub_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/culling_of_stratholme_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/drak_tharon_keep_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/forge_of_souls_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/gundrak_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/halls_of_lightning_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/halls_of_reflection_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/halls_of_stone_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/icecrown_citadel_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/naxxramas_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/nexus_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/obsidian_sanctum_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/oculus_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/pit_of_saron_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/ruby_sanctum_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/ulduar_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/utgarde_keep_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Wotlk/utgarde_pinnacle_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/arcatraz_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/auchenai_crypts_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/black_temple_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/blood_furnace_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/botanica_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/coilfang_reservoir_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/hellfire_ramparts_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/karazhan_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/magisters_terrace_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/mana_tombs_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/mechanar_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/sethekk_halls_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/shadow_labyrinth_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/shattered_halls_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/slave_pens_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/steam_vault_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/sunwel_plateau_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/tempest_keep_the_eye_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/BurningCrusade/underbog_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Classic/blackwing_lair_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Classic/molten_core_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Classic/ruins_of_ahnqiraj_trash.cpp
  QuantumCoreScripts/InstanceTrashScripts/Classic/temple_of_ahnqiraj_trash.cpp
  QuantumCoreScripts/SystemScripts/world_achievement_system.cpp
  QuantumCoreScripts/SystemScripts/world_anniversary_system.cpp
  QuantumCoreScripts/SystemScripts/world_anti_advertising_system.cpp
  QuantumCoreScripts/SystemScripts/world_auto_learn_spell_system.cpp
  QuantumCoreScripts/SystemScripts/world_battleground_kill_system.cpp
  QuantumCoreScripts/SystemScripts/world_chat_filter_system.cpp
  QuantumCoreScripts/SystemScripts/world_chat_icon_system.cpp
  QuantumCoreScripts/SystemScripts/world_duel_reset_cooldown_system.cpp
  QuantumCoreScripts/SystemScripts/world_fun_new_system.cpp
  QuantumCoreScripts/SystemScripts/world_fun_old_system.cpp
  QuantumCoreScripts/SystemScripts/world_fun_system.h
  QuantumCoreScripts/SystemScripts/world_game_master_monitoring_system.cpp
  QuantumCoreScripts/SystemScripts/world_game_master_system.cpp
  QuantumCoreScripts/SystemScripts/world_login_info_system.cpp
  QuantumCoreScripts/SystemScripts/world_raid_progress_system.cpp
  QuantumCoreScripts/SystemScripts/world_rewarding_system.cpp
  QuantumCoreScripts/SystemScripts/world_starting_player_system.cpp
  QuantumCoreScripts/SystemScripts/world_transmogrification_system.cpp
  QuantumCoreScripts/SystemScripts/world_transmogrification_system.h
  QuantumCoreScripts/WorldCustomScripts/battle_tyrael.cpp
  QuantumCoreScripts/WorldCustomScripts/funny_panda_of_shaolin.cpp
  QuantumCoreScripts/WorldCustomScripts/insidious_cat.cpp
  QuantumCoreScripts/WorldCustomScripts/server_helper.cpp
  QuantumCoreScripts/WorldScripts/alliance_and_horde_bosses.cpp
  QuantumCoreScripts/WorldScripts/alliance_and_horde_guards.cpp
  QuantumCoreScripts/WorldScripts/world_event_scripts.cpp
  QuantumCoreScripts/WorldScripts/world_guard_scripts.cpp
  QuantumCoreScripts/WorldScripts/world_quest_scripts.cpp
  QuantumCoreScripts/WorldScripts/world_rare_elite_scripts.cpp
  QuantumCoreScripts/WorldScripts/world_season_events.cpp
  QuantumCoreScripts/WorldZoneScripts/eastern_kingdoms_npc_scripts.cpp
  QuantumCoreScripts/WorldZoneScripts/kalimdor_npc_scripts.cpp
  QuantumCoreScripts/WorldZoneScripts/northrend_npc_scripts.cpp
  QuantumCoreScripts/WorldZoneScripts/outland_npc_scripts.cpp
)

message("  -> Prepared: Quantum Core Scripts")
