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
  Spells/ClassSpells/spell_dk.cpp
  Spells/ClassSpells/spell_druid.cpp
  Spells/ClassSpells/spell_hunter.cpp
  Spells/ClassSpells/spell_mage.cpp
  Spells/ClassSpells/spell_paladin.cpp
  Spells/ClassSpells/spell_priest.cpp
  Spells/ClassSpells/spell_rogue.cpp
  Spells/ClassSpells/spell_shaman.cpp
  Spells/ClassSpells/spell_warlock.cpp
  Spells/ClassSpells/spell_warrior.cpp
  Spells/spell_generic.cpp
  Spells/spell_item.cpp
  Spells/spell_pet.cpp
  Spells/spell_quest.cpp
)

message("  -> Prepared: Spells")
