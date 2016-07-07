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

#include <ace/Sig_Handler.h>
#include "Common.h"
#include "SystemConfig.h"
#include "SignalHandler.h"
#include "World.h"
#include "WorldRunnable.h"
#include "WorldSocket.h"
#include "WorldSocketMgr.h"
#include "Configuration/Config.h"
#include "Database/DatabaseEnv.h"
#include "Database/DatabaseWorkerPool.h"
#include "CliRunnable.h"
#include "Log.h"
#include "Master.h"
#include "RARunnable.h"
#include "TCSoap.h"
#include "Timer.h"
#include "Util.h"
#include "AuthSocket.h"
#include "RealmList.h"
#include "BigNumber.h"

#ifdef _WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif

/// Handle worldservers's termination signals
class WorldServerSignalHandler : public Trinity::SignalHandler
{
    public:
        virtual void HandleSignal(int SigNum)
        {
            switch (SigNum)
            {
                case SIGINT:
                    World::StopNow(RESTART_EXIT_CODE);
                    break;
                case SIGTERM:
                #ifdef _WIN32
                case SIGBREAK:
                    if (m_ServiceStatus != 1)
                #endif /* _WIN32 */
                    World::StopNow(SHUTDOWN_EXIT_CODE);
                    break;
            }
        }
};

class FreezeDetectorRunnable : public ACE_Based::Runnable
{
public:
    FreezeDetectorRunnable() { _delaytime = 0; }
    uint32 m_loops, m_lastchange;
    uint32 w_loops, w_lastchange;
    uint32 _delaytime;
    void SetDelayTime(uint32 t) { _delaytime = t; }
    void run(void)
    {
        if (!_delaytime)
            return;
		sLog->OutConsole("QUANTUMCORE: ANTI-FREEZE SYSTEM ENABLE %u SECONDS MAXIMAL FREEZE TIME.", _delaytime/1000);
        m_loops = 0;
        w_loops = 0;
        m_lastchange = 0;
        w_lastchange = 0;
        while (!World::IsStopped())
        {
            ACE_Based::Thread::Sleep(1000);
            uint32 curtime = getMSTime();
            // normal work
            if (w_loops != World::m_worldLoopCounter)
            {
                w_lastchange = curtime;
                w_loops = World::m_worldLoopCounter;
            }
            // possible freeze
            else if (getMSTimeDiff(w_lastchange, curtime) > _delaytime)
            {
                sLog->OutErrorConsole("QUANTUMCORE: WORLD THREAD HANGS, KICKING OUT SERVER!");
                ASSERT(false);
            }
        }
        sLog->OutConsole("QUANTUMCORE: ANTI-FREEZE SYSTEM SHUTDOWN WITHOUT PROBLEMS...");
    }
};

Master::Master(){}

Master::~Master(){}

int Master::Run()
{
    BigNumber seed1;
    seed1.SetRand(16 * 8);

    sLog->OutConsole("%s (Worldserver)", _FULLVERSION);
    sLog->OutConsole("<Ctrl-C> to stop.\n");

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

    /// worldserver PID file creation
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

    ///- Start the databases
    if (!_StartDB())
        return 1;

    // set server offline (not connectable)
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = (flag & ~%u) | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, REALM_FLAG_INVALID, realmID);

    ///- Initialize the World
    sWorld->SetInitialWorldSettings();

    // Initialise the signal handlers
    WorldServerSignalHandler SignalINT, SignalTERM;
    #ifdef _WIN32
    WorldServerSignalHandler SignalBREAK;
    #endif /* _WIN32 */

    // Register worldserver's signal handlers
    ACE_Sig_Handler Handler;
    Handler.register_handler(SIGINT, &SignalINT);
    Handler.register_handler(SIGTERM, &SignalTERM);
    #ifdef _WIN32
    Handler.register_handler(SIGBREAK, &SignalBREAK);
    #endif /* _WIN32 */

    ///- Launch WorldRunnable thread
    ACE_Based::Thread world_thread(new WorldRunnable);
    world_thread.setPriority(ACE_Based::Highest);

    ACE_Based::Thread* cliThread = NULL;

#ifdef _WIN32
    if (GetConfigSettings::GetBoolState("Console.Enable", true) && (m_ServiceStatus == -1)/* need disable console in service mode*/)
#else
    if (GetConfigSettings::GetBoolState("Console.Enable", true))
#endif
    {
        ///- Launch CliRunnable thread
        cliThread = new ACE_Based::Thread(new CliRunnable);
    }

    ACE_Based::Thread rar_thread(new RARunnable);

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
                {
                    sLog->OutErrorConsole("Processors marked in UseProcessors bitmask (hex) %x are not accessible for the worldserver. Accessible processors bitmask (hex): %x", Aff, appAff);
                }
                else
                {
                    if (SetProcessAffinityMask(hProcess, curAff))
                        sLog->OutConsole("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog->OutErrorConsole("Can't set used processors (hex): %x", curAff);
                }
            }
            sLog->OutConsole("");
        }

        bool Prio = GetConfigSettings::GetBoolState("ProcessPriority", false);

        //if (Prio && (m_ServiceStatus == -1)  /* need set to default process priority class in service mode*/)
        if (Prio)
        {
            if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
				sLog->OutConsole("WORLDSERVER: SELECT HIGH PROCESS PRIORITY.");
            else
                sLog->OutErrorConsole("Can't set worldserver process priority class.");
			sLog->OutConsole();
        }
    }
    #endif
    //Start soap serving thread
    ACE_Based::Thread* soap_thread = NULL;

    if (GetConfigSettings::GetBoolState("SOAP.Enabled", false))
    {
        TCSoapRunnable* runnable = new TCSoapRunnable();
        runnable->setListenArguments(GetConfigSettings::GetStringDefault("SOAP.IP", "127.0.0.1"), GetConfigSettings::GetIntState("SOAP.Port", 7878));
        soap_thread = new ACE_Based::Thread(runnable);
    }

    ///- Start up freeze catcher thread
    if (uint32 freeze_delay = GetConfigSettings::GetIntState("MaxCoreStuckTime", 0))
    {
        FreezeDetectorRunnable* fdr = new FreezeDetectorRunnable();
        fdr->SetDelayTime(freeze_delay*1000);
        ACE_Based::Thread freeze_thread(fdr);
        freeze_thread.setPriority(ACE_Based::Highest);
    }

    ///- Launch the world listener socket
    uint16 wsport = sWorld->getIntConfig(CONFIG_PORT_WORLD);
    std::string bind_ip = GetConfigSettings::GetStringDefault("BindIP", "0.0.0.0");

    if (sWorldSocketMgr->StartNetwork(wsport, bind_ip.c_str ()) == -1)
    {
        sLog->OutErrorConsole("Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    // set server online (allow connecting now)
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = flag & ~%u, population = 0 WHERE id = '%u'", REALM_FLAG_INVALID, realmID);

    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
    world_thread.wait();
    rar_thread.wait();

    if (soap_thread)
    {
        soap_thread->wait();
        soap_thread->destroy();
        delete soap_thread;
    }

    // set server offline
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = flag | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, realmID);

    ///- Clean database before leaving
    ClearOnlineAccounts();

    _StopDB();

    sLog->OutConsole("Halting process...");

    if (cliThread)
    {
        #ifdef _WIN32

        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[5];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        WriteConsoleInput(hStdIn, b, 4, &numb);

        cliThread->wait();

        #else

        cliThread->destroy();

        #endif

        delete cliThread;
    }

    // for some unknown reason, unloading scripts here and not in worldrunnable
    // fixes a memory leak related to detaching threads from the module
    //UnloadScriptingModule();

    // Exit the process with specified return value
    return World::GetExitCode();
}

/// Initialize connection to the databases
bool Master::_StartDB()
{
    MySQL::Library_Init();

    sLog->SetLogDB(false);
    std::string dbstring;
    uint8 async_threads, synch_threads;

    dbstring = GetConfigSettings::GetStringDefault("WorldDatabaseInfo", "");
    if (dbstring.empty())
    {
        sLog->OutErrorConsole("World database not specified in configuration file");
        return false;
    }

    async_threads = GetConfigSettings::GetIntState("WorldDatabase.WorkerThreads", 1);
    if (async_threads < 1 || async_threads > 32)
    {
        sLog->OutErrorConsole("World database: invalid number of worker threads specified. "
            "Please pick a value between 1 and 32.");
        return false;
    }

    synch_threads = GetConfigSettings::GetIntState("WorldDatabase.SynchThreads", 1);
    ///- Initialise the world database
    if (!WorldDatabase.Open(dbstring, async_threads, synch_threads))
    {
        sLog->OutErrorConsole("Cannot connect to world database %s", dbstring.c_str());
        return false;
    }

    ///- Get character database info from configuration file
    dbstring = GetConfigSettings::GetStringDefault("CharacterDatabaseInfo", "");
    if (dbstring.empty())
    {
        sLog->OutErrorConsole("Character database not specified in configuration file");
        return false;
    }

    async_threads = GetConfigSettings::GetIntState("CharacterDatabase.WorkerThreads", 1);
    if (async_threads < 1 || async_threads > 32)
    {
        sLog->OutErrorConsole("Character database: invalid number of worker threads specified. "
            "Please pick a value between 1 and 32.");
        return false;
    }

    synch_threads = GetConfigSettings::GetIntState("CharacterDatabase.SynchThreads", 2);

    ///- Initialise the Character database
    if (!CharacterDatabase.Open(dbstring, async_threads, synch_threads))
    {
        sLog->OutErrorConsole("Cannot connect to Character database %s", dbstring.c_str());
        return false;
    }

    ///- Get login database info from configuration file
    dbstring = GetConfigSettings::GetStringDefault("LoginDatabaseInfo", "");
    if (dbstring.empty())
    {
        sLog->OutErrorConsole("Login database not specified in configuration file");
        return false;
    }

    async_threads = GetConfigSettings::GetIntState("LoginDatabase.WorkerThreads", 1);
    if (async_threads < 1 || async_threads > 32)
    {
        sLog->OutErrorConsole("Login database: invalid number of worker threads specified. "
            "Please pick a value between 1 and 32.");
        return false;
    }

    synch_threads = GetConfigSettings::GetIntState("LoginDatabase.SynchThreads", 1);
    ///- Initialise the login database
    if (!LoginDatabase.Open(dbstring, async_threads, synch_threads))
    {
        sLog->OutErrorConsole("Cannot connect to login database %s", dbstring.c_str());
        return false;
    }

    ///- Get the realm Id from the configuration file
    realmID = GetConfigSettings::GetIntState("RealmID", 0);
    if (!realmID)
    {
        sLog->OutErrorConsole("Realm ID not defined in configuration file");
        return false;
    }
    sLog->OutConsole("Realm running as realm ID %d", realmID);

    ///- Initialize the DB logging system
    sLog->SetLogDBLater(GetConfigSettings::GetBoolState("EnableLogDB", false)); // set var to enable DB logging once startup finished.
    sLog->SetLogDB(false);
    sLog->SetRealmID(realmID);

    ///- Clean the database before starting
    ClearOnlineAccounts();

    ///- Insert version info into DB
    WorldDatabase.PExecute("UPDATE version SET core_version = '%s', core_revision = '%s'", _FULLVERSION, _HASH);        // One-time query

    sWorld->LoadDBVersion();

    sLog->OutConsole("Using World DB: %s", sWorld->GetDBVersion());
	sLog->OutConsole("Using creature EventAI: %s", sWorld->GetCreatureEventAIVersion());
    return true;
}

void Master::_StopDB()
{
    CharacterDatabase.Close();
    WorldDatabase.Close();
    LoginDatabase.Close();

    MySQL::Library_End();
}

/// Clear 'online' status for all accounts with characters in this realm
void Master::ClearOnlineAccounts()
{
    // Reset online status for all accounts with characters on the current realm
    LoginDatabase.DirectPExecute("UPDATE account SET online = 0 WHERE online > 0 AND id IN (SELECT accountid FROM account_char_per_realm WHERE realmid = %d)", realmID);
    // Reset online status for all characters
    CharacterDatabase.DirectExecute("UPDATE characters SET online = 0 WHERE online <> 0");
    // Battleground instance ids reset at server restart
    CharacterDatabase.DirectExecute("UPDATE character_battleground_data SET instanceId = 0");
}