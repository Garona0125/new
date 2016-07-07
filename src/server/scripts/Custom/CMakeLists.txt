# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
  Custom/mod_account_history.cpp
  Custom/mod_arena_watcher.cpp
  Custom/mod_character_model.cpp
  Custom/npc_enchancement_master.cpp
  Custom/npc_profession_master.cpp
  Custom/npc_vip_vendor.cpp
)

message("  -> Prepared: Custom")
