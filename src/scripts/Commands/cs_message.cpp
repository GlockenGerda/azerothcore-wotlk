/*
 * Copyright (C) 
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

/* ScriptData
Name: message_commandscript
%Complete: 100
Comment: All message related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "Language.h"
#include "Player.h"

class message_commandscript : public CommandScript
{
public:
    message_commandscript() : CommandScript("message_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand commandTable[] =
        {
            { "nameannounce",   SEC_GAMEMASTER,      true,   &HandleNameAnnounceCommand,         "", NULL },
            { "gmnameannounce", SEC_GAMEMASTER,      true,   &HandleGMNameAnnounceCommand,       "", NULL },
            { "announce",       SEC_GAMEMASTER,      true,   &HandleAnnounceCommand,             "", NULL },
            { "gmannounce",     SEC_GAMEMASTER,      true,   &HandleGMAnnounceCommand,           "", NULL },
            { "notify",         SEC_GAMEMASTER,      true,   &HandleNotifyCommand,               "", NULL },
            { "gmnotify",       SEC_GAMEMASTER,      true,   &HandleGMNotifyCommand,             "", NULL },
            { "whispers",       SEC_GAMEMASTER,      false,  &HandleWhispersCommand,             "", NULL },
            { NULL,             0,                  false,  NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleNameAnnounceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string name("Console");
        if (WorldSession* session = handler->GetSession())
            name = session->GetPlayer()->GetName();

        sWorld->SendWorldText(LANG_ANNOUNCE_COLOR, name.c_str(), args);
        return true;
    }

    static bool HandleGMNameAnnounceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string name("Console");
        if (WorldSession* session = handler->GetSession())
            name = session->GetPlayer()->GetName();

        sWorld->SendGMText(LANG_GM_ANNOUNCE_COLOR, name.c_str(), args);
        return true;
    }
    // global announce
    static bool HandleAnnounceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char buff[2048];
        sprintf(buff, handler->GetTrinityString(LANG_SYSTEMMESSAGE), args);
        sWorld->SendServerMessage(SERVER_MSG_STRING, buff);
        return true;
    }
    // announce to logged in GMs
    static bool HandleGMAnnounceCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        sWorld->SendGMText(LANG_GM_BROADCAST, args);
        return true;
    }
    // notification player at the screen
    static bool HandleNotifyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string str = handler->GetTrinityString(LANG_GLOBAL_NOTIFY);
        str += args;

        WorldPacket data(SMSG_NOTIFICATION, (str.size()+1));
        data << str;
        sWorld->SendGlobalMessage(&data);

        return true;
    }
    // notification GM at the screen
    static bool HandleGMNotifyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string str = handler->GetTrinityString(LANG_GM_NOTIFY);
        str += args;

        WorldPacket data(SMSG_NOTIFICATION, (str.size()+1));
        data << str;
        sWorld->SendGlobalGMMessage(&data);

        return true;
    }
    // Enable\Dissable accept whispers (for GM)
    static bool HandleWhispersCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            handler->PSendSysMessage(LANG_COMMAND_WHISPERACCEPTING, handler->GetSession()->GetPlayer()->isAcceptWhispers() ?  handler->GetTrinityString(LANG_ON) : handler->GetTrinityString(LANG_OFF));
            return true;
        }

        std::string argStr = (char*)args;
        // whisper on
        if (argStr == "on")
        {
            handler->GetSession()->GetPlayer()->SetAcceptWhispers(true);
            handler->SendSysMessage(LANG_COMMAND_WHISPERON);
            return true;
        }

        // whisper off
        if (argStr == "off")
        {
            // Remove all players from the Gamemaster's whisper whitelist
            handler->GetSession()->GetPlayer()->ClearWhisperWhiteList();
            handler->GetSession()->GetPlayer()->SetAcceptWhispers(false);
            handler->SendSysMessage(LANG_COMMAND_WHISPEROFF);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_message_commandscript()
{
    new message_commandscript();
}