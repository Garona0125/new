/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/ACE.h>
#include <ace/Sig_Handler.h>
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Configuration/Config.h"
#include "Log.h"
#include "SystemConfig.h"
#include "Util.h"
#include "SignalHandler.h"
#include "RealmList.h"
#include "RealmAcceptor.h"

#ifndef _TRINITY_REALM_CONFIG
# define _TRINITY_REALM_CONFIG "authserver.conf"
#endif

bool StartDB();
void StopDB();

bool stopEvent = false;                                     // Setting it to true stops the server

LoginDatabaseWorkerPool LoginDatabase;                      // Accessor to the auth server database

// Handle authserver's termination signals
class AuthServerSignalHandler : public Trinity::SignalHandler
{
public:
    virtual void HandleSignal(int SigNum)
    {
        switch (SigNum)
        {
            case SIGINT:
            case SIGTERM:
				stopEvent = true;
				break;
        }
    }
};

/// Print out the usage string for this program on the console.
void usage(const char *prog)
{
    sLog->OutConsole("Usage: \n %s [<options>]\n"
        "    -c config_file           use config_file as configuration file\n\r",
        prog);
}

// Launch the auth server
extern int main(int argc, char **argv)
{
    sLog->SetLogDB(false);
    // Command line parsing to get the configuration file name
    char const* cfg_file = _TRINITY_REALM_CONFIG;
    int c = 1;
    while (c < argc)
    {
        if (strcmp(argv[c], "-c") == 0)
        {
            if (++c >= argc)
            {
                sLog->OutErrorConsole("Runtime-Error: -c option requires an input argument\n");
                usage(argv[0]);
                return 1;
            }
            else
                cfg_file = argv[c];
        }
        ++c;
    }

    if (!GetConfigSettings::Load(cfg_file))
    {
        sLog->OutErrorConsole("Invalid or missing configuration file : %s\n", cfg_file);
        sLog->OutErrorConsole("Verify that the file exists and has \'[authserver]\' written in the top of the file!\n");
        return 1;
    }
    sLog->Initialize();

    sLog->OutConsole("%s (Authserver)", _FULLVERSION);
    sLog->OutConsole("<Press Ctrl-C> to stop.\n");
    sLog->OutConsole("Using configuration file %s.\n", cfg_file);

	sLog->OutConsole("QuantumCore (Popularity, Quality, Productivity!)\n");

	sLog->OutConsole("-----------------------------------------------------------------");
	sLog->OutConsole("WARNING BE CAREFUL OF FAKES...");
	sLog->OutConsole("REAL AUTOR OF THE SERVER (PantadeDogmen,Crispi,Expecto,Pandemonium)");
	sLog->OutConsole("REAL ICQ OF THE AUTOR [472 478 074]");
	sLog->OutConsole("REAL SKYPE OF THE AUTOR (Pandemonium902)");
	sLog->OutConsole("REAL CONTACT OF THE AUTOR https://vk.com/shpierkommel");
	sLog->OutConsole("Emulator Based in TrinityCore");
	sLog->OutConsole("Quantum Core DataBase Special For Emulator [QDB]");
	sLog->OutConsole("Trinity Core Project 2008-2010 All Rights Reserved");
	sLog->OutConsole("Mangos Project 2010-2012 All Rights Reserved");
	sLog->OutConsole("ScriptDev2 2010-2012 All Rights Reserved");
	sLog->OutConsole("Quantum Core Project 2010-2015 All Rights Reserved");
	sLog->OutConsole("-----------------------------------------------------------------\n");

	sLog->OutConsole("______Q_________________****,");
	sLog->OutConsole("______U_______________**_*__*,");
	sLog->OutConsole("______A_____________**__*___*,");
	sLog->OutConsole("______N____________*___*___*,");
	sLog->OutConsole("______T___________*____*___*,");
	sLog->OutConsole("______U__________*____*_____**,");
	sLog->OutConsole("______M_________*____*__**____**,");
	sLog->OutConsole("_______________*_____*__****_*__**,");
	sLog->OutConsole("______C________*_____*_******_____*,");
	sLog->OutConsole("______O_______*_____*____***_______*,");
	sLog->OutConsole("______R______*____**_****___________*,");
	sLog->OutConsole("______E______*___*_**_______________*,");
	sLog->OutConsole("_____________*__*___________________*,");
	sLog->OutConsole("____________*___*___________________*,");
	sLog->OutConsole("___________*___*____________________*,");
	sLog->OutConsole("________***____*___________________*,");
	sLog->OutConsole("______**__*___*____________________*,");
	sLog->OutConsole("_____*____*___*____________________*,");
	sLog->OutConsole("______***_*_*_*___________________*,");
	sLog->OutConsole("________*_**__*___________________*,");
	sLog->OutConsole("______*****___*____________________**,");
	sLog->OutConsole("____**_______*_______________________*,");
	sLog->OutConsole("___*_________*________________________*,");
	sLog->OutConsole("__*__________*_________________________*,");
	sLog->OutConsole("_*___________*__________________________*,");
	sLog->OutConsole("*____________*_________________***_______*,");
	sLog->OutConsole("**********___*_________________*__****___*,");
	sLog->OutConsole("_________*___*________________*_______**_*,");
	sLog->OutConsole("_________*___*_______________*__________*,");
	sLog->OutConsole("__________*___*_____________*,");
	sLog->OutConsole("______S___*___*____________*,");
	sLog->OutConsole("______E____*___*__________*,");
	sLog->OutConsole("______R_____*___*________*,");
	sLog->OutConsole("______V______*___*_______*,");
	sLog->OutConsole("______E_______*___*______*,");
	sLog->OutConsole("______R________*___*_____*,");
	sLog->OutConsole("________________*___*____*,");
	sLog->OutConsole("______P__________*___*___*,");
	sLog->OutConsole("______R___________*__*___*,");
	sLog->OutConsole("______O______________*_*___**,");
	sLog->OutConsole("______J________________**____*********,");
	sLog->OutConsole("______E_________________*_____________***,");
	sLog->OutConsole("______C_________________*________________**,");
	sLog->OutConsole("______T________________*_____**______*******,");
	sLog->OutConsole("______________________*______*_******,");
	sLog->OutConsole("_____________________*_____**,");
	sLog->OutConsole("_____________________*__***,");
	sLog->OutConsole("_____________________*_*,\n");

	sLog->OutConsole("THIS IS PRIVATE SERVER VERSION\n");

    sLog->OutConsole("Using SSL version: %s (library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
	sLog->OutConsole("Using ACE version: %s", ACE_VERSION);

#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
#else
    ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
#endif

    sLog->outBasic("Max allowed open files is %d", ACE::max_handles());

    // authserver PID file creation
    std::string pidfile = GetConfigSettings::GetStringDefault("PidFile", "");
    if (!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if (!pid)
        {
            sLog->OutErrorConsole("Cannot create PID file %s.\n", pidfile.c_str());
            return 1;
        }

        sLog->OutConsole("Daemon PID: %u\n", pid);
    }

    // Initialize the database connection
    if (!StartDB())
        return 1;

    // Initialize the log database
    sLog->SetLogDBLater(GetConfigSettings::GetBoolState("EnableLogDB", false)); // set var to enable DB logging once startup finished.
    sLog->SetLogDB(false);
    sLog->SetRealmID(0);                                               // ensure we've set realm to 0 (authserver realmid)

    // Get the list of realms for the server
    sRealmList->Initialize(GetConfigSettings::GetIntState("RealmsStateUpdateDelay", 20));
    if (sRealmList->size() == 0)
    {
        sLog->OutErrorConsole("No valid realms specified.");
        return 1;
    }

    // Launch the listening network socket
    RealmAcceptor acceptor;

    uint16 rmport = GetConfigSettings::GetIntState("RealmServerPort", 3724);
    std::string bind_ip = GetConfigSettings::GetStringDefault("BindIP", "0.0.0.0");

    ACE_INET_Addr bind_addr(rmport, bind_ip.c_str());

    if (acceptor.open(bind_addr, ACE_Reactor::instance(), ACE_NONBLOCK) == -1)
    {
        sLog->OutErrorConsole("Auth server can not bind to %s:%d", bind_ip.c_str(), rmport);
        return 1;
    }

    // Initialise the signal handlers
    AuthServerSignalHandler SignalINT, SignalTERM;

    // Register authservers's signal handlers
    ACE_Sig_Handler Handler;
    Handler.register_handler(SIGINT, &SignalINT);
    Handler.register_handler(SIGTERM, &SignalTERM);

    ///- Handle affinity for multiple processors and process priority on Windows
#ifdef _WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = GetConfigSettings::GetIntState("UseProcessors", 0);
        if (Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if (GetProcessAffinityMask(hProcess, &appAff, &sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if (!curAff)
                    sLog->OutErrorConsole("Processors marked in UseProcessors bitmask (hex) %x not accessible for authserver. Accessible processors bitmask (hex): %x", Aff, appAff);
                else if (SetProcessAffinityMask(hProcess, curAff))
                    sLog->OutConsole("Using processors (bitmask, hex): %x", curAff);
                else
                    sLog->OutErrorConsole("Can't set used processors (hex): %x", curAff);
            }
            sLog->OutConsole();
        }

        bool Prio = GetConfigSettings::GetBoolState("ProcessPriority", false);

        if (Prio)
        {
            if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
			{
				sLog->OutConsole("AUTHSERVER: SELECT HIGH PROCESS PRIORITY.");
			}
            else
                sLog->OutErrorConsole("Can't set auth server process priority class.");
			sLog->OutConsole();
        }
    }
#endif

    // maximum counter for next ping
    uint32 numLoops = (GetConfigSettings::GetIntState("MaxPingTime", 30) * (MINUTE * 1000000 / 100000));
    uint32 loopCounter = 0;

    // possibly enable db logging; avoid massive startup spam by doing it here.
    if (sLog->GetLogDBLater())
    {
        sLog->OutConsole("Enabling database logging...");
        sLog->SetLogDBLater(false);
        // login db needs thread for logging
        sLog->SetLogDB(true);
    }
    else
        sLog->SetLogDB(false);

    // Wait for termination signal
    while (!stopEvent)
    {
        // dont move this outside the loop, the reactor will modify it
        ACE_Time_Value interval(0, 100000);

        if (ACE_Reactor::instance()->run_reactor_event_loop(interval) == -1)
            break;

        if ((++loopCounter) == numLoops)
        {
            loopCounter = 0;
            sLog->outDetail("Ping MySQL to keep connection alive");
            LoginDatabase.KeepAlive();
        }
    }

    // Close the Database Pool and library
    StopDB();

    sLog->OutConsole("Halting process...");
    return 0;
}

// Initialize connection to the database
bool StartDB()
{
    MySQL::Library_Init();

    std::string dbstring = GetConfigSettings::GetStringDefault("LoginDatabaseInfo", "");
    if (dbstring.empty())
    {
        sLog->OutErrorConsole("Database not specified");
        return false;
    }

    uint8 worker_threads = GetConfigSettings::GetIntState("LoginDatabase.WorkerThreads", 1);
    if (worker_threads < 1 || worker_threads > 32)
    {
        sLog->OutErrorConsole("Improper value specified for LoginDatabase.WorkerThreads, defaulting to 1.");
        worker_threads = 1;
    }

    uint8 synch_threads = GetConfigSettings::GetIntState("LoginDatabase.SynchThreads", 1);
    if (synch_threads < 1 || synch_threads > 32)
    {
        sLog->OutErrorConsole("Improper value specified for LoginDatabase.SynchThreads, defaulting to 1.");
        synch_threads = 1;
    }

    // NOTE: While authserver is singlethreaded you should keep synch_threads == 1. Increasing it is just silly since only 1 will be used ever.
    if (!LoginDatabase.Open(dbstring.c_str(), worker_threads, synch_threads))
    {
        sLog->OutErrorConsole("Cannot connect to database");
        return false;
    }
    return true;
}

void StopDB()
{
    LoginDatabase.Close();
    MySQL::Library_End();
}