/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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

#include "Chat.h"
#include "Player.h"
#include "QuantumSystemText.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"

enum Spells
{
    LFG_SPELL_DUNGEON_DESERTER = 71041,
    BG_SPELL_DESERTER          = 26013
};

class deserter_commandscript : public CommandScript
{
public:
    deserter_commandscript() : CommandScript("deserter_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand deserterInstanceCommandTable[] =
        {
            { "add",      SEC_GAME_MASTER,  false, &HandleDeserterInstanceAdd,    "", NULL },
            { "remove",   SEC_GAME_MASTER,  false, &HandleDeserterInstanceRemove, "", NULL },
            { NULL,       0,                false, NULL,                          "", NULL }
        };
        static ChatCommand deserterBGCommandTable[] =
        {
            { "add",      SEC_GAME_MASTER,  false, &HandleDeserterBGAdd,    "", NULL },
            { "remove",   SEC_GAME_MASTER,  false, &HandleDeserterBGRemove, "", NULL },
            { NULL,       0,                false, NULL,                    "", NULL }
        };
        static ChatCommand deserterCommandTable[] =
        {
            { "instance", SEC_GAME_MASTER,  false, NULL, "", deserterInstanceCommandTable },
            { "bg",       SEC_GAME_MASTER,  false, NULL, "", deserterBGCommandTable },
            { NULL,       0,                false, NULL, "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "deserter", SEC_GAME_MASTER,  false, NULL, "", deserterCommandTable },
            { NULL,       0,                false, NULL, "", NULL }
        };
        return commandTable;
    }

    static bool HandleDeserterAdd(ChatHandler* handler, char const* args, bool isInstance)
    {
        if (!*args)
            return false;

        Player* player = handler->getSelectedPlayer();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }
        char* timeStr = strtok((char*)args, " ");
        if (!timeStr)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        uint32 time = atoi(timeStr);

        if (!time)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Aura* aura = player->AddAura(isInstance ? LFG_SPELL_DUNGEON_DESERTER : BG_SPELL_DESERTER, player);

        if (!aura)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        aura->SetDuration(time *IN_MILLISECONDS);

        return true;
    }

    static bool HandleDeserterRemove(ChatHandler* handler, char const* /*args*/, bool isInstance)
    {
        Player* player = handler->getSelectedPlayer();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->RemoveAura(isInstance ? LFG_SPELL_DUNGEON_DESERTER : BG_SPELL_DESERTER);

        return true;
    }

    static bool HandleDeserterInstanceAdd(ChatHandler* handler, char const* args)
    {
        return HandleDeserterAdd(handler, args, true);
    }

    static bool HandleDeserterBGAdd(ChatHandler* handler, char const* args)
    {
        return HandleDeserterAdd(handler, args, false);
    }

    static bool HandleDeserterInstanceRemove(ChatHandler* handler, char const* args)
    {
        return HandleDeserterRemove(handler, args, true);
    }

    static bool HandleDeserterBGRemove(ChatHandler* handler, char const* args)
    {
        return HandleDeserterRemove(handler, args, false);
    }
};

void AddSC_deserter_commandscript()
{
    new deserter_commandscript();
}